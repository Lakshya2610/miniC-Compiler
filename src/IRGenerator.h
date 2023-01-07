//
// Created by Fan Long on 2020/12/6.
//

#ifndef MINICC_IRGENERATOR_H
#define MINICC_IRGENERATOR_H

//add more header files if your want
#include "ASTVisitor.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "Program.h"

struct LLVMValueTable
{
    std::map<minicc::ASTNode*, llvm::Value*> m_table;

    llvm::Value* Get( minicc::ASTNode* node )
    {
        if ( m_table.count( node ) != 0 )
        {
            return m_table[ node ];
        }
        
        return nullptr;
    }

    void Add( minicc::ASTNode* node, llvm::Value* v )
    {
        assert( node );
        assert( v );
        m_table.insert( { node, v });
    }
};


namespace minicc {

    class IRGenerator : public ASTVisitor {
        std::unique_ptr<llvm::LLVMContext> TheContext;
        std::unique_ptr<llvm::Module> TheModule;
        std::unique_ptr<llvm::IRBuilder<>> TheBuilder;
        std::string ModuleName;

        //add your variables and member functions
        LLVMValueTable m_LLVMValueTable;
    public:
        //modify if needed
        explicit IRGenerator(const std::string moduleName) : ASTVisitor(), TheModule(), TheBuilder(), ModuleName(moduleName){
            TheContext = std::make_unique<llvm::LLVMContext>();
        }

        std::unique_ptr<llvm::Module> getModule() {
            TheBuilder.reset();
            return std::move(TheModule);
        }

        void AddLibraryFunctions();

        void visitProgram(Program* prog) override;

        void visitVarDecl(VarDeclaration *decl) override;

        void visitFuncDecl(FuncDeclaration *func) override;

        void visitIfStmt(IfStatement *stmt) override;

        void visitForStmt(ForStatement *stmt) override;

        void visitWhileStmt(WhileStatement *stmt) override;

        void visitReturnStmt(ReturnStatement *stmt) override;

        void visitBreakStmt(BreakStatement *stmt) override;

        void visitScope(ScopeStatement *stmt) override;

        void visitUnaryExpr(UnaryExpr *expr) override;

        void visitBinaryExpr(BinaryExpr *expr) override;

        void visitCallExpr(CallExpr *expr) override;

        void visitVarExpr(VarExpr *expr) override;

        void visitVarRef(VarReference* var) override;

        void visitAssignmentExpr(AssignmentExpr *expr) override;

        void visitIntLiteralExpr(IntLiteralExpr *literal) override;

        void visitBoolLiteralExpr(BoolLiteralExpr *literal) override;

        void HandleAndExpr(llvm::Value* leftOp, llvm::Value* rightOp, BinaryExpr* expr);

        void HandleOrExpr(llvm::Value* leftOp, llvm::Value* rightOp, BinaryExpr* expr);
    };
}

#endif //MINICC_IRGENERATOR_H
