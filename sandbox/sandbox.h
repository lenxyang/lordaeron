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
#include "lordaeron/context.h"
#include "lordaeron/effect/diffuse_effect.h"
#include "lordaeron/interactive/interactive_context.h"
#include "lordaeron/mainframe_render_delegate.h"
#include "lordaeron/scene/common_node_loader.h"
#include "lordaeron/scene/lord_scene_render.h"
#include "lordaeron/scene/scene_render_tree.h"
#include "lordaeron/scene/scene_node.h"
#include "lordaeron/resource/resource_loader.h"
#include "lordaeron/resource/light_loader.h"
#include "lordaeron/resource/scene_loader.h"
#include "lordaeron/resource/mesh_load_util.h"
#include "lordaeron/ui/light_property_pane.h"
#include "lordaeron/ui/scene_node_inspector_pane.h"
#include "lordaeron/ui/scene_render_window.h"
#include "lordaeron/ui/scene_tree_view.h"
#include "lordaeron/ui/simple_render_window.h"
#include "lordaeron/ui/toolbar/object_control_toolbar.h"
#include "lordaeron/ui/renderer_info_pane.h"


using lord::SceneNodePtr;
using lord::SceneNode;
using base::FilePath;
using base::UTF8ToUTF16;
using base::UTF16ToUTF8;
