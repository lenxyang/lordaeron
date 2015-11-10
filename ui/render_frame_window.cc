#include "lordaeron/ui/render_frame_window.h"

#include "ui/views/layout/layout_manager.h"
#include "lordaeron/context.h"

namespace lord {
RenderFrameWindow::RenderFrameWindow(const gfx::Rect& init_bounds,
                                     scoped_ptr<nelf::RenderDelegate> delegate) 
    : nelf::MainFrame(init_bounds, Context::instance()->GetNelfContext()),
      delegate_(delegate.Pass()),
      render_view_(NULL) {
  InitRenderView();
}

RenderFrameWindow::RenderFrameWindow(const gfx::Rect& init_bounds, 
                                     scoped_ptr<nelf::RenderDelegate> delegate,
                                     nelf::RenderLoop* render_loop)
    : nelf::MainFrame(init_bounds, Context::instance()->GetNelfContext()),
      delegate_(delegate.Pass()),
      render_view_(NULL),
      render_loop_(render_loop) {
  InitRenderView();
}

RenderFrameWindow::~RenderFrameWindow() {
}

void RenderFrameWindow::InitRenderView() {
  Init();
  if (!render_loop_.get()) {
    render_loop_ = new nelf::RenderLoop(this);
  }

  render_view_ = new nelf::RenderView(render_loop_.get(), delegate_.get());
  SetLayoutManager(new views::FillLayout);
  AddChildView(render_view_);
  render_loop_->AddRenderView(render_view_);
}
}  // namespace lord
