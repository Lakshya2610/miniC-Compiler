//
// Created by Fan Long on 2020/12/9.
//

//add more header files if your want
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/IRBuilder.h"
#include <set>

using namespace llvm;

#define DEBUG_PRINTS 0
#define DONT_DELETE_IF_EMPTY false

namespace
{
    struct Alloca2RegPass : public FunctionPass
    {
        static char ID;
        Alloca2RegPass() : FunctionPass(ID) {}

        //declare more varibles if you like
        std::set<AllocaInst *> TargetAllocas;
        std::map<BasicBlock *, std::map<AllocaInst *, Value *>> Post;
        std::map<BasicBlock *, std::map<AllocaInst *, PHINode *>> Pre;
        std::unique_ptr<IRBuilder<>> TheBuilder;

        bool IsTargetAlloca(Value *inst, AllocaInst **outAlloca)
        {
            AllocaInst *allocaInst = dyn_cast<AllocaInst>(inst);
            if (allocaInst == nullptr || TargetAllocas.find(allocaInst) == TargetAllocas.end())
                return false;

            *outAlloca = *(TargetAllocas.find(allocaInst));
            return true;
        }

        bool CanOptimizeAlloca(AllocaInst *inst)
        {
            for (const User *U : inst->users())
            {
                const Instruction *I = dyn_cast<Instruction>(U);
                assert(I != nullptr);
                if (I->getType()->isAggregateType())
                    return false;
                else if (I->getOpcode() != Instruction::Load && I->getOpcode() != Instruction::Store)
                    return false;
                else if (const LoadInst *LI = dyn_cast<LoadInst>(U))
                {
                    if (LI->getType() != inst->getAllocatedType())
                        return false;
                }
                else if (const StoreInst *SI = dyn_cast<StoreInst>(U))
                {
                    if (SI->getValueOperand() == inst || SI->getValueOperand()->getType() != inst->getAllocatedType())
                        return false;
                }
            }

            return true;
        }

        void UpdatePostTable(BasicBlock *bb, AllocaInst *alloca, Value *value)
        {
            if (Post.find(bb) == Post.end())
                Post.insert({bb, std::map<AllocaInst *, Value *>()});
            if (Post[bb].find(alloca) == Post[bb].end())
                Post[bb].insert({alloca, value});
            else
                Post[bb][alloca] = value;
        }

        void UpdatePreTable(BasicBlock *bb, AllocaInst *alloca, PHINode *value)
        {
            if (Pre.find(bb) == Pre.end())
                Pre.insert({bb, std::map<AllocaInst *, PHINode *>()});
            if (Pre[bb].find(alloca) == Pre[bb].end())
                Pre[bb].insert({alloca, value});
            else
                Pre[bb][alloca] = value;
        }

        bool PostValueExists(BasicBlock *BB, AllocaInst *AI)
        {
            return Post.find(BB) != Post.end() && Post[BB].find(AI) != Post[BB].end();
        }

        bool PreValueExists(BasicBlock *BB, AllocaInst *AI)
        {
            return Pre.find(BB) != Pre.end() && Pre[BB].find(AI) != Pre[BB].end();
        }

        bool PHIIncomingEdgeExists(PHINode *PHI, Value *V)
        {
            for (Value *inVal : PHI->incoming_values())
            {
                if (inVal == V)
                    return true;
            }

            return false;
        }

        bool AddIncomingPHIEdges(PHINode *PHI, AllocaInst *AI, BasicBlock *currBB)
        {
            bool changed = false;
            for (BasicBlock *pred : predecessors(currBB))
            {
                if (PostValueExists(pred, AI))
                {
                    if (!PHIIncomingEdgeExists(PHI, Post[pred][AI]))
                    {
                        PHI->addIncoming(Post[pred][AI], pred);
                        changed = true;
                    }
                }
                else if (PreValueExists(pred, AI))
                {
                    if (!PHIIncomingEdgeExists(PHI, Pre[pred][AI]))
                    {
                        PHI->addIncoming(Pre[pred][AI], pred);
                        changed = true;
                    }
                }
            }

            return changed;
        }

