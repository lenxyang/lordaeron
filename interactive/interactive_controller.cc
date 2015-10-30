#include "lordaeron/interactive/interactive_controller.h"

#include "base/logging.h"

namespace lord {
InteractiveController::InteractiveController() 
    : context_(NULL) {
}

InteractiveController::~InteractiveController() {
}

void InteractiveController::Start(InteractiveContext* ctx) {
  context_ = ctx;
  OnOperationStart(ctx);
}

void InteractiveController::Stop() {
  context_ = NULL;
}

}  // namespace lord
