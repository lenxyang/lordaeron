#include "lordaeron/resource/light_loader.h"

#include "azer/render/render.h"

namespace lord {
using namespace azer;

namespace {
bool LoadAttenuation(Attenuation* atten, ConfigNode* config) {
  std::vector<ConfigNodePtr> children  =
      std::move(config->GetTaggedChildren("attenuation"));
  if (children.size() != 1u) {
    LOG(ERROR) << "has no or too many attenuation node";
    return false;
  }

  ConfigNodePtr atten_node = children[0];
  CHECK(atten_node->GetChildTextAsVec3("coefficient", &atten->coefficient));
  CHECK(atten_node->GetChildTextAsFloat("range", &atten->range))
      << "attenuation node has distance";
  return true;
}
}  // namespace

const char LightLoader::kSpecialLoaderName[] = "lord::kSpecialLoaderName";
LightLoader::LightLoader() {}
LightLoader::~LightLoader() {}
const char* LightLoader::GetLoaderName() const {
  return kSpecialLoaderName;
}

bool LightLoader::CouldLoad(azer::ConfigNode* node) const {
  const std::string attr = std::move(node->GetAttr("type"));
  return node->tagname() == "node" && !attr.empty() && attr == "light";
}

Resource LightLoader::Load(const ConfigNode* config, ResourceLoaderContext* ctx) {
  const std::string& type =  config->GetAttr("type");
  DCHECK(type == "light");

  Vector4 diffuse;
  Vector4 ambient;
  Vector4 specular;
  DCHECK(config->HasTaggedChild("light")) << "has no light node";
  ConfigNode* light_node = config->GetTaggedChildren("light")[0];
  CHECK(light_node->GetChildTextAsVec4("ambient", &ambient))
      << "light node has ambient";
  CHECK(light_node->GetChildTextAsVec4("diffuse", &diffuse))
      << "light node has diffuse";
  CHECK(light_node->GetChildTextAsVec4("specular", &specular))
      << "light node has specular";

  Resource resource;
  resource.type = kResTypeLight;
  const std::string& light_type = light_node->GetAttr("type");
  if (light_type == "point_light") {
    PointLight light;
    light.diffuse = diffuse;
    light.ambient = ambient;
    light.specular = specular;
    CHECK(LoadAttenuation(&light.atten, light_node));
    resource.light = new Light(light);
  } else if (light_type == "spot_light") {
    SpotLight light;
    light.diffuse = diffuse;
    light.ambient = ambient;
    light.specular = specular;
    float inner_angle, outer_angle;
    CHECK(light_node->GetChildTextAsVec3("directional", &light.direction))
        << "light node has directional";
    CHECK(light_node->GetChildTextAsFloat("falloff", &light.falloff));
    CHECK(light_node->GetChildTextAsFloat("range", &light.range));
    CHECK(light_node->GetChildTextAsFloat("outer_angle", &outer_angle));
    CHECK(light_node->GetChildTextAsFloat("inner_angle", &inner_angle));
    light.phi = cos(Degree(outer_angle));
    light.theta = cos(Degree(inner_angle));
    CHECK(light.theta > light.phi);
    resource.light = new Light(light);
  } else if (light_type == "directional_light") {
    DirLight light;
    light.diffuse = diffuse;
    light.ambient = ambient;
    light.specular = specular;
    CHECK(light_node->GetChildTextAsVec3("directional", &light.direction))
        << "light node has directional";
    resource.light = new Light(light);
  } else {
    CHECK(false) << "unknonw light type: " << light_type;
  }

  return resource;
}

}  // namespace lord
