#pragma once

#include "nelf/nelf.h"
#include "nelf/theme/fallback_theme.h"
#include "nelf/theme/fallback/window_theme.h"

namespace lord {
class Theme : public nelf::FallbackTheme {
 public:
  Theme(nelf::Context* context);
  ~Theme();

  scoped_ptr<nelf::WindowTheme> CreateWindowTheme(nelf::Window* window) override;
 private:
  DISALLOW_COPY_AND_ASSIGN(Theme);
};
}  // namespace lord
