//
// Created by Fan Long on 2020/12/6.
//

//add more header files if your want
#include "IRGenerator.h"
#include "llvm/IR/Module.h"
#include "Declarations.h"
#include "Program.h"
#include "Exprs.h"
#include "Statements.h"
#include <iostream>

// MiniCIO funcs
#define MINICIO_PUTNEWLINE "putnewline"
#define MINICIO_PUTINT "putint"
#define MINICIO_GETINT "getint"

#define FUNCTION_MAIN_BLOCK_NAME "InstrBlock"
#define IF_TRUE_BLOCK "IfBlock"
#define IF_FALSE_BLOCK "ElseBlock"
#define IF_AFTER_BLOCK "IfAfterBlock"

#define LOOP_BODY "LoopBody"
#define LOOP_EXIT "LoopExit"
#define LOOP_COND "LoopCond"

#define BREAK_BLOCK "BreakBlock"
#define FUNCTION_RETURN_BLOCK "FunctionReturnBlock"
#define RETURN_BLOCK "ReturnBlock"

#define AND_SLOW_BLOCK "AndSlowBlock"
#define AND_OUT_BLOCK "AndOutBlock"

#define OR_SLOW_BLOCK "OrSlowBlock"
#define OR_OUT_BLOCK "OrOutBlock"

namespace minicc {
    //add your member helper functions
    llvm::Type* PrimitiveTypeToLLVMType( llvm::LLVMContext &ctx, Type::PrimitiveTypeEnum primType )
    {
        switch ( primType )
        {
        case Type::Int:
            return llvm::Type::getInt32Ty( ctx );
        case Type::Bool:
            return llvm::Type::getInt1Ty( ctx );
        case Type::Void:
            return llvm::Type::getVoidTy( ctx );
        default:
            return llvm::Type::getVoidTy( ctx );
        }
    }

    llvm::Constant* GetDefaultValueForType( llvm::IRBuilder<>* builder, Type::PrimitiveTypeEnum primType, llvm::Type* arrayTy )
    {
        bool isArray = arrayTy != nullptr;
        switch ( primType )
        {
        case Type::Int:
            return isArray ? dynamic_cast<llvm::Constant*>( llvm::ConstantAggregateZero::get( arrayTy ) ) : builder->getInt32( 0 );
        case Type::Bool:
            return isArray ? dynamic_cast<llvm::Constant*>( llvm::ConstantAggregateZero::get( arrayTy ) ) : builder->getInt1( 0 );
        case Type::Void:
            return nullptr;
        default:
            return nullptr;
        }
    }

    llvm::ArrayType* GetArrayType( llvm::Type* type, int arraySize )
    {
        return llvm::ArrayType::get( type, arraySize );
    }

    void PrintValue( llvm::Value* v )
    {
        llvm::raw_fd_ostream o( 1, false );
        v->print( o );
        o.flush();
        o.close();
    }

    void IRGenerator::AddLibraryFunctions()
    {
        std::vector<llvm::Type*> argTypes;
        argTypes.clear();
        
        // putnewline
        llvm::Type* llvmRetType = PrimitiveTypeToLLVMType( *TheContext.get(), Type::PrimitiveTypeEnum::Void );
        llvm::FunctionType* llvmFuncType = llvm::FunctionType::get( llvmRetType, argTypes, false );
        llvm::Function* llvmFunc = llvm::Function::Create( llvmFuncType, llvm::Function::ExternalLinkage, MINICIO_PUTNEWLINE, TheModule.get() );

        // putint
        argTypes.clear();
        argTypes.push_back( PrimitiveTypeToLLVMType( *TheContext.get(), Type::PrimitiveTypeEnum::Int ) );
        llvmRetType = PrimitiveTypeToLLVMType( *TheContext.get(), Type::PrimitiveTypeEnum::Void );
        llvmFuncType = llvm::FunctionType::get( llvmRetType, argTypes, false );
        llvmFunc = llvm::Function::Create( llvmFuncType, llvm::Function::ExternalLinkage, MINICIO_PUTINT, TheModule.get() );

        // getint
        argTypes.clear();
        llvmRetType = PrimitiveTypeToLLVMType( *TheContext.get(), Type::PrimitiveTypeEnum::Int );
        llvmFuncType = llvm::FunctionType::get( llvmRetType, argTypes, false );
        llvmFunc = llvm::Function::Create( llvmFuncType, llvm::Function::ExternalLinkage, MINICIO_GETINT, TheModule.get() );
    }

