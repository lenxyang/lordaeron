#include "lordaeron/resource/light_loader.h"

#include "azer/render/render.h"

namespace lord {
using namespace azer;

namespace {
bool LoadAttenuation(Attenuation* atten, const ConfigNode* config) {
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
  return node->tagname() == "light";
}

VariantResource LightLoader::Load(const ConfigNode* lnode,
                                  ResourceLoadContext* ctx) {
  Vector4 diffuse;
  Vector4 ambient;
  Vector4 specular;
  CHECK(lnode->GetChildTextAsVec4("ambient", &ambient))
      << "light node has ambient";
  CHECK(lnode->GetChildTextAsVec4("diffuse", &diffuse))
      << "light node has diffuse";
  CHECK(lnode->GetChildTextAsVec4("specular", &specular))
      << "light node has specular";

  VariantResource resource;
  resource.type = kResTypeLight;
  resource.retcode = 0;
  const std::string& light_type = lnode->GetAttr("type");
  if (light_type == "point_light") {
    PointLight light;
    light.diffuse = diffuse;
    light.ambient = ambient;
    light.specular = specular;
    CHECK(LoadAttenuation(&light.atten, lnode));
    resource.light = new Light(light);
  } else if (light_type == "spot_light") {
    SpotLight light;
    light.diffuse = diffuse;
    light.ambient = ambient;
    light.specular = specular;
    float inner_angle, outer_angle;
    CHECK(lnode->GetChildTextAsVec3("directional", &light.direction))
        << "light node has directional";
    CHECK(lnode->GetChildTextAsFloat("falloff", &light.falloff));
    CHECK(lnode->GetChildTextAsFloat("range", &light.range));
    CHECK(lnode->GetChildTextAsFloat("outer_angle", &outer_angle));
    CHECK(lnode->GetChildTextAsFloat("inner_angle", &inner_angle));
    light.phi = cos(Degree(outer_angle));
    light.theta = cos(Degree(inner_angle));
    CHECK(light.theta > light.phi);
    resource.light = new Light(light);
  } else if (light_type == "directional_light") {
    DirLight light;
    light.diffuse = diffuse;
    light.ambient = ambient;
    light.specular = specular;
    CHECK(lnode->GetChildTextAsVec3("directional", &light.direction))
        << "light node has directional";
    resource.light = new Light(light);
  } else {
    CHECK(false) << "unknonw light type: " << light_type;
    resource.retcode = -1;
  }

  return resource;
}

}  // namespace lord
