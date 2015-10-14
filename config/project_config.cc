#include "lordaeron/config/project_config.h"

#include "base/files/file_util.h"
#include "azer/util/xml/xml.h"
#include "lordaeron/config/xml_util.h"

namespace lord {
ProjectConfig::ProjectConfig() {
}

ProjectConfig::~ProjectConfig() {
}

bool ProjectConfig::ParseFromXMLFile(const base::FilePath& path) {
  std::string contents;
  if (!base::ReadFileToString(path, &contents)) {
    return false;
  }

  return ParseFromXMLString(contents);
}

bool ProjectConfig::ParseFromXMLString(const std::string& contents) {
  return true;
}
}  // namespace lord
