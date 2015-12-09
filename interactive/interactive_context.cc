#include "lordaeron/interactive/interactive_context.h"

#include "azer/math/ray.h"
#include "lordaeron/interactive/interactive_controller.h"
#include "lordaeron/scene/scene_node.h"
#include "lordaeron/scene/scene_node_picking.h"
#include "lordaeron/ui/scene_render_window.h"
#include "lordaeron/util/picking.h"

namespace lord {

using namespace azer;

InteractiveContext::InteractiveContext(SceneRenderWindow* window) 
    : window_(window),
      picking_node_(NULL) {
}

InteractiveContext::~InteractiveContext() {
  ResetController();
}

SceneNode* InteractiveContext::root() {
  return window_->root();
}

void InteractiveContext::Update(const azer::FrameArgs& args) {
  if (controller_.get())
    controller_->Update(args);
}

void InteractiveContext::Render(azer::Renderer* renderer) {
  if (controller_.get())
    controller_->Render(renderer);
}

void InteractiveContext::ResetController() {
  if (controller_.get()) {
    controller_->Stop();
    controller_.reset();
  }
}

void InteractiveContext::SetPickingNode(SceneNode* node) {
  if (node == picking_node_)
    return;

  if (picking_node_) {
    picking_node_->set_picked(false);
  }

  SceneNode* prev_sel = picking_node_;
  picking_node_ = node;
  if (picking_node_) {
    picking_node_->set_picked(true);
  }

  FOR_EACH_OBSERVER(InteractiveContextObserver, observers_,
    OnSceneNodeSelected(this, prev_sel));
}

void InteractiveContext::SetController(scoped_ptr<InteractiveController> controller) {
  ResetController();
  controller_ = controller.Pass();
  controller_->Start(this);
}

void InteractiveContext::OnKeyPressed(const ui::KeyEvent& event) {
  if (controller_.get()) {
    controller_->OnKeyPressed(event);
  }
}

void InteractiveContext::OnKeyReleased(const ui::KeyEvent& event) {
  if (controller_.get()) {
    controller_->OnKeyReleased(event);
  }
}

void InteractiveContext::OnMousePressed(const ui::MouseEvent& event) {
  if (controller_.get()) {
    controller_->OnMousePressed(event);
  }
}

void InteractiveContext::OnMouseDragged(const ui::MouseEvent& event) {
  if (controller_.get()) {
    controller_->OnMouseDragged(event);
  }
}

void InteractiveContext::OnMouseReleased(const ui::MouseEvent& event) {
  if (controller_.get()) {
    controller_->OnMouseReleased(event);
  }
}

void InteractiveContext::OnMouseMoved(const ui::MouseEvent& event) {
  if (controller_.get()) {
    controller_->OnMouseMoved(event);
  }
}

void InteractiveContext::OnMouseCaptureLost() {
  if (controller_.get()) {
    controller_->OnLostFocus();
  }
}

void InteractiveContext::OnLostFocus() {
  if (controller_.get()) {
    controller_->OnLostFocus();
  }
}

SceneNode* InteractiveContext::GetObjectFromLocation(const gfx::Point& point) {
  azer::Ray ray = GetPickingRay(point);
  SceneNodePickHelper helper(&ray);
  SceneNodeTraverse traverser(&helper);
  traverser.Traverse(root());
  return helper.GetPickingNode();
}

Vector4 InteractiveContext::CalcWorldPosFromScreen(const gfx::Point& pt) {
  const Camera& camera = window()->camera();
  const gfx::Size size  = window()->GetContentsBounds().size();
  return std::move(lord::CalcWorldPosFromScreen(pt, size, &camera));
}

Ray InteractiveContext::GetPickingRay(const gfx::Point& pt) {
  const Camera& camera = window()->camera();
  const gfx::Size size  = window()->GetContentsBounds().size();
  return std::move(lord::GetPickingRay(pt, size, &camera));
}

void InteractiveContext::AddObserver(InteractiveContextObserver* observer) {
  observers_.AddObserver(observer);
}

void InteractiveContext::RemoveObserver(InteractiveContextObserver* observer) {
  observers_.RemoveObserver(observer);
}

bool InteractiveContext::HasObserver(InteractiveContextObserver* observer) const {
  return observers_.HasObserver(observer);
}
}  // namespace lord