    void IRGenerator::visitProgram(Program *prog) {
        //Initlize llvm module and builder
        TheModule = std::make_unique<llvm::Module>(ModuleName, *TheContext);
        TheBuilder = std::make_unique<llvm::IRBuilder<>>(*TheContext);
        //start your code here

        AddLibraryFunctions();
        IRGenerator::visitASTNode( prog );
    }

    void IRGenerator::visitVarDecl(VarDeclaration *decl) {
        //start your code here
        llvm::Type* vartype = PrimitiveTypeToLLVMType( *( TheContext.get() ), decl->declType() );
        for ( size_t i = 0; i < decl->numVarReferences(); i++ )
        {
            VarReference* ref = decl->varReference( i );
            VarSymbolTable* declaringTable = decl->locateDeclaringTableForVar( ref->identifier()->name() );
            bool isArray = ref->isArray();
            int arraySize = 0;
            if ( isArray ) arraySize = dynamic_cast<IntLiteralExpr*>( ref->indexExpr() )->value();

            llvm::Type* finalType = nullptr;
            if ( declaringTable == decl->root()->scopeVarTable() )
            {
                // global var
                finalType = isArray ? GetArrayType( vartype, arraySize ) : vartype;
                llvm::GlobalVariable* globalVar = new llvm::GlobalVariable( *TheModule, finalType, false,
                    llvm::GlobalValue::LinkageTypes::CommonLinkage, GetDefaultValueForType( TheBuilder.get(), decl->declType(), isArray ? finalType : nullptr ), ref->identifier()->name() );
                declaringTable->setLLVMValue( ref->identifier()->name(), globalVar );
            }
            else
            {
                finalType = isArray ? GetArrayType( vartype, arraySize ) : vartype;
                // local var
                llvm::AllocaInst* localVar = TheBuilder->CreateAlloca( finalType, nullptr, ref->identifier()->name() );
                declaringTable->setLLVMValue( ref->identifier()->name(), localVar );
            }
            
        }
        
    }

    void InitializeParam( llvm::IRBuilder<>* builder, VarSymbolTable* defSymbolTable, Parameter* param, llvm::Argument* llvmArgRef )
    {
        // allocate param and copy passed in value
        llvm::Value* arg = builder->CreateAlloca( llvmArgRef->getType(), nullptr, param->name() );
        builder->CreateStore( llvmArgRef, arg );
        defSymbolTable->setLLVMValue( param->name(), arg );
    }

    void IRGenerator::visitFuncDecl(FuncDeclaration *func) {
        //start your code here
        std::vector<llvm::Type*> argTypes;
        argTypes.clear();
        FuncSymbolTable* table = func->root()->funcTable();
        FuncSymbolEntry entry = table->get( func->name() );
        for ( size_t i = 0; i < entry.ParameterTypes.size(); i++ )
        {
            llvm::Type* llvmType = PrimitiveTypeToLLVMType( *TheContext.get(), entry.ParameterTypes[i].primitiveType() );
            argTypes.push_back( llvmType );
        }
        
        llvm::Type* llvmRetType = PrimitiveTypeToLLVMType( *TheContext.get(), entry.ReturnType.primitiveType() );
        llvm::FunctionType* llvmFuncType = llvm::FunctionType::get( llvmRetType, argTypes, false );
        llvm::Function* llvmFunc = llvm::Function::Create( llvmFuncType, llvm::Function::ExternalLinkage, func->name(), TheModule.get() );
        if ( entry.HasBody )
        {
            llvm::BasicBlock* funcBlock = llvm::BasicBlock::Create( *TheContext.get(), func->name() + FUNCTION_MAIN_BLOCK_NAME, llvmFunc );
            TheBuilder->SetInsertPoint( funcBlock );

            for ( size_t i = 0; i < func->numParameters(); i++ )
            {
                InitializeParam( TheBuilder.get(), func->body()->scopeVarTable(), func->parameter( i ), llvmFunc->getArg( i ) );
            }

            visitScope( func->body() );
            
            if ( entry.ReturnType.isVoid() && TheBuilder->GetInsertBlock()->getTerminator() == nullptr ) TheBuilder->CreateRetVoid();
            TheBuilder->ClearInsertionPoint();
        }
    }

