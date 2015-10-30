#include "lordaeron/interactive/fps_camera_controller.h"

#include "azer/render/render.h"
#include "lordaeron/scene/scene_node.h"

namespace lord {
using namespace azer;

FPSCameraController::FPSCameraController() 
    : context_(NULL),
      camera_(NULL) {
  orientation_dragging_ = false;
  ResetState();
}

FPSCameraController::~FPSCameraController() {
}

void FPSCameraController::ResetState() {
  posx_ = 0;
  posz_ = 0;
  posy_ = 0;
  negx_ = 0;
  negz_ = 0;
  negy_ = 0;
}

void FPSCameraController::OnOperationStart(InteractiveContext* ctx)  {
  DCHECK(!context_);
  context_ = ctx;
}

void FPSCameraController::OnOperationStop() {
  DCHECK(context_);
  context_ = NULL;
  camera_ = NULL;
}

void OnLostFocus()  {
}

bool FPSCameraController::OnKeyPressed(const ui::KeyEvent& event) {
  if (event.key_code() == ui::VKEY_W) {
    posz_ = 1;
    return true;
  } else if (event.key_code() == ui::VKEY_S) {
    negz_ = 1;
    return true;
  } else if (event.key_code() == ui::VKEY_A) {
    negx_ = 1;
    return true;
  } else if (event.key_code() == ui::VKEY_D) {
    posx_ = 1;
    return true;
  } else if (event.key_code() == ui::VKEY_F) {
    posy_ = 1;
    return true;
  } else if (event.key_code() == ui::VKEY_R) {
    negy_ = 1;
    return true;
  } else {
    return false;
  }
}

bool FPSCameraController::OnKeyReleased(const ui::KeyEvent& event) {
  if (event.key_code() == ui::VKEY_W) {
    posz_ = 0;
    return true;
  } else if (event.key_code() == ui::VKEY_S) {
    negz_ = 0;
    return true;
  } else if (event.key_code() == ui::VKEY_A) {
    negx_ = 0;
    return true;
  } else if (event.key_code() == ui::VKEY_D) {
    posx_ = 0;
    return true;
  } else if (event.key_code() == ui::VKEY_F) {
    posy_ = 0;
    return true;
  } else if (event.key_code() == ui::VKEY_R) {
    negy_ = 0;
    return true;
  } else {
    return false;
  }
}

bool FPSCameraController::OnMousePressed(const ui::MouseEvent& event) {
  location_ = event.location();
  if (event.IsLeftMouseButton() && event.GetClickCount() == 1) {
    orientation_dragging_ = true;
    return true;
  } else {
    return false;
  }
}

bool FPSCameraController::OnMouseDragged(const ui::MouseEvent& event) {
  if (orientation_dragging_) {
    RotateCamera(location_, event.location());
    return true;
  } else {
    return false;
  }
}

bool FPSCameraController::OnMouseReleased(const ui::MouseEvent& event) {
  if (orientation_dragging_) {
    RotateCamera(location_, event.location());
    orientation_dragging_ = false;
    return true;
  } else {
    return false;
  }
}

void FPSCameraController::RotateCamera(const gfx::Point& prev, 
                                       const gfx::Point& cur) {
  TransformHolder* holder = camera_->mutable_holder();
  Degree to_yaw = Degree(cur.x() - prev.x()) * 0.1;
  // holder->yaw(to_yaw);
  holder->rotate(azer::Vector3(0.0f, 1.0f, 0.0f), to_yaw);
  Degree to_pitch = Degree(cur.y() - prev.y()) * 0.1;
  holder->pitch(to_pitch);
}

void FPSCameraController::Update(const azer::FrameArgs& args) {
  DCHECK(camera_);
  float unit = args.delta().InSecondsF() * 32.0f;
  TransformHolder* holder = camera_->mutable_holder();
  holder->strafe((posx_ - negx_) * unit);
  holder->walk((posz_ - negz_) * unit);
  holder->fly((posy_ - negy_)* unit);
  camera_->Update();
}
}  // namespace lord
