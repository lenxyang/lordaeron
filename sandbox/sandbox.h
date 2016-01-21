#pragma once

#include "base/strings/utf_string_conversions.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "ui/views/widget/widget_observer.h"

#include "azer/base/native_file_system.h"
#include "azer/render/util.h"
#include "nelf/nelf.h"
#include "nelf/gfx_util.h"
#include "nelf/res/grit/common.h"

#include "lordaeron/res/grit/common.h"
#include "lordaeron/env.h"
#include "lordaeron/effect/diffuse_effect.h"
#include "lordaeron/interactive/interactive_context.h"
#include "lordaeron/scene/ui_scene_render.h"
#include "lordaeron/scene/render_env_node.h"
#include "lordaeron/scene/render_node.h"
#include "lordaeron/scene/scene_node.h"
#include "lordaeron/resource/resource_loader.h"
#include "lordaeron/resource/resource_util.h"
#include "lordaeron/resource/light_loader.h"
#include "lordaeron/resource/scene_loader.h"
#include "lordaeron/resource/mesh_load_util.h"
#include "lordaeron/ui/light_property_pane.h"
#include "lordaeron/ui/scene_node_inspector_pane.h"
#include "lordaeron/ui/scene_tree_view.h"
#include "lordaeron/ui/toolbar/object_control_toolbar.h"
#include "lordaeron/ui/renderer_info_pane.h"
#include "lordaeron/ui/window/frame_window.h"
#include "lordaeron/ui/window/render_window.h"


using lord::SceneNodePtr;
using lord::SceneNode;
using base::FilePath;
using base::UTF8ToUTF16;
using base::UTF16ToUTF8;
