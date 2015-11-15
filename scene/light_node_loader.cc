#include "lordaeron/scene/light_node_loader.H"

#include "base/logging.h"
#include "lordaeron/render/light.h"
#include "lordaeron/scene/scene_node.h"
#include "lordaeron/scene/scene_node_data.h"

namespace lord {
namespace {
using namespace azer;
void CalcSceneOrientForZDirection(const Vector3& dir, Quaternion* orient) {
  Vector3 axis;
  if (dir.x > 0 && dir.z > 0) 
    axis = Vector3(1.0f, 0.0f, 0.0f);
  else if (dir.x > 0 && dir.z < 0)
    axis = Vector3(0.0f, 0.0f, -1.0f);
  else if (dir.x < 0 && dir.z < 0)
    axis = Vector3(-1.0f, 0.0f, 0.0f);
  else if (dir.x < 0 && dir.z > 0)
    axis = Vector3(0.0f, 0.0f, 1.0f);
  else
    axis = Vector3(1.0f, 0.0f, 0.0f);

  axis.Normalize();
  Vector3 v = dir.cross(axis);
  Vector3 v2 = v.cross(dir);
  Vector3 x_axis = v2.NormalizeCopy();
  Vector3 y_axis = v.NormalizeCopy();
  Vector3 z_axis = dir.NormalizeCopy();
  *orient = Quaternion::FromAxis(x_axis, y_axis, z_axis);
}
}

LightNodeLoader::LightNodeLoader() {
}

LightNodeLoader::~LightNodeLoader() {
}

const char* LightNodeLoader::node_type_name() const {
  return "light";
}

bool LightNodeLoader::LoadSceneNode(SceneNode* node, azer::ConfigNode* config) {
  using namespace azer;
  const std::string& type =  config->GetAttr("type");
  DCHECK(type == "light");

  Vector4 diffuse;
  Vector4 ambient;
  Vector4 specular;
  DCHECK(config->HasNamedChild("light")) << "has no light node";
  ConfigNode* light_node = config->GetNamedChildren("light")[0];
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
    CHECK(light_node->GetChildTextAsFloat("range", &light.range));
    LightPtr ptr(new Light(light));
    node->mutable_data()->AttachLight(ptr);
    return true;
  } else if (light_type == "spot_light") {
    SpotLight light;
    light.diffuse = diffuse;
    light.ambient = ambient;
    light.specular = specular;
    light.position = node->position();
    CHECK(light_node->GetChildTextAsFloat("range", &light.range));
    CHECK(light_node->GetChildTextAsVec3("directional", &light.direction))
        << "light node has directional";
    CHECK(light_node->GetChildTextAsFloat("phi", &light.phi));
    CHECK(light_node->GetChildTextAsFloat("theta", &light.theta));
    CHECK(light_node->GetChildTextAsVec3("attenuation", &light.attenuation))
        << "light node has directional";
    LightPtr ptr(new Light(light));
    Quaternion orient;
    CalcSceneOrientForZDirection(light.direction, &orient);
    node->set_orientation(orient);
    node->mutable_data()->AttachLight(ptr);
    return true;
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
    return true;
  } else {
    CHECK(false) << "unknonw light type: " << light_type;
    return false;
  }
}
}
