#include "lordaeron/ui/controls/vector_control.h"

#include "base/strings/utf_string_conversions.h"
#include "base/strings/stringprintf.h"
#include "base/strings/string_number_conversions.h"

namespace lord {
using base::UTF8ToUTF16;

namespace {
bool StringToFloat(const base::string16& text, float* f) {
  double d;
  bool ret = ::base::StringToDouble(UTF16ToUTF8(text), &d);
  *f = static_cast<float>(d);
  return ret;
}
}

const char VectorControl::kViewClassName[] = "lord::VectorControl";
VectorControl::VectorControl(azer::Vector2* vec)
    : vec2_(vec), vec3_(NULL), vec4_(NULL), quaternion_(NULL),
      type_(kTypeVector2) {
  InitUI();
  UpdateUIFromData();
}

VectorControl::VectorControl(azer::Vector3* vec) 
    : vec2_(NULL), vec3_(vec), vec4_(NULL), quaternion_(NULL),
      type_(kTypeVector3) {
  InitUI();
  UpdateUIFromData();
}

VectorControl::VectorControl(azer::Vector4* vec)
    : vec2_(NULL), vec3_(NULL), vec4_(vec), quaternion_(NULL),
      type_(kTypeVector4) {
  InitUI();
  UpdateUIFromData();
}

VectorControl::VectorControl(azer::Quaternion* q) 
    : vec2_(NULL), vec3_(NULL), vec4_(NULL), quaternion_(q),
      type_(kTypeQuaternion) {
  InitUI();
  UpdateUIFromData();
}

const char* VectorControl::GetClassName() const {
  return kViewClassName;
}

gfx::Size VectorControl::GetPreferredSize() const {
  switch (type_) {
    case kTypeVector2:
      return gfx::Size(90, 28);
    case kTypeVector3: 
      return gfx::Size(140, 28);
    case kTypeVector4:
    case kTypeQuaternion:
      return gfx::Size(180, 28);
    default:
      CHECK(false);
      return gfx::Size(180, 28);
  }
}

void VectorControl::Layout() {
  int32 count = 0;
  switch (type_) {
    case kTypeVector2: count = 2; break;
    case kTypeVector3: count = 3; break;
    case kTypeVector4:
    case kTypeQuaternion:
    case kTypeVector3: count = 4; break;
    default: CHECK(false);
  }

  const int32 kTextfieldWidth = 40;
  const int32 kLabelWidth = 20;
  const int32 kLabelSpacing = 3;
  const int32 kHorzMargin = 10;
  const int32 kVertMargin = 5;
  const int32 kControlHeight = 24;
  int x = 0;
  for (int i = 0; i < count; ++i) {
    x += kHorzMargin;
    label_[i]->SetRect(x, kVertMargin, kLabelWidth, kControlHeight);
    x += kLabelWidth;
    x += kLabelSpacing;
    textfield_[i]->SetRect(x, kVertMargin, kTextfieldWidth, kControlHeight);
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
  }
}

void VectorControl::UpdateUIFromData() {
  switch (type_) {
    case kTypeVector2: {
      DCHECK(vec2);
      textfield_[0]->SetText(UTF8ToUTF16(StringPrintf("%f", vec2_->x)));
      textfield_[1]->SetText(UTF8ToUTF16(StringPrintf("%f", vec2_->y)));
      break;
    }
    case kTypeVector3: {
      DCHECK(vec3);
      textfield_[0]->SetText(UTF8ToUTF16(StringPrintf("%f", vec3_->x)));
      textfield_[1]->SetText(UTF8ToUTF16(StringPrintf("%f", vec3_->y)));
      textfield_[2]->SetText(UTF8ToUTF16(StringPrintf("%f", vec3_->z)));
      break;
    }
    case kTypeVector4: {
      DCHECK(vec4);
      textfield_[0]->SetText(UTF8ToUTF16(StringPrintf("%f", vec4_->x)));
      textfield_[1]->SetText(UTF8ToUTF16(StringPrintf("%f", vec4_->y)));
      textfield_[2]->SetText(UTF8ToUTF16(StringPrintf("%f", vec4_->z)));
      textfield_[3]->SetText(UTF8ToUTF16(StringPrintf("%f", vec4_->w)));
      break;
    }
    case kTypeQuaternion: {
      DCHECK(quaternion_);
      textfield_[0]->SetText(UTF8ToUTF16(StringPrintf("%f", quaternion_->w)));
      textfield_[1]->SetText(UTF8ToUTF16(StringPrintf("%f", quaternion_->x)));
      textfield_[2]->SetText(UTF8ToUTF16(StringPrintf("%f", quaternion_->y)));
      textfield_[3]->SetText(UTF8ToUTF16(StringPrintf("%f", quaternion_->z)));
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
      DCHECK(vec2);
      CHECK(StringToFloat(textfield_[0]->text(), &vec2_->x));
      CHECK(StringToFloat(textfield_[1]->text(), &vec2_->y));
      break;
    }
    case kTypeVector3: {
      DCHECK(vec3);
      CHECK(StringToFloat(textfield_[0]->text(), &vec3_->x));
      CHECK(StringToFloat(textfield_[1]->text(), &vec3_->y));
      CHECK(StringToFloat(textfield_[2]->text(), &vec3_->z));
      break;
    }
    case kTypeVector4: {
      DCHECK(vec4);
      CHECK(StringToFloat(textfield_[0]->text(), &vec4_->x));
      CHECK(StringToFloat(textfield_[1]->text(), &vec4_->y));
      CHECK(StringToFloat(textfield_[2]->text(), &vec4_->z));
      CHECK(StringToFloat(textfield_[4]->text(), &vec4_->w));
      break;
    }
    case kTypeQuaternion: {
      DCHECK(quaternion_);
      CHECK(StringToFloat(textfield_[0]->text(), &quaternion->w));
      CHECK(StringToFloat(textfield_[1]->text(), &quaternion->x));
      CHECK(StringToFloat(textfield_[2]->text(), &quaternion->y));
      CHECK(StringToFloat(textfield_[3]->text(), &quaternion->z));
      break;
    }
    default:
      CHECK(false);
      break;
  }
}

void VectorControl::ContentsChanged(views::Textfield* sender,
                                    const base::string16& new_contents) {
  float t;
  if (!StringToFloat(new_contents, &f)) {
    sender->SetText(sender->text());
  }
}
}  // namespace lord
