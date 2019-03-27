//===--- Const_data_range_no_castCheck.cpp - clang-tidy -------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "Const_data_range_no_castCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/ExprCXX.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Lex/Lexer.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace misc {

void Const_data_range_no_castCheck::registerMatchers(MatchFinder *Finder) {
  // FIXME: Add matchers.
  Finder->addMatcher(
      cxxConstructExpr(
          allOf(hasDeclaration(cxxConstructorDecl(
                      hasName("ConstDataRange"))),
                hasAnyArgument(cxxReinterpretCastExpr()))).bind("cdr"), this);
}

void Const_data_range_no_castCheck::check(const MatchFinder::MatchResult &Result) {
  // FIXME: Add callback implementation.
  const auto* match = Result.Nodes.getNodeAs<CXXConstructExpr>("cdr");
  for (const auto& arg: match->arguments()) {
      if (CXXReinterpretCastExpr::classof(arg)) {
          auto castArg = static_cast<const CXXReinterpretCastExpr*>(arg);
          auto castToType = castArg->getSubExpr()->getType();
          if (castToType->isPointerType() && castToType->getPointeeType()->isCharType()) {
              auto subExpr = castArg->getSubExprAsWritten();
              auto replacementRange = Result.SourceManager->getExpansionRange(subExpr->getSourceRange());
              auto replacement = Lexer::getSourceText(replacementRange,
                                                      *Result.SourceManager,
                                                      getLangOpts());
              diag(match->getLocation(),
                   "Don't reinterpret_cast your arguments to ConstDataRange constructors")
                  << FixItHint::CreateReplacement(castArg->getSourceRange(), replacement);
          }
      }
  }
  

}

} // namespace misc
} // namespace tidy
} // namespace clang
