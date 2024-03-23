#include "Animation.h"

sw::Animation::Animation()
    : Center(
          // get
          [&]() -> const bool & {
              static bool result;
              result = this->GetStyle(ACS_CENTER);
              return result;
          },
          // set
          [&](const bool &value) {
              this->SetStyle(ACS_CENTER, value);
          }),

      AutoPlay(
          // get
          [&]() -> const bool & {
              static bool result;
              result = this->GetStyle(ACS_AUTOPLAY);
              return result;
          },
          // set
          [&](const bool &value) {
              this->SetStyle(ACS_AUTOPLAY, value);
          }),

      IsPlaying(
          // get
          [&]() -> const bool & {
              static bool result;
              result = this->SendMessageW(ACM_ISPLAYING, 0, 0);
              return result;
          })
{
    this->InitControl(ANIMATE_CLASSW, L"", WS_CHILD | WS_VISIBLE, 0);
    this->Rect = {0, 0, 200, 200};
}

bool sw::Animation::Open(HINSTANCE hInstance, int resourceId)
{
    return this->SendMessageW(ACM_OPENW, reinterpret_cast<WPARAM>(hInstance), reinterpret_cast<LPARAM>(MAKEINTRESOURCEW(resourceId)));
}

bool sw::Animation::Open(const std::wstring &fileName)
{
    return this->Open(fileName.c_str());
}

bool sw::Animation::Open(const wchar_t *fileName)
{
    return this->SendMessageW(ACM_OPENW, 0, reinterpret_cast<LPARAM>(fileName));
}

bool sw::Animation::Play(int times, int beginFrame, int endFrame)
{
    return this->SendMessageW(ACM_PLAY, (WPARAM)times, MAKELPARAM(beginFrame, endFrame));
}

bool sw::Animation::Play(int times)
{
    return this->Play(times, 0, -1);
}

bool sw::Animation::Stop()
{
    return this->SendMessageW(ACM_STOP, 0, 0);
}
