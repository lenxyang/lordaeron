#pragma once

#include "nelf/nelf.h"

namespace lord {
class Context {
 public:
  static Context* instance();
  static bool InitContext(int argc, char* argv[]);

  nelf::Context* GetNelfContext() { return nelf_context_.get();}
  bool LoadResourcePack(const base::FilePath& path);
  nelf::ResourceBundle* resource_bundle() { return resource_bundle_.get();}
 private:
  Context();
  ~Context();

  bool Init(int argc, char* argv[]);

  scoped_ptr<nelf::ResourceBundle> resource_bundle_;
  nelf::ContextPtr nelf_context_;
  static Context* instance_;
  DISALLOW_COPY_AND_ASSIGN(Context);
};
}  // namespace lord
