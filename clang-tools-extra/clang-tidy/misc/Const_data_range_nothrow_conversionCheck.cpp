//===--- Const_data_range_nothrow_conversionCheck.cpp - clang-tidy --------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include <iostream>
#include <regex>

#include "Const_data_range_nothrow_conversionCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace misc {

void Const_data_range_nothrow_conversionCheck::registerMatchers(MatchFinder *Finder) {
  // FIXME: Add matchers.
  // callExpr(allOf(callee(functionDecl(hasName("uassertStatusOKWithLocation"))), hasAnyArgument(hasDescendant(cxxMemberCallExpr(thisPointerType(hasDeclaration(cxxRecordDecl(anyOf(hasName("DataRangeCursor"), hasName("ConstDataRangeCursor"), hasName("DataRange"), hasName("ConstDataRange"))))))))))
  
  Finder->addMatcher(
      callExpr(allOf(
              callee(functionDecl(hasName("uassertStatusOKWithLocation"))),
              hasAnyArgument(hasDescendant(
                      cxxMemberCallExpr(thisPointerType(qualType(hasDeclaration(recordDecl(anyOf(
                                          hasName("DataRangeCursor"),
                                          hasName("ConstDataRangeCursor"),
                                          hasName("DataRange"),
                                          hasName("ConstDataRange")))))))))))
      .bind("uassert"), this);
}

void Const_data_range_nothrow_conversionCheck::check(const MatchFinder::MatchResult &Result) {
  // FIXME: Add callback implementation.
  const auto *match = Result.Nodes.getNodeAs<CallExpr>("uassert");

  auto range = Result.SourceManager->getExpansionRange(match->getBeginLoc());
  std::string sourceText = Lexer::getSourceText(range, *Result.SourceManager, getLangOpts());
  std::regex sourceMatcher("^uassertStatusOK\\((.+)\\)$)");
  std::smatch results;
  if (!std::regex_match(sourceText, results, sourceMatcher)) {
      diag(match->getBeginLoc(), "Did not match regex!");
      return;
  }

  diag(match->getBeginLoc(),
       "Prefer NoThrow version of CDR member calls rather than uassertStatusOK")
      << FixItHint::CreateReplacement(match->getSourceRange(), results.str(1));
}

} // namespace misc
} // namespace tidy
} // namespace clang
