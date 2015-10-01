#pragma once

#include "base/memory/scoped_ptr.h"
#include "nelf/nelf.h"
#include "lordaeron/context.h"

namespace lord {
class RenderFrameWindow : public nelf::Mainframe {
 public:
  RenderFrameWindow(const gfx::Rect& init_bounds,
                    scoped_ptr<nelf::RenderDelegate> delegate);
  RenderFrameWindow(const gfx::Rect& init_bounds,
                    scoped_ptr<nelf::RenderDelegate> delegate,
                    nelf::RenderLoop* render_loop);
  ~RenderFrameWindow();

  nelf::RenderLoop* GetRenderLoop() { return render_loop_.get();}
 private:
  void InitRenderView();
  scoped_ptr<nelf::RenderDelegate> delegate_;
  nelf::RenderView* render_view_;
  scoped_refptr<nelf::RenderLoop> render_loop_;
  DISALLOW_COPY_AND_ASSIGN(RenderFrameWindow);
};
}  // namespace lord
