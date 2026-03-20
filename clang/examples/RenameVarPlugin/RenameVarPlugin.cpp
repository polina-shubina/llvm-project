#include "clang/AST/ASTContext.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;

class RenameVarVisitor : public RecursiveASTVisitor<RenameVarVisitor> {
public:
    explicit RenameVarVisitor(ASTContext *Context, Rewriter &R)
        : Context(Context), TheRewriter(R) {}

    bool VisitVarDecl(VarDecl *VD) {
        // Пропускаем неявные объявления (их не видно в коде)
        if (VD->isImplicit()) return true;

        SourceLocation Loc = VD->getLocation();
        if (Loc.isInvalid()) return true;

        std::string OldName = VD->getNameAsString();
        if (OldName.empty()) return true;

        // Проверка, что имя ещё не содержит префикс
        if (OldName.find("global_") == 0 || 
            OldName.find("local_") == 0 || 
            OldName.find("static_") == 0 || 
            OldName.find("param_") == 0) {
            return true; // уже переименовано
        }

        std::string Prefix;

        if (isa<ParmVarDecl>(VD)) {
            // Параметр функции
            Prefix = "param_";
        }
        else if (VD->isStaticLocal()) {
            // Статическая локальная переменная (static внутри функции)
            Prefix = "static_";
        }
        else if (VD->hasGlobalStorage() && !VD->isStaticLocal()) {
            // Глобальная переменная (вне функции)
            Prefix = "global_";
        }
        else if (VD->isLocalVarDecl()) {
            // Обычная локальная переменная (внутри функции)
            Prefix = "local_";
        }
        else {
            return true; 
        }

        // Заменяем имя в исходном коде
        std::string NewName = Prefix + OldName;
        TheRewriter.ReplaceText(SourceRange(Loc), NewName);

        return true;
    }

    bool VisitDeclRefExpr(DeclRefExpr *DRE) {
        // Получаем объявление переменной, на которую ссылаются
        ValueDecl *VD = DRE->getDecl();
        if (!VD) return true;

        // Нас интересуют только переменные (не функции, не типы)
        VarDecl *Var = dyn_cast<VarDecl>(VD);
        if (!Var) return true;

        // Если это неявное объявление - пропускаем
        if (Var->isImplicit()) return true;

        // Получаем имя переменной
        std::string OldName = Var->getNameAsString();
        if (OldName.empty()) return true;

        // Проверка, что имя ещё не содержит префикс
        if (OldName.find("global_") == 0 || 
            OldName.find("local_") == 0 || 
            OldName.find("static_") == 0 || 
            OldName.find("param_") == 0) {
            return true; // уже переименовано
        }

        std::string Prefix;
        if (isa<ParmVarDecl>(Var)) {
            Prefix = "param_";
        } else if (Var->isStaticLocal()) {
            Prefix = "static_";
        } else if (Var->hasGlobalStorage() && !Var->isStaticLocal()) {
            Prefix = "global_";
        } else if (Var->isLocalVarDecl()) {
            Prefix = "local_";
        } else {
            return true;
        }

        // Заменяем имя в месте использования
        SourceLocation Loc = DRE->getLocation();
        if (Loc.isInvalid()) return true;

        std::string NewName = Prefix + OldName;
        TheRewriter.ReplaceText(SourceRange(Loc), NewName);

        return true;
    }

private:
    ASTContext *Context;
    Rewriter &TheRewriter;
};

class RenameVarConsumer : public ASTConsumer {
public:
    explicit RenameVarConsumer(ASTContext *Context, Rewriter &R)
        : Visitor(Context, R) {}

    void HandleTranslationUnit(ASTContext &Context) override {
        Visitor.TraverseDecl(Context.getTranslationUnitDecl());
    }

private:
    RenameVarVisitor Visitor;
};

class RenameVarAction : public PluginASTAction {
public:
    std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                    StringRef InFile) override {
        TheRewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
        return std::make_unique<RenameVarConsumer>(&CI.getASTContext(), TheRewriter);
    }

    void EndSourceFileAction() override {
        // Печатаем изменённый код в консоль
        TheRewriter.getEditBuffer(TheRewriter.getSourceMgr().getMainFileID())
            .write(llvm::outs());
    }

    bool ParseArgs(const CompilerInstance &CI,
                   const std::vector<std::string> &args) override {
        return true;
    }

private:
    Rewriter TheRewriter;
};

static FrontendPluginRegistry::Add<RenameVarAction>
X("rename-var", "rename variables with prefixes (static_, local_, global_, param_)");