        bool InitialBasicBlocksPass(Function &F)
        {
            bool changed = false;
            for (BasicBlock &BB : F)
            {
                for (Instruction &I : make_early_inc_range(BB))
                {
                    AllocaInst *alloca = nullptr;
                    if (LoadInst *LI = dyn_cast<LoadInst>(&I))
                    {
                        if (!IsTargetAlloca(LI->getPointerOperand(), &alloca))
                            continue;

                        // replace with last store val/phi if no store so far
                        if (PostValueExists(&BB, alloca))
                        {
                            // replace load with last store value
                            LI->replaceAllUsesWith(Post[&BB][alloca]);
                            LI->eraseFromParent();
                            changed = true;
                        }
                        else if (PreValueExists(&BB, alloca))
                        {
                            LI->replaceAllUsesWith(Pre[&BB][alloca]);
                            LI->eraseFromParent();
                            changed = true;
                        }
                        else
                        {
                            Value *v = nullptr;
                            if (BB.hasNPredecessorsOrMore(1))
                            {
                                v = PHINode::Create(alloca->getAllocatedType(), 0, "", &*(BB.begin()));
                                UpdatePreTable(&BB, alloca, (PHINode *)v);
                            }
                            else
                            {
                                v = llvm::UndefValue::get(alloca->getAllocatedType());
                                UpdatePostTable(&BB, alloca, v);
                            }

                            LI->replaceAllUsesWith(v);
                            LI->eraseFromParent();
                            changed = true;
                        }
                    }
                    else if (StoreInst *SI = dyn_cast<StoreInst>(&I))
                    {
                        if (!IsTargetAlloca(SI->getPointerOperand(), &alloca))
                            continue;

                        // found store on target alloca -> update it's post value
                        UpdatePostTable(&BB, alloca, SI->getValueOperand());
                        SI->eraseFromParent();
                        changed = true;
                    }
                }

                // fill in the missing alloca entries for Pre & Post
                for (BasicBlock *pred : predecessors(&BB))
                {
                    if (Post.find(pred) != Post.end())
                    {
                        for (auto pair : Post[pred])
                        {
                            AllocaInst *AI = pair.first;
                            if (!PreValueExists(&BB, AI) && !PostValueExists(&BB, AI))
                            {
                                PHINode *PHI = PHINode::Create(AI->getAllocatedType(), 0, "", &*BB.begin());
                                UpdatePreTable(&BB, AI, PHI);
                                changed = true;
                            }
                        }
                    }

                    if (Pre.find(pred) != Pre.end())
                    {
                        for (auto pair : Pre[pred])
                        {
                            AllocaInst *AI = pair.first;
                            if (!PreValueExists(&BB, AI) && !PostValueExists(&BB, AI))
                            {
                                PHINode *PHI = PHINode::Create(AI->getAllocatedType(), 0, "", &*BB.begin());
                                UpdatePreTable(&BB, AI, PHI);
                                changed = true;
                            }
                        }
                    }
                }
            }

            return changed;
        }

        void collectTargetAllocas(Function &F)
        {
            //start your code here
            for (BasicBlock &BB : F)
            {
                for (Instruction &I : BB)
                {
                    if (AllocaInst *AI = dyn_cast<AllocaInst>(&I))
                    {
                        if (!CanOptimizeAlloca(AI))
                            continue;

                        TargetAllocas.insert(AI);
                    }
                }
            }
        }

        void DeletePromotedAllocas()
        {
            for (AllocaInst *AI : TargetAllocas)
            {
                AI->eraseFromParent();
            }
        }

