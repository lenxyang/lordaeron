#pragma once

#include "lordaeron/effect/diffuse_effect.h"
#include "lordaeron/interactive/interactive_controller.h"

namespace lord {
class AxisObject;
class InteractiveContext;
class SceneNode;
class TranslationControllerObject;

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
  scoped_ptr<TranslationControllerObject> object_;
  azer::Vector3 origin_position_;
  bool dragging_;
  DISALLOW_COPY_AND_ASSIGN(TranslationController);
};

class TransformAxisObject {
 public:
  TransformAxisObject(azer::VertexDesc* desc);
  ~TransformAxisObject();
  
  void set_length(float length);
  float length() const { return length_;}
  void RenderAxis(azer::Renderer* renderer);
  void RenderPlane(azer::Renderer* renderer);
 private:
  void CreatePlane(azer::VertexDesc* desc);
  void CreatePlaneFrame(azer::VertexDesc* desc);
  float length_;
  azer::EntityPtr plane_frame_;
  azer::EntityPtr plane_;
  scoped_ptr<AxisObject> axis_;
  azer::VertexDescPtr desc_;
  DISALLOW_COPY_AND_ASSIGN(TransformAxisObject);
};

class TranslationControllerObject {
 public:
  TranslationControllerObject();
  ~TranslationControllerObject();

  float length() { return axis_->length();}
  void set_length(float length);
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
 private:
  void SetSelectedAxis(int32 axis);
  scoped_ptr<TransformAxisObject> axis_;
  bool selected_axis_[3];
  bool selected_plane_[3];
  azer::Vector4 selected_color_;
  azer::Vector4 color_[3];
  azer::Matrix4 world_;
  azer::Matrix4 rotation_[3];
  DiffuseEffectPtr effect_;
  DISALLOW_COPY_AND_ASSIGN(TranslationControllerObject);
};
}
