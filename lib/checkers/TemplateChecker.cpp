#include "checkers/TemplateChecker.h"

void TemplateChecker::check() {
  // for example
  auto astr_iter = getASTRsBegin();
  while (astr_iter != getASTRsEnd()) {
    auto fds = astr_iter->second.GetFunctionDecls();
    for (auto fd : fds) {
      std::cout << common::getFullName(fd) << std::endl;
      auto fd_cfg = common::buildCFG(fd);
      // Traverse CFG
      // ...
    }
    ++astr_iter;
  }
}