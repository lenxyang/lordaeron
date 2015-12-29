#pragma once

#include "base/memory/scoped_ptr.h"
#include "base/observer_list.h"
#include "azer/render/render.h"
#include "nelf/render/event_listener.h"

namespace lord {
class InteractiveController;
class InteractiveContext;
class SceneNode;
class SceneNodePropertyPane;
class RenderWindow;
typedef scoped_refptr<SceneNode> SceneNodePtr;

class InteractiveContextObserver {
 public:
  virtual void OnSceneNodeSelected(InteractiveContext* context, SceneNode* prevsel) {}
};

class InteractiveContext : public nelf::EventListener {
 public:
  InteractiveContext(RenderWindow* win, SceneNode* root);
  ~InteractiveContext();
  
  void ResetController();
  void SetController(scoped_ptr<InteractiveController> controller);

  void Update(const azer::FrameArgs& args);
  void Render(azer::Renderer* renderer);

  SceneNode* root();
  RenderWindow* window() { return window_;}

  void SetPickingNode(SceneNode* node);
  SceneNode* GetPickingNode() { return picking_node_;}
  SceneNode* GetObjectFromLocation(const gfx::Point& point);
  azer::Ray GetPickingRay(const gfx::Point& pt);
  azer::Vector4 CalcWorldPosFromScreen(const gfx::Point& pt);

  void AddObserver(InteractiveContextObserver* observer);
  void RemoveObserver(InteractiveContextObserver* observer);
  bool HasObserver(InteractiveContextObserver* observer) const;

  // override from nelf::EventListener
  void OnKeyPressed(const ui::KeyEvent& event) override;
  void OnKeyReleased(const ui::KeyEvent& event) override;
  void OnMousePressed(const ui::MouseEvent& event) override;
  void OnMouseDragged(const ui::MouseEvent& event) override;
  void OnMouseReleased(const ui::MouseEvent& event) override;;
  void OnMouseMoved(const ui::MouseEvent& event) override;
  void OnMouseCaptureLost() override;
  void OnLostFocus() override;
 private:
  SceneNodePtr root_;
  RenderWindow* window_;
  SceneNode* picking_node_;
  scoped_ptr<InteractiveController> controller_;
  ObserverList<InteractiveContextObserver> observers_;
  DISALLOW_COPY_AND_ASSIGN(InteractiveContext);
};
}  // namespace lord
