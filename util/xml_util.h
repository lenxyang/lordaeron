#pragma once

#include <string>

#include "azer/math/math.h"
#include "azer/util/xml/xml.h"

namespace lord {
std::vector<util::xml::Element*> GetXPathElement(const std::string& path, 
                                                 util::xml::Node* node);

std::vector<std::string> GetXPathElementText(const std::string& path, 
                                             util::xml::Node* node);
}  // namespace lord