        bool CompletePHINodes()
        {
            bool changed = false;
            for (auto pair : Pre)
            {
                BasicBlock *BB = pair.first;
                for (auto ipair : pair.second)
                {
                    AllocaInst *AI = ipair.first;
                    PHINode *PHI = ipair.second;
                    changed = changed || AddIncomingPHIEdges(PHI, AI, BB);
                }
            }

            return changed;
        }

        bool AreAllPHIEdgesSame(PHINode *PHI)
        {
            if (PHI->getNumIncomingValues() <= 1)
            {
                return false;
            }

            Value *ref = PHI->getIncomingValue(0);
            for (size_t i = 1; i < PHI->getNumIncomingValues(); i++)
            {
                if (PHI->getIncomingValue(i) != ref)
                    return false;
            }

            return true;
        }

        void CleanupUselessPHINodes()
        {
            // need to iteratively eliminate redundant PHI nodes to make sure we don't miss anything after the initial pass
            bool changed = false;
            std::set<PHINode *> deletedPHIs;
            deletedPHIs.clear();
            int i = 0;
            while (true)
            {
                changed = false;
                for (auto pair : Pre)
                {
                    BasicBlock *BB = pair.first;
                    for (auto ipair : pair.second)
                    {
                        AllocaInst *AI = ipair.first;
                        PHINode *PHI = ipair.second;
                        if (deletedPHIs.find(PHI) != deletedPHIs.end())
                            continue;

                        if (PHI->getNumUses() == 0 || PHI->getNumIncomingValues() == 0)
                        {
                            PHI->eraseFromParent();
                            deletedPHIs.insert(PHI);
                            changed = true;
                        }
                        else
                        {
                            // remove self-referencing phi edges
                            for (size_t i = 0; i < PHI->getNumIncomingValues(); i++)
                            {
                                if (PHI->getIncomingValue(i) == PHI)
                                {
                                    PHI->removeIncomingValue(i, DONT_DELETE_IF_EMPTY);
                                    changed = true;
                                }
                            }

                            if (PHI->getNumIncomingValues() == 1)
                            {
                                PHI->replaceAllUsesWith(PHI->getIncomingValue(0));
                                PHI->eraseFromParent();
                                deletedPHIs.insert(PHI);
                                changed = true;
                            }
                            else if (PHI->getNumIncomingValues() == 0)
                            {
                                PHI->eraseFromParent();
                                deletedPHIs.insert(PHI);
                                changed = true;
                            }
                            else if (AreAllPHIEdgesSame(PHI) && PHI->getIncomingValue(0) != PHI)
                            {
                                PHI->replaceAllUsesWith(PHI->getIncomingValue(0));
                                PHI->eraseFromParent();
                                deletedPHIs.insert(PHI);
                                changed = true;
                            }
                        }
                    }
                }

                if (!changed)
                    break;
            }
        }

        virtual bool runOnFunction(Function &F)
        {
            //start your code here
            TargetAllocas.clear();
            Post.clear();
            Pre.clear();

            TheBuilder = std::make_unique<llvm::IRBuilder<>>(F.getContext());
            errs() << "Working on function called " << F.getName() << "!\n";

            collectTargetAllocas(F);

            bool changed = true;
            while (changed)
            {
                changed = false;
                changed = changed || InitialBasicBlocksPass(F);
                changed = changed || CompletePHINodes();
            }

            CleanupUselessPHINodes();
            DeletePromotedAllocas();

#if DEBUG_PRINTS
            errs() << F << "\n";
#endif
            return true;
        }
    };
}

char Alloca2RegPass::ID = 0;

static RegisterPass<Alloca2RegPass> X("alloca2reg", "Alloca-to-register pass for minic", false, false);

// Automatically enable the pass.
// http://adriansampson.net/blog/clangpass.html
static void registerAlloca2RegPass(const PassManagerBuilder &,
                                   legacy::PassManagerBase &PM)
{
    PM.add(new Alloca2RegPass());
}
static RegisterStandardPasses
    RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
                   registerAlloca2RegPass);