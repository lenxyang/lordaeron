#pragma once

#include "nelf/nelf.h"
#include "nelf/adapter/stand_alone_context.h"

namespace lord {
class NelfContext : public nelf::StandAloneContext {
 public:
  NelfContext();
  ~NelfContext();

  bool InitTheme(int argc, char* argv[]) override;
 private:
  DISALLOW_COPY_AND_ASSIGN(NelfContext);
};
}  // namespace lord
