#include "lordaeron/scene/common_node_loader.h"

#include "base/logging.h"
#include "lordaeron/effect/light.h"
#include "lordaeron/scene/scene_node.h"

namespace lord {
namespace {
using namespace azer;
void CalcSceneOrientForZDirection(const Vector3& d, Quaternion* orient) {
  Camera camera;
  Vector3 dir = d.NormalizeCopy();
  Vector3 up = Vector3(dir.x, 10.0f, dir.z);
  Vector3 newz = dir.NormalizeCopy();
  Vector3 newx = std::move(up.cross(newz).NormalizeCopy());
  Vector3 newy = std::move(newz.cross(newx).NormalizeCopy());
  *orient = Quaternion::FromAxis(newx, newy, newz);
}
}

LightNodeLoader::LightNodeLoader() {
}

LightNodeLoader::~LightNodeLoader() {
}

const char* LightNodeLoader::node_type_name() const {
  return "light";
}

bool LightNodeLoader::LoadSceneNode(SceneNode* node, azer::ConfigNode* config,
                                    SceneLoadContext* ctx) {
  using namespace azer;
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

  const std::string& light_type = light_node->GetAttr("type");
  if (light_type == "point_light") {
    PointLight light;
    light.diffuse = diffuse;
    light.ambient = ambient;
    light.specular = specular;
    light.position = node->position();
    CHECK(LoadAttenuation(&light.atten, light_node));
    LightPtr ptr(new Light(light));
    node->mutable_data()->AttachLight(ptr);
  } else if (light_type == "spot_light") {
    SpotLight light;
    light.diffuse = diffuse;
    light.ambient = ambient;
    light.specular = specular;
    light.position = node->position();
    CHECK(light_node->GetChildTextAsVec3("directional", &light.direction))
        << "light node has directional";
    CHECK(light_node->GetChildTextAsFloat("phi", &light.phi));
    CHECK(light_node->GetChildTextAsFloat("theta", &light.theta));
    CHECK(LoadAttenuation(&light.atten, light_node));
    
    LightPtr ptr(new Light(light));
    Quaternion orient;
    CalcSceneOrientForZDirection(light.direction, &orient);
    node->set_orientation(orient);
    node->mutable_data()->AttachLight(ptr);
  } else if (light_type == "directional_light") {
    DirLight light;
    light.diffuse = diffuse;
    light.ambient = ambient;
    light.specular = specular;
    CHECK(light_node->GetChildTextAsVec3("directional", &light.direction))
        << "light node has directional";
    LightPtr ptr(new Light(light));
    Quaternion orient;
    CalcSceneOrientForZDirection(light.direction, &orient);
    node->set_orientation(orient);
    node->mutable_data()->AttachLight(ptr);
  } else {
    CHECK(false) << "unknonw light type: " << light_type;
    return false;
  }

  CHECK_EQ(node->type(), kLampSceneNode);
  return true;
}

bool LightNodeLoader::LoadAttenuation(Attenuation* atten, ConfigNode* config) {
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

// class EnvNodeLoader
const char EnvNodeLoader::kNodeTypeName[] = "environment";
EnvNodeLoader::EnvNodeLoader() {
}

EnvNodeLoader::~EnvNodeLoader() {
}

const char* EnvNodeLoader::node_type_name() const {
  return kNodeTypeName;
}

bool EnvNodeLoader::LoadSceneNode(SceneNode* node, azer::ConfigNode* config,
                                  SceneLoadContext* ctx) {
  node->SetNodeType(kEnvSceneNode);
  return true;
}
}
