#include "Animation.h"

sw::Animation::Animation()
    : Center(
          // get
          [this]() -> bool {
              return this->GetStyle(ACS_CENTER);
          },
          // set
          [this](const bool &value) {
              this->SetStyle(ACS_CENTER, value);
          }),

      AutoPlay(
          // get
          [this]() -> bool {
              return this->GetStyle(ACS_AUTOPLAY);
          },
          // set
          [this](const bool &value) {
              this->SetStyle(ACS_AUTOPLAY, value);
          }),

      IsPlaying(
          // get
          [this]() -> bool {
              return this->SendMessageW(ACM_ISPLAYING, 0, 0);
          })
{
    this->InitControl(ANIMATE_CLASSW, L"", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, 0);
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