    void IRGenerator::visitIfStmt(IfStatement *stmt) {
        //start your code here
        stmt->condExpr()->accept( this );

        llvm::BasicBlock* oldInsertionPoint = TheBuilder->GetInsertBlock();
        llvm::Function* func = oldInsertionPoint->getParent();
        
        llvm::BasicBlock* afterBlock = llvm::BasicBlock::Create( *TheContext.get(), IF_AFTER_BLOCK, func );
        llvm::BasicBlock* ifBlock = llvm::BasicBlock::Create( *TheContext.get(), IF_TRUE_BLOCK, func );
        TheBuilder->SetInsertPoint( ifBlock );
        stmt->thenStmt()->accept( this );
        if ( TheBuilder->GetInsertBlock()->getTerminator() == nullptr ) TheBuilder->CreateBr( afterBlock );
        TheBuilder->SetInsertPoint( oldInsertionPoint );

        llvm::BasicBlock* elseBlock = nullptr;
        if ( stmt->hasElse() )
        {
            elseBlock = llvm::BasicBlock::Create( *TheContext.get(), IF_FALSE_BLOCK, func );
            TheBuilder->SetInsertPoint( elseBlock );
            stmt->elseStmt()->accept( this );
            if ( TheBuilder->GetInsertBlock()->getTerminator() == nullptr ) TheBuilder->CreateBr( afterBlock );
            TheBuilder->SetInsertPoint( oldInsertionPoint );
        }

        llvm::Value* cond = m_LLVMValueTable.Get( stmt->condExpr() );
        TheBuilder->CreateCondBr( cond, ifBlock, stmt->hasElse() ? elseBlock : afterBlock );

        TheBuilder->SetInsertPoint( afterBlock );
    }

    void IRGenerator::visitForStmt(ForStatement *stmt) {
        //start your code here
        if ( stmt->initExpr() ) stmt->initExpr()->accept( this );

        llvm::BasicBlock* oldInsertionPoint = TheBuilder->GetInsertBlock();
        llvm::Function* func = oldInsertionPoint->getParent();

        llvm::BasicBlock* condBlock = llvm::BasicBlock::Create( *TheContext.get(), LOOP_COND, func );
        llvm::BasicBlock* bodyBlock = llvm::BasicBlock::Create( *TheContext.get(), LOOP_BODY, func );
        llvm::BasicBlock* exitBlock = llvm::BasicBlock::Create( *TheContext.get(), LOOP_EXIT, func );
        m_LLVMValueTable.Add( stmt, exitBlock );

        TheBuilder->CreateBr( condBlock );

        // condition
        TheBuilder->SetInsertPoint( condBlock );
        if ( stmt->condExpr() == nullptr )
        {
            // no condition
            TheBuilder->CreateBr( bodyBlock );
        }
        else
        {
            stmt->condExpr()->accept( this );
            TheBuilder->CreateCondBr( m_LLVMValueTable.Get( stmt->condExpr() ), bodyBlock, exitBlock );
        }

        // body + iter
        TheBuilder->SetInsertPoint( bodyBlock );
        stmt->body()->accept( this );
        if ( stmt->iterExpr() ) stmt->iterExpr()->accept( this );
        if ( TheBuilder->GetInsertBlock()->getTerminator() == nullptr ) TheBuilder->CreateBr( condBlock );

        // post-loop
        TheBuilder->SetInsertPoint( exitBlock );
    }

