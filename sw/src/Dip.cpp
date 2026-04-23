#include "Dip.h"
#include <cmath>

#if !defined(USER_DEFAULT_SCREEN_DPI)
#define USER_DEFAULT_SCREEN_DPI 96
#endif

namespace
{
    /**
     * @brief 内部类，储存缩放信息
     */
    struct _ScaleInfo {
        /**
         * @brief 横向缩放比例
         */
        double scaleX;

        /**
         * @brief 纵向缩放比例
         */
        double scaleY;

        /**
         * @brief 构造函数，根据系统DPI计算缩放比例
         */
        _ScaleInfo() noexcept
        {
            HDC hdc = GetDC(NULL);
            if (hdc == NULL) {
                scaleX = 1;
                scaleY = 1;
            } else {
                scaleX = static_cast<double>(USER_DEFAULT_SCREEN_DPI) / GetDeviceCaps(hdc, LOGPIXELSX);
                scaleY = static_cast<double>(USER_DEFAULT_SCREEN_DPI) / GetDeviceCaps(hdc, LOGPIXELSY);
                ReleaseDC(NULL, hdc);
            }
        }
    };

    /**
     * @brief 储存缩放信息
     */
    thread_local _ScaleInfo _scaleInfo;
}

/*================================================================================*/

const sw::ReadOnlyProperty<double> sw::Dip::ScaleX(
    Property<double>::Init()
        .Getter([]() -> double {
            return _scaleInfo.scaleX;
        }) //
);

const sw::ReadOnlyProperty<double> sw::Dip::ScaleY(
    Property<double>::Init()
        .Getter([]() -> double {
            return _scaleInfo.scaleY;
        }) //
);

void sw::Dip::Update(int dpiX, int dpiY) noexcept
{
    _scaleInfo.scaleX = static_cast<double>(USER_DEFAULT_SCREEN_DPI) / dpiX;
    _scaleInfo.scaleY = static_cast<double>(USER_DEFAULT_SCREEN_DPI) / dpiY;
}

double sw::Dip::PxToDipX(int px) noexcept
{
    return px * _scaleInfo.scaleX;
}

double sw::Dip::PxToDipY(int px) noexcept
{
    return px * _scaleInfo.scaleY;
}

int sw::Dip::DipToPxX(double dip) noexcept
{
    return (int)std::lround(dip / _scaleInfo.scaleX);
}

int sw::Dip::DipToPxY(double dip) noexcept
{
    return (int)std::lround(dip / _scaleInfo.scaleY);
}
