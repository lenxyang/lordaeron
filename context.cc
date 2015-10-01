#include "lordaeron/context.h"

#include "base/logging.h"

namespace lord {
Context* Context::instance_ = NULL;;

Context* Context::instance() {
  CHECK(instance_);
  return instance_;
}

bool Context::InitContext(int argc, char* argv[]) {
  CHECK(!instance_);
  instance_ = new Context;
  return instance_->Init(argc, argv);
}

Context::Context() {
}

Context::~Context() {
}

bool Context::Init(int argc, char* argv[]) {
  nelf_context_ = nelf::Context::CreateStandAloneContext(argc, argv);
  if (!nelf_context_.get())
    return false;

  CHECK(LoadResourcePack(::base::FilePath(FILE_PATH_LITERAL(
      "out/dbg/lordaeron_resource.pak"))));

  return true;
}

bool Context::LoadResourcePack(const base::FilePath& path) {
  resource_bundle_.reset(new nelf::ResourceBundle);
  return resource_bundle_->Load(path);
}
}  // namespace lord