    void IRGenerator::visitWhileStmt(WhileStatement *stmt) {
        //start your code here
        llvm::BasicBlock* oldInsertionPoint = TheBuilder->GetInsertBlock();
        llvm::Function* func = oldInsertionPoint->getParent();

        llvm::BasicBlock* condBlock = llvm::BasicBlock::Create( *TheContext.get(), LOOP_COND, func );
        llvm::BasicBlock* bodyBlock = llvm::BasicBlock::Create( *TheContext.get(), LOOP_BODY, func );
        llvm::BasicBlock* exitBlock = llvm::BasicBlock::Create( *TheContext.get(), LOOP_EXIT, func );
        m_LLVMValueTable.Add( stmt, exitBlock );

        TheBuilder->CreateBr( condBlock );

        // condition
        TheBuilder->SetInsertPoint( condBlock );
        if ( stmt->condExpr() == nullptr ) TheBuilder->CreateBr( bodyBlock );
        else
        {
            stmt->condExpr()->accept( this );
            TheBuilder->CreateCondBr( m_LLVMValueTable.Get( stmt->condExpr() ), bodyBlock, exitBlock );
        }

        // body
        TheBuilder->SetInsertPoint( bodyBlock );
        stmt->body()->accept( this );
        if ( TheBuilder->GetInsertBlock()->getTerminator() == nullptr ) TheBuilder->CreateBr( condBlock );

        // post-loop
        TheBuilder->SetInsertPoint( exitBlock );
    }

    void IRGenerator::visitReturnStmt(ReturnStatement *stmt) {
        //start your code here
        if ( stmt->hasReturnExpr() )
        {
            visitASTNode( stmt );
            llvm::Value* retVal = m_LLVMValueTable.Get( stmt->returnExpr() );
            m_LLVMValueTable.Add( stmt, TheBuilder->CreateRet( retVal ) );
            return;
        }

        m_LLVMValueTable.Add( stmt, TheBuilder->CreateRetVoid() );
    }

    void IRGenerator::visitBreakStmt(BreakStatement *stmt) {
        //start your code here
        Statement* parentLoop = stmt->getParentLoop();
        assert( parentLoop != nullptr );

        auto oldInsertPt = TheBuilder->GetInsertBlock();

        llvm::BasicBlock* breakBlock = llvm::BasicBlock::Create( *TheContext.get(), BREAK_BLOCK, oldInsertPt->getParent() );
        TheBuilder->CreateBr( breakBlock );
        // TODO: deal with break in the middle of scope (not at the end => getTerminator = null)
        llvm::BasicBlock* exitBlock = static_cast<llvm::BasicBlock*>( m_LLVMValueTable.Get( parentLoop ) );
        TheBuilder->SetInsertPoint( breakBlock );
        TheBuilder->CreateBr( exitBlock );
        TheBuilder->SetInsertPoint( oldInsertPt );
    }

    void IRGenerator::visitUnaryExpr(UnaryExpr *expr) {
        //start your code here
        visitASTNode( expr );
        llvm::Value* operand = m_LLVMValueTable.Get( expr->getChild( 0 ) );
        llvm::Value* outVal;
        if ( expr->opcode() == Expr::ExprOpcode::Sub )
        {
            outVal = TheBuilder->CreateNeg( operand );
        }
        else // not
        {
            outVal = TheBuilder->CreateNot( operand );
        }

        m_LLVMValueTable.Add( expr, outVal );
    }

