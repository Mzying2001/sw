#include "StatusBar.h"
#include "Utils.h"

sw::StatusBar::StatusBar()
    : SizingGrip(
          // get
          [&]() -> const bool & {
              static bool result;
              result = this->GetStyle(SBARS_SIZEGRIP);
              return result;
          },
          // set
          [&](const bool &value) {
              if (this->SizingGrip != value) {
                  auto style   = this->GetStyle();
                  auto exstyle = this->GetExtendedStyle();
                  this->ResetHandle(value ? (style | SBARS_SIZEGRIP) : (style & ~SBARS_SIZEGRIP), exstyle);
              }
          }),

      PartsCount(
          // get
          [&]() -> const int & {
              static int result;
              result = (int)this->SendMessageW(SB_GETPARTS, 0, 0);
              return result;
          }),

      UseUnicode(
          // get
          [&]() -> const bool & {
              static bool result;
              result = this->SendMessageW(SB_GETUNICODEFORMAT, 0, 0);
              return result;
          },
          // set
          [&](const bool &value) {
              this->SendMessageW(SB_SETUNICODEFORMAT, value, 0);
          })
{
    this->InitControl(STATUSCLASSNAMEW, L"", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | CCS_NORESIZE, 0);
    this->Height = 25;
    this->SetAlignment(HorizontalAlignment::Stretch, VerticalAlignment::Bottom);
    this->Control::SetBackColor(KnownColor::Control, false);
}

bool sw::StatusBar::SetParts(std::initializer_list<double> parts)
{
    int count = (int)parts.size();

    if (count <= 0) {
        int tmp = -1;
        this->SendMessageW(SB_SETPARTS, 1, reinterpret_cast<LPARAM>(&tmp));
    }

    std::vector<int> vec;
    vec.reserve(count);

    int right = 0;
    for (double item : parts) {
        if (item == -1) {
            vec.push_back(-1);
            break;
        } else {
            right += Utils::Max(0, Dip::DipToPxX(item));
            vec.push_back(right);
            right += 2; // 分隔条
        }
    }

    return this->SendMessageW(SB_SETPARTS, vec.size(), reinterpret_cast<LPARAM>(vec.data()));
}

bool sw::StatusBar::GetTextAt(uint8_t index, std::wstring &out)
{
    int len = LOWORD(this->SendMessageW(SB_GETTEXTLENGTHW, index, 0));
    if (len <= 0) return false;

    out.resize(len + 1);
    this->SendMessageW(SB_GETTEXTW, index, reinterpret_cast<LPARAM>(&out[0]));
    out.resize(len);
    return true;
}

bool sw::StatusBar::SetTextAt(uint8_t index, const std::wstring &text)
{
    return this->SendMessageW(SB_SETTEXTW, index, reinterpret_cast<LPARAM>(text.c_str()));
}

bool sw::StatusBar::GetRectAt(uint8_t index, sw::Rect &out)
{
    RECT rect;
    if (this->SendMessageW(SB_GETRECT, index, reinterpret_cast<LPARAM>(&rect))) {
        out = rect;
        return true;
    }
    return false;
}

void sw::StatusBar::SetBackColor(Color color, bool redraw)
{
    this->Control::SetBackColor(color, false);
    this->SendMessageW(SB_SETBKCOLOR, 0, (COLORREF)color);
}
