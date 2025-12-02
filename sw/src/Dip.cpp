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
        _ScaleInfo()
        {
            HDC hdc = GetDC(NULL);
            if (hdc == NULL) {
                this->scaleX = 1;
                this->scaleY = 1;
            } else {
                this->scaleX = static_cast<double>(USER_DEFAULT_SCREEN_DPI) / GetDeviceCaps(hdc, LOGPIXELSX);
                this->scaleY = static_cast<double>(USER_DEFAULT_SCREEN_DPI) / GetDeviceCaps(hdc, LOGPIXELSY);
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

void sw::Dip::Update(int dpiX, int dpiY)
{
    _scaleInfo.scaleX = static_cast<double>(USER_DEFAULT_SCREEN_DPI) / dpiX;
    _scaleInfo.scaleY = static_cast<double>(USER_DEFAULT_SCREEN_DPI) / dpiY;
}

double sw::Dip::PxToDipX(int px)
{
    return px * _scaleInfo.scaleX;
}

double sw::Dip::PxToDipY(int px)
{
    return px * _scaleInfo.scaleY;
}

int sw::Dip::DipToPxX(double dip)
{
    return (int)std::lround(dip / _scaleInfo.scaleX);
}

int sw::Dip::DipToPxY(double dip)
{
    return (int)std::lround(dip / _scaleInfo.scaleY);
}