    void IRGenerator::visitBinaryExpr(BinaryExpr *expr) {
        //start your code here
        visitASTNode( expr );
        llvm::Value* leftOp = m_LLVMValueTable.Get( expr->getChild( 0 ) );
        llvm::Value* rightOp = m_LLVMValueTable.Get( expr->getChild( 1 ) );

        switch ( expr->opcode() )
        {
        case Expr::ExprOpcode::Add:
            m_LLVMValueTable.Add( expr, TheBuilder->CreateAdd( leftOp, rightOp ) );
            break;
        case Expr::ExprOpcode::Sub:
            m_LLVMValueTable.Add( expr, TheBuilder->CreateSub( leftOp, rightOp ) );
            break;
        case Expr::ExprOpcode::Mul:
            m_LLVMValueTable.Add( expr, TheBuilder->CreateMul( leftOp, rightOp ) );
            break;
        case Expr::ExprOpcode::Div:
            m_LLVMValueTable.Add( expr, TheBuilder->CreateSDiv( leftOp, rightOp ) );
            break;
        case Expr::ExprOpcode::Equal:
            m_LLVMValueTable.Add( expr, TheBuilder->CreateICmpEQ( leftOp, rightOp ) );
            break;
        case Expr::ExprOpcode::NotEqual:
            m_LLVMValueTable.Add( expr, TheBuilder->CreateICmpNE( leftOp, rightOp ) );
            break;
        case Expr::ExprOpcode::Less:
            m_LLVMValueTable.Add( expr, TheBuilder->CreateICmpSLT( leftOp, rightOp ) );
            break;
        case Expr::ExprOpcode::LessEqual:
            m_LLVMValueTable.Add( expr, TheBuilder->CreateICmpSLE( leftOp, rightOp ) );
            break;
        case Expr::ExprOpcode::Greater:
            m_LLVMValueTable.Add( expr, TheBuilder->CreateICmpSGT( leftOp, rightOp ) );
            break;
        case Expr::ExprOpcode::GreaterEqual:
            m_LLVMValueTable.Add( expr, TheBuilder->CreateICmpSGE( leftOp, rightOp ) );
            break;
        case Expr::ExprOpcode::And:
            HandleAndExpr( leftOp, rightOp, expr );
            break;
        case Expr::ExprOpcode::Or:
            HandleOrExpr( leftOp, rightOp, expr );
            break;
        default:
            break;
        }
    }

    void IRGenerator::HandleAndExpr( llvm::Value* leftOp, llvm::Value* rightOp, BinaryExpr* expr )
    {
        auto oldInsertionPt = TheBuilder->GetInsertBlock();
        llvm::BasicBlock* slowBlock = llvm::BasicBlock::Create( *TheContext, AND_SLOW_BLOCK, oldInsertionPt->getParent() );
        llvm::BasicBlock* outBlock = llvm::BasicBlock::Create( *TheContext, AND_OUT_BLOCK, oldInsertionPt->getParent() );

        TheBuilder->CreateCondBr( leftOp, slowBlock, outBlock );

        TheBuilder->SetInsertPoint( slowBlock );
        TheBuilder->CreateBr( outBlock );

        TheBuilder->SetInsertPoint( outBlock );
        llvm::PHINode* phiNode = TheBuilder->CreatePHI( PrimitiveTypeToLLVMType( *TheContext, Type::PrimitiveTypeEnum::Bool ), 2 );
        phiNode->addIncoming( llvm::ConstantInt::get( PrimitiveTypeToLLVMType( *TheContext.get(), Type::PrimitiveTypeEnum::Bool ), 0, true ), oldInsertionPt );
        phiNode->addIncoming( rightOp, slowBlock );

        m_LLVMValueTable.Add( expr, phiNode );
    }

    void IRGenerator::HandleOrExpr( llvm::Value* leftOp, llvm::Value* rightOp, BinaryExpr* expr )
    {
        auto oldInsertionPt = TheBuilder->GetInsertBlock();
        llvm::BasicBlock* slowBlock = llvm::BasicBlock::Create( *TheContext, OR_SLOW_BLOCK, oldInsertionPt->getParent() );
        llvm::BasicBlock* outBlock = llvm::BasicBlock::Create( *TheContext, OR_OUT_BLOCK, oldInsertionPt->getParent() );

        TheBuilder->CreateCondBr( leftOp, outBlock, slowBlock );

        TheBuilder->SetInsertPoint( slowBlock );
        TheBuilder->CreateBr( outBlock );

        TheBuilder->SetInsertPoint( outBlock );
        llvm::PHINode* phiNode = TheBuilder->CreatePHI( PrimitiveTypeToLLVMType( *TheContext, Type::PrimitiveTypeEnum::Bool ), 2 );
        phiNode->addIncoming( llvm::ConstantInt::get( PrimitiveTypeToLLVMType( *TheContext.get(), Type::PrimitiveTypeEnum::Bool ), 1, true ), oldInsertionPt );
        phiNode->addIncoming( rightOp, slowBlock );

        m_LLVMValueTable.Add( expr, phiNode );
    }

