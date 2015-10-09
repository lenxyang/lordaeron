#include "lordaeron/config/xml_util.h"

namespace lord {
std::vector<util::xml::Element*> GetXPathElement(const std::string& path, 
                                                 util::xml::Node* node) {
  std::vector<util::xml::Element*> ret;
  util::xml::XPathContext xpathcontext;
  util::xml::XPath xpath(path);
  node->execute(xpath, &xpathcontext);
  const std::vector<util::xml::Node*>& res = xpathcontext.GetResult();
  for (auto iter = res.begin(); iter != res.end(); ++iter) {
    if ((*iter)->type() == util::xml::Node::kElementNode) {
      ret.push_back((*iter)->ToElement());
    }
  }

  return ret;
}

std::vector<std::string> GetXPathElementText(const std::string& path, 
                                             util::xml::Node* node) {
  std::vector<std::string> ret;
  std::vector<util::xml::Element*> elements = std::move(GetXPathElement(path, node));
  for (auto iter = elements.begin(); iter != elements.end(); ++iter) {
    ret.push_back((*iter)->GetText());
  }
  return ret;
}
}  // namespace lord
