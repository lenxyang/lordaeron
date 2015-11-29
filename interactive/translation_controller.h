#pragma once

#include "lordaeron/effect/diffuse_effect.h"
#include "lordaeron/interactive/interactive_controller.h"

namespace lord {
class InteractiveContext;
class SceneNode;
class TranslationControllerObject;
class XYZAxisObject;

class TranslationController : public InteractiveController {
 public:
  TranslationController();
  ~TranslationController() override;

  // override
  void Update(const azer::FrameArgs& args) override;
  void Render(azer::Renderer* renderer) override;
  void OnOperationStart(InteractiveContext* ctx) override;
  void OnOperationStop() override;
  void OnLostFocus() override;
  bool OnMousePressed(const ui::MouseEvent& event) override;
  bool OnMouseDragged(const ui::MouseEvent& event) override;
  bool OnMouseReleased(const ui::MouseEvent& event) override;
  void OnMouseMoved(const ui::MouseEvent& event) override;

  void UpdateControllerObjectPos();
  void UpdateControllerObjectState(const gfx::Point& pt);
 private:
  void GetPickingPosOffset(const azer::Ray& r, azer::Vector3* pos);
  scoped_ptr<TranslationControllerObject> object_;
  azer::Vector3 pos_offset_;
  int32 picking_object_;
  bool dragging_;
  DISALLOW_COPY_AND_ASSIGN(TranslationController);
};

class TransformAxisObject {
 public:
  TransformAxisObject(DiffuseEffect* effect);
  ~TransformAxisObject();

  void set_length(float length);
  float length() const { return length_;}
  void reset_color();
  void set_color(const azer::Vector4& color, int32 axis);
  void SetPosition(const azer::Vector3& position) { position_ = position;}
  int32 Picking(const azer::Ray& ray, const azer::Camera& camera) const;

  void Render(const azer::Matrix4& pv, azer::Renderer* renderer);
 private:
  void CreatePlane(azer::VertexDesc* desc);
  void CreatePlaneFrame(azer::VertexDesc* desc);
  float length_;
  azer::EntityPtr plane_frame_;
  azer::EntityPtr plane_;
  azer::Vector3 position_;
  azer::Matrix4 rotation_[3];
  azer::Vector4 color_[3];
  DiffuseEffectPtr effect_;
  DISALLOW_COPY_AND_ASSIGN(TransformAxisObject);
};

class TranslationControllerObject {
 public:
  TranslationControllerObject();
  ~TranslationControllerObject();

  float length() { return plane_->length();}
  void set_length(float length);
  const azer::Vector3& position() const;
  void SetPosition(const azer::Vector3& position);
  void SetSelectedColor(const azer::Vector4& color) { selected_color_ = color;}
  void Render(const azer::Matrix4& pv, azer::Renderer* renderer);

  void set_selected_axis(int32 axis);
  void set_selected_plane(int32 axis);
  void reset_selected();
  bool has_selected() const {
    return is_xaxis_selected()
        || is_yaxis_selected()
        || is_zaxis_selected()
        || is_xyplane_selected()
        || is_yzplane_selected()
        || is_zxplane_selected();
  }
  bool is_xaxis_selected() const { return selected_axis_[0];}
  bool is_yaxis_selected() const { return selected_axis_[1];}
  bool is_zaxis_selected() const { return selected_axis_[2];}
  bool is_xyplane_selected() const { return selected_plane_[0];}
  bool is_yzplane_selected() const { return selected_plane_[1];}
  bool is_zxplane_selected() const { return selected_plane_[2];}

  int32 Picking(const azer::Ray& ray, const azer::Camera& camera) const;
  int32 UpdatePicking(const azer::Ray& ray, const azer::Camera& camera);
 private:
  void SetSelectedAxis(int32 axis);
  scoped_ptr<TransformAxisObject> plane_;
  scoped_ptr<XYZAxisObject> axis_;
  bool selected_axis_[3];
  bool selected_plane_[3];
  azer::Vector4 selected_color_;
  DISALLOW_COPY_AND_ASSIGN(TranslationControllerObject);
};
}
