#pragma once

#include <string>
#include <vector>

#include "base/files/file_path.h"
#include "base/strings/string16.h"

namespace lord {
class ProjectConfig {
 public:
  ProjectConfig();
  ~ProjectConfig();

  const base::string16& GetProjectName() const { return name_;}
  const std::vector<base::FilePath>& GetAssetsRootPath() const { 
    return assets_path_;
  }

  bool ParseFromXMLFile(const base::FilePath& path);
  bool ParseFromXMLString(const std::string& contents);
 private:
  base::string16 name_;
  std::vector<base::FilePath> assets_path_;
  DISALLOW_COPY_AND_ASSIGN(ProjectConfig);
};
}  // namespace lord
