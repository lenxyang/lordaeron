#include "lordaeron/ui/controls/vector_control.h"

#include "base/strings/utf_string_conversions.h"
#include "base/strings/stringprintf.h"
#include "base/strings/string_number_conversions.h"

namespace lord {
using base::UTF8ToUTF16;
using base::UTF16ToUTF8;
using base::StringPrintf;

namespace {
const int32 kTextfieldWidth = 50;
const int32 kLabelWidth = 10;
const int32 kLabelSpacing = 1;
const int32 kHorzMargin = 1;
const int32 kVertMargin = 1;
const int32 kControlHeight = 18;
bool StringToFloat(const base::string16& text, float* f) {
  double d;
  bool ret = ::base::StringToDouble(UTF16ToUTF8(text), &d);
  *f = static_cast<float>(d);
  return ret;
}
}

const char VectorControl::kViewClassName[] = "lord::VectorControl";
VectorControl::VectorControl(const azer::Vector2& vec)
    : vec2_(vec),
      delegate_(NULL),
      type_(kTypeVector2) {
  InitUI();
  UpdateUIFromData();
}

VectorControl::VectorControl(const azer::Vector3& vec) 
    : vec3_(vec), 
      delegate_(NULL),
      type_(kTypeVector3) {
  InitUI();
  UpdateUIFromData();
}

VectorControl::VectorControl(const azer::Vector4& vec)
    : vec4_(vec),
      delegate_(NULL),
      type_(kTypeVector4) {
  InitUI();
  UpdateUIFromData();
}

VectorControl::VectorControl(const azer::Quaternion& q) 
    : quaternion_(q),
      delegate_(NULL),
      type_(kTypeQuaternion) {
  InitUI();
  UpdateUIFromData();
}

void VectorControl::set_delegate(VectorControlDelegate* delegate) {
  DCHECK(!delegate_);
  delegate_ = delegate;
}

const char* VectorControl::GetClassName() const {
  return kViewClassName;
}

void VectorControl::SetReadOnly(bool readonly) {
  for (int32 i = 0; i < arraysize(textfield_); ++i) {
    textfield_[i]->SetReadOnly(readonly);
  }
}

bool VectorControl::read_only() const {
  return textfield_[0]->read_only();
}

gfx::Size VectorControl::GetPreferredSize() const {
  const int32 height = kControlHeight + kVertMargin * 2;
  const int32 cell_width = (kLabelWidth + kTextfieldWidth);
  switch (type_) {
    case kTypeVector2:
      return gfx::Size(cell_width * 2 + kHorzMargin, height);
    case kTypeVector3: 
      return gfx::Size(cell_width * 3 + kHorzMargin * 2, height);
    case kTypeVector4:
    case kTypeQuaternion:
      return gfx::Size(cell_width * 4 + kHorzMargin * 3, height);
    default:
      CHECK(false);
      return gfx::Size(280, height);
  }
}

void VectorControl::Layout() {
  int32 count = 0;
  switch (type_) {
    case kTypeVector2: count = 2; break;
    case kTypeVector3: count = 3; break;
    case kTypeVector4:
    case kTypeQuaternion:
      count = 4; break;
    default: CHECK(false);
  }

  int x = 0;
  for (int i = 0; i < count; ++i) {
    x += kHorzMargin;
    label_[i]->SetBounds(x, kVertMargin, kLabelWidth, kControlHeight);
    x += kLabelWidth;
    x += kLabelSpacing;
    textfield_[i]->SetBounds(x, kVertMargin, kTextfieldWidth, kControlHeight);
    x += kTextfieldWidth;
  }
}

base::string16 VectorControl::label_text(int32 i) const {
  switch (type_) {
    case kTypeVector2: {
      const char* labels[] = {"x", "y", "", ""};
      return ::base::UTF8ToUTF16(labels[i]);
    }
    case kTypeVector3: {
      const char* labels[] = {"x", "y", "z", ""};
      return ::base::UTF8ToUTF16(labels[i]);
    }
    case kTypeVector4: {
      const char* labels[] = {"x", "y", "z", "w"};
      return ::base::UTF8ToUTF16(labels[i]);
    }
    case kTypeQuaternion: {
      const char* labels[] = {"w", "x", "y", "z"};
      return ::base::UTF8ToUTF16(labels[i]);
    }
    default:
      CHECK(false);
      return ::base::UTF8ToUTF16("");
  }
}

void VectorControl::InitUI() {
  for (int32 i = 0; i < arraysize(label_); ++i) {
    label_[i] = new views::Label(label_text(i));
    AddChildView(label_[i]);
    textfield_[i] = new views::Textfield;
    textfield_[i]->set_controller(this);
    AddChildView(textfield_[i]);
  }
}

void VectorControl::UpdateUIFromData() {
  switch (type_) {
    case kTypeVector2: {
      textfield_[0]->SetText(UTF8ToUTF16(StringPrintf("%.4f", vec2_.x)));
      textfield_[1]->SetText(UTF8ToUTF16(StringPrintf("%.4f", vec2_.y)));
      break;
    }
    case kTypeVector3: {
      textfield_[0]->SetText(UTF8ToUTF16(StringPrintf("%.4f", vec3_.x)));
      textfield_[1]->SetText(UTF8ToUTF16(StringPrintf("%.4f", vec3_.y)));
      textfield_[2]->SetText(UTF8ToUTF16(StringPrintf("%.4f", vec3_.z)));
      break;
    }
    case kTypeVector4: {
      textfield_[0]->SetText(UTF8ToUTF16(StringPrintf("%.4f", vec4_.x)));
      textfield_[1]->SetText(UTF8ToUTF16(StringPrintf("%.4f", vec4_.y)));
      textfield_[2]->SetText(UTF8ToUTF16(StringPrintf("%.4f", vec4_.z)));
      textfield_[3]->SetText(UTF8ToUTF16(StringPrintf("%.4f", vec4_.w)));
      break;
    }
    case kTypeQuaternion: {
      textfield_[0]->SetText(UTF8ToUTF16(StringPrintf("%.4f", quaternion_.w)));
      textfield_[1]->SetText(UTF8ToUTF16(StringPrintf("%.4f", quaternion_.x)));
      textfield_[2]->SetText(UTF8ToUTF16(StringPrintf("%.4f", quaternion_.y)));
      textfield_[3]->SetText(UTF8ToUTF16(StringPrintf("%.4f", quaternion_.z)));
      break;
    }
    default:
      CHECK(false);
      break;
  }
}

void VectorControl::UpdateDataFromUI() {
  switch (type_) {
    case kTypeVector2: {
      CHECK(StringToFloat(textfield_[0]->text(), &vec2_.x));
      CHECK(StringToFloat(textfield_[1]->text(), &vec2_.y));
      break;
    }
    case kTypeVector3: {
      CHECK(StringToFloat(textfield_[0]->text(), &vec3_.x));
      CHECK(StringToFloat(textfield_[1]->text(), &vec3_.y));
      CHECK(StringToFloat(textfield_[2]->text(), &vec3_.z));
      break;
    }
    case kTypeVector4: {
      CHECK(StringToFloat(textfield_[0]->text(), &vec4_.x));
      CHECK(StringToFloat(textfield_[1]->text(), &vec4_.y));
      CHECK(StringToFloat(textfield_[2]->text(), &vec4_.z));
      CHECK(StringToFloat(textfield_[4]->text(), &vec4_.w));
      break;
    }
    case kTypeQuaternion: {
      CHECK(StringToFloat(textfield_[0]->text(), &quaternion_.w));
      CHECK(StringToFloat(textfield_[1]->text(), &quaternion_.x));
      CHECK(StringToFloat(textfield_[2]->text(), &quaternion_.y));
      CHECK(StringToFloat(textfield_[3]->text(), &quaternion_.z));
      break;
    }
    default:
      CHECK(false);
      break;
  }

  if (delegate_) {
    delegate_->OnVectorChanged(this);
  }
}

void VectorControl::ContentsChanged(views::Textfield* sender,
                                    const base::string16& new_contents) {
  float f;
  if (!StringToFloat(new_contents, &f)) {
    sender->SetText(sender->text());
    UpdateDataFromUI();
  }
}
}  // namespace lord