    void IRGenerator::visitCallExpr(CallExpr *expr) {
        //start your code here
        Identifier* id = (Identifier*)expr->getChild( 0 );
        llvm::Function* f = TheModule->getFunction( id->name() );
        std::vector<llvm::Value*> args;
        args.clear();
        visitASTNode( expr );
        for (size_t i = 0; i < expr->numArgs(); i++)
        {
            args.push_back( m_LLVMValueTable.Get( expr->arg( i ) ) );
        }

        m_LLVMValueTable.Add( expr, TheBuilder->CreateCall( TheModule->getFunction( id->name() ), args ) );
    }

    void IRGenerator::visitVarExpr(VarExpr *expr) {
        //start your code here
        visitASTNode( expr );
        m_LLVMValueTable.Add( expr, m_LLVMValueTable.Get( expr->getChild( 0 ) ) );
    }

    void IRGenerator::visitVarRef(VarReference *ref)
    {
        Identifier* id = (Identifier*)ref->getChild( 0 );
        VarSymbolTable* table = ref->locateDeclaringTableForVar( id->name() );
        VarSymbolEntry entry = table->get( id->name() );
        llvm::LoadInst* load;
        if ( entry.VarType.arrayBound() > 0 )
        {
            // this is an array
            assert( ref->isArray() );
            visitASTNode( ref );
            std::vector<llvm::Value*> idx;
            idx.clear();
            
            // TODO: broken
            llvm::Value* indexVal = m_LLVMValueTable.Get( ref->indexExpr() );
            idx.push_back( TheBuilder->getInt32( 0 ) );
            idx.push_back( indexVal );
            llvm::Value* indexedAddr = TheBuilder->CreateGEP( entry.LLVMValue, idx );
            load = TheBuilder->CreateLoad( indexedAddr );
        }
        else load = TheBuilder->CreateLoad( entry.LLVMValue, id->name() );
        
        
        m_LLVMValueTable.Add( ref, load );
    }

    void IRGenerator::visitAssignmentExpr(AssignmentExpr *expr) {
        //start your code here
        visitASTNode( expr );
        llvm::Value* ptr = m_LLVMValueTable.Get( expr->getChild( 0 ) );
        llvm::Value* val = m_LLVMValueTable.Get( expr->getChild( 1 ) );
        llvm::LoadInst* loadInstLHS = static_cast<llvm::LoadInst*>( ptr );
        // llvm::LoadInst* loadInstRHS = static_cast<llvm::LoadInst*>( val );

        // store value
        TheBuilder->CreateStore( val, loadInstLHS ? loadInstLHS->getPointerOperand() : ptr );
    }

    void IRGenerator::visitIntLiteralExpr(IntLiteralExpr *literal) {
        //start your code here
        auto v = llvm::ConstantInt::get( PrimitiveTypeToLLVMType( *TheContext.get(), Type::PrimitiveTypeEnum::Int ), literal->value(), true );
        m_LLVMValueTable.Add( literal, v );
    }

    void IRGenerator::visitBoolLiteralExpr(BoolLiteralExpr *literal) {
        //start your code here
        auto v = llvm::ConstantInt::get( PrimitiveTypeToLLVMType( *TheContext.get(), Type::PrimitiveTypeEnum::Bool ), literal->value(), true );
        m_LLVMValueTable.Add( literal, v );
    }

    void IRGenerator::visitScope(ScopeStatement *stmt) {
        //start your code here
        for ( size_t i = 0; i < stmt->numChildren(); i++ )
        {
            auto child = stmt->getChild( i );
            if ( !child ) continue;
            child->accept( this );

            if ( child->isBreak() || child->isReturn() ) return;
        }
        
    }

}