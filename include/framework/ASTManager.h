#ifndef AST_MANAGER_H
#define AST_MANAGER_H

#include <fstream>
#include <iostream>
#include <list>
#include <unordered_map>

#include <clang/AST/ASTConsumer.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Analysis/CFG.h>
#include <clang/Frontend/ASTUnit.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendActions.h>

#include "Common.h"
#include "Config.h"

using namespace clang;

class ASTResource;
class ASTTraverse;
class ASTManager;

class ASTResource {
  friend ASTTraverse;
  friend ASTManager;

public:
  ASTResource() {}
  ASTResource(ASTResource &&ASTR) {
    fds = move(ASTR.fds);
    cxxds = move(ASTR.cxxds);
  }
  ASTResource &operator=(ASTResource &&ASTR) {
    if (this == &ASTR) {
      return *this;
    }
    fds = move(ASTR.fds);
    cxxds = move(ASTR.cxxds);
    return *this;
  }

  std::vector<FunctionDecl *> &GetFunctionDecls() { return fds; }
  std::vector<CXXMethodDecl *> &GetCXXMethodDecls() { return cxxds; }

private:
  std::vector<FunctionDecl *> fds;
  std::vector<CXXMethodDecl *> cxxds;
};

class ASTTraverse : public ASTConsumer,
                    public RecursiveASTVisitor<ASTTraverse> {

  friend class ASTManager;

public:
  void HandleTranslationUnit(ASTContext &Context) override {
    TranslationUnitDecl *TUD = Context.getTranslationUnitDecl();
    TraverseDecl(TUD);
  }

  bool TraverseDecl(Decl *D) {
    if (!D)
      return true;
    bool rval = true;
    if (D->getASTContext().getSourceManager().isInMainFile(D->getLocation()) ||
        D->getKind() == Decl::TranslationUnit) {
      rval = RecursiveASTVisitor<ASTTraverse>::TraverseDecl(D);
    }
    return rval;
  }

  // bool Traverse##Type##Decl(Type##Decl *TD){}
  bool TraverseFunctionDecl(FunctionDecl *FD) {
    if (FD && FD->isThisDeclarationADefinition()) {
      ASTR.fds.push_back(FD);
    }
    return true;
  }

  bool TraverseCXXMethodDecl(CXXMethodDecl *CXXMD) {
    if (CXXMD && CXXMD->isThisDeclarationADefinition()) {
      ASTR.cxxds.push_back(CXXMD);
    }
    return true;
  }

private:
  ASTResource ASTR;
};

/**
 * a class that manages all ASTs.
 */
class ASTManager {
public:
  ASTManager(std::vector<std::string> &ASTFiles, Config &configure);

  std::unordered_map<std::string, ASTResource>::iterator getASTRsBegin() {
    return ASTRs.begin();
  }
  std::unordered_map<std::string, ASTResource>::iterator getASTRsEnd() {
    return ASTRs.end();
  }

private:
  Config &config;
  std::unordered_map<std::string, std::unique_ptr<ASTUnit>> ASTUs;
  std::unordered_map<std::string, ASTResource> ASTRs;
};

#endif
