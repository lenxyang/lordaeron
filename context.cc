);
  effect_context_.RegisteAdapter(new LoadSceneBVParamsAdapter);
}

bool Context::LoadResourcePack(const base::FilePath& path) {
  resource_bundle_.reset(new nelf::ResourceBundle);
  return resource_bundle_->Load(path);
}
}  // namespace lord
