#include "lordaeron/ui/nelf_context.h"

#include "lordaeron/ui/theme/theme.h"

namespace lord {
NelfContext::NelfContext() {
}

NelfContext::~NelfContext() {
}

bool NelfContext::InitTheme(int argc, char* argv[]) {
  theme_.reset(new Theme(this));
  theme_->Init();
  return true;
}
}  // namespace lord
