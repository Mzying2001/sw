// https://github.com/Mzying2001/sw

#pragma once
#include <Windows.h>
#include <CommCtrl.h>
#include <Shlobj.h>
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <map>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <typeindex>
#include <vector>
#include <windowsx.h>

// ScrollEnums.h


namespace sw
{
    /**
     * @brief 滚动条方向
     */
    enum class ScrollOrientation {
        Horizontal, // 水平滚动条
        Vertical,   // 垂直滚动条
    };

    /**
     * @brief 滚动条事件
     */
    enum class ScrollEvent {
        LineUp        = SB_LINEUP,        // Scrolls one line up.
        LineLeft      = SB_LINELEFT,      // Scrolls left by one unit.
        LineDown      = SB_LINEDOWN,      // Scrolls one line down.
        LineRight     = SB_LINERIGHT,     // Scrolls right by one unit.
        PageUp        = SB_PAGEUP,        // Scrolls one page up.
        PageLeft      = SB_PAGELEFT,      // Scrolls left by the width of the window.
        PageDown      = SB_PAGEDOWN,      // Scrolls one page down.
        PageRight     = SB_PAGERIGHT,     // Scrolls right by the width of the window.
        ThumbPosition = SB_THUMBPOSITION, // The user has dragged the scroll box (thumb) and released the mouse button. The HIWORD indicates the position of the scroll box at the end of the drag operation.
        ThubmTrack    = SB_THUMBTRACK,    // The user is dragging the scroll box. This message is sent repeatedly until the user releases the mouse button. The HIWORD indicates the position that the scroll box has been dragged to.
        Top           = SB_TOP,           // Scrolls to the upper left.
        Left          = SB_LEFT,          // Scrolls to the upper left.
        Bottom        = SB_BOTTOM,        // Scrolls to the lower right.
        Right         = SB_RIGHT,         // Scrolls to the lower right.
        EndScroll     = SB_ENDSCROLL,     // Ends scroll.
    };
}

// Alignment.h

namespace sw
{
    /**
     * @brief 水平对齐方式
     */
    enum class HorizontalAlignment {
        Center,  // 中心
        Stretch, // 拉伸
        Left,    // 左对齐
        Right,   // 右对齐
    };

    /**
     * @brief 垂直对齐方式
     */
    enum class VerticalAlignment {
        Center,  // 中心
        Stretch, // 拉伸
        Top,     // 顶部对齐
        Bottom,  // 底部对齐
    };

    /**
     * @brief 排列方式
     */
    enum class Orientation {
        Horizontal, // 水平排列
        Vertical,   // 垂直排列
    };
}

// ProcMsg.h


namespace sw
{
    /**
     * @brief 对Windows窗口消息的封装
     */
    struct ProcMsg {
        /**
         * @brief 接收到消息的窗口句柄
         */
        HWND hwnd;

        /**
         * @brief 消息类型
         */
        UINT uMsg;

        /**
         * @brief 消息的附加信息
         */
        WPARAM wParam;

        /**
         * @brief 消息的附加信息
         */
        LPARAM lParam;

        /**
         * @brief 构造所有字段均为0的ProcMsg
         */
        ProcMsg();

        /**
         * @brief 构造ProcMsg
         */
        ProcMsg(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    };
}

// EnumBit.h


#define _SW_ENUM_ENABLE_BIT_OPERATIONS(T)                  \
    template <>                                            \
    struct _EnumSupportBitOperations<T> : std::true_type { \
    }

namespace sw
{
    /**
     * @brief 用于标记枚举是否支持位运算
     */
    template <typename T>
    struct _EnumSupportBitOperations : std::false_type {
    };

    /**
     * @brief 为标记_EnumSupportBitOperations的枚举类型提供按位或运算
     */
    template <typename T>
    inline constexpr typename std::enable_if<std::is_enum<T>::value && _EnumSupportBitOperations<T>::value, T>::type
    operator|(T a, T b)
    {
        using TUnderlying = typename std::underlying_type<T>::type;
        return static_cast<T>(static_cast<TUnderlying>(a) | static_cast<TUnderlying>(b));
    }

    /**
     * @brief 为标记_EnumSupportBitOperations的枚举类型提供按位或赋值运算
     */
    template <typename T>
    inline constexpr typename std::enable_if<std::is_enum<T>::value && _EnumSupportBitOperations<T>::value, T &>::type
    operator|=(T &a, T b)
    {
        return a = a | b;
    }

    /**
     * @brief 为标记_EnumSupportBitOperations的枚举类型提供按位与运算
     */
    template <typename T>
    inline constexpr typename std::enable_if<std::is_enum<T>::value && _EnumSupportBitOperations<T>::value, T>::type
    operator&(T a, T b)
    {
        using TUnderlying = typename std::underlying_type<T>::type;
        return static_cast<T>(static_cast<TUnderlying>(a) & static_cast<TUnderlying>(b));
    }

    /**
     * @brief 为标记_EnumSupportBitOperations的枚举类型提供按位与赋值运算
     */
    template <typename T>
    inline constexpr typename std::enable_if<std::is_enum<T>::value && _EnumSupportBitOperations<T>::value, T &>::type
    operator&=(T &a, T b)
    {
        return a = a & b;
    }

    /**
     * @brief 为标记_EnumSupportBitOperations的枚举类型提供按位异或运算
     */
    template <typename T>
    inline constexpr typename std::enable_if<std::is_enum<T>::value && _EnumSupportBitOperations<T>::value, T>::type
    operator^(T a, T b)
    {
        using TUnderlying = typename std::underlying_type<T>::type;
        return static_cast<T>(static_cast<TUnderlying>(a) ^ static_cast<TUnderlying>(b));
    }

    /**
     * @brief 为标记_EnumSupportBitOperations的枚举类型提供按位异或赋值运算
     */
    template <typename T>
    inline constexpr typename std::enable_if<std::is_enum<T>::value && _EnumSupportBitOperations<T>::value, T &>::type
    operator^=(T &a, T b)
    {
        return a = a ^ b;
    }

    /**
     * @brief 为标记_EnumSupportBitOperations的枚举类型提供按位取反运算
     */
    template <typename T>
    inline constexpr typename std::enable_if<std::is_enum<T>::value && _EnumSupportBitOperations<T>::value, T>::type
    operator~(T a)
    {
        using TUnderlying = typename std::underlying_type<T>::type;
        return static_cast<T>(~static_cast<TUnderlying>(a));
    }
}

// Font.h


namespace sw
{
    /**
     * @brief 字体的粗细
     */
    enum class FontWeight : int {
        DontCare   = 0,
        Thin       = 100,
        ExtraLight = 200,
        UltraLight = 200,
        Light      = 300,
        Normal     = 400,
        Regular    = 400,
        Medium     = 500,
        SemiBold   = 600,
        DemiBold   = 600,
        Bold       = 700,
        ExtraBold  = 800,
        UltraBold  = 800,
        Heavy      = 900,
        Black      = 900,
    };

    /**
     * @brief 字符集
     */
    enum class FontCharSet : uint8_t {
        ANSI        = ANSI_CHARSET,
        Baltic      = BALTIC_CHARSET,
        CheneseBig5 = CHINESEBIG5_CHARSET,
        Default     = DEFAULT_CHARSET,
        EastEurope  = EASTEUROPE_CHARSET,
        GB2312      = GB2312_CHARSET,
        Greek       = GREEK_CHARSET,
        Hangul      = HANGUL_CHARSET,
        Mac         = MAC_CHARSET,
        Oem         = OEM_CHARSET,
        Russian     = RUSSIAN_CHARSET,
        ShiftJIS    = SHIFTJIS_CHARSET,
        Symbol      = SYMBOL_CHARSET,
        Turkish     = TURKISH_CHARSET,
        Vietnamese  = VIETNAMESE_CHARSET,

        // Windows 的朝鲜语版本：
        Johab = JOHAB_CHARSET,

        // Windows 中东语言版本：
        Arabic = ARABIC_CHARSET,
        Hebrew = HEBREW_CHARSET,

        // Windows 的泰语版本：
        Thai = THAI_CHARSET,
    };

    /**
     * @brief 输出质量
     */
    enum class FontOutPrecision : uint8_t {
        Character      = OUT_CHARACTER_PRECIS, // 未使用。
        Default        = OUT_DEFAULT_PRECIS,   // 指定默认字体映射器行为。
        Device         = OUT_DEVICE_PRECIS,    // 当系统包含多个同名字体时，指示字体映射器选择设备字体。
        Outline        = OUT_OUTLINE_PRECIS,   // 此值指示字体映射器从 TrueType 和其他基于大纲的字体中进行选择。
        PostScriptOnly = OUT_PS_ONLY_PRECIS,   // 指示字体映射器仅从 PostScript 字体中进行选择。 如果系统中没有安装 PostScript 字体，字体映射器将返回到默认行为。
        Raster         = OUT_RASTER_PRECIS,    // 当系统包含多个同名字体时，指示字体映射器选择光栅字体。
        String         = OUT_STRING_PRECIS,    // 字体映射器不使用此值，但在枚举光栅字体时会返回此值。
        Stroke         = OUT_STROKE_PRECIS,    // 字体映射器不使用此值，但在枚举 TrueType、其他基于轮廓的字体和矢量字体时返回此值。
        TrueTypeOnly   = OUT_TT_ONLY_PRECIS,   // 指示字体映射器仅从 TrueType 字体中进行选择。 如果系统中没有安装 TrueType 字体，字体映射器将返回到默认行为。
        TrueType       = OUT_TT_PRECIS,        // 当系统包含多个同名字体时，指示字体映射器选择 TrueType 字体。
    };

    /**
     * @brief 剪裁精度
     */
    enum class FontClipPrecision : uint8_t {
        CharacterPrecis = CLIP_CHARACTER_PRECIS, // 未使用。
        DefaultPrecis   = CLIP_DEFAULT_PRECIS,   // 指定默认剪辑行为。

#if defined(CLIP_DFA_DISABLE)
        DFA_Disable = CLIP_DFA_DISABLE, // Windows XP SP1： 关闭字体的字体关联。 请注意，此标志不保证在 Windows Server 2003 之后对任何平台产生任何影响。
#endif

        Embedded  = CLIP_EMBEDDED,  // 必须指定此标志才能使用嵌入的只读字体。
        LH_Angles = CLIP_LH_ANGLES, // 使用此值时，所有字体的旋转取决于坐标系的方向是左手还是右手。如果未使用，设备字体始终逆时针旋转，但其他字体的旋转取决于坐标系的方向。
        Mask      = CLIP_MASK,      // 未使用。

#if defined(CLIP_DFA_OVERRIDE)
        DFA_Override = CLIP_DFA_OVERRIDE, // 关闭字体的字体关联。 这与CLIP_DFA_DISABLE相同，但在某些情况下可能会有问题：建议使用的标志是CLIP_DFA_DISABLE。
#endif

        StrokePrecis = CLIP_STROKE_PRECIS, // 字体映射器不使用，但在枚举光栅、矢量或 TrueType 字体时返回。 为了兼容，枚举字体时始终返回此值。
        TT_Always    = CLIP_TT_ALWAYS,     // 未使用。
    };

    /**
     * @brief 输出质量
     */
    enum class FontQuality : uint8_t {
        AntiAliased   = ANTIALIASED_QUALITY,    // 如果字体支持该字体，并且字体大小不是太小或太大，则字体始终为抗锯齿。
        ClearType     = CLEARTYPE_QUALITY,      // 如果设置，则尽可能使用 ClearType 抗锯齿方法呈现文本。
        Default       = DEFAULT_QUALITY,        // 字体的外观并不重要。
        Draft         = DRAFT_QUALITY,          // 字体的外观不如使用PROOF_QUALITY时重要。 对于 GDI 光栅字体，会启用缩放，这意味着可以使用更多字号，但质量可能较低。 如有必要，将合成粗体、斜体、下划线和删除线字体。
        NoAntiAliased = NONANTIALIASED_QUALITY, // 字体永远不会抗锯齿。
        Proof         = PROOF_QUALITY,          // 字体的字符质量比逻辑字体属性的精确匹配更重要。 对于 GDI 光栅字体，将禁用缩放，并选择大小最接近的字体。 虽然在使用PROOF_QUALITY时可能无法精确映射所选字号，但字体质量较高，外观不会失真。 如有必要，将合成粗体、斜体、下划线和删除线字体。
    };

    /**
     * @brief 字体的间距
     */
    enum class FontPitch : uint8_t {
        Default  = DEFAULT_PITCH,
        Fixed    = FIXED_PITCH,
        Variable = VARIABLE_PITCH,
    };

    /**
     * @brief 字体系列
     */
    enum class FontFamily : uint8_t {
        Decorative = FF_DECORATIVE, // 新奇字体。 例如 Old English。
        DontCare   = FF_DONTCARE,   // 使用默认字体。
        Modern     = FF_MODERN,     // 具有固定笔划宽度的字体 (带衬线或无衬线) 的空白字体。 Monospace 字体通常是新式字体。 例如，Pica、Elite 和 CourierNew。
        Roman      = FF_ROMAN,      // 笔划宽度可变的字体 (比例) 和衬线。 例如 MS Serif。
        Script     = FF_SCRIPT,     // 设计为类似于手写的字体。 例如 Script 和 Cursive。
        Swiss      = FF_SWISS,      // 笔划宽度可变的字体 (成比例) 且不带衬线。 例如 MS Sans Serif。
    };

    /**
     * @brief 字体类
     */
    class Font
    {
    public:
        /**
         * @brief 字体的字体名称，此字符串的长度不能超过 32
         */
        std::wstring name = L"";

        /**
         * @brief 字体大小，以dip为单位
         */
        double size = 12;

        /**
         * @brief 转义向量与设备的 x 轴之间的角度（以十分之一度为单位）
         */
        int escapement = 0;

        /**
         * @brief 每个字符的基线和设备 x 轴之间的角度（以十分之一度为单位）
         */
        int orientation = 0;

        /**
         * @brief 字体的粗细，范围为 0 到 1000
         */
        FontWeight weight = FontWeight::DontCare;

        /**
         * @brief 是否为斜体
         */
        bool italic = false;

        /**
         * @brief 是否有下划线
         */
        bool underline = false;

        /**
         * @brief 是否有删除线
         */
        bool strikeOut = false;

        /**
         * @brief 字符集
         */
        FontCharSet charSet = FontCharSet::Default;

        /**
         * @brief 输出精度
         */
        FontOutPrecision outPrecision = FontOutPrecision::Default;

        /**
         * @brief 裁剪精度
         */
        FontClipPrecision clipPrecision = FontClipPrecision::DefaultPrecis;

        /**
         * @brief 输出质量
         */
        FontQuality quality = FontQuality::Default;

        /**
         * @brief 字体的间距和系列
         */
        /*uint8_t pitchAndFamily;*/

        /**
         * @brief 字体的间距
         */
        FontPitch pitch = FontPitch::Default;

        /**
         * @brief 字体系列
         */
        FontFamily family = FontFamily::DontCare;

    public:
        /**
         * @brief 构造字体对象
         */
        Font();

        /**
         * @brief 构造指定字体名称和字符集的字体对象
         */
        Font(const std::wstring &name, FontCharSet charSet);

        /**
         * @brief 构造指定字体名称、大小与粗细的字体对象
         */
        Font(const std::wstring &name, double size = 12, FontWeight weight = FontWeight::DontCare);

        /**
         * @brief 从LOGFONTW构造字体对象
         */
        Font(const LOGFONTW &logFont);

        /**
         * @brief 隐式转换LOGFONTW
         */
        operator LOGFONTW() const;

        /**
         * @brief 创建HFONT句柄
         */
        HFONT CreateHandle() const;

        /**
         * @brief 通过HFONT获取字体信息
         */
        static Font GetFont(HFONT hFont);

        /**
         * @brief        获取默认字体，可修改返回的引用来更改控件的默认字体，当首次调用或参数update为true时会获取系统默认字体（已创建的控件字体不会因此改变）
         * @param update 是否重新获取
         * @return       返回默认字体
         */
        static Font &GetDefaultFont(bool update = false);
    };
}

// WndMsg.h


namespace sw
{
    /**
     * @brief 包含SimpleWindow用到的一些窗口消息
     */
    enum WndMsg : UINT {

        // SimpleWindow所用消息的起始位置
        WM_SimpleWindowBegin = WM_USER + 0x3000,

        // 控件布局发生变化时控件所在顶级窗口将收到该消息，wParam和lParam均未使用
        WM_UpdateLayout,

        // 在窗口线程上执行指定委托，lParam为指向sw::Action<>的指针，wParam表示是否对委托指针执行delete
        WM_InvokeAction,

        // SimpleWindow所用消息的结束位置
        WM_SimpleWindowEnd,
    };
}

// Icon.h


namespace sw
{
    /**
     * @brief 系统标准图标样式
     * @brief https://learn.microsoft.com/en-us/windows/win32/menurc/about-icons
     */
    enum class StandardIcon {
        Application = 32512, // Default application icon
        Error       = 32513, // Error icon
        Question    = 32514, // Question mark icon
        Warning     = 32515, // Warning icon
        Information = 32516, // Information icon
        WinLogo     = 32517, // Windows logo icon
        Shield      = 32518, // Security shield icon
    };

    /**
     * @brief 用于获取图标句柄的工具类
     */
    class IconHelper
    {
    private:
        IconHelper() = delete;

    public:
        /**
         * @brief      获取系统标准图标句柄
         * @param icon 图标样式
         * @return     图标句柄
         */
        static HICON GetIconHandle(StandardIcon icon);

        /**
         * @brief            从指定模块中获取图标句柄
         * @param hInstance  DLL或EXE的模块句柄
         * @param resourceId 图标的资源序号
         * @return           成功则返回图标句柄，否则返回NULL
         */
        static HICON GetIconHandle(HINSTANCE hInstance, int resourceId);

        /**
         * @brief          从文件加载图标句柄
         * @param fileName 图标文件的路径
         * @return         成功则返回图标句柄，否则返回NULL
         */
        static HICON GetIconHandle(const std::wstring &fileName);
    };
}

// ITag.h


namespace sw
{
    /**
     * @brief Tag接口
     */
    class ITag
    {
    public:
        /**
         * @brief 默认虚析构函数
         */
        virtual ~ITag() = default;

    public:
        /**
         * @brief 获取Tag
         */
        virtual uint64_t GetTag() = 0;

        /**
         * @brief 设置Tag
         */
        virtual void SetTag(uint64_t tag) = 0;
    };
}

// Delegate.h


namespace sw
{
    // ICallable接口声明
    template <typename>
    struct ICallable;

    // Delegate类声明
    template <typename>
    class Delegate;

    /*================================================================================*/

    /**
     * @brief ICallable接口，用于表示可调用对象的接口
     */
    template <typename TRet, typename... Args>
    struct ICallable<TRet(Args...)> {
        /**
         * @brief 析构函数
         */
        virtual ~ICallable() = default;

        /**
         * @brief      调用函数
         * @param args 函数参数
         * @return     函数返回值
         */
        virtual TRet Invoke(Args... args) const = 0;

        /**
         * @brief 克隆当前可调用对象
         */
        virtual ICallable *Clone() const = 0;

        /**
         * @brief 获取当前可调用对象的类型信息
         */
        virtual std::type_index GetType() const = 0;

        /**
         * @brief       判断当前可调用对象是否与另一个可调用对象相等
         * @param other 另一个可调用对象
         * @return      如果相等则返回true，否则返回false
         */
        virtual bool Equals(const ICallable &other) const = 0;
    };

    /*================================================================================*/

    /**
     * @brief 用于存储和管理多个可调用对象的列表，针对单个可调用对象的情况进行优化
     */
    template <typename T>
    class CallableList
    {
    public:
        /**
         * @brief 可调用对象类型别名
         */
        using TCallable = ICallable<T>;

        /**
         * @brief 智能指针类型别名，用于存储可调用对象的唯一指针
         */
        using TSinglePtr = std::unique_ptr<TCallable>;

        /**
         * @brief 列表类型别名，用于存储多个可调用对象的智能指针
         */
        using TSharedList = std::vector<std::shared_ptr<TCallable>>;

    private:
        /**
         * @brief 内部存储可调用对象的联合体
         */
        mutable union {
            alignas(TSinglePtr) uint8_t _single[sizeof(TSinglePtr)];
            alignas(TSharedList) uint8_t _list[sizeof(TSharedList)];
        } _data = {};

        /**
         * @brief 当前状态枚举
         */
        enum : uint8_t {
            STATE_NONE,   // 未存储任何可调用对象
            STATE_SINGLE, // 储存了一个可调用对象
            STATE_LIST,   // 储存了多个可调用对象
        } _state = STATE_NONE;

    public:
        /**
         * @brief 默认构造函数
         */
        CallableList()
        {
        }

        /**
         * @brief 拷贝构造函数
         */
        CallableList(const CallableList &other)
        {
            *this = other;
        }

        /**
         * @brief 移动构造函数
         */
        CallableList(CallableList &&other) noexcept
        {
            *this = std::move(other);
        }

        /**
         * @brief 拷贝赋值运算
         */
        CallableList &operator=(const CallableList &other)
        {
            if (this == &other) {
                return *this;
            }

            _Reset(other._state);

            switch (other._state) {
                case STATE_SINGLE: {
                    _GetSingle().reset(other._GetSingle()->Clone());
                    break;
                }
                case STATE_LIST: {
                    _GetList() = other._GetList();
                    break;
                }
            }
            return *this;
        }

        /**
         * @brief 移动赋值运算
         */
        CallableList &operator=(CallableList &&other) noexcept
        {
            if (this == &other) {
                return *this;
            }

            _Reset(other._state);

            switch (other._state) {
                case STATE_SINGLE: {
                    _GetSingle() = std::move(other._GetSingle());
                    other._Reset();
                    break;
                }
                case STATE_LIST: {
                    _GetList() = std::move(other._GetList());
                    other._Reset();
                    break;
                }
            }
            return *this;
        }

        /**
         * @brief 析构函数
         */
        ~CallableList()
        {
            _Reset();
        }

        /**
         * @brief  获取当前存储的可调用对象数量
         * @return 可调用对象的数量
         */
        size_t Count() const noexcept
        {
            switch (_state) {
                case STATE_SINGLE: {
                    return 1;
                }
                case STATE_LIST: {
                    return _GetList().size();
                }
                default: {
                    return 0;
                }
            }
        }

        /**
         * @brief  判断当前存储的可调用对象是否为空
         * @return 如果没有存储任何可调用对象则返回true，否则返回false
         */
        bool IsEmpty() const noexcept
        {
            return _state == STATE_NONE;
        }

        /**
         * @brief 清空当前存储的可调用对象
         */
        void Clear() noexcept
        {
            _Reset();
        }

        /**
         * @brief 添加一个可调用对象到列表中
         * @note  传入对象的生命周期将由CallableList管理
         */
        void Add(TCallable *callable)
        {
            if (callable == nullptr) {
                return;
            }

            switch (_state) {
                case STATE_NONE: {
                    _Reset(STATE_SINGLE);
                    _GetSingle().reset(callable);
                    break;
                }
                case STATE_SINGLE: {
                    TSharedList list;
                    list.emplace_back(_GetSingle().release());
                    list.emplace_back(callable);
                    _Reset(STATE_LIST);
                    _GetList() = std::move(list);
                    break;
                }
                case STATE_LIST: {
                    _GetList().emplace_back(callable);
                    break;
                }
            }
        }

        /**
         * @brief  移除指定索引处的可调用对象
         * @return 如果成功移除则返回true，否则返回false
         */
        bool RemoveAt(size_t index) noexcept
        {
            switch (_state) {
                case STATE_SINGLE: {
                    if (index != 0) {
                        return false;
                    } else {
                        _Reset();
                        return true;
                    }
                }
                case STATE_LIST: {
                    auto &list = _GetList();
                    if (index >= list.size()) {
                        return false;
                    }
                    list.erase(list.begin() + index);
                    if (list.empty()) {
                        _Reset();
                    } else if (list.size() == 1) {
                        auto ptr = list.front()->Clone();
                        _Reset(STATE_SINGLE);
                        _GetSingle().reset(ptr);
                    }
                    return true;
                }
                default: {
                    return false;
                }
            }
        }

        /**
         * @brief  获取指定索引处的可调用对象
         * @return 如果索引有效则返回对应的可调用对象，否则返回nullptr
         */
        TCallable *GetAt(size_t index) const noexcept
        {
            switch (_state) {
                case STATE_SINGLE: {
                    return index == 0 ? _GetSingle().get() : nullptr;
                }
                case STATE_LIST: {
                    auto &list = _GetList();
                    return (index < list.size()) ? list[index].get() : nullptr;
                }
                default: {
                    return nullptr;
                }
            }
        }

        /**
         * @brief  获取指定索引处的可调用对象
         * @return 如果索引有效则返回对应的可调用对象，否则返回nullptr
         */
        TCallable *operator[](size_t index) const noexcept
        {
            return GetAt(index);
        }

    private:
        /**
         * @brief 内部函数，当状态为STATE_SINGLE时返回单个可调用对象的引用，
         */
        constexpr TSinglePtr &_GetSingle() const noexcept
        {
            return *reinterpret_cast<TSinglePtr *>(_data._single);
        }

        /**
         * @brief 内部函数，当状态为STATE_LIST时返回可调用对象列表的引用
         */
        constexpr TSharedList &_GetList() const noexcept
        {
            return *reinterpret_cast<TSharedList *>(_data._list);
        }

        /**
         * @brief 重置当前状态，释放存储的可调用对象
         */
        void _Reset() noexcept
        {
            switch (_state) {
                case STATE_SINGLE: {
                    _GetSingle().~TSinglePtr();
                    _state = STATE_NONE;
                    break;
                }
                case STATE_LIST: {
                    _GetList().~TSharedList();
                    _state = STATE_NONE;
                    break;
                }
            }
        }

        /**
         * @brief 重置当前状态并根据给定状态进行初始化
         */
        void _Reset(uint8_t state) noexcept
        {
            _Reset();

            switch (state) {
                case STATE_SINGLE: {
                    new (_data._single) TSinglePtr();
                    _state = STATE_SINGLE;
                    break;
                }
                case STATE_LIST: {
                    new (_data._list) TSharedList();
                    _state = STATE_LIST;
                    break;
                }
            }
        }
    };

    /*================================================================================*/

    /**
     * @brief 委托类，类似于C#中的委托，支持存储和调用任意可调用对象
     */
    template <typename TRet, typename... Args>
    class Delegate<TRet(Args...)> final : public ICallable<TRet(Args...)>
    {
    private:
        using _ICallable = ICallable<TRet(Args...)>;

        template <typename T, typename = void>
        struct _IsEqualityComparable : std::false_type {
        };

        template <typename T>
        struct _IsEqualityComparable<
            T,
            typename std::enable_if<true, decltype(void(std::declval<T>() == std::declval<T>()))>::type> : std::true_type {
        };

        template <typename T, typename = void>
        struct _IsMemcmpSafe : std::false_type {
        };

        template <typename T>
        struct _IsMemcmpSafe<
            T,
            typename std::enable_if</*std::is_trivial<T>::value &&*/ std::is_standard_layout<T>::value, void>::type> : std::true_type {
        };

        template <typename T>
        class _CallableWrapperImpl : public _ICallable
        {
            alignas(T) mutable uint8_t _storage[sizeof(T)];

        public:
            _CallableWrapperImpl(const T &value)
            {
                memset(_storage, 0, sizeof(_storage));
                new (_storage) T(value);
            }
            _CallableWrapperImpl(T &&value)
            {
                memset(_storage, 0, sizeof(_storage));
                new (_storage) T(std::move(value));
            }
            virtual ~_CallableWrapperImpl()
            {
                GetValue().~T();
                // memset(_storage, 0, sizeof(_storage));
            }
            T &GetValue() const noexcept
            {
                return *reinterpret_cast<T *>(_storage);
            }
            TRet Invoke(Args... args) const override
            {
                return GetValue()(std::forward<Args>(args)...);
            }
            _ICallable *Clone() const override
            {
                return new _CallableWrapperImpl(GetValue());
            }
            virtual std::type_index GetType() const override
            {
                return typeid(T);
            }
            bool Equals(const _ICallable &other) const override
            {
                return EqualsImpl(other);
            }
            template <typename U = T>
            typename std::enable_if<_IsEqualityComparable<U>::value, bool>::type
            EqualsImpl(const _ICallable &other) const
            {
                if (this == &other) {
                    return true;
                }
                if (GetType() != other.GetType()) {
                    return false;
                }
                const auto &otherWrapper = static_cast<const _CallableWrapperImpl &>(other);
                return GetValue() == otherWrapper.GetValue();
            }
            template <typename U = T>
            typename std::enable_if<!_IsEqualityComparable<U>::value && _IsMemcmpSafe<U>::value, bool>::type
            EqualsImpl(const _ICallable &other) const
            {
                if (this == &other) {
                    return true;
                }
                if (GetType() != other.GetType()) {
                    return false;
                }
                const auto &otherWrapper = static_cast<const _CallableWrapperImpl &>(other);
                return memcmp(_storage, otherWrapper._storage, sizeof(_storage)) == 0;
            }
            template <typename U = T>
            typename std::enable_if<!_IsEqualityComparable<U>::value && !_IsMemcmpSafe<U>::value, bool>::type
            EqualsImpl(const _ICallable &other) const
            {
                return this == &other;
            }
        };

        template <typename T>
        using _CallableWrapper = _CallableWrapperImpl<typename std::decay<T>::type>;

        template <typename T>
        class _MemberFuncWrapper : public _ICallable
        {
            T *obj;
            TRet (T::*func)(Args...);

        public:
            _MemberFuncWrapper(T &obj, TRet (T::*func)(Args...))
                : obj(&obj), func(func)
            {
            }
            TRet Invoke(Args... args) const override
            {
                return (obj->*func)(std::forward<Args>(args)...);
            }
            _ICallable *Clone() const override
            {
                return new _MemberFuncWrapper(*obj, func);
            }
            virtual std::type_index GetType() const override
            {
                return typeid(func);
            }
            bool Equals(const _ICallable &other) const override
            {
                if (this == &other) {
                    return true;
                }
                if (GetType() != other.GetType()) {
                    return false;
                }
                const auto &otherWrapper = static_cast<const _MemberFuncWrapper &>(other);
                return obj == otherWrapper.obj && func == otherWrapper.func;
            }
        };

        template <typename T>
        class _ConstMemberFuncWrapper : public _ICallable
        {
            const T *obj;
            TRet (T::*func)(Args...) const;

        public:
            _ConstMemberFuncWrapper(const T &obj, TRet (T::*func)(Args...) const)
                : obj(&obj), func(func)
            {
            }
            TRet Invoke(Args... args) const override
            {
                return (obj->*func)(std::forward<Args>(args)...);
            }
            _ICallable *Clone() const override
            {
                return new _ConstMemberFuncWrapper(*obj, func);
            }
            virtual std::type_index GetType() const override
            {
                return typeid(func);
            }
            bool Equals(const _ICallable &other) const override
            {
                if (this == &other) {
                    return true;
                }
                if (GetType() != other.GetType()) {
                    return false;
                }
                const auto &otherWrapper = static_cast<const _ConstMemberFuncWrapper &>(other);
                return obj == otherWrapper.obj && func == otherWrapper.func;
            }
        };

    private:
        /**
         * @brief 内部存储可调用对象的容器
         */
        CallableList<TRet(Args...)> _data;

    public:
        /**
         * @brief 默认构造函数
         */
        Delegate(std::nullptr_t = nullptr)
        {
        }

        /**
         * @brief 构造函数，接受一个可调用对象
         */
        Delegate(const ICallable<TRet(Args...)> &callable)
        {
            Add(callable);
        }

        /**
         * @brief 构造函数，接受一个函数指针
         */
        Delegate(TRet (*func)(Args...))
        {
            Add(func);
        }

        /**
         * @brief 构造函数，接受一个可调用对象
         */
        template <typename T, typename std::enable_if<!std::is_base_of<_ICallable, T>::value, int>::type = 0>
        Delegate(const T &callable)
        {
            Add(callable);
        }

        /**
         * @brief 构造函数，接受一个成员函数指针
         */
        template <typename T>
        Delegate(T &obj, TRet (T::*func)(Args...))
        {
            Add(obj, func);
        }

        /**
         * @brief 构造函数，接受一个常量成员函数指针
         */
        template <typename T>
        Delegate(const T &obj, TRet (T::*func)(Args...) const)
        {
            Add(obj, func);
        }

        /**
         * @brief 拷贝构造函数
         */
        Delegate(const Delegate &other)
        {
            for (size_t i = 0; i < other._data.Count(); ++i) {
                _data.Add(other._data[i]->Clone());
            }
        }

        /**
         * @brief 移动构造函数
         */
        Delegate(Delegate &&other)
            : _data(std::move(other._data))
        {
        }

        /**
         * @brief 拷贝赋值运算符
         */
        Delegate &operator=(const Delegate &other)
        {
            if (this == &other) {
                return *this;
            }
            _data.Clear();
            for (size_t i = 0; i < other._data.Count(); ++i) {
                _data.Add(other._data[i]->Clone());
            }
            return *this;
        }

        /**
         * @brief 移动赋值运算符
         */
        Delegate &operator=(Delegate &&other)
        {
            if (this != &other) {
                _data = std::move(other._data);
            }
            return *this;
        }

        /**
         * @brief 添加一个可调用对象到委托中
         */
        void Add(const ICallable<TRet(Args...)> &callable)
        {
            // 当添加的可调用对象与当前委托类型相同时（针对单播委托进行优化）：
            // - 若委托内容为空，则直接返回
            // - 若委托内容只有一个元素，则克隆该元素并添加到当前委托中
            // - 否则，直接添加该可调用对象的克隆
            if (callable.GetType() == GetType()) {
                auto &delegate = static_cast<const Delegate &>(callable);
                if (delegate._data.IsEmpty()) {
                    return;
                } else if (delegate._data.Count() == 1) {
                    _data.Add(delegate._data[0]->Clone());
                    return;
                }
            }
            _data.Add(callable.Clone());
        }

        /**
         * @brief 添加一个函数指针到委托中
         */
        void Add(TRet (*func)(Args...))
        {
            if (func != nullptr) {
                _data.Add(new _CallableWrapper<decltype(func)>(func));
            }
        }

        /**
         * @brief 添加一个可调用对象到委托中
         */
        template <typename T>
        typename std::enable_if<!std::is_base_of<_ICallable, T>::value, void>::type
        Add(const T &callable)
        {
            _data.Add(new _CallableWrapper<T>(callable));
        }

        /**
         * @brief 添加一个成员函数指针到委托中
         */
        template <typename T>
        void Add(T &obj, TRet (T::*func)(Args...))
        {
            _data.Add(new _MemberFuncWrapper<T>(obj, func));
        }

        /**
         * @brief 添加一个常量成员函数指针到委托中
         */
        template <typename T>
        void Add(const T &obj, TRet (T::*func)(Args...) const)
        {
            _data.Add(new _ConstMemberFuncWrapper<T>(obj, func));
        }

        /**
         * @brief 清空委托中的所有可调用对象
         */
        void Clear()
        {
            _data.Clear();
        }

        /**
         * @brief  移除一个可调用对象
         * @return 如果成功移除则返回true，否则返回false
         * @note   按照添加顺序从后向前查找，找到第一个匹配的可调用对象并移除
         */
        bool Remove(const ICallable<TRet(Args...)> &callable)
        {
            // 当移除的可调用对象与当前委托类型相同时（与Add逻辑相对应）：
            // - 若委托内容为空，则直接返回false
            // - 若委托内容只有一个元素，则尝试移除该元素
            // - 否则，直接调用_Remove函数进行移除
            if (callable.GetType() == GetType()) {
                auto &delegate = static_cast<const Delegate &>(callable);
                if (delegate._data.IsEmpty()) {
                    return false;
                } else if (delegate._data.Count() == 1) {
                    return _Remove(*delegate._data[0]);
                }
            }
            return _Remove(callable);
        }

        /**
         * @brief  移除一个函数指针
         * @return 如果成功移除则返回true，否则返回false
         * @note   按照添加顺序从后向前查找，找到第一个匹配的函数指针并移除
         */
        bool Remove(TRet (*func)(Args...))
        {
            if (func == nullptr) {
                return false;
            }
            return _Remove(_CallableWrapper<decltype(func)>(func));
        }

        /**
         * @brief  移除一个可调用对象
         * @return 如果成功移除则返回true，否则返回false
         * @note   按照添加顺序从后向前查找，找到第一个匹配的可调用对象并移除
         */
        template <typename T>
        typename std::enable_if<!std::is_base_of<_ICallable, T>::value, bool>::type
        Remove(const T &callable)
        {
            return _Remove(_CallableWrapper<T>(callable));
        }

        /**
         * @brief  移除一个成员函数指针
         * @return 如果成功移除则返回true，否则返回false
         * @note   按照添加顺序从后向前查找，找到第一个匹配的可调用对象并移除
         */
        template <typename T>
        bool Remove(T &obj, TRet (T::*func)(Args...))
        {
            return _Remove(_MemberFuncWrapper<T>(obj, func));
        }

        /**
         * @brief  移除一个常量成员函数指针
         * @return 如果成功移除则返回true，否则返回false
         * @note   按照添加顺序从后向前查找，找到第一个匹配的可调用对象并移除
         */
        template <typename T>
        bool Remove(const T &obj, TRet (T::*func)(Args...) const)
        {
            return _Remove(_ConstMemberFuncWrapper<T>(obj, func));
        }

        /**
         * @brief      调用委托，执行所有存储的可调用对象
         * @param args 函数参数
         * @return     最后一个可调用对象的返回值
         * @throw      std::runtime_error 如果委托为空
         */
        TRet operator()(Args... args) const
        {
            return Invoke(std::forward<Args>(args)...);
        }

        /**
         * @brief       判断当前委托是否等于另一个委托
         * @param other 另一个委托
         * @return      如果相等则返回true，否则返回false
         */
        bool operator==(const Delegate &other) const
        {
            return Equals(other);
        }

        /**
         * @brief       判断当前委托是否不等于另一个委托
         * @param other 另一个委托
         * @return      如果不相等则返回true，否则返回false
         */
        bool operator!=(const Delegate &other) const
        {
            return !Equals(other);
        }

        /**
         * @brief  判断当前委托是否等于nullptr
         * @return 如果委托为空则返回true，否则返回false
         */
        bool operator==(std::nullptr_t) const
        {
            return _data.IsEmpty();
        }

        /**
         * @brief  判断当前委托是否不等于nullptr
         * @return 如果委托不为空则返回true，否则返回false
         */
        bool operator!=(std::nullptr_t) const
        {
            return !_data.IsEmpty();
        }

        /**
         * @brief  判断当前委托是否有效
         * @return 如果委托不为空则返回true，否则返回false
         */
        operator bool() const
        {
            return !_data.IsEmpty();
        }

        /**
         * @brief 添加一个可调用对象到委托中
         * @note  该函数调用Add函数
         */
        Delegate &operator+=(const ICallable<TRet(Args...)> &callable)
        {
            Add(callable);
            return *this;
        }

        /**
         * @brief 添加一个函数指针到委托中
         * @note  该函数调用Add函数
         */
        Delegate &operator+=(TRet (*func)(Args...))
        {
            Add(func);
            return *this;
        }

        /**
         * @brief 添加一个可调用对象到委托中
         * @note  该函数调用Add函数
         */
        template <typename T>
        typename std::enable_if<!std::is_base_of<_ICallable, T>::value, Delegate &>::type
        operator+=(const T &callable)
        {
            Add(callable);
            return *this;
        }

        /**
         * @brief 移除一个可调用对象
         * @note  该函数调用Remove函数
         */
        Delegate &operator-=(const ICallable<TRet(Args...)> &callable)
        {
            Remove(callable);
            return *this;
        }

        /**
         * @brief 移除一个函数指针
         * @note  该函数调用Remove函数
         */
        Delegate &operator-=(TRet (*func)(Args...))
        {
            Remove(func);
            return *this;
        }

        /**
         * @brief 移除一个可调用对象
         * @note  该函数调用Remove函数
         */
        template <typename T>
        typename std::enable_if<!std::is_base_of<_ICallable, T>::value, Delegate &>::type
        operator-=(const T &callable)
        {
            Remove(callable);
            return *this;
        }

        /**
         * @brief      调用委托，执行所有存储的可调用对象
         * @param args 函数参数
         * @return     最后一个可调用对象的返回值
         * @throw      std::runtime_error 如果委托为空
         */
        virtual TRet Invoke(Args... args) const override
        {
            size_t count = _data.Count();
            if (count == 0) {
                throw std::runtime_error("Delegate is empty");
            } else if (count == 1) {
                return _data[0]->Invoke(std::forward<Args>(args)...);
            } else {
                auto list = _data;
                for (size_t i = 0; i < count - 1; ++i)
                    list[i]->Invoke(std::forward<Args>(args)...);
                return list[count - 1]->Invoke(std::forward<Args>(args)...);
            }
        }

        /**
         * @brief  克隆当前委托
         * @return 返回一个新的Delegate对象，包含相同的可调用对象
         */
        virtual ICallable<TRet(Args...)> *Clone() const override
        {
            return new Delegate(*this);
        }

        /**
         * @brief  获取当前委托的类型信息
         * @return 返回typeid(Delegate<TRet(Args...)>)
         */
        virtual std::type_index GetType() const override
        {
            return typeid(Delegate<TRet(Args...)>);
        }

        /**
         * @brief       判断当前委托是否与另一个可调用对象相等
         * @param other 另一个可调用对象
         * @return      如果相等则返回true，否则返回false
         */
        virtual bool Equals(const ICallable<TRet(Args...)> &other) const override
        {
            if (this == &other) {
                return true;
            }
            if (GetType() != other.GetType()) {
                return false;
            }
            const auto &otherDelegate = static_cast<const Delegate &>(other);
            if (_data.Count() != otherDelegate._data.Count()) {
                return false;
            }
            for (size_t i = _data.Count(); i > 0; --i) {
                if (!_data[i - 1]->Equals(*otherDelegate._data[i - 1])) {
                    return false;
                }
            }
            return true;
        }

        /**
         * @brief      调用所有存储的可调用对象，并返回它们的结果
         * @param args 函数参数
         * @return     返回一个包含所有可调用对象返回值的vector
         */
        template <typename U = TRet>
        typename std::enable_if<!std::is_void<U>::value, std::vector<U>>::type
        InvokeAll(Args... args) const
        {
            auto list  = _data;
            auto count = list.Count();
            std::vector<U> results;
            results.reserve(count);
            for (size_t i = 0; i < count; ++i)
                results.emplace_back(list[i]->Invoke(std::forward<Args>(args)...));
            return results;
        }

    private:
        /**
         * @brief 内部函数，用于从后向前查找并移除一个可调用对象
         */
        bool _Remove(const _ICallable &callable)
        {
            for (size_t i = _data.Count(); i > 0; --i) {
                if (_data[i - 1]->Equals(callable)) {
                    return _data.RemoveAt(i - 1);
                }
            }
            return false;
        }
    };

    /*================================================================================*/

    /**
     * @brief 比较委托和nullptr
     * @note  如果委托为空则返回true，否则返回false
     */
    template <typename TRet, typename... Args>
    inline bool operator==(std::nullptr_t, const Delegate<TRet(Args...)> &d)
    {
        return d == nullptr;
    }

    /**
     * @brief 比较委托和nullptr
     * @note  如果委托不为空则返回true，否则返回false
     */
    template <typename TRet, typename... Args>
    inline bool operator!=(std::nullptr_t, const Delegate<TRet(Args...)> &d)
    {
        return d != nullptr;
    }

    /*================================================================================*/

    /**
     * @brief Action类型别名，表示无返回值的委托
     */
    template <typename... Args>
    using Action = Delegate<void(Args...)>;

    /*================================================================================*/

    /**
     * @brief _FuncTraits模板，用于提取函数类型的返回值和参数类型
     */
    template <typename...>
    struct _FuncTraits;

    /**
     * @brief _FuncTraits特化
     */
    template <typename Last>
    struct _FuncTraits<Last> {
        using TRet       = Last;
        using TArgsTuple = std::tuple<>;
    };

    /**
     * @brief _FuncTraits特化
     */
    template <typename First, typename... Rest>
    struct _FuncTraits<First, Rest...> {
        using TRet       = typename _FuncTraits<Rest...>::TRet;
        using TArgsTuple = decltype(std::tuple_cat(std::declval<std::tuple<First>>(), std::declval<typename _FuncTraits<Rest...>::TArgsTuple>()));
    };

    /**
     * @brief _FuncTypeHelper模板，用于根据参数元组生成对应的Func类型
     */
    template <typename TArgsTuple>
    struct _FuncTypeHelper;

    /**
     * @brief _FuncTypeHelper特化
     */
    template <typename... Args>
    struct _FuncTypeHelper<std::tuple<Args...>> {
        template <typename TRet>
        using TFunc = Delegate<TRet(Args...)>;
    };

    /**
     * @brief Func类型别名，类似C#中的Func<T1, T2, ..., TResult>
     */
    template <typename... Types>
    using Func = typename _FuncTypeHelper<typename _FuncTraits<Types...>::TArgsTuple>::template TFunc<typename _FuncTraits<Types...>::TRet>;
}

// Thickness.h


namespace sw
{
    /**
     * @brief 表示矩形区域周围边框的厚度
     */
    struct Thickness {
        /**
         * @brief 左边
         */
        double left;

        /**
         * @brief 顶边
         */
        double top;

        /**
         * @brief 右边
         */
        double right;

        /**
         * @brief 底边
         */
        double bottom;

        /**
         * @brief 构造一个四边都为0的Thickness结构体
         */
        Thickness();

        /**
         * @brief 构造一个四边都相同的Thickness结构体
         */
        Thickness(double thickness);

        /**
         * @brief 指定横向和纵向值构造Thickness结构体
         */
        Thickness(double horizontal, double vertical);

        /**
         * @brief 指定四边的值构造Thickness结构体
         */
        Thickness(double left, double top, double right, double bottom);

        /**
         * @brief 判断两个Thickness是否相同
         */
        bool operator==(const Thickness &other) const;

        /**
         * @brief 判断两个Thickness是否相同
         */
        bool operator!=(const Thickness &other) const;

        /**
         * @brief 获取描述当前对象的字符串
         */
        std::wstring ToString() const;
    };
}

// HitTestResult.h


namespace sw
{
    /**
     * @brief NcHitTest（WM_NCHITTEST）的返回值
     */
    enum class HitTestResult {
        HitBorder      = HTBORDER,      // 在没有大小边界的窗口边框中。
        HitBottom      = HTBOTTOM,      // 在可调整大小的窗口的下水平边框中（用户可以单击鼠标以垂直调整窗口大小）。
        HitBottomLeft  = HTBOTTOMLEFT,  // 在可调整大小的窗口的边框左下角（用户可以单击鼠标以对角线调整窗口大小）。
        HitBottomRight = HTBOTTOMRIGHT, // 在可调整大小的窗口的边框右下角（用户可以单击鼠标以对角线调整窗口大小）。
        HitCaption     = HTCAPTION,     // 在标题栏中。
        HitClient      = HTCLIENT,      // 在客户端区中。
        HitClose       = HTCLOSE,       // 在关闭按钮中。
        HitError       = HTERROR,       // 在屏幕背景上或窗口之间的分割线上（与 HTNOWHERE 相同，只是 DefWindowProc 函数会生成系统蜂鸣音以指示错误）。
        HitGrowBox     = HTGROWBOX,     // 在大小框中（与 HTSIZE 相同）。
        HitHelp        = HTHELP,        // 在帮助按钮中。
        HitHScroll     = HTHSCROLL,     // 在水平滚动条中。
        HitLeft        = HTLEFT,        // 在可调整大小的窗口的左边框中（用户可以单击鼠标以水平调整窗口大小）。
        HitMenu        = HTMENU,        // 在菜单中。
        HitMaxButton   = HTMAXBUTTON,   // 在最大化按钮中。
        HitMinButton   = HTMINBUTTON,   // 在最小化按钮中。
        HitNoWhere     = HTNOWHERE,     // 在屏幕背景上，或在窗口之间的分隔线上。
        HitReduce      = HTREDUCE,      // 在最小化按钮中。
        HitRight       = HTRIGHT,       // 在可调整大小的窗口的右左边框中（用户可以单击鼠标以水平调整窗口大小）。
        HitSize        = HTSIZE,        // 在大小框中（与 HTGROWBOX 相同）。
        HitSysMenu     = HTSYSMENU,     // 在窗口菜单或子窗口的关闭按钮中。
        HitTop         = HTTOP,         // 在窗口的上水平边框中。
        HitTopLeft     = HTTOPLEFT,     // 在窗口边框的左上角。
        HitTopRight    = HTTOPRIGHT,    // 在窗口边框的右上角。
        HitTransparent = HTTRANSPARENT, // 在同一线程当前由另一个窗口覆盖的窗口中（消息将发送到同一线程中的基础窗口，直到其中一个窗口返回不是 HTTRANSPARENT 的代码）。
        HitVScroll     = HTVSCROLL,     // 在垂直滚动条中。
        HitZoom        = HTZOOM,        // 在最大化按钮中。
    };
}

// KnownColor.h


namespace sw
{
    /**
     * @brief 已知的颜色
     */
    enum class KnownColor {
        ActiveBorder            = RGB(0xb4, 0xb4, 0xb4), // 活动边框颜色
        ActiveCaption           = RGB(0x99, 0xb4, 0xd1), // 活动标题栏颜色
        ActiveCaptionText       = RGB(0x00, 0x00, 0x00), // 活动标题栏文本颜色
        AppWorkspace            = RGB(0xab, 0xab, 0xab), // 应用程序工作区颜色
        Control                 = RGB(0xf0, 0xf0, 0xf0), // 控件颜色
        ControlDark             = RGB(0xa0, 0xa0, 0xa0), // 控件深色颜色
        ControlDarkDark         = RGB(0x69, 0x69, 0x69), // 控件深深色颜色
        ControlLight            = RGB(0xe3, 0xe3, 0xe3), // 控件浅色颜色
        ControlLightLight       = RGB(0xff, 0xff, 0xff), // 控件浅浅色颜色
        ControlText             = RGB(0x00, 0x00, 0x00), // 控件文本颜色
        Desktop                 = RGB(0x00, 0x00, 0x00), // 桌面颜色
        GrayText                = RGB(0x6d, 0x6d, 0x6d), // 灰色文本颜色
        Highlight               = RGB(0x00, 0x78, 0xd7), // 高亮颜色
        HighlightText           = RGB(0xff, 0xff, 0xff), // 高亮文本颜色
        HotTrack                = RGB(0x00, 0x66, 0xcc), // 热跟踪颜色
        InactiveBorder          = RGB(0xf4, 0xf7, 0xfc), // 非活动边框颜色
        InactiveCaption         = RGB(0xbf, 0xcd, 0xdb), // 非活动标题栏颜色
        InactiveCaptionText     = RGB(0x00, 0x00, 0x00), // 非活动标题栏文本颜色
        Info                    = RGB(0xff, 0xff, 0xe1), // 信息颜色
        InfoText                = RGB(0x00, 0x00, 0x00), // 信息文本颜色
        Menu                    = RGB(0xf0, 0xf0, 0xf0), // 菜单颜色
        MenuText                = RGB(0x00, 0x00, 0x00), // 菜单文本颜色
        ScrollBar               = RGB(0xc8, 0xc8, 0xc8), // 滚动条颜色
        Window                  = RGB(0xff, 0xff, 0xff), // 窗口颜色
        WindowFrame             = RGB(0x64, 0x64, 0x64), // 窗口框架颜色
        WindowText              = RGB(0x00, 0x00, 0x00), // 窗口文本颜色
        Transparent             = RGB(0xff, 0xff, 0xff), // 透明色
        AliceBlue               = RGB(0xf0, 0xf8, 0xff), // 爱丽丝蓝色
        AntiqueWhite            = RGB(0xfa, 0xeb, 0xd7), // 古董白色
        Aqua                    = RGB(0x00, 0xff, 0xff), // 青色
        Aquamarine              = RGB(0x7f, 0xff, 0xd4), // 碧绿色
        Azure                   = RGB(0xf0, 0xff, 0xff), // 天蓝色
        Beige                   = RGB(0xf5, 0xf5, 0xdc), // 米色
        Bisque                  = RGB(0xff, 0xe4, 0xc4), // 橘黄色
        Black                   = RGB(0x00, 0x00, 0x00), // 黑色
        BlanchedAlmond          = RGB(0xff, 0xeb, 0xcd), // 白杏色
        Blue                    = RGB(0x00, 0x00, 0xff), // 蓝色
        BlueViolet              = RGB(0x8a, 0x2b, 0xe2), // 蓝紫色
        Brown                   = RGB(0xa5, 0x2a, 0x2a), // 棕色
        BurlyWood               = RGB(0xde, 0xb8, 0x87), // 硬木色
        CadetBlue               = RGB(0x5f, 0x9e, 0xa0), // 军装蓝色
        Chartreuse              = RGB(0x7f, 0xff, 0x00), // 黄绿色
        Chocolate               = RGB(0xd2, 0x69, 0x1e), // 巧克力色
        Coral                   = RGB(0xff, 0x7f, 0x50), // 珊瑚色
        CornflowerBlue          = RGB(0x64, 0x95, 0xed), // 矢车菊蓝色
        Cornsilk                = RGB(0xff, 0xf8, 0xdc), // 米绸色
        Crimson                 = RGB(0xdc, 0x14, 0x3c), // 绯红色
        Cyan                    = RGB(0x00, 0xff, 0xff), // 青色
        DarkBlue                = RGB(0x00, 0x00, 0x8b), // 深蓝色
        DarkCyan                = RGB(0x00, 0x8b, 0x8b), // 深青色
        DarkGoldenrod           = RGB(0xb8, 0x86, 0x0b), // 深金黄色
        DarkGray                = RGB(0xa9, 0xa9, 0xa9), // 深灰色
        DarkGreen               = RGB(0x00, 0x64, 0x00), // 深绿色
        DarkKhaki               = RGB(0xbd, 0xb7, 0x6b), // 深卡其色
        DarkMagenta             = RGB(0x8b, 0x00, 0x8b), // 深洋红色
        DarkOliveGreen          = RGB(0x55, 0x6b, 0x2f), // 深橄榄绿色
        DarkOrange              = RGB(0xff, 0x8c, 0x00), // 深橙色
        DarkOrchid              = RGB(0x99, 0x32, 0xcc), // 深兰花色
        DarkRed                 = RGB(0x8b, 0x00, 0x00), // 深红色
        DarkSalmon              = RGB(0xe9, 0x96, 0x7a), // 深鲑鱼色
        DarkSeaGreen            = RGB(0x8f, 0xbc, 0x8f), // 深海绿色
        DarkSlateBlue           = RGB(0x48, 0x3d, 0x8b), // 深青蓝色
        DarkSlateGray           = RGB(0x2f, 0x4f, 0x4f), // 深青灰色
        DarkTurquoise           = RGB(0x00, 0xce, 0xd1), // 深青色
        DarkViolet              = RGB(0x94, 0x00, 0xd3), // 深紫罗兰色
        DeepPink                = RGB(0xff, 0x14, 0x93), // 深粉色
        DeepSkyBlue             = RGB(0x00, 0xbf, 0xff), // 深天蓝色
        DimGray                 = RGB(0x69, 0x69, 0x69), // 暗灰色
        DodgerBlue              = RGB(0x1e, 0x90, 0xff), // 道奇蓝色
        Firebrick               = RGB(0xb2, 0x22, 0x22), // 火砖色
        FloralWhite             = RGB(0xff, 0xfa, 0xf0), // 花白色
        ForestGreen             = RGB(0x22, 0x8b, 0x22), // 森林绿色
        Fuchsia                 = RGB(0xff, 0x00, 0xff), // 紫红色
        Gainsboro               = RGB(0xdc, 0xdc, 0xdc), // 盖茨伯勒色
        GhostWhite              = RGB(0xf8, 0xf8, 0xff), // 幽灵白色
        Gold                    = RGB(0xff, 0xd7, 0x00), // 金色
        Goldenrod               = RGB(0xda, 0xa5, 0x20), // 金菊黄色
        Gray                    = RGB(0x80, 0x80, 0x80), // 灰色
        Green                   = RGB(0x00, 0x80, 0x00), // 绿色
        GreenYellow             = RGB(0xad, 0xff, 0x2f), // 绿黄色
        Honeydew                = RGB(0xf0, 0xff, 0xf0), // 蜜色
        HotPink                 = RGB(0xff, 0x69, 0xb4), // 亮粉红色
        IndianRed               = RGB(0xcd, 0x5c, 0x5c), // 印度红色
        Indigo                  = RGB(0x4b, 0x00, 0x82), // 靛青色
        Ivory                   = RGB(0xff, 0xff, 0xf0), // 象牙色
        Khaki                   = RGB(0xf0, 0xe6, 0x8c), // 卡其色
        Lavender                = RGB(0xe6, 0xe6, 0xfa), // 熏衣草色
        LavenderBlush           = RGB(0xff, 0xf0, 0xf5), // 淡紫红色
        LawnGreen               = RGB(0x7c, 0xfc, 0x00), // 草绿色
        LemonChiffon            = RGB(0xff, 0xfa, 0xcd), // 柠檬绸色
        LightBlue               = RGB(0xad, 0xd8, 0xe6), // 淡蓝色
        LightCoral              = RGB(0xf0, 0x80, 0x80), // 淡珊瑚色
        LightCyan               = RGB(0xe0, 0xff, 0xff), // 淡青色
        LightGoldenrodYellow    = RGB(0xfa, 0xfa, 0xd2), // 浅金菊黄色
        LightGray               = RGB(0xd3, 0xd3, 0xd3), // 浅灰色
        LightGreen              = RGB(0x90, 0xee, 0x90), // 浅绿色
        LightPink               = RGB(0xff, 0xb6, 0xc1), // 浅粉色
        LightSalmon             = RGB(0xff, 0xa0, 0x7a), // 浅鲑鱼色
        LightSeaGreen           = RGB(0x20, 0xb2, 0xaa), // 浅海绿色
        LightSkyBlue            = RGB(0x87, 0xce, 0xfa), // 浅天蓝色
        LightSlateGray          = RGB(0x77, 0x88, 0x99), // 浅青灰色
        LightSteelBlue          = RGB(0xb0, 0xc4, 0xde), // 浅钢蓝色
        LightYellow             = RGB(0xff, 0xff, 0xe0), // 浅黄色
        Lime                    = RGB(0x00, 0xff, 0x00), // 酸橙色
        LimeGreen               = RGB(0x32, 0xcd, 0x32), // 柠檬绿色
        Linen                   = RGB(0xfa, 0xf0, 0xe6), // 亚麻色
        Magenta                 = RGB(0xff, 0x00, 0xff), // 洋红色
        Maroon                  = RGB(0x80, 0x00, 0x00), // 栗色
        MediumAquamarine        = RGB(0x66, 0xcd, 0xaa), // 中碧绿色
        MediumBlue              = RGB(0x00, 0x00, 0xcd), // 中蓝色
        MediumOrchid            = RGB(0xba, 0x55, 0xd3), // 中兰花紫色
        MediumPurple            = RGB(0x93, 0x70, 0xdb), // 中紫色
        MediumSeaGreen          = RGB(0x3c, 0xb3, 0x71), // 中海绿色
        MediumSlateBlue         = RGB(0x7b, 0x68, 0xee), // 中青蓝色
        MediumSpringGreen       = RGB(0x00, 0xfa, 0x9a), // 中春绿色
        MediumTurquoise         = RGB(0x48, 0xd1, 0xcc), // 中青色
        MediumVioletRed         = RGB(0xc7, 0x15, 0x85), // 中紫罗兰色
        MidnightBlue            = RGB(0x19, 0x19, 0x70), // 午夜蓝色
        MintCream               = RGB(0xf5, 0xff, 0xfa), // 薄荷色
        MistyRose               = RGB(0xff, 0xe4, 0xe1), // 浅玫瑰色
        Moccasin                = RGB(0xff, 0xe4, 0xb5), // 鹿皮色
        NavajoWhite             = RGB(0xff, 0xde, 0xad), // 纳瓦白色
        Navy                    = RGB(0x00, 0x00, 0x80), // 海军蓝色
        OldLace                 = RGB(0xfd, 0xf5, 0xe6), // 老花色
        Olive                   = RGB(0x80, 0x80, 0x00), // 橄榄色
        OliveDrab               = RGB(0x6b, 0x8e, 0x23), // 橄榄褐色
        Orange                  = RGB(0xff, 0xa5, 0x00), // 橙色
        OrangeRed               = RGB(0xff, 0x45, 0x00), // 橙红色
        Orchid                  = RGB(0xda, 0x70, 0xd6), // 兰花紫色
        PaleGoldenrod           = RGB(0xee, 0xe8, 0xaa), // 苍麒麟色
        PaleGreen               = RGB(0x98, 0xfb, 0x98), // 苍绿色
        PaleTurquoise           = RGB(0xaf, 0xee, 0xee), // 苍宝石绿色
        PaleVioletRed           = RGB(0xdb, 0x70, 0x93), // 苍紫罗兰红色
        PapayaWhip              = RGB(0xff, 0xef, 0xd5), // 番木瓜色
        PeachPuff               = RGB(0xff, 0xda, 0xb9), // 桃肉色
        Peru                    = RGB(0xcd, 0x85, 0x3f), // 秘鲁色
        Pink                    = RGB(0xff, 0xc0, 0xcb), // 粉红色
        Plum                    = RGB(0xdd, 0xa0, 0xdd), // 李紫色
        PowderBlue              = RGB(0xb0, 0xe0, 0xe6), // 粉蓝色
        Purple                  = RGB(0x80, 0x00, 0x80), // 紫色
        Red                     = RGB(0xff, 0x00, 0x00), // 红色
        RosyBrown               = RGB(0xbc, 0x8f, 0x8f), // 玫瑰棕色
        RoyalBlue               = RGB(0x41, 0x69, 0xe1), // 皇家蓝色
        SaddleBrown             = RGB(0x8b, 0x45, 0x13), // 马鞍棕色
        Salmon                  = RGB(0xfa, 0x80, 0x72), // 鲑鱼色
        SandyBrown              = RGB(0xf4, 0xa4, 0x60), // 沙棕色
        SeaGreen                = RGB(0x2e, 0x8b, 0x57), // 海绿色
        SeaShell                = RGB(0xff, 0xf5, 0xee), // 海贝色
        Sienna                  = RGB(0xa0, 0x52, 0x2d), // 黄土赭色
        Silver                  = RGB(0xc0, 0xc0, 0xc0), // 银色
        SkyBlue                 = RGB(0x87, 0xce, 0xeb), // 天蓝色
        SlateBlue               = RGB(0x6a, 0x5a, 0xcd), // 青蓝色
        SlateGray               = RGB(0x70, 0x80, 0x90), // 青灰色
        Snow                    = RGB(0xff, 0xfa, 0xfa), // 雪白色
        SpringGreen             = RGB(0x00, 0xff, 0x7f), // 春绿色
        SteelBlue               = RGB(0x46, 0x82, 0xb4), // 钢蓝色
        Tan                     = RGB(0xd2, 0xb4, 0x8c), // 茶色
        Teal                    = RGB(0x00, 0x80, 0x80), // 鸭翅绿色
        Thistle                 = RGB(0xd8, 0xbf, 0xd8), // 蓟色
        Tomato                  = RGB(0xff, 0x63, 0x47), // 番茄色
        Turquoise               = RGB(0x40, 0xe0, 0xd0), // 青绿色
        Violet                  = RGB(0xee, 0x82, 0xee), // 紫罗兰色
        Wheat                   = RGB(0xf5, 0xde, 0xb3), // 小麦色
        White                   = RGB(0xff, 0xff, 0xff), // 白色
        WhiteSmoke              = RGB(0xf5, 0xf5, 0xf5), // 白烟色
        Yellow                  = RGB(0xff, 0xff, 0x00), // 黄色
        YellowGreen             = RGB(0x9a, 0xcd, 0x32), // 黄绿色
        ButtonFace              = RGB(0xf0, 0xf0, 0xf0), // 按钮表面色
        ButtonHighlight         = RGB(0xff, 0xff, 0xff), // 按钮高亮色
        ButtonShadow            = RGB(0xa0, 0xa0, 0xa0), // 按钮阴影色
        GradientActiveCaption   = RGB(0xb9, 0xd1, 0xea), // 渐变激活标题栏色
        GradientInactiveCaption = RGB(0xd7, 0xe4, 0xf2), // 渐变非激活标题栏色
        MenuBar                 = RGB(0xf0, 0xf0, 0xf0), // 菜单栏色
        MenuHighlight           = RGB(0x33, 0x99, 0xff), // 菜单高亮色
        RebeccaPurple           = RGB(0x66, 0x33, 0x99), // 紫罗兰色
    };
}

// Size.h


namespace sw
{
    /**
     * @brief 尺寸
     */
    struct Size {
        /**
         * @brief 宽度
         */
        double width;

        /**
         * @brief 高度
         */
        double height;

        /**
         * @brief 构造宽高均为0的Size结构体
         */
        Size();

        /**
         * @brief 构造指定宽高的Size结构体
         */
        Size(double width, double height);

        /**
         * @brief 从SIZE构造Size结构体
         */
        Size(const SIZE &size);

        /**
         * @brief 隐式转换SIZE
         */
        operator SIZE() const;

        /**
         * @brief 判断两个Size是否相等
         */
        bool operator==(const Size &other) const;

        /**
         * @brief 判断两个Size是否不相等
         */
        bool operator!=(const Size &other) const;

        /**
         * @brief 获取描述当前对象的字符串
         */
        std::wstring ToString() const;
    };
}

// Cursor.h


namespace sw
{
    /**
     * @brief 系统标准鼠标样式
     * @brief https://learn.microsoft.com/en-us/windows/win32/menurc/about-cursors
     */
    enum class StandardCursor {
        Arrow       = 32512, // Normal select
        IBeam       = 32513, // Text select
        Wait        = 32514, // Busy
        Cross       = 32515, // Precision select
        UpArrow     = 32516, // Alternate select
        Handwriting = 32631, // Handwriting
        SizeNWSE    = 32642, // Diagonal resize 1
        SizeNESW    = 32643, // Diagonal resize 2
        SizeWE      = 32644, // Horizontal resize
        SizeNS      = 32645, // Vertical resize
        SizeAll     = 32646, // Move
        No          = 32648, // Unavailable
        Hand        = 32649, // Link select
        AppStarting = 32650, // Working in background
        Help        = 32651, // Help select
        Pin         = 32671, // Location select
        Person      = 32672, // Person select
    };

    /**
     * @brief 用于获取鼠标句柄的工具类
     */
    class CursorHelper
    {
    private:
        CursorHelper() = delete;

    public:
        /**
         * @brief        获取系统标准鼠标样式句柄
         * @param cursor 鼠标样式
         * @return       鼠标句柄
         */
        static HCURSOR GetCursorHandle(StandardCursor cursor);

        /**
         * @brief            从指定模块中获取鼠标句柄
         * @param hInstance  DLL或EXE的模块句柄
         * @param resourceId 鼠标的资源序号
         * @return           成功则返回鼠标句柄，否则返回NULL
         */
        static HCURSOR GetCursorHandle(HINSTANCE hInstance, int resourceId);

        /**
         * @brief          从文件加载鼠标句柄
         * @param fileName 鼠标文件路径
         * @return         成功则返回鼠标句柄，否则返回NULL
         */
        static HCURSOR GetCursorHandle(const std::wstring &fileName);
    };
}

// Path.h


namespace sw
{
    /**
     * @brief 用于处理文件路径的工具类
     */
    class Path
    {
    private:
        Path() = delete;

    public:
        /**
         * @brief      获取文件名
         * @param path 文件的路径
         * @return     文件名，包含扩展名
         */
        static std::wstring GetFileName(const std::wstring &path);

        /**
         * @brief      获取文件名
         * @param path 文件的路径
         * @return     文件名，不含扩展名
         */
        static std::wstring GetFileNameWithoutExt(const std::wstring &path);

        /**
         * @brief      获取扩展名
         * @param path 文件的路径
         * @return     文件的扩展名，不包含前面的点
         */
        static std::wstring GetExtension(const std::wstring &path);

        /**
         * @brief      获取文件所在路径
         * @param path 文件的路径
         * @return     文件所在路径
         */
        static std::wstring GetDirectory(const std::wstring &path);

        /**
         * @brief       对路径进行拼接
         * @param paths 要拼接的路径
         * @return      完整的路径
         */
        static std::wstring Combine(std::initializer_list<std::wstring> paths);

        /**
         * @brief       对路径进行拼接
         * @param first 第一个路径
         * @param rest  要拼接的路径
         * @return      完整的路径
         */
        template <typename... Args>
        static inline std::wstring Combine(const std::wstring &first, const Args &...rest)
        {
            return Path::Combine({first, rest...});
        }

        /**
         * @brief       获取路径所对应的绝对路径
         * @param paths 要转换的路径
         * @return      若函数成功则返回绝对路径，否则返回空字符串
         */
        static std::wstring GetAbsolutePath(const std::wstring &path);
    };
}

// Point.h


namespace sw
{
    /**
     * @brief 表示相对于左上角的点坐标
     */
    struct Point {
        /**
         * @brief 横坐标
         */
        double x;

        /**
         * @brief 纵坐标
         */
        double y;

        /**
         * @brief 构造xy均为0的Point结构体
         */
        Point();

        /**
         * @brief 构造指定xy值的Point结构体
         */
        Point(double x, double y);

        /**
         * @brief 从POINT构造Point结构体
         */
        Point(const POINT &point);

        /**
         * @brief 隐式转换POINT
         */
        operator POINT() const;

        /**
         * @brief 判断两个Point是否相等
         */
        bool operator==(const Point &other) const;

        /**
         * @brief 判断两个Point是否不相等
         */
        bool operator!=(const Point &other) const;

        /**
         * @brief 获取描述当前对象的字符串
         */
        std::wstring ToString() const;
    };
}

// Color.h


namespace sw
{
    /**
     * @brief 颜色
     */
    struct Color {
        /**
         * @brief R分量
         */
        uint8_t r;

        /**
         * @brief G分量
         */
        uint8_t g;

        /**
         * @brief B分量
         */
        uint8_t b;

        /**
         * @brief 保留字段
         */
        uint8_t _reserved{0};

        /**
         * @brief 构造一个rgb分量均为0的Color结构体
         */
        Color();

        /**
         * @brief 通过rgb构造Color结构体
         */
        Color(uint8_t r, uint8_t g, uint8_t b);

        /**
         * @brief 通过KnownColor构造Color结构体
         */
        Color(KnownColor knownColor);

        /**
         * @brief 通过COLORREF构造Color结构体
         */
        Color(COLORREF color);

        /**
         * @brief 隐式转换COLORREF
         */
        operator COLORREF() const;

        /**
         * @brief 判断两个Color是否相等
         */
        bool operator==(const Color &other) const;

        /**
         * @brief 判断两个Color是否不相等
         */
        bool operator!=(const Color &other) const;

        /**
         * @brief 获取描述当前对象的字符串
         */
        std::wstring ToString() const;
    };
}

// RoutedEvent.h


namespace sw
{
    /**
     * @brief 路由事件类型枚举
     */
    enum RoutedEventType : uint32_t {

        // 从该值开始到RoutedEventType_UserEnd结束表示用户可以自定义路由事件的值范围
        RoutedEventType_User = 0,

        // 用户自定义路由事件的值的最大值
        RoutedEventType_UserEnd = 0x80000000,

        // 尺寸改变，参数类型为sw::SizeChangedEventArgs
        UIElement_SizeChanged,

        // 位置改变，参数类型为sw::PositionChangedEventArgs
        UIElement_PositionChanged,

        // Text属性发生变化，参数类型为sw::RoutedEventArgs
        UIElement_TextChanged,

        // 获取到焦点，参数类型为sw::RoutedEventArgs
        UIElement_GotFocus,

        // 失去焦点，参数类型为sw::RoutedEventArgs
        UIElement_LostFocus,

        // 输入字符，参数类型为sw::GotCharEventArgs
        UIElement_GotChar,

        // 键盘按键按下，参数类型为sw::KeyDownEventArgs
        UIElement_KeyDown,

        // 键盘按键抬起，参数类型为sw::KeyUpEventArgs
        UIElement_KeyUp,

        // 鼠标移动，参数类型为sw::MouseMoveEventArgs
        UIElement_MouseMove,

        // 鼠标离开，参数类型为sw::RoutedEventArgs
        UIElement_MouseLeave,

        // 鼠标滚轮滚动，参数类型为sw::MouseWheelEventArgs
        UIElement_MouseWheel,

        // 鼠标按键按下，参数类型为sw::MouseButtonDownEventArgs
        UIElement_MouseButtonDown,

        // 鼠标按键抬起，参数类型为sw::MouseButtonUpEventArgs
        UIElement_MouseButtonUp,

        // 要显示用户自定义的上下文菜单前触发该事件，参数类型为sw::ShowContextMenuEventArgs
        UIElement_ShowContextMenu,

        // 接收到文件拖放，参数类型为sw::DropFilesEventArgs
        UIElement_DropFiles,

        // 窗口正在关闭，参数类型为sw::WindowClosingEventArgs
        Window_Closing,

        // 窗口已关闭，参数类型为sw::RoutedEventArgs
        Window_Closed,

        // 窗口成为前台窗口，参数类型为sw::RoutedEventArgs
        Window_Actived,

        // 窗口成为后台窗口，参数类型为sw::RoutedEventArgs
        Window_Inactived,

        // 按钮被单击，参数类型为sw::RoutedEventArgs
        ButtonBase_Clicked,

        // 按钮被双击，参数类型为sw::RoutedEventArgs
        ButtonBase_DoubleClicked,

        // 列表视图/列表框/组合框的选中项改变，参数类型为sw::RoutedEventArgs
        ItemsControl_SelectionChanged,

        // 列表视图某个复选框的选中状态改变，参数类型为sw::ListViewCheckStateChangedEventArgs
        ListView_CheckStateChanged,

        // 鼠标左键单击列表视图的列标题，参数类型为sw::ListViewHeaderClickedEventArgs
        ListView_HeaderClicked,

        // 鼠标左键双击列表视图的列标题，参数类型为sw::ListViewHeaderClickedEventArgs
        ListView_HeaderDoubleClicked,

        // 鼠标左键单击列表视图某个项，参数类型为sw::ListViewItemClickedEventArgs
        ListView_ItemClicked,

        // 鼠标左键单击列表视图某个项，参数类型为sw::ListViewItemClickedEventArgs
        ListView_ItemDoubleClicked,

        // 编辑状态结束，参数类型为sw::ListViewEndEditEventArgs
        ListView_EndEdit,

        // 滑块的值被改变，参数类型为sw::RoutedEventArgs
        Slider_ValueChanged,

        // 滑块被释放，参数类型为sw::RoutedEventArgs
        Slider_EndTrack,

        // 窗口/面板滚动条滚动，参数类型为sw::ScrollingEventArgs
        Layer_Scrolling,

        // SelectedIndex属性被改变，参数类型为sw::RoutedEventArgs
        TabControl_SelectedIndexChanged,

        // DateTimePicker控件的时间改变，参数类型为sw::DateTimePickerTimeChangedEventArgs
        DateTimePicker_TimeChanged,

        // 月历控件的时间改变，参数类型为sw::MonthCalendarTimeChangedEventArgs
        MonthCalendar_TimeChanged,

        // IP地址框地址被改变，参数类型为sw::RoutedEventArgs
        IPAddressControl_AddressChanged,

        // SysLink控件链接被单击，参数类型为sw::SysLinkClickedEventArgs
        SysLink_Clicked,

        // 热键框的值被改变，参数类型为sw::HotKeyValueChangedEventArgs
        HotKeyControl_ValueChanged,
    };

    /*================================================================================*/

    class UIElement; // UIElement.h

    /**
     * @brief 路由事件的参数
     */
    struct RoutedEventArgs {
        /**
         * @brief 事件类型
         */
        RoutedEventType eventType;

        /**
         * @brief 事件是否已被处理，若将此字段设为true，则事件不会继续往上传递
         */
        bool handled = false;

        /**
         * @brief 表示是否已处理事件所对应的Windows消息，对于部分消息将字段设为true可取消对DefaultWndProc的调用，若当前事件无对应消息则该字段无意义
         */
        bool handledMsg = false;

        /**
         * @brief 事件源，指向触发当前事件的UIElement
         */
        UIElement *source = nullptr;

        /**
         * @brief 原始事件源，指向最初触发事件的UIElement
         */
        UIElement *originalSource = nullptr;

        /**
         * @brief RoutedEventArgs构造函数
         */
        RoutedEventArgs(RoutedEventType eventType);
    };

    /**
     * @brief 路由事件类型
     * @note  第一个参数为注册事件监听器的元素，第二个参数为具体的事件参数
     */
    using RoutedEventHandler = Action<UIElement &, RoutedEventArgs &>;
}

// Property.h


#define _SW_DEFINE_OPERATION_HELPER(NAME, OP)                                                                                   \
    template <typename T, typename U, typename = void>                                                                          \
    struct NAME : std::false_type {                                                                                             \
    };                                                                                                                          \
    template <typename T, typename U>                                                                                           \
    struct NAME<T,                                                                                                              \
                U,                                                                                                              \
                typename std::enable_if<true, decltype(void(std::declval<T>() OP std::declval<U>()))>::type> : std::true_type { \
        using type = decltype(std::declval<T>() OP std::declval<U>());                                                          \
    }

#define _SW_DEFINE_UNARY_OPERATION_HELPER(NAME, OP)                                                           \
    template <typename T, typename = void>                                                                    \
    struct NAME : std::false_type {                                                                           \
    };                                                                                                        \
    template <typename T>                                                                                     \
    struct NAME<T,                                                                                            \
                typename std::enable_if<true, decltype(void(OP std::declval<T>()))>::type> : std::true_type { \
        using type = decltype(OP std::declval<T>());                                                          \
    }

namespace sw
{
    // 向前声明
    template <typename T>
    class Property;

    // 向前声明
    template <typename T>
    class ReadOnlyProperty;

    // 向前声明
    template <typename T>
    class WriteOnlyProperty;

    // SFINAE templates
    _SW_DEFINE_OPERATION_HELPER(_AddOperationHelper, +);
    _SW_DEFINE_OPERATION_HELPER(_SubOperationHelper, -);
    _SW_DEFINE_OPERATION_HELPER(_MulOperationHelper, *);
    _SW_DEFINE_OPERATION_HELPER(_DivOperationHelper, /);
    _SW_DEFINE_OPERATION_HELPER(_ModOperationHelper, %);
    _SW_DEFINE_OPERATION_HELPER(_EqOperationHelper, ==);
    _SW_DEFINE_OPERATION_HELPER(_NeOperationHelper, !=);
    _SW_DEFINE_OPERATION_HELPER(_LtOperationHelper, <);
    _SW_DEFINE_OPERATION_HELPER(_LeOperationHelper, <=);
    _SW_DEFINE_OPERATION_HELPER(_GtOperationHelper, >);
    _SW_DEFINE_OPERATION_HELPER(_GeOperationHelper, >=);
    _SW_DEFINE_OPERATION_HELPER(_BitAndOperationHelper, &);
    _SW_DEFINE_OPERATION_HELPER(_BitOrOperationHelper, |);
    _SW_DEFINE_OPERATION_HELPER(_BitXorOperationHelper, ^);
    _SW_DEFINE_OPERATION_HELPER(_ShlOperationHelper, <<);
    _SW_DEFINE_OPERATION_HELPER(_ShrOperationHelper, >>);
    _SW_DEFINE_OPERATION_HELPER(_LogicAndOperationHelper, &&);
    _SW_DEFINE_OPERATION_HELPER(_LogicOrOperationHelper, ||);
    _SW_DEFINE_UNARY_OPERATION_HELPER(_LogicNotOperationHelper, !);
    _SW_DEFINE_UNARY_OPERATION_HELPER(_BitNotOperationHelper, ~);
    _SW_DEFINE_UNARY_OPERATION_HELPER(_DerefOperationHelper, *);
    _SW_DEFINE_UNARY_OPERATION_HELPER(_AddrOperationHelper, &);
    _SW_DEFINE_UNARY_OPERATION_HELPER(_UnaryPlusOperationHelper, +);
    _SW_DEFINE_UNARY_OPERATION_HELPER(_UnaryMinusOperationHelper, -);
    // _SW_DEFINE_UNARY_OPERATION_HELPER(_PreIncOperationHelper, ++);
    // _SW_DEFINE_UNARY_OPERATION_HELPER(_PreDecOperationHelper, --);

    /**
     * @brief _IsProperty的实现
     */
    template <typename>
    struct _IsPropertyImpl : std::false_type {
    };

    /**
     * @brief _IsPropertyImpl模板特化
     */
    template <typename T>
    struct _IsPropertyImpl<Property<T>> : std::true_type {
    };

    /**
     * @brief _IsPropertyImpl模板特化
     */
    template <typename T>
    struct _IsPropertyImpl<ReadOnlyProperty<T>> : std::true_type {
    };

    /**
     * @brief _IsPropertyImpl模板特化
     */
    template <typename T>
    struct _IsPropertyImpl<WriteOnlyProperty<T>> : std::true_type {
    };

    /**
     * @brief 判断类型是否为属性的辅助模板
     */
    template <typename T>
    struct _IsProperty : _IsPropertyImpl<typename std::decay<T>::type> {
    };

    /**
     * @brief 判断类型是否可以使用[]操作符的辅助模板
     */
    template <typename T, typename U, typename = void>
    struct _BracketOperationHelper : std::false_type {
    };

    /**
     * @brief _BracketOperationHelper模板特化
     */
    template <typename T, typename U>
    struct _BracketOperationHelper<
        T, U,
        typename std::enable_if<true, decltype(void(std::declval<T>()[std::declval<U>()]))>::type> : std::true_type {
        using type = decltype(std::declval<T>()[std::declval<U>()]);
    };

    /**
     * @brief 判断类型是否可以显式转换的辅助模板
     */
    template <typename TFrom, typename TTo, typename = void>
    struct _IsExplicitlyConvertable : std::false_type {
    };

    /**
     * @brief _IsExplicitlyConvertable模板特化
     */
    template <typename TFrom, typename TTo>
    struct _IsExplicitlyConvertable<
        TFrom, TTo,
        typename std::enable_if<true, decltype(void(static_cast<TTo>(std::declval<TFrom>())))>::type> : std::true_type {
    };

    /**
     * @brief 判断类型是否有operator->的辅助模板
     */
    template <typename T, typename = void>
    struct _HasArrowOperator : std::false_type {
    };

    /**
     * @brief _HasArrowOperator模板特化
     */
    template <typename T>
    struct _HasArrowOperator<
        T, typename std::enable_if<true, decltype(void(std::declval<T>().operator->()))>::type> : std::true_type {
        using type = decltype(std::declval<T>().operator->());
    };

    /*================================================================================*/

    /**
     * @brief 字段访问器，用于实现使用operator->取属性字段
     */
    template <typename T>
    struct FieldsAccessor {
        /**
         * @brief 字段访问器所维护的值
         */
        T value;

        /**
         * @brief 构造字段访问器
         */
        template <typename... Args>
        FieldsAccessor(Args &&...args)
            : value(std::forward<Args>(args)...)
        {
        }

        /**
         * @brief 指针类型，直接返回值
         */
        template <typename U = T>
        typename std::enable_if<std::is_pointer<U>::value, U>::type operator->()
        {
            return value;
        }

        /**
         * @brief 非指针类型，且无operator->，返回值的地址
         */
        template <typename U = T>
        typename std::enable_if<!std::is_pointer<U>::value && !_HasArrowOperator<U>::value, U *>::type operator->()
        {
            return &value;
        }

        /**
         * @brief 非指针类型，且有operator->，转发operator->
         */
        template <typename U = T>
        typename std::enable_if<!std::is_pointer<U>::value && _HasArrowOperator<U>::value, typename _HasArrowOperator<U>::type>::type operator->()
        {
            return value.operator->();
        }
    };

    /*================================================================================*/

    /**
     * @brief 属性基类模板
     */
    template <typename T, typename TDerived>
    class PropertyBase
    {
    public:
        // 使用默认构造函数
        PropertyBase() = default;

        // 删除移动构造
        PropertyBase(PropertyBase &&) = delete;

        // 删除拷贝构造
        PropertyBase(const PropertyBase &) = delete;

        // 删除移动赋值
        PropertyBase &operator=(PropertyBase &&) = delete;

        // /**
        //  * @brief 获取属性值，由子类实现
        //  */
        // T GetterImpl() const;

        // /**
        //  * @brief 设置属性值，由子类实现
        //  */
        // void SetterImpl(const T &value) const;

        /**
         * @brief 访问属性字段，可由子类重写
         */
        FieldsAccessor<T> AccessFields() const
        {
            return FieldsAccessor<T>(this->Get());
        }

        /**
         * @brief 获取属性值
         */
        T Get() const
        {
            return static_cast<const TDerived *>(this)->GetterImpl();
        }

        /**
         * @brief 设置属性值
         */
        void Set(const T &value) const
        {
            static_cast<const TDerived *>(this)->SetterImpl(value);
        }

        /**
         * @brief 取属性字段
         */
        auto operator->() const
        {
            return static_cast<const TDerived *>(this)->AccessFields();
        }

        /**
         * @brief 隐式转换
         */
        operator T() const
        {
            return this->Get();
        }

        /**
         * @brief 隐式转换
         */
        template <
            typename U = T,
            typename   = typename std::enable_if<!std::is_arithmetic<T>::value && std::is_convertible<T, U>::value, U>::type>
        operator U() const
        {
            return static_cast<U>(this->Get());
        }

        /**
         * @brief 显式转换
         */
        template <
            typename U = T,
            typename   = typename std::enable_if<!std::is_arithmetic<T>::value && !std::is_convertible<T, U>::value, U>::type,
            typename   = typename std::enable_if<!std::is_arithmetic<T>::value && _IsExplicitlyConvertable<T, U>::value, U>::type>
        explicit operator U() const
        {
            return static_cast<U>(this->Get());
        }

        /**
         * @brief 设置属性值
         */
        TDerived &operator=(const T &value)
        {
            this->Set(value);
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 设置属性值
         */
        const TDerived &operator=(const T &value) const
        {
            this->Set(value);
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 设置属性值
         */
        TDerived &operator=(const PropertyBase &prop)
        {
            this->Set(prop.Get());
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 设置属性值
         */
        const TDerived &operator=(const PropertyBase &prop) const
        {
            this->Set(prop.Get());
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 加赋值运算
         */
        template <typename U = T>
        typename std::enable_if<_AddOperationHelper<T, U>::value, TDerived &>::type operator+=(const U &value)
        {
            this->Set(this->Get() + value);
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 加赋值运算
         */
        template <typename U = T>
        typename std::enable_if<_AddOperationHelper<T, U>::value, const TDerived &>::type operator+=(const U &value) const
        {
            this->Set(this->Get() + value);
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 加赋值运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_AddOperationHelper<T, U>::value, TDerived &>::type operator+=(const PropertyBase<U, D> &prop)
        {
            this->Set(this->Get() + prop.Get());
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 加赋值运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_AddOperationHelper<T, U>::value, const TDerived &>::type operator+=(const PropertyBase<U, D> &prop) const
        {
            this->Set(this->Get() + prop.Get());
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 减赋值运算
         */
        template <typename U = T>
        typename std::enable_if<_SubOperationHelper<T, U>::value, TDerived &>::type operator-=(const U &value)
        {
            this->Set(this->Get() - value);
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 减赋值运算
         */
        template <typename U = T>
        typename std::enable_if<_SubOperationHelper<T, U>::value, const TDerived &>::type operator-=(const U &value) const
        {
            this->Set(this->Get() - value);
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 减赋值运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_SubOperationHelper<T, U>::value, TDerived &>::type operator-=(const PropertyBase<U, D> &prop)
        {
            this->Set(this->Get() - prop.Get());
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 减赋值运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_SubOperationHelper<T, U>::value, const TDerived &>::type operator-=(const PropertyBase<U, D> &prop) const
        {
            this->Set(this->Get() - prop.Get());
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 乘赋值运算
         */
        template <typename U = T>
        typename std::enable_if<_MulOperationHelper<T, U>::value, TDerived &>::type operator*=(const U &value)
        {
            this->Set(this->Get() * value);
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 乘赋值运算
         */
        template <typename U = T>
        typename std::enable_if<_MulOperationHelper<T, U>::value, const TDerived &>::type operator*=(const U &value) const
        {
            this->Set(this->Get() * value);
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 乘赋值运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_MulOperationHelper<T, U>::value, TDerived &>::type operator*=(const PropertyBase<U, D> &prop)
        {
            this->Set(this->Get() * prop.Get());
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 乘赋值运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_MulOperationHelper<T, U>::value, const TDerived &>::type operator*=(const PropertyBase<U, D> &prop) const
        {
            this->Set(this->Get() * prop.Get());
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 除赋值运算
         */
        template <typename U = T>
        typename std::enable_if<_DivOperationHelper<T, U>::value, TDerived &>::type operator/=(const U &value)
        {
            this->Set(this->Get() / value);
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 除赋值运算
         */
        template <typename U = T>
        typename std::enable_if<_DivOperationHelper<T, U>::value, const TDerived &>::type operator/=(const U &value) const
        {
            this->Set(this->Get() / value);
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 除赋值运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_DivOperationHelper<T, U>::value, TDerived &>::type operator/=(const PropertyBase<U, D> &prop)
        {
            this->Set(this->Get() / prop.Get());
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 除赋值运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_DivOperationHelper<T, U>::value, const TDerived &>::type operator/=(const PropertyBase<U, D> &prop) const
        {
            this->Set(this->Get() / prop.Get());
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 前置自增运算
         */
        template <typename U = T>
        typename std::enable_if<_AddOperationHelper<U, int>::value, TDerived &>::type operator++()
        {
            this->Set(this->Get() + 1);
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 前置自增运算
         */
        template <typename U = T>
        typename std::enable_if<_AddOperationHelper<U, int>::value, const TDerived &>::type operator++() const
        {
            this->Set(this->Get() + 1);
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 前置自减运算
         */
        template <typename U = T>
        typename std::enable_if<_SubOperationHelper<U, int>::value, TDerived &>::type operator--()
        {
            this->Set(this->Get() - 1);
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 前置自减运算
         */
        template <typename U = T>
        typename std::enable_if<_SubOperationHelper<U, int>::value, const TDerived &>::type operator--() const
        {
            this->Set(this->Get() - 1);
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 后置自增运算
         */
        template <typename U = T>
        typename std::enable_if<_AddOperationHelper<U, int>::value, T>::type operator++(int) const
        {
            T oldval = this->Get();
            this->Set(oldval + 1);
            return oldval;
        }

        /**
         * @brief 后置自减运算
         */
        template <typename U = T>
        typename std::enable_if<_SubOperationHelper<U, int>::value, T>::type operator--(int) const
        {
            T oldval = this->Get();
            this->Set(oldval - 1);
            return oldval;
        }

        /**
         * @brief 按位与赋值运算
         */
        template <typename U = T>
        typename std::enable_if<_BitAndOperationHelper<T, U>::value, TDerived &>::type operator&=(const U &value)
        {
            this->Set(this->Get() & value);
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 按位与赋值运算
         */
        template <typename U = T>
        typename std::enable_if<_BitAndOperationHelper<T, U>::value, const TDerived &>::type operator&=(const U &value) const
        {
            this->Set(this->Get() & value);
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 按位与赋值运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_BitAndOperationHelper<T, U>::value, TDerived &>::type operator&=(const PropertyBase<U, D> &prop)
        {
            this->Set(this->Get() & prop.Get());
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 按位与赋值运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_BitAndOperationHelper<T, U>::value, const TDerived &>::type operator&=(const PropertyBase<U, D> &prop) const
        {
            this->Set(this->Get() & prop.Get());
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 按位或赋值运算
         */
        template <typename U = T>
        typename std::enable_if<_BitOrOperationHelper<T, U>::value, TDerived &>::type operator|=(const U &value)
        {
            this->Set(this->Get() | value);
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 按位或赋值运算
         */
        template <typename U = T>
        typename std::enable_if<_BitOrOperationHelper<T, U>::value, const TDerived &>::type operator|=(const U &value) const
        {
            this->Set(this->Get() | value);
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 按位或赋值运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_BitOrOperationHelper<T, U>::value, TDerived &>::type operator|=(const PropertyBase<U, D> &prop)
        {
            this->Set(this->Get() | prop.Get());
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 按位或赋值运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_BitOrOperationHelper<T, U>::value, const TDerived &>::type operator|=(const PropertyBase<U, D> &prop) const
        {
            this->Set(this->Get() | prop.Get());
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 按位异或赋值运算
         */
        template <typename U = T>
        typename std::enable_if<_BitXorOperationHelper<T, U>::value, TDerived &>::type operator^=(const U &value)
        {
            this->Set(this->Get() ^ value);
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 按位异或赋值运算
         */
        template <typename U = T>
        typename std::enable_if<_BitXorOperationHelper<T, U>::value, const TDerived &>::type operator^=(const U &value) const
        {
            this->Set(this->Get() ^ value);
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 按位异或赋值运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_BitXorOperationHelper<T, U>::value, TDerived &>::type operator^=(const PropertyBase<U, D> &prop)
        {
            this->Set(this->Get() ^ prop.Get());
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 按位异或赋值运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_BitXorOperationHelper<T, U>::value, const TDerived &>::type operator^=(const PropertyBase<U, D> &prop) const
        {
            this->Set(this->Get() ^ prop.Get());
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 左移赋值运算
         */
        template <typename U = T>
        typename std::enable_if<_ShlOperationHelper<T, U>::value, TDerived &>::type operator<<=(const U &value)
        {
            this->Set(this->Get() << value);
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 左移赋值运算
         */
        template <typename U = T>
        typename std::enable_if<_ShlOperationHelper<T, U>::value, const TDerived &>::type operator<<=(const U &value) const
        {
            this->Set(this->Get() << value);
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 左移赋值运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_ShlOperationHelper<T, U>::value, TDerived &>::type operator<<=(const PropertyBase<U, D> &prop)
        {
            this->Set(this->Get() << prop.Get());
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 左移赋值运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_ShlOperationHelper<T, U>::value, const TDerived &>::type operator<<=(const PropertyBase<U, D> &prop) const
        {
            this->Set(this->Get() << prop.Get());
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 右移赋值运算
         */
        template <typename U = T>
        typename std::enable_if<_ShrOperationHelper<T, U>::value, TDerived &>::type operator>>=(const U &value)
        {
            this->Set(this->Get() >> value);
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 右移赋值运算
         */
        template <typename U = T>
        typename std::enable_if<_ShrOperationHelper<T, U>::value, const TDerived &>::type operator>>=(const U &value) const
        {
            this->Set(this->Get() >> value);
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 右移赋值运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_ShrOperationHelper<T, U>::value, TDerived &>::type operator>>=(const PropertyBase<U, D> &prop)
        {
            this->Set(this->Get() >> prop.Get());
            return *static_cast<TDerived *>(this);
        }

        /**
         * @brief 右移赋值运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_ShrOperationHelper<T, U>::value, const TDerived &>::type operator>>=(const PropertyBase<U, D> &prop) const
        {
            this->Set(this->Get() >> prop.Get());
            return *static_cast<const TDerived *>(this);
        }

        /**
         * @brief 逻辑非运算
         */
        template <typename U = T>
        typename std::enable_if<_LogicNotOperationHelper<U>::value, typename _LogicNotOperationHelper<U>::type>::type operator!() const
        {
            return !this->Get();
        }

        /**
         * @brief 按位非运算
         */
        template <typename U = T>
        typename std::enable_if<_BitNotOperationHelper<U>::value, typename _BitNotOperationHelper<U>::type>::type operator~() const
        {
            return ~this->Get();
        }

        /**
         * @brief 解引用运算
         */
        template <typename U = T>
        typename std::enable_if<_DerefOperationHelper<U>::value, typename _DerefOperationHelper<U>::type>::type operator*() const
        {
            return *this->Get();
        }

        /**
         * @brief 地址运算
         */
        template <typename U = T>
        typename std::enable_if<_AddrOperationHelper<U>::value, typename _AddrOperationHelper<U>::type>::type operator&() const
        {
            return &this->Get();
        }

        /**
         * @brief 一元加运算
         */
        template <typename U = T>
        typename std::enable_if<_UnaryPlusOperationHelper<U>::value, typename _UnaryPlusOperationHelper<U>::type>::type operator+() const
        {
            return +this->Get();
        }

        /**
         * @brief 一元减运算
         */
        template <typename U = T>
        typename std::enable_if<_UnaryMinusOperationHelper<U>::value, typename _UnaryMinusOperationHelper<U>::type>::type operator-() const
        {
            return -this->Get();
        }

        /**
         * @brief 加法运算
         */
        template <typename U = T>
        typename std::enable_if<_AddOperationHelper<T, U>::value, typename _AddOperationHelper<T, U>::type>::type operator+(const U &value) const
        {
            return this->Get() + value;
        }

        /**
         * @brief 加法运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_AddOperationHelper<T, U>::value, typename _AddOperationHelper<T, U>::type>::type operator+(const PropertyBase<U, D> &prop) const
        {
            return this->Get() + prop.Get();
        }

        /**
         * @brief 减法运算
         */
        template <typename U = T>
        typename std::enable_if<_SubOperationHelper<T, U>::value, typename _SubOperationHelper<T, U>::type>::type operator-(const U &value) const
        {
            return this->Get() - value;
        }

        /**
         * @brief 减法运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_SubOperationHelper<T, U>::value, typename _SubOperationHelper<T, U>::type>::type operator-(const PropertyBase<U, D> &prop) const
        {
            return this->Get() - prop.Get();
        }

        /**
         * @brief 乘法运算
         */
        template <typename U = T>
        typename std::enable_if<_MulOperationHelper<T, U>::value, typename _MulOperationHelper<T, U>::type>::type operator*(const U &value) const
        {
            return this->Get() * value;
        }

        /**
         * @brief 乘法运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_MulOperationHelper<T, U>::value, typename _MulOperationHelper<T, U>::type>::type operator*(const PropertyBase<U, D> &prop) const
        {
            return this->Get() * prop.Get();
        }

        /**
         * @brief 除法运算
         */
        template <typename U = T>
        typename std::enable_if<_DivOperationHelper<T, U>::value, typename _DivOperationHelper<T, U>::type>::type operator/(const U &value) const
        {
            return this->Get() / value;
        }

        /**
         * @brief 除法运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_DivOperationHelper<T, U>::value, typename _DivOperationHelper<T, U>::type>::type operator/(const PropertyBase<U, D> &prop) const
        {
            return this->Get() / prop.Get();
        }

        /**
         * @brief 取模运算
         */
        template <typename U = T>
        typename std::enable_if<_ModOperationHelper<T, U>::value, typename _ModOperationHelper<T, U>::type>::type operator%(const U &value) const
        {
            return this->Get() % value;
        }

        /**
         * @brief 取模运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_ModOperationHelper<T, U>::value, typename _ModOperationHelper<T, U>::type>::type operator%(const PropertyBase<U, D> &prop) const
        {
            return this->Get() % prop.Get();
        }

        /**
         * @brief 等于运算
         */
        template <typename U = T>
        typename std::enable_if<_EqOperationHelper<T, U>::value, typename _EqOperationHelper<T, U>::type>::type operator==(const U &value) const
        {
            return this->Get() == value;
        }

        /**
         * @brief 等于运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_EqOperationHelper<T, U>::value, typename _EqOperationHelper<T, U>::type>::type operator==(const PropertyBase<U, D> &prop) const
        {
            return this->Get() == prop.Get();
        }

        /**
         * @brief 不等于运算
         */
        template <typename U = T>
        typename std::enable_if<_NeOperationHelper<T, U>::value, typename _NeOperationHelper<T, U>::type>::type operator!=(const U &value) const
        {
            return this->Get() != value;
        }

        /**
         * @brief 不等于运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_NeOperationHelper<T, U>::value, typename _NeOperationHelper<T, U>::type>::type operator!=(const PropertyBase<U, D> &prop) const
        {
            return this->Get() != prop.Get();
        }

        /**
         * @brief 小于运算
         */
        template <typename U = T>
        typename std::enable_if<_LtOperationHelper<T, U>::value, typename _LtOperationHelper<T, U>::type>::type operator<(const U &value) const
        {
            return this->Get() < value;
        }

        /**
         * @brief 小于运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_LtOperationHelper<T, U>::value, typename _LtOperationHelper<T, U>::type>::type operator<(const PropertyBase<U, D> &prop) const
        {
            return this->Get() < prop.Get();
        }

        /**
         * @brief 小于等于运算
         */
        template <typename U = T>
        typename std::enable_if<_LeOperationHelper<T, U>::value, typename _LeOperationHelper<T, U>::type>::type operator<=(const U &value) const
        {
            return this->Get() <= value;
        }

        /**
         * @brief 小于等于运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_LeOperationHelper<T, U>::value, typename _LeOperationHelper<T, U>::type>::type operator<=(const PropertyBase<U, D> &prop) const
        {
            return this->Get() <= prop.Get();
        }

        /**
         * @brief 大于运算
         */
        template <typename U = T>
        typename std::enable_if<_GtOperationHelper<T, U>::value, typename _GtOperationHelper<T, U>::type>::type operator>(const U &value) const
        {
            return this->Get() > value;
        }

        /**
         * @brief 大于运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_GtOperationHelper<T, U>::value, typename _GtOperationHelper<T, U>::type>::type operator>(const PropertyBase<U, D> &prop) const
        {
            return this->Get() > prop.Get();
        }

        /**
         * @brief 大于等于运算
         */
        template <typename U = T>
        typename std::enable_if<_GeOperationHelper<T, U>::value, typename _GeOperationHelper<T, U>::type>::type operator>=(const U &value) const
        {
            return this->Get() >= value;
        }

        /**
         * @brief 大于等于运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_GeOperationHelper<T, U>::value, typename _GeOperationHelper<T, U>::type>::type operator>=(const PropertyBase<U, D> &prop) const
        {
            return this->Get() >= prop.Get();
        }

        /**
         * @brief 按位与运算
         */
        template <typename U = T>
        typename std::enable_if<_BitAndOperationHelper<T, U>::value, typename _BitAndOperationHelper<T, U>::type>::type operator&(const U &value) const
        {
            return this->Get() & value;
        }

        /**
         * @brief 按位与运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_BitAndOperationHelper<T, U>::value, typename _BitAndOperationHelper<T, U>::type>::type operator&(const PropertyBase<U, D> &prop) const
        {
            return this->Get() & prop.Get();
        }

        /**
         * @brief 按位或运算
         */
        template <typename U = T>
        typename std::enable_if<_BitOrOperationHelper<T, U>::value, typename _BitOrOperationHelper<T, U>::type>::type operator|(const U &value) const
        {
            return this->Get() | value;
        }

        /**
         * @brief 按位或运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_BitOrOperationHelper<T, U>::value, typename _BitOrOperationHelper<T, U>::type>::type operator|(const PropertyBase<U, D> &prop) const
        {
            return this->Get() | prop.Get();
        }

        /**
         * @brief 按位异或运算
         */
        template <typename U = T>
        typename std::enable_if<_BitXorOperationHelper<T, U>::value, typename _BitXorOperationHelper<T, U>::type>::type operator^(const U &value) const
        {
            return this->Get() ^ value;
        }

        /**
         * @brief 按位异或运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_BitXorOperationHelper<T, U>::value, typename _BitXorOperationHelper<T, U>::type>::type operator^(const PropertyBase<U, D> &prop) const
        {
            return this->Get() ^ prop.Get();
        }

        /**
         * @brief 左移运算
         */
        template <typename U = T>
        typename std::enable_if<_ShlOperationHelper<T, U>::value, typename _ShlOperationHelper<T, U>::type>::type operator<<(const U &value) const
        {
            return this->Get() << value;
        }

        /**
         * @brief 左移运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_ShlOperationHelper<T, U>::value, typename _ShlOperationHelper<T, U>::type>::type operator<<(const PropertyBase<U, D> &prop) const
        {
            return this->Get() << prop.Get();
        }

        /**
         * @brief 右移运算
         */
        template <typename U = T>
        typename std::enable_if<_ShrOperationHelper<T, U>::value, typename _ShrOperationHelper<T, U>::type>::type operator>>(const U &value) const
        {
            return this->Get() >> value;
        }

        /**
         * @brief 右移运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_ShrOperationHelper<T, U>::value, typename _ShrOperationHelper<T, U>::type>::type operator>>(const PropertyBase<U, D> &prop) const
        {
            return this->Get() >> prop.Get();
        }

        /**
         * @brief 逻辑与运算
         */
        template <typename U = T>
        typename std::enable_if<_LogicAndOperationHelper<T, U>::value, typename _LogicAndOperationHelper<T, U>::type>::type operator&&(const U &value) const
        {
            return this->Get() && value;
        }

        /**
         * @brief 逻辑与运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_LogicAndOperationHelper<T, U>::value, typename _LogicAndOperationHelper<T, U>::type>::type operator&&(const PropertyBase<U, D> &prop) const
        {
            return this->Get() && prop.Get();
        }

        /**
         * @brief 逻辑或运算
         */
        template <typename U = T>
        typename std::enable_if<_LogicOrOperationHelper<T, U>::value, typename _LogicOrOperationHelper<T, U>::type>::type operator||(const U &value) const
        {
            return this->Get() || value;
        }

        /**
         * @brief 逻辑或运算
         */
        template <typename D, typename U = T>
        typename std::enable_if<_LogicOrOperationHelper<T, U>::value, typename _LogicOrOperationHelper<T, U>::type>::type operator||(const PropertyBase<U, D> &prop) const
        {
            return this->Get() || prop.Get();
        }

        /**
         * @brief 下标运算
         */
        template <typename U>
        typename std::enable_if<_BracketOperationHelper<T, U>::value, typename _BracketOperationHelper<T, U>::type>::type operator[](const U &value) const
        {
            return this->Get()[value];
        }

        /**
         * @brief 下标运算
         */
        template <typename D, typename U>
        typename std::enable_if<_BracketOperationHelper<T, U>::value, typename _BracketOperationHelper<T, U>::type>::type operator[](const PropertyBase<U, D> &prop) const
        {
            return this->Get()[prop.Get()];
        }
    };

    /*================================================================================*/

    /**
     * @brief 加法运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _AddOperationHelper<T &, U>::value, typename _AddOperationHelper<T &, U>::type>::type
    operator+(T &left, const PropertyBase<U, D> &right)
    {
        return left + right.Get();
    }

    /**
     * @brief 加法运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _AddOperationHelper<const T &, U>::value, typename _AddOperationHelper<const T &, U>::type>::type
    operator+(const T &left, const PropertyBase<U, D> &right)
    {
        return left + right.Get();
    }

    /**
     * @brief 减法运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _SubOperationHelper<T &, U>::value, typename _SubOperationHelper<T &, U>::type>::type
    operator-(T &left, const PropertyBase<U, D> &right)
    {
        return left - right.Get();
    }

    /**
     * @brief 减法运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _SubOperationHelper<const T &, U>::value, typename _SubOperationHelper<const T &, U>::type>::type
    operator-(const T &left, const PropertyBase<U, D> &right)
    {
        return left - right.Get();
    }

    /**
     * @brief 乘法运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _MulOperationHelper<T &, U>::value, typename _MulOperationHelper<T &, U>::type>::type
    operator*(T &left, const PropertyBase<U, D> &right)
    {
        return left * right.Get();
    }

    /**
     * @brief 乘法运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _MulOperationHelper<const T &, U>::value, typename _MulOperationHelper<const T &, U>::type>::type
    operator*(const T &left, const PropertyBase<U, D> &right)
    {
        return left * right.Get();
    }

    /**
     * @brief 除法运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _DivOperationHelper<T &, U>::value, typename _DivOperationHelper<T &, U>::type>::type
    operator/(T &left, const PropertyBase<U, D> &right)
    {
        return left / right.Get();
    }

    /**
     * @brief 除法运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _DivOperationHelper<const T &, U>::value, typename _DivOperationHelper<const T &, U>::type>::type
    operator/(const T &left, const PropertyBase<U, D> &right)
    {
        return left / right.Get();
    }

    /**
     * @brief 取模运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _ModOperationHelper<T &, U>::value, typename _ModOperationHelper<T &, U>::type>::type
    operator%(T &left, const PropertyBase<U, D> &right)
    {
        return left % right.Get();
    }

    /**
     * @brief 取模运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _ModOperationHelper<const T &, U>::value, typename _ModOperationHelper<const T &, U>::type>::type
    operator%(const T &left, const PropertyBase<U, D> &right)
    {
        return left % right.Get();
    }

    /**
     * @brief 等于运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _EqOperationHelper<T &, U>::value, typename _EqOperationHelper<T &, U>::type>::type
    operator==(T &left, const PropertyBase<U, D> &right)
    {
        return left == right.Get();
    }

    /**
     * @brief 等于运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _EqOperationHelper<const T &, U>::value, typename _EqOperationHelper<const T &, U>::type>::type
    operator==(const T &left, const PropertyBase<U, D> &right)
    {
        return left == right.Get();
    }

    /**
     * @brief 不等于运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _NeOperationHelper<T &, U>::value, typename _NeOperationHelper<T &, U>::type>::type
    operator!=(T &left, const PropertyBase<U, D> &right)
    {
        return left != right.Get();
    }

    /**
     * @brief 不等于运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _NeOperationHelper<const T &, U>::value, typename _NeOperationHelper<const T &, U>::type>::type
    operator!=(const T &left, const PropertyBase<U, D> &right)
    {
        return left != right.Get();
    }

    /**
     * @brief 小于运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _LtOperationHelper<T &, U>::value, typename _LtOperationHelper<T &, U>::type>::type
    operator<(T &left, const PropertyBase<U, D> &right)
    {
        return left < right.Get();
    }

    /**
     * @brief 小于运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _LtOperationHelper<const T &, U>::value, typename _LtOperationHelper<const T &, U>::type>::type
    operator<(const T &left, const PropertyBase<U, D> &right)
    {
        return left < right.Get();
    }

    /**
     * @brief 小于等于运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _LeOperationHelper<T &, U>::value, typename _LeOperationHelper<T &, U>::type>::type
    operator<=(T &left, const PropertyBase<U, D> &right)
    {
        return left <= right.Get();
    }

    /**
     * @brief 小于等于运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _LeOperationHelper<const T &, U>::value, typename _LeOperationHelper<const T &, U>::type>::type
    operator<=(const T &left, const PropertyBase<U, D> &right)
    {
        return left <= right.Get();
    }

    /**
     * @brief 大于运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _GtOperationHelper<T &, U>::value, typename _GtOperationHelper<T &, U>::type>::type
    operator>(T &left, const PropertyBase<U, D> &right)
    {
        return left > right.Get();
    }

    /**
     * @brief 大于运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _GtOperationHelper<const T &, U>::value, typename _GtOperationHelper<const T &, U>::type>::type
    operator>(const T &left, const PropertyBase<U, D> &right)
    {
        return left > right.Get();
    }

    /**
     * @brief 大于等于运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _GeOperationHelper<T &, U>::value, typename _GeOperationHelper<T &, U>::type>::type
    operator>=(T &left, const PropertyBase<U, D> &right)
    {
        return left >= right.Get();
    }

    /**
     * @brief 大于等于运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _GeOperationHelper<const T &, U>::value, typename _GeOperationHelper<const T &, U>::type>::type
    operator>=(const T &left, const PropertyBase<U, D> &right)
    {
        return left >= right.Get();
    }

    /**
     * @brief 按位与运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _BitAndOperationHelper<T &, U>::value, typename _BitAndOperationHelper<T &, U>::type>::type
    operator&(T &left, const PropertyBase<U, D> &right)
    {
        return left & right.Get();
    }

    /**
     * @brief 按位与运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _BitAndOperationHelper<const T &, U>::value, typename _BitAndOperationHelper<const T &, U>::type>::type
    operator&(const T &left, const PropertyBase<U, D> &right)
    {
        return left & right.Get();
    }

    /**
     * @brief 按位或运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _BitOrOperationHelper<T &, U>::value, typename _BitOrOperationHelper<T &, U>::type>::type
    operator|(T &left, const PropertyBase<U, D> &right)
    {
        return left | right.Get();
    }

    /**
     * @brief 按位或运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _BitOrOperationHelper<const T &, U>::value, typename _BitOrOperationHelper<const T &, U>::type>::type
    operator|(const T &left, const PropertyBase<U, D> &right)
    {
        return left | right.Get();
    }

    /**
     * @brief 按位异或运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _BitXorOperationHelper<T &, U>::value, typename _BitXorOperationHelper<T &, U>::type>::type
    operator^(T &left, const PropertyBase<U, D> &right)
    {
        return left ^ right.Get();
    }

    /**
     * @brief 按位异或运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _BitXorOperationHelper<const T &, U>::value, typename _BitXorOperationHelper<const T &, U>::type>::type
    operator^(const T &left, const PropertyBase<U, D> &right)
    {
        return left ^ right.Get();
    }

    /**
     * @brief 左移运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _ShlOperationHelper<T &, U>::value, typename _ShlOperationHelper<T &, U>::type>::type
    operator<<(T &left, const PropertyBase<U, D> &right)
    {
        return left << right.Get();
    }

    /**
     * @brief 左移运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _ShlOperationHelper<const T &, U>::value, typename _ShlOperationHelper<const T &, U>::type>::type
    operator<<(const T &left, const PropertyBase<U, D> &right)
    {
        return left << right.Get();
    }

    /**
     * @brief 右移运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _ShrOperationHelper<T &, U>::value, typename _ShrOperationHelper<T &, U>::type>::type
    operator>>(T &left, const PropertyBase<U, D> &right)
    {
        return left >> right.Get();
    }

    /**
     * @brief 右移运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _ShrOperationHelper<const T &, U>::value, typename _ShrOperationHelper<const T &, U>::type>::type
    operator>>(const T &left, const PropertyBase<U, D> &right)
    {
        return left >> right.Get();
    }

    /**
     * @brief 逻辑与运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _LogicAndOperationHelper<T &, U>::value, typename _LogicAndOperationHelper<T &, U>::type>::type
    operator&&(T &left, const PropertyBase<U, D> &right)
    {
        return left && right.Get();
    }

    /**
     * @brief 逻辑与运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _LogicAndOperationHelper<const T &, U>::value, typename _LogicAndOperationHelper<const T &, U>::type>::type
    operator&&(const T &left, const PropertyBase<U, D> &right)
    {
        return left && right.Get();
    }

    /**
     * @brief 逻辑或运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _LogicOrOperationHelper<T &, U>::value, typename _LogicOrOperationHelper<T &, U>::type>::type
    operator||(T &left, const PropertyBase<U, D> &right)
    {
        return left || right.Get();
    }

    /**
     * @brief 逻辑或运算
     */
    template <typename D, typename T, typename U = T>
    typename std::enable_if<!_IsProperty<T>::value && _LogicOrOperationHelper<const T &, U>::value, typename _LogicOrOperationHelper<const T &, U>::type>::type
    operator||(const T &left, const PropertyBase<U, D> &right)
    {
        return left || right.Get();
    }

    /*================================================================================*/

    /**
     * @brief 属性
     */
    template <typename T>
    class Property : public PropertyBase<T, Property<T>>
    {
    public:
        using TBase = PropertyBase<T, Property<T>>;
        using FnGet = Func<T>;
        using FnSet = Action<const T &>;
        using TBase::operator=;

    private:
        FnGet _getter;
        FnSet _setter;

    public:
        /**
         * @brief 构造属性
         */
        Property(const FnGet &getter, const FnSet &setter)
            : _getter(getter), _setter(setter)
        {
        }

        /**
         * @brief 获取属性值
         */
        T GetterImpl() const
        {
            return this->_getter();
        }

        /**
         * @brief 设置属性值
         */
        void SetterImpl(const T &value) const
        {
            this->_setter(value);
        }
    };

    /**
     * @brief 只读属性
     */
    template <typename T>
    class ReadOnlyProperty : public PropertyBase<T, ReadOnlyProperty<T>>
    {
    public:
        using TBase = PropertyBase<T, ReadOnlyProperty<T>>;
        using FnGet = Func<T>;

    private:
        FnGet _getter;

    public:
        /**
         * @brief 构造只读属性
         */
        ReadOnlyProperty(const FnGet &getter)
            : _getter(getter)
        {
        }

        /**
         * @brief 获取属性值
         */
        T GetterImpl() const
        {
            return this->_getter();
        }
    };

    /**
     * @brief 只写属性
     */
    template <typename T>
    class WriteOnlyProperty : public PropertyBase<T, WriteOnlyProperty<T>>
    {
    public:
        using TBase = PropertyBase<T, WriteOnlyProperty<T>>;
        using FnSet = Action<const T &>;
        using TBase::operator=;

    private:
        FnSet _setter;

    public:
        /**
         * @brief 构造属性
         */
        WriteOnlyProperty(const FnSet &setter)
            : _setter(setter)
        {
        }

        /**
         * @brief 设置属性值
         */
        void SetterImpl(const T &value) const
        {
            this->_setter(value);
        }
    };
}

// Rect.h


namespace sw
{
    /**
     * @brief 表示一个矩形区域
     */
    struct Rect {
        /**
         * @brief 左边
         */
        double left;

        /**
         * @brief 顶边
         */
        double top;

        /**
         * @brief 宽度
         */
        double width;

        /**
         * @brief 高度
         */
        double height;

        /**
         * @brief 构造Rect
         */
        Rect();

        /**
         * @brief 构造Rect
         */
        Rect(double left, double top, double width, double height);

        /**
         * @brief 从RECT构造Rect
         */
        Rect(const RECT &rect);

        /**
         * @brief 隐式转换RECT
         */
        operator RECT() const;

        /**
         * @brief 获取Rect左上角的位置
         */
        Point GetPos() const;

        /**
         * @brief 获取Rect的尺寸
         */
        Size GetSize() const;

        /**
         * @brief 判断两个Rect是否相等
         */
        bool operator==(const Rect &other) const;

        /**
         * @brief 判断两个Rect是否不相等
         */
        bool operator!=(const Rect &other) const;

        /**
         * @brief 获取描述当前对象的字符串
         */
        std::wstring ToString() const;
    };
}

// MenuItem.h


namespace sw
{
    class MenuItem; // 向前声明

    /**
     * @brief 菜单项关联的回调函数类型
     */
    using MenuItemCommand = Action<MenuItem &>;

    /**
     * @brief 菜单项
     */
    class MenuItem : public ITag
    {
    public:
        /**
         * @brief 储存用户自定义信息
         */
        uint64_t tag;

        /**
         * @brief 菜单项的文本，当值为“-”时表示当前项为分隔条
         */
        std::wstring text;

        /**
         * @brief 菜单项被单击时调用的函数
         */
        MenuItemCommand command;

        /**
         * @brief 子项
         */
        std::vector<std::shared_ptr<MenuItem>> subItems{};

    public:
        /**
         * @brief      构造一个MenuItem，并设置文本
         * @param text 菜单项的文本
         */
        MenuItem(const std::wstring &text);

        /**
         * @brief         构造一个MenuItem，并设置其回调函数
         * @param text    菜单项的文本
         * @param command 被单击时调用的函数
         */
        MenuItem(const std::wstring &text, const MenuItemCommand &command);

        /**
         * @brief          构造一个MenuItem，并设置其子项
         * @param text     菜单下的文本
         * @param subItems 子项列表
         */
        MenuItem(const std::wstring &text, std::initializer_list<MenuItem> subItems);

        /**
         * @brief      构造一个MenuItem，并设置tag及文本
         * @param text 菜单项的文本
         */
        MenuItem(uint64_t tag, const std::wstring &text);

        /**
         * @brief         构造一个MenuItem，并设置tag及回调函数
         * @param text    菜单项的文本
         * @param command 被单击时调用的函数
         */
        MenuItem(uint64_t tag, const std::wstring &text, const MenuItemCommand &command);

        /**
         * @brief         构造一个MenuItem，设置成员函数为回调函数
         * @tparam T      成员函数所在的类
         * @param obj     成员函数所在的对象
         * @param handler 处理函数
         */
        template <typename T>
        MenuItem(const std::wstring &text, T &obj, void (T::*handler)(MenuItem &))
            : MenuItem(0, text, obj, handler)
        {
        }

        /**
         * @brief         构造一个MenuItem，设置成员函数为回调函数
         * @tparam T      成员函数所在的类
         * @param obj     成员函数所在的对象
         * @param handler 处理函数
         */
        template <typename T>
        MenuItem(uint64_t tag, const std::wstring &text, T &obj, void (T::*handler)(MenuItem &))
            : MenuItem(tag, text, MenuItemCommand(obj, handler))
        {
        }

    public:
        /**
         * @brief 获取一个值，表示当前菜单项是否为分隔条
         */
        bool IsSeparator() const;

        /**
         * @brief 调用command
         */
        void CallCommand();

        /**
         * @brief 获取Tag
         */
        virtual uint64_t GetTag() override;

        /**
         * @brief 设置Tag
         */
        virtual void SetTag(uint64_t tag) override;
    };
}

// Keys.h


namespace sw
{
    /**
     * @brief https://learn.microsoft.com/en-us/windows/win32/inputdev/about-keyboard-input#keystroke-message-flags
     */
    struct KeyFlags {
        uint16_t repeatCount;  // repeat count, > 0 if several keydown messages was combined into one message
        uint8_t scanCode;      // scan code
        bool isExtendedKey;    // extended-key flag, 1 if scancode has 0xE0 prefix
        bool contextCode;      // indicates whether the ALT key was down
        bool previousKeyState; // indicates whether the key that generated the keystroke message was previously up or down
        bool transitionState;  // transition-state flag, 1 on keyup
        KeyFlags(LPARAM lParam);
    };

    /**
     * @brief 虚拟按键
     */
    enum class VirtualKey {
        MouseLeft   = 0x01, // Left mouse button
        MouseRight  = 0x02, // Right mouse button
        Cancel      = 0x03, // Control-break processing
        MouseMiddle = 0x04, // Middle mouse button (three-button mouse)
        MouseX1     = 0x05, // X1 mouse button
        MouseX2     = 0x06, // X2 mouse button

        //- 0x07 Undefined

        Backspace = 0x08, // BACKSPACE key
        Tab       = 0x09, // TAB key

        //- 0x0A-0B Reserved

        Clear = 0x0C, // CLEAR key
        Enter = 0x0D, // ENTER key

        //- 0x0E-0F Undefined

        Shift      = 0x10, // SHIFT key
        Ctrl       = 0x11, // CTRL key
        Alt        = 0x12, // ALT key
        Pause      = 0x13, // PAUSE key
        CapsLock   = 0x14, // CAPS LOCK key
        Kana       = 0x15, // IME Kana mode
        Hanguel    = 0x15, // IME Hanguel mode (maintained for compatibility; use VK_HANGUL)
        Hangul     = 0x15, // IME Hangul mode
        IME_On     = 0x16, // IME On
        Junja      = 0x17, // IME Junja mode
        Final      = 0x18, // IME final mode
        Hanja      = 0x19, // IME Hanja mode
        KANJI      = 0x19, // IME Kanji mode
        IME_Off    = 0x1A, // IME Off
        Esc        = 0x1B, // ESC key
        Convert    = 0x1C, // IME convert
        Nonconvert = 0x1D, // IME nonconvert
        Accept     = 0x1E, // IME accept
        ModeChange = 0x1F, // IME mode change request
        Space      = 0x20, // SPACEBAR
        PageUp     = 0x21, // PAGE UP key
        PageDown   = 0x22, // PAGE DOWN key
        End        = 0x23, // END key
        Home       = 0x24, // HOME key
        Left       = 0x25, // LEFT ARROW key
        Up         = 0x26, // UP ARROW key
        Right      = 0x27, // RIGHT ARROW key
        Down       = 0x28, // DOWN ARROW key
        Select     = 0x29, // SELECT key
        Print      = 0x2A, // PRINT key
        Execute    = 0x2B, // EXECUTE key
        Snapshot   = 0x2C, // PRINT SCREEN key
        Insert     = 0x2D, // INS key
        Delete     = 0x2E, // DEL key
        Help       = 0x2F, // HELP key

        Zero  = 0x30, // 0 key
        One   = 0x31, // 1 key
        Two   = 0x32, // 2 key
        Three = 0x33, // 3 key
        Four  = 0x34, // 4 key
        Five  = 0x35, // 5 key
        Six   = 0x36, // 6 key
        Seven = 0x37, // 7 key
        Eight = 0x38, // 8 key
        Nine  = 0x39, // 9 key

        //- 0x3A-40 Undefined

        A = 0x41, // A key
        B = 0x42, // B key
        C = 0x43, // C key
        D = 0x44, // D key
        E = 0x45, // E key
        F = 0x46, // F key
        G = 0x47, // G key
        H = 0x48, // H key
        I = 0x49, // I key
        J = 0x4A, // J key
        K = 0x4B, // K key
        L = 0x4C, // L key
        M = 0x4D, // M key
        N = 0x4E, // N key
        O = 0x4F, // O key
        P = 0x50, // P key
        Q = 0x51, // Q key
        R = 0x52, // R key
        S = 0x53, // S key
        T = 0x54, // T key
        U = 0x55, // U key
        V = 0x56, // V key
        W = 0x57, // W key
        X = 0x58, // X key
        Y = 0x59, // Y key
        Z = 0x5A, // Z key

        LeftWindows  = 0x5B, // Left Windows key (Natural keyboard)
        RightWindows = 0x5C, // Right Windows key (Natural keyboard)
        Applications = 0x5D, // Applications key (Natural keyboard)

        //- 0x5E Reserved

        Sleep     = 0x5F, // Computer Sleep key
        NumPad0   = 0x60, // Numeric keypad 0 key
        NumPad1   = 0x61, // Numeric keypad 1 key
        NumPad2   = 0x62, // Numeric keypad 2 key
        NumPad3   = 0x63, // Numeric keypad 3 key
        NumPad4   = 0x64, // Numeric keypad 4 key
        NumPad5   = 0x65, // Numeric keypad 5 key
        NumPad6   = 0x66, // Numeric keypad 6 key
        NumPad7   = 0x67, // Numeric keypad 7 key
        NumPad8   = 0x68, // Numeric keypad 8 key
        NumPad9   = 0x69, // Numeric keypad 9 key
        Multipy   = 0x6A, // Multiply key
        Add       = 0x6B, // Add key
        Separator = 0x6C, // Separator key
        Subtract  = 0x6D, // Subtract key
        Decimal   = 0x6E, // Decimal key
        Divide    = 0x6F, // Divide key
        F1        = 0x70, // F1 key
        F2        = 0x71, // F2 key
        F3        = 0x72, // F3 key
        F4        = 0x73, // F4 key
        F5        = 0x74, // F5 key
        F6        = 0x75, // F6 key
        F7        = 0x76, // F7 key
        F8        = 0x77, // F8 key
        F9        = 0x78, // F9 key
        F10       = 0x79, // F10 key
        F11       = 0x7A, // F11 key
        F12       = 0x7B, // F12 key
        F13       = 0x7C, // F13 key
        F14       = 0x7D, // F14 key
        F15       = 0x7E, // F15 key
        F16       = 0x7F, // F16 key
        F17       = 0x80, // F17 key
        F18       = 0x81, // F18 key
        F19       = 0x82, // F19 key
        F20       = 0x83, // F20 key
        F21       = 0x84, // F21 key
        F22       = 0x85, // F22 key
        F23       = 0x86, // F23 key
        F24       = 0x87, // F24 key

        //- 0x88-8F Unassigned

        NumLock    = 0x90, // NUM LOCK key
        ScrollLock = 0x91, // SCROLL LOCK key

        // 0x92-96 OEM specific
        //- 0x97-9F Unassigned

        LeftShift          = 0xA0, // Left SHIFT key
        RightShift         = 0xA1, // Right SHIFT key
        LeftCtrl           = 0xA2, // Left CONTROL key
        RightCtrl          = 0xA3, // Right CONTROL key
        LeftAlt            = 0xA4, // Left ALT key
        RightAlt           = 0xA5, // Right ALT key
        BrowserBack        = 0xA6, // Browser Back key
        BrowserForward     = 0xA7, // Browser Forward key
        BrowserRefresh     = 0xA8, // Browser Refresh key
        BrowserStop        = 0xA9, // Browser Stop key
        BrowserSearch      = 0xAA, // Browser Search key
        BrowserFavorites   = 0xAB, // Browser Favorites key
        BrowserHome        = 0xAC, // Browser Start and Home key
        VolumeMute         = 0xAD, // Volume Mute key
        VolumeDown         = 0xAE, // Volume Down key
        VolumeUp           = 0xAF, // Volume Up key
        MediaNextTrack     = 0xB0, // Next Track key
        MediaPreviousTrack = 0xB1, // Previous Track key
        MediaStop          = 0xB2, // Stop Media key
        MediaPlayPause     = 0xB3, // Play/Pause Media key
        LaunchMail         = 0xB4, // Start Mail key
        LaunchMediaSelect  = 0xB5, // Select Media key
        LaunchApplication1 = 0xB6, // Start Application 1 key
        LaunchApplication2 = 0xB7, // Start Application 2 key

        //- 0xB8-B9 Reserved

        OEM_1      = 0xBA, // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the ';:' key
        OEM_Plus   = 0xBB, // For any country/region, the '+' key
        OEM_Comma  = 0xBC, // For any country/region, the ',' key
        OEM_Minus  = 0xBD, // For any country/region, the '-' key
        OEM_Period = 0xBE, // For any country/region, the '.' key
        OEM_2      = 0xBF, // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '/?' key
        OEM_3      = 0xC0, // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '`~' key

        //- 0xC1-D7 Reserved
        //- 0xD8-DA Unassigned

        OEM_4 = 0xDB, // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '[{' key
        OEM_5 = 0xDC, // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '\|' key
        OEM_6 = 0xDD, // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the ']}' key
        OEM_7 = 0xDE, // Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the 'single-quote/double-quote' key
        OEM_8 = 0xDF, // Used for miscellaneous characters; it can vary by keyboard.

        //- 0xE0 Reserved
        // 0xE1 OEM specific

        OEM_102 = 0xE2, // The <> keys on the US standard keyboard, or the \\| key on the non-US 102-key keyboard

        // 0xE3-E4 OEM specific

        ProcessKey = 0xE5, // IME PROCESS key

        // 0xE6 OEM specific

        Packet = 0xE7, // Used to pass Unicode characters as if they were keystrokes. The VK_PACKET key is the low word of a 32-bit Virtual Key value used for non-keyboard input methods. For more information, see Remark in KEYBDINPUT, SendInput, WM_KEYDOWN, and WM_KEYUP

        //- 0xE8 Unassigned
        // 0xE9-F5 OEM specific

        Attn      = 0xF6, // Attn key
        CrSel     = 0xF7, // CrSel key
        ExSel     = 0xF8, // ExSel key
        EraseEOF  = 0xF9, // Erase EOF key
        Play      = 0xFA, // Play key
        Zoom      = 0xFB, // Zoom key
        NONAME    = 0xFC, // Reserved
        PA1       = 0xFD, // PA1 key
        OEM_Clear = 0xFE, // Clear key
    };

    /**
     * @brief 鼠标事件时用于判断按键状态
     */
    enum class MouseKey {
        Ctrl        = MK_CONTROL,  // The CTRL key is down.
        MouseLeft   = MK_LBUTTON,  // The left mouse button is down.
        MouseMiddle = MK_MBUTTON,  // The middle mouse button is down.
        MouseRight  = MK_RBUTTON,  // The right mouse button is down.
        Shift       = MK_SHIFT,    // The SHIFT key is down.
        MouseX1     = MK_XBUTTON1, // The first X button is down.
        MouseX2     = MK_XBUTTON2, // The second X button is down.
    };

    /**
     * @brief 标记MouseKey枚举类型支持位运算
     */
    _SW_ENUM_ENABLE_BIT_OPERATIONS(MouseKey);

    /**
     * @brief 表示热键框控件中的辅助按键，可以是一个或多个按键
     */
    enum class HotKeyModifier {
        None  = 0,                       // 无按键
        Shift = /*HOTKEYF_SHIFT*/ 0x1,   // Alt键
        Ctrl  = /*HOTKEYF_CONTROL*/ 0x2, // Ctrl键
        Alt   = /*HOTKEYF_ALT*/ 0x4,     // 扩展键
        Ext   = /*HOTKEYF_EXT*/ 0x8,     // Shift键
    };

    /**
     * @brief 标记HotKeyModifier枚举类型支持位运算
     */
    _SW_ENUM_ENABLE_BIT_OPERATIONS(HotKeyModifier);
}

// MenuBase.h


namespace sw
{
    /**
     * @brief 菜单类型的基类
     */
    class MenuBase
    {
    private:
        /**
         * @brief 包含子项的菜单项的句柄信息
         */
        struct _PopupMenuInfo {
            std::shared_ptr<MenuItem> pItem; // 菜单项
            HMENU hSelf;                     // 菜单句柄
        };

        /**
         * @brief 记录菜单项的依赖关系
         */
        struct _MenuItemDependencyInfo {
            HMENU hParent; // 所在菜单的句柄
            HMENU hSelf;   // 若本身含有子项，则此项为本身的菜单句柄，否则为NULL
            int index;     // 所在菜单中的索引
        };

    private:
        /**
         * @brief 菜单句柄，使用InitMenuBase函数设置该值
         */
        HMENU _hMenu = NULL;

        /**
         * @brief 菜单所直接包含的菜单项集合（即第一级菜单项）
         */
        std::vector<std::shared_ptr<MenuItem>> _items;

        /**
         * @brief 记录包含子项的菜单项的句柄信息
         */
        std::vector<_PopupMenuInfo> _popupMenus;

        /**
         * @brief 记录每个菜单项的ID，可通过菜单项所在索引获取ID（调用IndexToID）
         */
        std::vector<std::shared_ptr<MenuItem>> _ids;

        /**
         * @brief 记录每个菜单项直接依赖关系的map
         */
        std::map<MenuItem *, _MenuItemDependencyInfo> _dependencyInfoMap;

    protected:
        /**
         * @brief 初始化菜单
         */
        MenuBase(HMENU hMenu);

        MenuBase(const MenuBase &)            = delete; // 删除拷贝构造函数
        MenuBase(MenuBase &&)                 = delete; // 删除移动构造函数
        MenuBase &operator=(const MenuBase &) = delete; // 删除拷贝赋值运算符
        MenuBase &operator=(MenuBase &&)      = delete; // 删除移动赋值运算符

    public:
        /**
         * @brief 释放资源
         */
        virtual ~MenuBase();

        /**
         * @brief 获取菜单句柄
         */
        HMENU GetHandle();

        /**
         * @brief 更新菜单，该操作会导致菜单项的Enabled、Checked等恢复到初始状态
         */
        void Update();

        /**
         * @brief 初始化菜单并添加菜单项
         */
        void SetItems(std::initializer_list<MenuItem> items);

        /**
         * @brief          重新设置当前菜单中某个菜单项的子项
         * @param item     要修改的菜单项，当该项原先不含有子项时将会调用Update更新整个菜单
         * @param subItems 新的子项列表
         * @return         返回一个bool值，表示操作是否成功
         */
        bool SetSubItems(MenuItem &item, std::initializer_list<MenuItem> subItems);

        /**
         * @brief      添加新的菜单项到菜单
         * @param item 新的菜单项
         */
        void AddItem(const MenuItem &item);

        /**
         * @brief         向当前菜单中的某个菜单项添加新的子项
         * @param item    要添加子项的菜单项，当该项原本不含有子项时将会调用Update更新整个菜单
         * @param subItem 要添加的子菜单项
         * @return        返回一个bool值，表示操作是否成功
         */
        bool AddSubItem(MenuItem &item, const MenuItem &subItem);

        /**
         * @brief      移除当前菜单中的某个子项
         * @param item 要移除的菜单项
         * @return     返回一个bool值，表示操作是否成功
         */
        bool RemoveItem(MenuItem &item);

        /**
         * @brief    通过id获取菜单项
         * @param id 要获取菜单项的id
         * @return   若函数成功则返回菜单项的指针，否则返回nullptr
         */
        MenuItem *GetMenuItem(int id);

        /**
         * @brief      通过索引来获取菜单项
         * @param path 要找项所在下索引
         * @return     若函数成功则返回菜单项的指针，否则返回nullptr
         */
        MenuItem *GetMenuItem(std::initializer_list<int> path);

        /**
         * @brief      通过菜单项的text来获取菜单项
         * @param path 每层要找的text
         * @return     若函数成功则返回菜单项的指针，否则返回nullptr
         */
        MenuItem *GetMenuItem(std::initializer_list<std::wstring> path);

        /**
         * @brief     通过tag值获取菜单项
         * @param tag 指定的tag
         * @return    若函数成功则返回菜单项的指针，否则返回nullptr
         */
        MenuItem *GetMenuItemByTag(uint64_t tag);

        /**
         * @brief      获取当前菜单中指定菜单项的直接父菜单项
         * @param item 要查询的子菜单项
         * @return     若函数成功则返回指向直接父菜单项的指针，否则返回nullptr
         */
        MenuItem *GetParent(MenuItem &item);

        /**
         * @brief      获取一个值，表示菜单项是否可用
         * @param item 要获取的菜单项
         * @param out  输出值
         * @return     函数是否成功
         */
        bool GetEnabled(MenuItem &item, bool &out);

        /**
         * @brief       设置菜单项是否可用
         * @param item  要修改的菜单项
         * @param value 设置的值
         * @return      修改是否成功
         */
        bool SetEnabled(MenuItem &item, bool value);

        /**
         * @brief      获取一个值，表示菜单项是否选中
         * @param item 要获取的菜单项
         * @param out  输出值
         * @return     函数是否成功
         */
        bool GetChecked(MenuItem &item, bool &out);

        /**
         * @brief       设置菜单项是否选中
         * @param item  要修改的菜单项
         * @param value 设置的值
         * @return      修改是否成功
         */
        bool SetChecked(MenuItem &item, bool value);

        /**
         * @brief       设置菜单项文本
         * @param item  要修改的菜单项
         * @param value 设置的值
         * @return      修改是否成功
         */
        bool SetText(MenuItem &item, const std::wstring &value);

        /**
         * @brief         设置菜单项要显示的位图
         * @param item    要修改的菜单项
         * @param hBitmap 要设置的位图句柄
         * @return        修改是否成功
         */
        bool SetBitmap(MenuItem &item, HBITMAP hBitmap);

        /**
         * @brief               设置菜单不同选中状态下显示的位图
         * @param item          要修改的菜单项
         * @param hBmpUnchecked 未选中时显示的位图
         * @param hBmpChecked   选中时显示的位图
         * @return              修改是否成功
         */
        bool SetCheckBitmap(MenuItem &item, HBITMAP hBmpUnchecked, HBITMAP hBmpChecked);

    private:
        /**
         * @brief 清除已添加的所有菜单项
         */
        void _ClearAddedItems();

        /**
         * @brief       添加菜单项到指定句柄
         * @param hMenu 要添加子项的菜单句柄
         * @param pItem 要添加的菜单项
         * @param index 菜单项在父菜单中的索引
         */
        void _AppendMenuItem(HMENU hMenu, std::shared_ptr<MenuItem> pItem, int index);

        /**
         * @brief      获取菜单项的依赖信息
         * @param item 要获取信息的菜单项
         * @return     若函数成功则返回指向_MenuItemDependencyInfo的指针，否则返回nullptr
         */
        _MenuItemDependencyInfo *_GetMenuItemDependencyInfo(MenuItem &item);

        /**
         * @brief       通过tag值获取菜单项
         * @param items 查找的vector
         * @param tag   指定的tag
         * @return      若函数成功则返回菜单项的指针，否则返回nullptr
         */
        MenuItem *_GetMenuItemByTag(std::vector<std::shared_ptr<MenuItem>> &items, uint64_t tag);

    protected:
        /**
         * @brief       根据索引获取ID
         * @param index 索引
         * @return      菜单项的ID
         */
        virtual int IndexToID(int index) = 0;

        /**
         * @brief    根据ID获取索引
         * @param id 菜单项的ID
         * @return   索引
         */
        virtual int IDToIndex(int id) = 0;
    };
}

// RoutedEventArgs.h


namespace sw
{
    struct RoutedEventArgs; // RoutedEvent.h

    /**
     * @brief       表示特定类型路由事件的事件参数类型，继承自该类的类型可以直接作为RegisterRoutedEvent函数的模板参数
     * @tparam TYPE 一个RoutedEventType枚举值，表示路由事件类型
     */
    template <RoutedEventType TYPE>
    struct TypedRoutedEventArgs : RoutedEventArgs {
        /**
         * @brief 路由事件的类型，RegisterRoutedEvent模板函数使用此字段注册事件
         */
        static constexpr RoutedEventType EventType = TYPE;

        /**
         * @brief 构造函数，初始化事件类型为EventType
         */
        TypedRoutedEventArgs() : RoutedEventArgs(EventType) {}
    };

    /**
     * @brief 结构体模板，用于检测类型T是否含有名为EventType的静态字段
     */
    template <typename T, typename = void>
    struct _HasEventType : std::false_type {
    };

    /**
     * @brief 模板特化：当T包含EventType时，将_IsTypedRoutedEventArgs<T>设为std::true_type
     */
    template <typename T>
    struct _HasEventType<T, decltype(void(std::declval<T>().EventType))> : std::true_type {
    };

    /**
     * @brief 结构体模板，用于检测类型T是否包含事件类型信息
     */
    template <typename T>
    struct _IsTypedRoutedEventArgs : _HasEventType<T> {
    };

    /**
     * @brief 尺寸改变事件参数类型
     */
    struct SizeChangedEventArgs : TypedRoutedEventArgs<UIElement_SizeChanged> {
        Size newClientSize; // 用户区的新尺寸
        SizeChangedEventArgs(Size newClientSize) : newClientSize(newClientSize) {}
    };

    /**
     * @brief 位置改变事件参数类型
     */
    struct PositionChangedEventArgs : TypedRoutedEventArgs<UIElement_PositionChanged> {
        Point newClientPosition; // 移动后用户区左上角的位置
        PositionChangedEventArgs(Point newClientPosition) : newClientPosition(newClientPosition) {}
    };

    /**
     * @brief 输入字符事件类型参数
     */
    struct GotCharEventArgs : TypedRoutedEventArgs<UIElement_GotChar> {
        wchar_t ch;     // 输入的字符
        KeyFlags flags; // 附加信息
        GotCharEventArgs(wchar_t ch, KeyFlags flags) : ch(ch), flags(flags) {}
    };

    /**
     * @brief 键盘按键按下事件参数类型
     */
    struct KeyDownEventArgs : TypedRoutedEventArgs<UIElement_KeyDown> {
        VirtualKey key; // 虚拟按键
        KeyFlags flags; // 附加信息
        KeyDownEventArgs(VirtualKey key, KeyFlags flags) : key(key), flags(flags) {}
    };

    /**
     * @brief 键盘按键抬起事件参数类型
     */
    struct KeyUpEventArgs : TypedRoutedEventArgs<UIElement_KeyUp> {
        VirtualKey key; // 虚拟按键
        KeyFlags flags; // 附加信息
        KeyUpEventArgs(VirtualKey key, KeyFlags flags) : key(key), flags(flags) {}
    };

    /**
     * @brief 鼠标移动事件参数类型
     */
    struct MouseMoveEventArgs : TypedRoutedEventArgs<UIElement_MouseMove> {
        Point mousePosition; // 鼠标位置
        MouseKey keyState;   // 按键状态
        MouseMoveEventArgs(Point mousePosition, MouseKey keyState)
            : mousePosition(mousePosition), keyState(keyState) {}
    };

    /**
     * @brief 鼠标滚轮滚动事件参数类型
     */
    struct MouseWheelEventArgs : TypedRoutedEventArgs<UIElement_MouseWheel> {
        int wheelDelta;      // 滚轮滚动的距离，为120的倍数
        Point mousePosition; // 鼠标位置
        MouseKey keyState;   // 按键状态
        MouseWheelEventArgs(int wheelDelta, Point mousePosition, MouseKey keyState)
            : wheelDelta(wheelDelta), mousePosition(mousePosition), keyState(keyState) {}
    };

    /**
     * @brief 鼠标按键按下事件参数类型
     */
    struct MouseButtonDownEventArgs : TypedRoutedEventArgs<UIElement_MouseButtonDown> {
        MouseKey key;        // 按下的按键（左键、中间、右键）
        Point mousePosition; // 鼠标位置
        MouseKey keyState;   // 按键状态
        MouseButtonDownEventArgs(MouseKey key, Point mousePosition, MouseKey keyState)
            : key(key), mousePosition(mousePosition), keyState(keyState) {}
    };

    /**
     * @brief 鼠标按键抬起事件参数类型
     */
    struct MouseButtonUpEventArgs : TypedRoutedEventArgs<UIElement_MouseButtonUp> {
        MouseKey key;        // 抬起的按键（左键、中间、右键）
        Point mousePosition; // 鼠标位置
        MouseKey keyState;   // 按键状态
        MouseButtonUpEventArgs(MouseKey key, Point mousePosition, MouseKey keyState)
            : key(key), mousePosition(mousePosition), keyState(keyState) {}
    };

    /**
     * @brief 显示用户自定义上下文菜单的事件参数类型
     */
    struct ShowContextMenuEventArgs : TypedRoutedEventArgs<UIElement_ShowContextMenu> {
        bool cancel = false; // 是否取消显示上下文菜单
        bool isKeyboardMsg;  // 消息是否由按下快捷键（Shift+F10、VK_APPS）产生
        Point mousePosition; // 鼠标在屏幕中的位置
        ShowContextMenuEventArgs(bool isKeyboardMsg, Point mousePosition)
            : isKeyboardMsg(isKeyboardMsg), mousePosition(mousePosition) {}
    };

    /**
     * @brief 文件拖放事件参数类型
     */
    struct DropFilesEventArgs : TypedRoutedEventArgs<UIElement_DropFiles> {
        HDROP hDrop; // 描述拖入文件的句柄
        DropFilesEventArgs(HDROP hDrop) : hDrop(hDrop) {}
    };

    /**
     * @brief 窗口正在关闭事件参数类型
     */
    struct WindowClosingEventArgs : TypedRoutedEventArgs<Window_Closing> {
        bool cancel = false; // 是否取消本次关闭
    };

    /**
     * @brief 窗口/面板滚动条滚动事件参数类型
     */
    struct ScrollingEventArgs : TypedRoutedEventArgs<Layer_Scrolling> {
        bool cancel = false;         // 是否取消滚动条默认行为
        ScrollOrientation scrollbar; // 滚动条类型
        ScrollEvent event;           // 滚动条事件
        double pos;                  // 当event为ThumbPosition或ThubmTrack时表示当前滚动条位置，其他情况固定为0
        ScrollingEventArgs(ScrollOrientation scrollbar, ScrollEvent event, double pos)
            : scrollbar(scrollbar), event(event), pos(pos) {}
    };

    /**
     * @brief 列表视图某个复选框选中状态改变的事件参数类型
     */
    struct ListViewCheckStateChangedEventArgs : TypedRoutedEventArgs<ListView_CheckStateChanged> {
        int index; // 改变项的索引
        ListViewCheckStateChangedEventArgs(int index) : index(index) {}
    };

    /**
     * @brief 列表视图的列标题单击与双击事件参数类型
     */
    struct ListViewHeaderClickedEventArgs : RoutedEventArgs {
        int index; // 被点击列标题的索引
        ListViewHeaderClickedEventArgs(RoutedEventType eventType, int index)
            : RoutedEventArgs(eventType), index(index) {}
    };

    /**
     * @brief 列表视图项单击与双击事件参数类型
     */
    struct ListViewItemClickedEventArgs : RoutedEventArgs {
        int row; // 被点击的行
        int col; // 被点击的列
        ListViewItemClickedEventArgs(RoutedEventType eventType, int row, int col)
            : RoutedEventArgs(eventType), row(row), col(col) {}
    };

    /**
     * @brief 列表视图编辑状态结束事件参数类型
     */
    struct ListViewEndEditEventArgs : TypedRoutedEventArgs<ListView_EndEdit> {
        bool cancel = false; // 是否取消文本更改，默认为false
        int index;           // 被编辑项的索引
        wchar_t *newText;    // 新的文本
        ListViewEndEditEventArgs(int index, wchar_t *newText) : index(index), newText(newText) {}
    };

    /**
     * @brief DateTimePicker控件时间改变事件参数类型
     */
    struct DateTimePickerTimeChangedEventArgs : TypedRoutedEventArgs<DateTimePicker_TimeChanged> {
        SYSTEMTIME time; // 时间的新值
        DateTimePickerTimeChangedEventArgs(const SYSTEMTIME &time) : time(time) {}
    };

    /**
     * @brief 月历控件时间改变事件参数类型
     */
    struct MonthCalendarTimeChangedEventArgs : TypedRoutedEventArgs<MonthCalendar_TimeChanged> {
        SYSTEMTIME time; // 时间的新值
        MonthCalendarTimeChangedEventArgs(const SYSTEMTIME &time) : time(time) {}
    };

    /**
     * @brief SysLink控件链接被单击事件参数类型
     */
    struct SysLinkClickedEventArgs : TypedRoutedEventArgs<SysLink_Clicked> {
        wchar_t *id;  // 被单击链接的id
        wchar_t *url; // 被单击链接的url（即href）
        SysLinkClickedEventArgs(wchar_t *id, wchar_t *url) : id(id), url(url) {}
    };

    /**
     * @brief 热键框值改变事件参数类型
     */
    struct HotKeyValueChangedEventArgs : TypedRoutedEventArgs<HotKeyControl_ValueChanged> {
        VirtualKey key;          // 按键
        HotKeyModifier modifier; // 辅助按键
        HotKeyValueChangedEventArgs(VirtualKey key, HotKeyModifier modifier) : key(key), modifier(modifier) {}
    };
}

// App.h


namespace sw
{
    /**
     * @brief 程序退出消息循环的方式
     */
    enum class AppQuitMode {
        Auto,   // 当所有窗口都销毁时自动退出消息循环
        Manual, // 需手动调用QuitMsgLoop以退出消息循环
    };

    /**
     * @brief App类
     */
    class App
    {
    private:
        App() = delete;

    public:
        /**
         * @brief 应用程序的当前实例的句柄
         */
        static const ReadOnlyProperty<HINSTANCE> Instance;

        /**
         * @brief 当前exe的文件路径
         */
        static const ReadOnlyProperty<std::wstring> ExePath;

        /**
         * @brief 当前exe所在的文件夹路径
         */
        static const ReadOnlyProperty<std::wstring> ExeDirectory;

        /**
         * @brief 当前工作路径
         */
        static const Property<std::wstring> CurrentDirectory;

        /**
         * @brief 程序退出消息循环的方式
         */
        static const Property<AppQuitMode> QuitMode;

        /**
         * @brief 消息循环中处理空句柄消息的回调函数
         */
        static Action<MSG &> NullHwndMsgHandler;

        /**
         * @brief  消息循环
         * @return 退出代码
         */
        static int MsgLoop();

        /**
         * @brief          退出当前消息循环
         * @param exitCode 退出代码
         */
        static void QuitMsgLoop(int exitCode = 0);
    };
}

// EventHandlerWrapper.h


namespace sw
{
    /**
     * @brief 路由事件处理函数包装类，用于需要转换RoutedEventArgs为特定事件参数类型的情况
     */
    template <
        typename TEventArgs,
        typename std::enable_if<std::is_base_of<RoutedEventArgs, TEventArgs>::value, int>::type = 0>
    class RoutedEventHandlerWrapper : public ICallable<void(UIElement &, RoutedEventArgs &)>
    {
    private:
        /**
         * @brief 事件处理函数
         */
        Action<UIElement &, TEventArgs &> _handler;

    public:
        /**
         * @brief         构造函数
         * @param handler 事件处理函数
         */
        RoutedEventHandlerWrapper(const Action<UIElement &, TEventArgs &> &handler)
            : _handler(handler)
        {
        }

        /**
         * @brief 调用事件处理函数
         */
        virtual void Invoke(UIElement &sender, RoutedEventArgs &args) const override
        {
            if (_handler) _handler(sender, static_cast<TEventArgs &>(args));
        }

        /**
         * @brief 克隆当前可调用对象
         */
        virtual ICallable<void(UIElement &, RoutedEventArgs &)> *Clone() const override
        {
            return new RoutedEventHandlerWrapper(_handler);
        }

        /**
         * @brief 获取当前可调用对象的类型信息
         */
        virtual std::type_index GetType() const override
        {
            return typeid(RoutedEventHandlerWrapper<TEventArgs>);
        }

        /**
         * @brief       判断当前可调用对象是否与另一个可调用对象相等
         * @param other 另一个可调用对象
         * @return      如果相等则返回true，否则返回false
         */
        virtual bool Equals(const ICallable<void(UIElement &, RoutedEventArgs &)> &other) const override
        {
            if (this == &other) {
                return true;
            }
            if (GetType() != other.GetType()) {
                return false;
            }
            const auto &otherWrapper = static_cast<const RoutedEventHandlerWrapper &>(other);
            return _handler.Equals(otherWrapper._handler);
        }
    };
}

// ILayout.h


namespace sw
{
    /**
     * @brief 布局接口
     */
    class ILayout
    {
    public:
        /**
         * @brief 默认虚析构函数
         */
        virtual ~ILayout() = default;

    public:
        /**
         * @brief 获取布局标记
         */
        virtual uint64_t GetLayoutTag() = 0;

        /**
         * @brief 获取子控件的数量
         */
        virtual int GetChildLayoutCount() = 0;

        /**
         * @brief 获取对应索引处的子控件
         */
        virtual ILayout &GetChildLayoutAt(int index) = 0;

        /**
         * @brief 获取控件所需尺寸
         */
        virtual Size GetDesireSize() = 0;

        /**
         * @brief 设置当前控件所需的尺寸
         */
        virtual void SetDesireSize(const Size &size) = 0;

        /**
         * @brief               测量控件所需尺寸
         * @param availableSize 可用的尺寸
         */
        virtual void Measure(const Size &availableSize) = 0;

        /**
         * @brief               安排控件位置
         * @param finalPosition 最终控件所安排的位置
         */
        virtual void Arrange(const Rect &finalPosition) = 0;
    };
}

// Utils.h


namespace sw
{
    /**
     * @brief 判断一个类型是否有ToString方法
     */
    template <typename T>
    struct _HasToString {
    private:
        template <typename U>
        static auto test(int) -> decltype(std::declval<U>().ToString(), std::true_type());

        template <typename U>
        static auto test(...) -> std::false_type;

    public:
        static constexpr bool value = decltype(test<T>(0))::value;
    };

    /**
     * @brief 工具类
     */
    class Utils
    {
    private:
        Utils() = delete; // 删除构造函数

    public:
        /**
         * @brief      将窄字符串转为宽字符串
         * @param str  要转换的字符串
         * @param utf8 是否使用utf8编码
         * @return     转换后的字符串
         */
        static std::wstring ToWideStr(const std::string &str, bool utf8 = false);

        /**
         * @brief      将宽字符串转为窄字符串
         * @param wstr 要转换的字符串
         * @param utf8 是否使用utf8编码
         * @return     转换后的字符串
         */
        static std::string ToMultiByteStr(const std::wstring &wstr, bool utf8 = false);

        /**
         * @brief     删除首尾空白字符
         * @param str 输入的字符串
         * @return    删除首位空白字符后的字符串
         */
        static std::wstring Trim(const std::wstring &str);

        /**
         * @brief     删除串首空白字符
         * @param str 输入的字符串
         * @return    删除串首空白字符后的字符串
         */
        static std::wstring TrimStart(const std::wstring &str);

        /**
         * @brief     删除串尾空白字符
         * @param str 输入的字符串
         * @return    删除串尾空白字符后的字符串
         */
        static std::wstring TrimEnd(const std::wstring &str);

        /**
         * @brief           对字符串按照指定分隔符进行拆分
         * @param str       输入的字符串
         * @param delimiter 分隔符
         * @result          包含字串的vector
         */
        static std::vector<std::wstring> Split(const std::wstring &str, const std::wstring &delimiter);

        /**
         * @brief     格式化字符串，类似于 `swprintf`，但返回一个动态分配的 `std::wstring`
         * @param fmt 格式化字符串
         * @param ... 可变参数，符合 `fmt` 格式的输入
         * @return    返回一个包含格式化结果的字符串
         */
        static std::wstring FormatStr(const wchar_t *fmt, ...);

    public:
        /**
         * @brief 取两值中的较大值
         */
        template <typename T>
        static constexpr inline T Max(const T &a, const T &b)
        {
            return a > b ? a : b;
        }

        /**
         * @brief 取两值中的较小值
         */
        template <typename T>
        static constexpr inline T Min(const T &a, const T &b)
        {
            return a < b ? a : b;
        }

        /**
         * @brief 拼接字符串，也可使用此函数将其他类型转为wstring
         */
        template <typename... Args>
        static inline std::wstring BuildStr(const Args &...args)
        {
            std::wstringstream wss;
            int _[]{(Utils::_BuildStr(wss, args), 0)...};
            return wss.str();
        }

    private:
        /**
         * @brief BuildStr函数内部实现
         */
        template <typename T>
        static inline typename std::enable_if<!_IsProperty<T>::value && !_HasToString<T>::value, void>::type
        _BuildStr(std::wostream &wos, const T &arg)
        {
            wos << arg;
        }

        /**
         * @brief 让BuildStr函数支持自定义类型
         */
        template <typename T>
        static inline typename std::enable_if<!_IsProperty<T>::value && _HasToString<T>::value, void>::type
        _BuildStr(std::wostream &wos, const T &arg)
        {
            Utils::_BuildStr(wos, arg.ToString());
        }

        /**
         * @brief 让BuildStr函数支持属性
         */
        template <typename T>
        static inline typename std::enable_if<_IsProperty<T>::value, void>::type
        _BuildStr(std::wostream &wos, const T &prop)
        {
            Utils::_BuildStr(wos, prop.Get());
        }

        /**
         * @brief 让BuildStr函数将bool类型转化为"true"或"false"而不是数字1或0
         */
        static inline void _BuildStr(std::wostream &wos, bool b)
        {
            wos << (b ? L"true" : L"false");
        }

        /**
         * @brief 让BuildStr函数支持窄字符串
         */
        static inline void _BuildStr(std::wostream &wos, const char *str)
        {
            wos << Utils::ToWideStr(str);
        }

        /**
         * @brief 让BuildStr函数支持窄字符串
         */
        static inline void _BuildStr(std::wostream &wos, const std::string &str)
        {
            wos << Utils::ToWideStr(str);
        }

        /**
         * @brief 让BuildStr函数支持std::vector
         */
        template <typename T>
        static inline void _BuildStr(std::wostream &wos, const std::vector<T> &vec)
        {
            auto beg = vec.begin();
            auto end = vec.end();
            wos << L"[";
            for (auto it = beg; it != end; ++it) {
                if (it != beg)
                    wos << L", ";
                Utils::_BuildStr(wos, *it);
            }
            wos << L"]";
        }

        /**
         * @brief 让BildStr函数支持std::map
         */
        template <typename TKey, typename TVal>
        static inline void _BuildStr(std::wostream &wos, const std::map<TKey, TVal> &map)
        {
            auto beg = map.begin();
            auto end = map.end();
            wos << L"{";
            for (auto it = beg; it != end; ++it) {
                if (it != beg)
                    wos << L", ";
                Utils::_BuildStr(wos, it->first);
                wos << L":";
                Utils::_BuildStr(wos, it->second);
            }
            wos << L"}";
        }
    };
}

// Dip.h


namespace sw
{
    /**
     * @brief 用于处理设备独立像素（dip）与屏幕像素之间的转换
     */
    class Dip
    {
    private:
        Dip() = delete;

    public:
        /**
         * @brief 水平缩放比例
         */
        static const ReadOnlyProperty<double> ScaleX;

        /**
         * @brief 垂直缩放比例
         */
        static const ReadOnlyProperty<double> ScaleY;

        /**
         * @brief dpi改变时调用该函数更新缩放比例
         */
        static void Update(int dpiX, int dpiY);

        /**
         * @brief 像素转dip（水平方向）
         */
        static double PxToDipX(int px);

        /**
         * @brief 像素转dip（垂直方向）
         */
        static double PxToDipY(int px);

        /**
         * @brief dip转像素（水平方向）
         */
        static int DipToPxX(double dip);

        /**
         * @brief dip转像素（垂直方向）
         */
        static int DipToPxY(double dip);
    };
}

// Screen.h


namespace sw
{
    /**
     * @brief 屏幕相关
     */
    class Screen
    {
    private:
        Screen() = delete;

    public:
        /**
         * @brief 屏幕宽度
         */
        static const ReadOnlyProperty<double> Width;

        /**
         * @brief 屏幕高度
         */
        static const ReadOnlyProperty<double> Height;

        /**
         * @brief 鼠标在屏幕中的位置
         */
        static const ReadOnlyProperty<Point> CursorPosition;
    };
}

// List.h


namespace sw
{
    template <typename T>
    class List; // 向前声明

    /**
     * @brief 字符串列表
     */
    using StrList = List<std::wstring>;

    /**
     * @brief 列表类，内部维护了一个指向std::vector的智能指针
     */
    template <typename T>
    class List
    {
    private:
        /**
         * @brief 指向std::vector的智能指针
         */
        std::shared_ptr<std::vector<T>> _pVec;

    public:
        /**
         * @brief 初始化列表
         */
        List()
            : _pVec(std::make_shared<std::vector<T>>())
        {
        }

        /**
         * @brief 使用初始化列表
         */
        List(std::initializer_list<T> list)
            : _pVec(std::make_shared<std::vector<T>>(list))
        {
        }

        /**
         * @brief 初始化列表并指定容量
         */
        explicit List(int capacity)
            : List()
        {
            this->_pVec->reserve(capacity);
        }

        /**
         * @brief 正向迭代器开始
         */
        auto begin() const
        {
            return this->_pVec->begin();
        }

        /**
         * @brief 正向迭代器结束
         */
        auto end() const
        {
            return this->_pVec->end();
        }

        /**
         * @brief 反向迭代器开始
         */
        auto rbegin() const
        {
            return this->_pVec->rbegin();
        }

        /**
         * @brief 反向迭代器结束
         */
        auto rend() const
        {
            return this->_pVec->rend();
        }

        /**
         * @brief 获取或设置列表中指定位置的值
         */
        auto &operator[](int index) const
        {
            return this->_pVec->at(index);
        }

        /**
         * @brief 判断是否为同一个列表
         */
        bool operator==(const List &other) const
        {
            return this->_pVec == other._pVec;
        }

        /**
         * @brief 判断是否不是同一个列表
         */
        bool operator!=(const List &other) const
        {
            return this->_pVec != other._pVec;
        }

        /**
         * @brief 列表当前的容量
         */
        int Capacity() const
        {
            return (int)this->_pVec->capacity();
        }

        /**
         * @brief 获取元素个数
         */
        int Count() const
        {
            return (int)this->_pVec->size();
        }

        /**
         * @brief 列表是否为空
         */
        bool IsEmpty() const
        {
            return this->_pVec->empty();
        }

        /**
         * @brief  添加一个值到列表末尾
         * @return 当前列表
         */
        auto Append(const T &value) const
        {
            this->_pVec->push_back(value);
            return *this;
        }

        /**
         * @brief  添加一个值到列表末尾
         * @return 当前列表
         */
        auto Append(T &&value) const
        {
            this->_pVec->push_back(std::forward<T>(value));
            return *this;
        }

        /**
         * @brief  在指定位置插入值
         * @return 当前列表
         */
        auto Insert(int index, const T &value) const
        {
            this->_pVec->insert(this->_pVec->begin() + index, value);
            return *this;
        }

        /**
         * @brief  在指定位置插入值
         * @return 当前列表
         */
        auto Insert(int index, T &&value) const
        {
            this->_pVec->insert(this->_pVec->begin() + index, std::forward<T>(value));
            return *this;
        }

        /**
         * @brief       列表是否包含某个值
         * @param value 要查找的值
         */
        bool Contains(const T &value) const
        {
            return std::find(this->_pVec->begin(), this->_pVec->end(), value) != this->_pVec->end();
        }

        /**
         * @brief       查找值在列表中的索引
         * @param value 要查找的值
         * @return      若列表中包含该值则返回其索引，否则返回-1
         */
        int IndexOf(const T &value) const
        {
            auto it = std::find(this->_pVec->begin(), this->_pVec->end(), value);
            return it == this->_pVec->end() ? -1 : int(it - this->_pVec->begin());
        }

        /**
         * @brief       移除列表中第一个指定的值
         * @param value 要移除的值
         * @return      是否成功移除
         */
        bool Remove(const T &value) const
        {
            auto it = std::find(this->_pVec->begin(), this->_pVec->end(), value);
            if (it == this->_pVec->end())
                return false;
            this->_pVec->erase(it);
            return true;
        }

        /**
         * @brief       移除指定索引处的值
         * @param index 要移除元素的索引
         */
        void RemoveAt(int index) const
        {
            this->_pVec->erase(this->_pVec->begin() + index);
        }

        /**
         * @brief 清空列表
         */
        void Clear() const
        {
            this->_pVec->clear();
        }

        /**
         * @brief 复制当前列表
         */
        List Copy() const
        {
            List list((int)this->_pVec->capacity());
            list._pVec->assign(this->_pVec->begin(), this->_pVec->end());
            return list;
        }

        /**
         * @brief 获取列表内部维护的std::vector
         */
        std::vector<T> &GetStdVector() const
        {
            return *this->_pVec;
        }

        /**
         * @brief 获取描述当前对象的字符串
         */
        std::wstring ToString() const
        {
            return Utils::BuildStr(*this->_pVec);
        }
    };
}

// ImageList.h


namespace sw
{
    /**
     * @brief 图像列表
     */
    class ImageList
    {
    private:
        /**
         * @brief 图像列表的句柄
         */
        HIMAGELIST _hImageList;

        /**
         * @brief 记录是否为包装对象
         */
        bool _isWrap;

    protected:
        /**
         * @brief            初始化图像列表
         * @param hImageList 图像列表的句柄
         * @param isWrap     是否为包装对象
         */
        ImageList(HIMAGELIST hImageList, bool isWrap);

    public:
        /**
         * @brief 创建图像列表，参数与ImageList_Create相同
         */
        ImageList(int cx, int cy, UINT flags, int cInitial, int cGrow);

        /**
         * @brief 拷贝构造
         */
        ImageList(const ImageList &value);

        /**
         * @brief 移动构造
         */
        ImageList(ImageList &&rvalue);

        /**
         * @brief 析构函数
         */
        virtual ~ImageList();

        /**
         * @brief 拷贝赋值
         */
        ImageList &operator=(const ImageList &value);

        /**
         * @brief 移动赋值
         */
        ImageList &operator=(ImageList &&rvalue);

        /**
         * @brief  创建一个图像列表，该函数调用ImageList_Create
         * @return 图像列表对象
         */
        static ImageList Create(int cx, int cy, UINT flags, int cInitial, int cGrow);

        /**
         * @brief            包装一个图像列表句柄为ImageList对象，通过该函数创建的对象析构时不会销毁句柄
         * @param hImageList 要包装的句柄
         * @return           包装原有句柄的对象
         */
        static ImageList Wrap(HIMAGELIST hImageList);

        /**
         * @brief 复制图像，该函数调用ImageList_Copy
         */
        static bool Copy(const ImageList &dst, int iDst, const ImageList &src, int iSrc, UINT uFlags);

        /**
         * @brief 锁定窗口并在指定窗口内显示拖拽图像，该函数调用ImageList_DragEnter
         */
        static bool DragEnter(HWND hwndLock, double x, double y);

        /**
         * @brief 解除窗口锁定并隐藏显示的拖拽图像，该函数调用ImageList_DragLeave
         */
        static bool DragLeave(HWND hwndLock);

        /**
         * @brief 拖拽移动，一般在WM_MOUSEMOVE函数中调用，该函数调用ImageList_DragMove
         */
        static bool DragMove(double x, double y);

        /**
         * @brief 拖拽时显示或隐藏图像，该函数调用ImageList_DragShowNolock
         */
        static bool DragShowNolock(bool fShow);

        /**
         * @brief 结束拖拽，该函数调用ImageList_EndDrag
         */
        static void EndDrag();

        /**
         * @brief 获取拖拽中图像的列表，该函数调用ImageList_GetDragImage
         */
        static ImageList GetDragImage(POINT *ppt, POINT *pptHotspot);

        /**
         * @brief 加载图像列表，该函数调用ImageList_LoadImageA
         */
        static ImageList LoadImageA(HINSTANCE hi, LPCSTR lpbmp, int cx, int cGrow, COLORREF crMask, UINT uType, UINT uFlags);

        /**
         * @brief 加载图像列表，该函数调用ImageList_LoadImageW
         */
        static ImageList LoadImageW(HINSTANCE hi, LPCWSTR lpbmp, int cx, int cGrow, COLORREF crMask, UINT uType, UINT uFlags);

        /**
         * @brief 合并两个图像列表，该函数调用ImageList_Merge
         */
        static ImageList Merge(const ImageList &iml1, int i1, const ImageList &iml2, int i2, int dx, int dy);

        /**
         * @brief 读取图像列表，该函数调用ImageList_Read
         */
        static ImageList Read(IStream *pstm);

    public:
        /**
         * @brief 获取图像列表的句柄
         */
        HIMAGELIST GetHandle() const;

        /**
         * @brief 判断当前对象是否为包装对象
         */
        bool IsWrap() const;

        /**
         * @brief  获取图像列表句柄并取消对句柄的托管，调用该函数后当前对象将不可用，析构时也不会销毁句柄
         * @return 当前对象的图像列表句柄
         */
        HIMAGELIST ReleaseHandle();

        /**
         * @brief 添加图像，该函数调用ImageList_Add
         */
        int Add(HBITMAP hbmImage, HBITMAP hbmMask);

        /**
         * @brief 添加图标，该函数调用ImageList_AddIcon
         */
        int AddIcon(HICON hIcon);

        /**
         * @brief 添加图像，指定颜色为mask，该函数调用ImageList_AddMasked
         */
        int AddMasked(HBITMAP hbmImage, COLORREF crMask);

        /**
         * @brief 开始拖拽图像，该函数调用ImageList_BeginDrag
         */
        bool BeginDrag(int iTrack, int dxHotspot, int dyHotspot);

        /**
         * @brief 在指定上下文DC下绘制图像，该函数调用ImageList_Draw
         */
        bool Draw(int i, HDC hdcDst, double x, double y, UINT fStyle);

        /**
         * @brief 在指定上下文DC下绘制图像，该函数调用ImageList_DrawEx
         */
        bool Draw(int i, HDC hdcDst, double x, double y, double dx, double dy, COLORREF rgbBk, COLORREF rgbFg, UINT fStyle);

        /**
         * @brief 以像素为单位，在指定上下文DC下绘制图像，该函数调用ImageList_Draw
         */
        bool DrawPx(int i, HDC hdcDst, int x, int y, UINT fStyle);

        /**
         * @brief 以像素为单位，在指定上下文DC下绘制图像，该函数调用ImageList_DrawEx
         */
        bool DrawPx(int i, HDC hdcDst, int x, int y, int dx, int dy, COLORREF rgbBk, COLORREF rgbFg, UINT fStyle);

        /**
         * @brief 复制当前图像列表，该函数调用ImageList_Duplicate
         */
        ImageList Duplicate();

        /**
         * @brief 获取背景颜色，该函数调用ImageList_GetBkColor
         */
        COLORREF GetBkColor();

        /**
         * @brief 通过指定位置的图像创建图标句柄，该函数调用ImageList_GetIcon
         */
        HICON GetIcon(int i, UINT flags);

        /**
         * @brief 获取图标大小，该函数调用ImageList_GetIconSize
         */
        bool GetIconSize(int &cx, int &cy);

        /**
         * @brief 获取图像个数，该函数调用ImageList_GetImageCount
         */
        int GetImageCount();

        /**
         * @brief 获取图像信息，该函数调用ImageList_GetImageInfo
         */
        bool GetImageInfo(int i, IMAGEINFO *pImageInfo);

        /**
         * @brief 移除指定图像，该函数调用ImageList_Remove
         */
        bool Remove(int i);

        /**
         * @brief 移除所有图像，该函数调用ImageList_Remove
         */
        bool RemoveAll();

        /**
         * @brief 更换指定位置的图像，该函数调用ImageList_Replace
         */
        bool Replace(int i, HBITMAP hbmImage, HBITMAP hbmMask);

        /**
         * @brief 更换图标，该函数调用ImageList_ReplaceIcon
         */
        int ReplaceIcon(int i, HICON hicon);

        /**
         * @brief 设置背景颜色，该函数调用ImageList_SetBkColor
         */
        COLORREF SetBkColor(COLORREF clrBk);

        /**
         * @brief 设置拖拽图标为指定图标与当前拖拽图标的结合，该函数调用ImageList_SetDragCursorImage
         */
        bool SetDragCursorImage(int iDrag, int dxHotspot, int dyHotspot);

        /**
         * @brief 设置图像大小并移除所有图像，该函数调用ImageList_SetIconSize
         */
        bool SetIconSize(int cx, int cy);

        /**
         * @brief 设置图像个数，该函数调用ImageList_SetImageCount
         */
        bool SetImageCount(UINT uNewCount);

        /**
         * @brief 将指定的图像添加到要用作覆盖遮罩的图像列表中，该函数调用ImageList_SetOverlayImage
         */
        bool SetOverlayImage(int iImage, int iOverlay);

        /**
         * @brief 写图像列表，该函数调用ImageList_Write
         */
        bool Write(IStream *pstm);

    private:
        /**
         * @brief 若_isWrap为false时调用ImageList_Destroy
         */
        void _DestroyIfNotWrap();
    };
}

// ContextMenu.h


namespace sw
{
    /**
     * @brief 上下文菜单
     */
    class ContextMenu : public MenuBase
    {
    public:
        /**
         * @brief 初始化上下文菜单
         */
        ContextMenu();

        /**
         * @brief 初始化上下文菜单并设置菜单项
         */
        ContextMenu(std::initializer_list<MenuItem> items);

        /**
         * @brief    判断ID是否为上下文菜单项的ID
         * @param id 要判断的ID
         * @return   ID是否为上下文菜单项的ID
         */
        static bool IsContextMenuID(int id);

    protected:
        /**
         * @brief       根据索引获取ID
         * @param index 索引
         * @return      菜单项的ID
         */
        virtual int IndexToID(int index) override;

        /**
         * @brief    根据ID获取索引
         * @param id 菜单项的ID
         * @return   索引
         */
        virtual int IDToIndex(int id) override;
    };
}

// Menu.h


namespace sw
{
    /**
     * @brief 菜单
     */
    class Menu : public MenuBase
    {
    public:
        /**
         * @brief 初始化菜单
         */
        Menu();

        /**
         * @brief 初始化菜单并设置菜单项
         */
        Menu(std::initializer_list<MenuItem> items);

    protected:
        /**
         * @brief       根据索引获取ID
         * @param index 索引
         * @return      菜单项的ID
         */
        virtual int IndexToID(int index) override;

        /**
         * @brief    根据ID获取索引
         * @param id 菜单项的ID
         * @return   索引
         */
        virtual int IDToIndex(int id) override;
    };
}

// WndBase.h


namespace sw
{
    class UIElement; // UIElement.h
    class Control;   // Control.h
    class Window;    // Window.h

    /**
     * @brief Action<>的别名，表示无参数、无返回值的委托
     */
    using SimpleAction = Action<>;

    /**
     * @brief 表示一个Windows窗口，是所有窗口和控件的基类
     */
    class WndBase
    {
        // 部分控件可能会改变HWND，设为友元类向Control类暴露_hwnd字段
        friend class Control;

        // HwndWrapper不使用InitWindow或InitControl初始化句柄，向其暴露底层细节以便实现相关功能
        friend class HwndWrapper;

    private:
        /**
         * @brief 用于判断给定指针是否为指向WndBase的指针
         */
        const uint32_t _check;

        /**
         * @brief 窗口句柄
         */
        HWND _hwnd = NULL;

        /**
         * @brief 字体句柄
         */
        HFONT _hfont = NULL;

        /**
         * @brief 字体信息
         */
        sw::Font _font;

        /**
         * @brief 窗口的位置和尺寸
         */
        sw::Rect _rect{};

        /**
         * @brief 窗口标题或文本
         */
        std::wstring _text{};

        /**
         * @brief 窗口是否拥有焦点
         */
        bool _focused = false;

        /**
         * @brief 窗口是否已销毁
         */
        bool _isDestroyed = false;

        /**
         * @brief 当前对象是否是控件
         */
        bool _isControl = false;

        /**
         * @brief 窗口句柄原本的WndProc
         */
        WNDPROC _originalWndProc = NULL;

    public:
        /**
         * @brief 窗口句柄
         */
        const ReadOnlyProperty<HWND> Handle;

        /**
         * @brief 字体
         */
        const Property<sw::Font> Font;

        /**
         * @brief 字体名称
         */
        const Property<std::wstring> FontName;

        /**
         * @brief 字体大小
         */
        const Property<double> FontSize;

        /**
         * @brief 字体粗细
         */
        const Property<sw::FontWeight> FontWeight;

        /**
         * @brief 位置和尺寸
         */
        const Property<sw::Rect> Rect;

        /**
         * @brief 左边
         */
        const Property<double> Left;

        /**
         * @brief 顶边
         */
        const Property<double> Top;

        /**
         * @brief 宽度
         */
        const Property<double> Width;

        /**
         * @brief 高度
         */
        const Property<double> Height;

        /**
         * @brief 用户区尺寸
         */
        const ReadOnlyProperty<sw::Rect> ClientRect;

        /**
         * @brief 用户区宽度
         */
        const ReadOnlyProperty<double> ClientWidth;

        /**
         * @brief 用户区高度
         */
        const ReadOnlyProperty<double> ClientHeight;

        /**
         * @brief 窗口或控件是否可用
         */
        const Property<bool> Enabled;

        /**
         * @brief 窗口或控件是否可见
         */
        const Property<bool> Visible;

        /**
         * @brief 窗口标题或控件文本
         */
        const Property<std::wstring> Text;

        /**
         * @brief 窗口是否拥有焦点
         */
        const Property<bool> Focused;

        /**
         * @brief 父窗口
         */
        const ReadOnlyProperty<WndBase *> Parent;

        /**
         * @brief 是否已销毁，当该值为true时不应该继续使用当前对象
         */
        const ReadOnlyProperty<bool> IsDestroyed;

        /**
         * @brief 是否接受拖放文件
         */
        const Property<bool> AcceptFiles;

        /**
         * @brief 当前对象是否是控件
         */
        const ReadOnlyProperty<bool> IsControl;

    protected:
        /**
         * @brief 初始化WndBase
         */
        WndBase();

        WndBase(const WndBase &)            = delete; // 删除拷贝构造函数
        WndBase(WndBase &&)                 = delete; // 删除移动构造函数
        WndBase &operator=(const WndBase &) = delete; // 删除拷贝赋值运算符
        WndBase &operator=(WndBase &&)      = delete; // 删除移动赋值运算符

    public:
        /**
         * @brief 析构函数，这里用纯虚函数使该类成为抽象类
         */
        virtual ~WndBase() = 0;

        /**
         * @brief 判断两个WndBase是否为同一实例
         */
        bool operator==(const WndBase &other) const;

        /**
         * @brief 判断两个WndBase是否为不同实例
         */
        bool operator!=(const WndBase &other) const;

        /**
         * @brief  尝试将对象转换成UIElement
         * @return 若函数成功则返回UIElement指针，否则返回nullptr
         */
        virtual UIElement *ToUIElement();

        /**
         * @brief  尝试将对象转换成Control
         * @return 若函数成功则返回Control指针，否则返回nullptr
         */
        virtual Control *ToControl();

        /**
         * @brief  尝试将对象转换成Window
         * @return 若函数成功则返回Window指针，否则返回nullptr
         */
        virtual Window *ToWindow();

    protected:
        /**
         * @brief 初始化为窗口，该函数会调用CreateWindowExW
         */
        void InitWindow(LPCWSTR lpWindowName, DWORD dwStyle, DWORD dwExStyle);

        /**
         * @brief 初始化为控件，该函数会调用CreateWindowExW
         */
        void InitControl(LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, DWORD dwExStyle, LPVOID lpParam = NULL);

        /**
         * @brief 调用默认的WndProc，对于窗口则调用DefWindowProcW，控件则调用_controlOldWndProc
         */
        LRESULT DefaultWndProc(const ProcMsg &refMsg);

        /**
         * @brief 对WndProc的封装
         */
        virtual LRESULT WndProc(const ProcMsg &refMsg);

        /**
         * @brief 更新_text字段
         */
        void UpdateText();

        /**
         * @brief  获取窗口文本
         * @return _text字段
         */
        virtual std::wstring &GetText();

        /**
         * @brief       调用SetWindowTextW设置窗口文本
         * @param value 要设置的文本
         */
        virtual void SetText(const std::wstring &value);

        /**
         * @brief  接收到WM_CREATE时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnCreate();

        /**
         * @brief  接收到WM_CLOSE时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnClose();

        /**
         * @brief  接收到WM_DESTROY时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnDestroy();

        /**
         * @brief  接收到WM_PAINT时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnPaint();

        /**
         * @brief 在OnPaint函数完成之后调用该函数
         */
        virtual void OnEndPaint();

        /**
         * @brief                   接收到WM_MOVE时调用该函数
         * @param newClientPosition 移动后用户区左上角的位置
         * @return                  若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMove(Point newClientPosition);

        /**
         * @brief               接收到WM_SIZE时调用该函数
         * @param newClientSize 改变后的用户区尺寸
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSize(Size newClientSize);

        /**
         * @brief Text属性更改时调用此函数
         */
        virtual void OnTextChanged();

        /**
         * @brief            接收到WM_SETFOCUS时调用该函数
         * @param hPrevFocus 丢失焦点的hwnd，可能为NULL
         * @return           若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSetFocus(HWND hPrevFocus);

        /**
         * @brief            接收到WM_KILLFOCUS时调用该函数
         * @param hNextFocus 接收到焦点的hwnd，可能为NULL
         * @return           若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnKillFocus(HWND hNextFocus);

        /**
         * @brief               接收到WM_MOUSEMOVE时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseMove(Point mousePosition, MouseKey keyState);

        /**
         * @brief  接收到WM_MOUSELEAVE时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseLeave();

        /**
         * @brief               接收到WM_MOUSEWHEEL时调用该函数
         * @param wheelDelta    滚轮滚动的距离，为120的倍数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseWheel(int wheelDelta, Point mousePosition, MouseKey keyState);

        /**
         * @brief               接收到WM_LBUTTONDOWN时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseLeftButtonDown(Point mousePosition, MouseKey keyState);

        /**
         * @brief               接收到WM_LBUTTONUP时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseLeftButtonUp(Point mousePosition, MouseKey keyState);

        /**
         * @brief               接收到WM_LBUTTONDBLCLK时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseLeftButtonDoubleClick(Point mousePosition, MouseKey keyState);

        /**
         * @brief               接收到WM_RBUTTONDOWN时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseRightButtonDown(Point mousePosition, MouseKey keyState);

        /**
         * @brief               接收到WM_RBUTTONUP时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseRightButtonUp(Point mousePosition, MouseKey keyState);

        /**
         * @brief               接收到WM_RBUTTONDBLCLK时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseRightButtonDoubleClick(Point mousePosition, MouseKey keyState);

        /**
         * @brief               接收到WM_MBUTTONDOWN时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseMiddleButtonDown(Point mousePosition, MouseKey keyState);

        /**
         * @brief               接收到WM_MBUTTONUP时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseMiddleButtonUp(Point mousePosition, MouseKey keyState);

        /**
         * @brief               接收到WM_MBUTTONDBLCLK时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseMiddleButtonDoubleClick(Point mousePosition, MouseKey keyState);

        /**
         * @brief       接收到WM_CHAR时调用该函数
         * @param ch    按键的字符代码
         * @param flags 附加信息
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnChar(wchar_t ch, KeyFlags flags);

        /**
         * @brief       接收到WM_DEADCHAR时调用该函数
         * @param ch    按键的字符代码
         * @param flags 附加信息
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnDeadChar(wchar_t ch, KeyFlags flags);

        /**
         * @brief       接收到WM_KEYDOWN时调用该函数
         * @param key   虚拟按键
         * @param flags 附加信息
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnKeyDown(VirtualKey key, KeyFlags flags);

        /**
         * @brief       接收到WM_KEYUP时调用该函数
         * @param key   虚拟按键
         * @param flags 附加信息
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnKeyUp(VirtualKey key, KeyFlags flags);

        /**
         * @brief       接收到WM_SYSCHAR时调用该函数
         * @param ch    按键的字符代码
         * @param flags 附加信息
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSysChar(wchar_t ch, KeyFlags flags);

        /**
         * @brief       接收到WM_SYSDEADCHAR时调用该函数
         * @param ch    按键的字符代码
         * @param flags 附加信息
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSysDeadChar(wchar_t ch, KeyFlags flags);

        /**
         * @brief       接收到WM_SYSKEYDOWN时调用该函数
         * @param key   虚拟按键
         * @param flags 附加信息
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSysKeyDown(VirtualKey key, KeyFlags flags);

        /**
         * @brief       接收到WM_SYSKEYUP时调用该函数
         * @param key   虚拟按键
         * @param flags 附加信息
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSysKeyUp(VirtualKey key, KeyFlags flags);

        /**
         * @brief Visible属性改变时调用此函数
         */
        virtual void VisibleChanged(bool newVisible);

        /**
         * @brief  设置父窗口
         * @return 设置是否成功
         */
        virtual bool SetParent(WndBase *parent);

        /**
         * @brief           父窗口改变时调用此函数
         * @param newParent 新的父窗口
         */
        virtual void ParentChanged(WndBase *newParent);

        /**
         * @brief      当父窗口接收到控件的WM_COMMAND时调用该函数
         * @param code 通知代码
         */
        virtual void OnCommand(int code);

        /**
         * @brief          当WM_COMMAND接收到控件命令时调用该函数
         * @param pControl 控件对象指针
         * @param code     通知代码
         * @param id       控件id
         */
        virtual void OnControlCommand(WndBase *pControl, int code, int id);

        /**
         * @brief    当WM_COMMAND接收到菜单命令时调用该函数
         * @param id 菜单id
         */
        virtual void OnMenuCommand(int id);

        /**
         * @brief    当WM_COMMAND接收到快捷键命令时调用该函数
         * @param id 快捷键id
         */
        virtual void OnAcceleratorCommand(int id);

        /**
         * @brief      窗口句柄初始化完成
         * @param hwnd 窗口句柄
         */
        virtual void HandleInitialized(HWND hwnd);

        /**
         * @brief       字体改变时调用该函数
         * @param hfont 字体句柄
         */
        virtual void FontChanged(HFONT hfont);

        /**
         * @brief         接收到WM_SETCURSOR消息时调用该函数
         * @param hwnd    鼠标所在窗口的句柄
         * @param hitTest hit-test的结果，详见WM_NCHITTEST消息的返回值
         * @param message 触发该事件的鼠标消息，如WM_MOUSEMOVE
         * @param result  消息的返回值，默认为false
         * @return        若返回true则将result作为消息的返回值，否则使用DefaultWndProc的返回值
         */
        virtual bool OnSetCursor(HWND hwnd, HitTestResult hitTest, int message, bool &result);

        /**
         * @brief               接收到WM_CONTEXTMENU后调用目标控件的该函数
         * @param isKeyboardMsg 消息是否由按下快捷键（Shift+F10、VK_APPS）产生
         * @param mousePosition 鼠标在屏幕中的位置
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnContextMenu(bool isKeyboardMsg, Point mousePosition);

        /**
         * @brief        接收到WM_NOTIFY后调用该函数
         * @param pNMHDR 包含有关通知消息的信息
         * @param result 函数返回值为true时将该值作为消息的返回值，默认值为0
         * @return       若已处理该消息则返回true，否则调用发出通知控件的OnNotified函数，依据其返回值判断是否调用DefaultWndProc
         */
        virtual bool OnNotify(NMHDR *pNMHDR, LRESULT &result);

        /**
         * @brief        父窗口接收到WM_NOTIFY后且父窗口OnNotify函数返回false时调用发出通知控件的该函数
         * @param pNMHDR 包含有关通知消息的信息
         * @param result 函数返回值为true时将该值作为消息的返回值
         * @return       若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnNotified(NMHDR *pNMHDR, LRESULT &result);

        /**
         * @brief       接收到WM_VSCROLL时调用目标控件的该函数
         * @param event 事件类型，即消息wParam的低字
         * @param pos   当前滚动条的位置，仅当event为SB_THUMBPOSITION或SB_THUMBTRACK时有效
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnVerticalScroll(int event, int pos);

        /**
         * @brief       接收到WM_HSCROLL时调用目标控件的该函数
         * @param event 事件类型，即消息wParam的低字
         * @param pos   当前滚动条的位置，仅当event为SB_THUMBPOSITION或SB_THUMBTRACK时有效
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnHorizontalScroll(int event, int pos);

        /**
         * @brief          接收到WM_ENABLE时调用该函数
         * @param newValue Enabled的新值
         * @return         若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnEnabledChanged(bool newValue);

        /**
         * @brief           接收到WM_CTLCOLORxxx时调用该函数
         * @param pControl  消息相关的控件
         * @param hdc       控件的显示上下文句柄
         * @param hRetBrush 要返回的画笔
         * @return          若返回true则将hRetBrush作为消息的返回值，否则使用DefaultWndProc的返回值
         */
        virtual bool OnCtlColor(WndBase *pControl, HDC hdc, HBRUSH &hRetBrush);

        /**
         * @brief           父窗口接收到WM_CTLCOLORxxx时调用对应控件的该函数
         * @param hdc       控件的显示上下文句柄
         * @param hRetBrush 要返回的画笔
         * @return          若返回true则将hRetBrush作为消息的返回值，否则使用DefaultWndProc的返回值
         */
        virtual bool OnColor(HDC hdc, HBRUSH &hRetBrush);

        /**
         * @brief           接收到WM_NCHITTEST后调用该函数
         * @param testPoint 要测试的点在屏幕中的位置
         * @param result    测试的结果，默认为调用DefaultWndProc的结果
         */
        virtual void OnNcHitTest(const Point &testPoint, HitTestResult &result);

        /**
         * @brief        接收到WM_ERASEBKGND时调用该函数
         * @param hdc    设备上下文句柄
         * @param result 若已处理该消息则设为非零值，默认值为0
         * @return       若返回true则将result作为消息的返回值，否则使用DefaultWndProc的返回值
         */
        virtual bool OnEraseBackground(HDC hdc, LRESULT &result);

        /**
         * @brief           接收到WM_DRAWITEM时调用该函数
         * @param id        控件的标识符，若消息是通过菜单发送的则此参数为零
         * @param pDrawItem 包含有关要绘制的项和所需绘图类型的信息的结构体指针
         * @return          若已处理该消息则返回true，否则调用发出通知控件的OnDrawItemSelf函数，依据其返回值判断是否调用DefaultWndProc
         */
        virtual bool OnDrawItem(int id, DRAWITEMSTRUCT *pDrawItem);

        /**
         * @brief           父窗口接收到WM_DRAWITEM后且父窗口OnDrawItem函数返回false时调用发出通知控件的该函数
         * @param pDrawItem 包含有关要绘制的项和所需绘图类型的信息的结构体指针
         * @return          若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnDrawItemSelf(DRAWITEMSTRUCT *pDrawItem);

        /**
         * @brief          接收到WM_MEASUREITEM时调用该函数
         * @param id       控件的标识符，若消息是通过菜单发送的则此参数为零
         * @param pMeasure 包含有关要绘制的项的信息的结构体指针
         * @return         若已处理该消息则返回true，否则调用发出通知控件的OnMeasureItemSelf函数，依据其返回值判断是否调用DefaultWndProc
         */
        virtual bool OnMeasureItem(int id, MEASUREITEMSTRUCT *pMeasure);

        /**
         * @brief          父窗口接收到WM_MEASUREITEM后且父窗口OnMeasureItem函数返回false时调用发出通知控件的该函数
         * @param pMeasure 包含有关要绘制的项的信息的结构体指针
         * @return         若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMeasureItemSelf(MEASUREITEMSTRUCT *pMeasure);

        /**
         * @brief       接收到WM_DROPFILES时调用该函数
         * @param hDrop 描述拖入文件的句柄
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnDropFiles(HDROP hDrop);

    public:
        /**
         * @brief 该函数调用ShowWindow
         */
        void Show(int nCmdShow);

        /**
         * @brief 发送关闭消息
         */
        void Close();

        /**
         * @brief 该函数调用UpdateWindow
         */
        void Update();

        /**
         * @brief 更新字体
         */
        void UpdateFont();

        /**
         * @brief 获取字体句柄
         */
        HFONT GetFontHandle();

        /**
         * @brief              重画
         * @param erase        是否擦除旧的背景
         * @param updateWindow 是否调用UpdateWindow
         */
        void Redraw(bool erase = false, bool updateWindow = false);

        /**
         * @brief 判断当前对象在界面中是否可视，与Visible属性不同的是该函数返回值会受父窗口的影响
         */
        bool IsVisible();

        /**
         * @brief 获取窗口样式
         */
        DWORD GetStyle();

        /**
         * @brief 设置窗口样式
         */
        void SetStyle(DWORD style);

        /**
         * @brief      判断窗口是否设有指定样式
         * @param mask 样式的位掩码，可以是多个样式
         */
        bool GetStyle(DWORD mask);

        /**
         * @brief       打开或关闭指定的样式
         * @param mask  样式的位掩码，可以是多个样式
         * @param value 是否启用指定的样式
         */
        void SetStyle(DWORD mask, bool value);

        /**
         * @brief 获取扩展窗口样式
         */
        DWORD GetExtendedStyle();

        /**
         * @brief 设置扩展窗口样式
         */
        void SetExtendedStyle(DWORD style);

        /**
         * @brief      判断窗口是否设有指定扩展样式
         * @param mask 扩展样式的位掩码，可以是多个扩展样式
         */
        bool GetExtendedStyle(DWORD mask);

        /**
         * @brief       打开或关闭指定的扩展样式
         * @param mask  扩展样式的位掩码，可以是多个扩展样式
         * @param value 是否启用指定的扩展样式
         */
        void SetExtendedStyle(DWORD mask, bool value);

        /**
         * @brief       获取用户区点在屏幕上点的位置
         * @param point 用户区坐标
         * @return      该点在屏幕上的坐标
         */
        Point PointToScreen(const Point &point);

        /**
         * @brief             获取屏幕上点在当前用户区点的位置
         * @param screenPoint 屏幕上点的坐标
         * @return            该点在用户区的坐标
         */
        Point PointFromScreen(const Point &screenPoint);

        /**
         * @brief 发送消息（ASCII）
         */
        LRESULT SendMessageA(UINT uMsg, WPARAM wParam, LPARAM lParam);

        /**
         * @brief 发送消息（UNICODE）
         */
        LRESULT SendMessageW(UINT uMsg, WPARAM wParam, LPARAM lParam);

        /**
         * @brief 发送消息（ASCII）并立即返回
         */
        BOOL PostMessageA(UINT uMsg, WPARAM wParam, LPARAM lParam);

        /**
         * @brief 发送消息（UNICODE）并立即返回
         */
        BOOL PostMessageW(UINT uMsg, WPARAM wParam, LPARAM lParam);

        /**
         * @brief           测试指定点在窗口的哪一部分
         * @param testPoint 要测试的点在屏幕中的位置
         */
        HitTestResult NcHitTest(const Point &testPoint);

        /**
         * @brief        在窗口线程上执行指定委托
         * @param action 要执行的委托
         */
        void Invoke(const SimpleAction &action);

        /**
         * @brief        在窗口线程上执行指定委托，并立即返回
         * @param action 要执行的委托
         */
        void InvokeAsync(const SimpleAction &action);

    private:
        /**
         * @brief 窗口过程函数，调用对象的WndProc
         */
        static LRESULT CALLBACK _WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

        /**
         * @brief 初始化控件创建时所在的容器
         */
        static void _InitControlContainer();

        /**
         * @brief 获取一个新的控件id
         */
        static int _NextControlId();

        /**
         * @brief      关联窗口句柄与WndBase对象
         * @param hwnd 窗口句柄
         * @param wnd  与句柄关联的对象
         */
        static void _SetWndBase(HWND hwnd, WndBase &wnd);

    public:
        /**
         * @brief      通过窗口句柄获取WndBase
         * @param hwnd 窗口句柄
         * @return     若函数成功则返回对象的指针，否则返回nullptr
         */
        static WndBase *GetWndBase(HWND hwnd);
    };
}

// LayoutHost.h


namespace sw
{
    /**
     * @brief 用于托管元素的布局方式的对象类型，是所有布局方式类型的基类
     */
    class LayoutHost : public ILayout
    {
    private:
        /**
         * @brief 关联的对象
         */
        ILayout *_associatedObj = nullptr;

    public:
        /**
         * @brief     设置关联的对象，每个LayoutHost只能关联一个对象
         * @param obj 要关联的对象
         */
        void Associate(ILayout *obj);

    public:
        /**
         * @brief 获取布局标记
         */
        virtual uint64_t GetLayoutTag() override;

        /**
         * @brief 获取关联对象子控件的数量
         */
        virtual int GetChildLayoutCount() override;

        /**
         * @brief 获取关联对象对应索引处的子控件
         */
        virtual ILayout &GetChildLayoutAt(int index) override;

        /**
         * @brief 获取关联对象所需尺寸
         */
        virtual Size GetDesireSize() override;

        /**
         * @brief 设置关联对象所需的尺寸
         */
        virtual void SetDesireSize(const Size &size) override;

        /**
         * @brief               测量控件所需尺寸
         * @param availableSize 可用的尺寸
         */
        virtual void Measure(const Size &availableSize) override;

        /**
         * @brief               安排控件位置
         * @param finalPosition 最终控件所安排的位置
         */
        virtual void Arrange(const Rect &finalPosition) override;

        /**
         * @brief 重写此函数计算所需尺寸
         */
        virtual void MeasureOverride(Size &availableSize) = 0;

        /**
         * @brief 重写此函数安排控件
         */
        virtual void ArrangeOverride(Size &finalSize) = 0;
    };
}

// Dictionary.h


namespace sw
{
    template <typename TKey, typename TVal>
    class Dictionary; // 向前声明

    /**
     * @brief 以字符串为键值的字典
     */
    template <typename TVal>
    using StrDictionary = Dictionary<std::wstring, TVal>;

    /**
     * @brief 字典类，内部维护了一个指向std::map的智能指针
     */
    template <typename TKey, typename TVal>
    class Dictionary
    {
    private:
        /**
         * @brief 指向std::map的智能指针
         */
        std::shared_ptr<std::map<TKey, TVal>> _pMap;

    public:
        /**
         * @brief 初始化字典
         */
        Dictionary()
            : _pMap(std::make_shared<std::map<TKey, TVal>>())
        {
        }

        /**
         * @brief 使用初始化列表
         */
        Dictionary(std::initializer_list<std::pair<const TKey, TVal>> list)
            : _pMap(std::make_shared<std::map<TKey, TVal>>(list))
        {
        }

        /**
         * @brief 正向迭代器开始
         */
        auto begin() const
        {
            return this->_pMap->begin();
        }

        /**
         * @brief 正向迭代器结束
         */
        auto end() const
        {
            return this->_pMap->end();
        }

        /**
         * @brief 反向迭代器开始
         */
        auto rbegin() const
        {
            return this->_pMap->rbegin();
        }

        /**
         * @brief 反向迭代器结束
         */
        auto rend() const
        {
            return this->_pMap->rend();
        }

        /**
         * @brief     获取或设置值
         * @param key 键值
         */
        auto &operator[](const TKey &key) const
        {
            return this->_pMap->at(key);
        }

        /**
         * @brief 判断是否为同一个字典
         */
        bool operator==(const Dictionary &other) const
        {
            return this->_pMap == other._pMap;
        }

        /**
         * @brief 判断是否不是同一个字典
         */
        bool operator!=(const Dictionary &other) const
        {
            return this->_pMap != other._pMap;
        }

        /**
         * @brief 获取键值对个数
         */
        int Count() const
        {
            return (int)this->_pMap->size();
        }

        /**
         * @brief 字典是否为空
         */
        bool IsEmpty() const
        {
            return this->_pMap->empty();
        }

        /**
         * @brief  添加键值对到字典
         * @return 当前字典
         */
        auto Add(const TKey &key, const TVal &value) const
        {
            this->_pMap->insert(std::make_pair(key, value));
            return *this;
        }

        /**
         * @brief     是否存在某个键值
         * @param key 要查询的键值
         */
        bool ContainsKey(const TKey &key) const
        {
            return this->_pMap->count(key);
        }

        /**
         * @brief       遍历字典，查询是否存在某个值
         * @param value 要查询的值
         */
        bool ContainsValue(const TVal &value) const
        {
            for (const auto &pair : *this->_pMap) {
                if (pair.second == value) {
                    return true;
                }
            }
            return false;
        }

        /**
         * @brief     移除指定键值对
         * @param key 要删除的键值
         */
        void Remove(const TKey &key) const
        {
            this->_pMap->erase(key);
        }

        /**
         * @brief 清空字典
         */
        void Clear() const
        {
            this->_pMap->clear();
        }

        /**
         * @brief 复制当前字典
         */
        Dictionary Copy() const
        {
            Dictionary dic;
            dic._pMap->insert(this->_pMap->begin(), this->_pMap->end());
            return dic;
        }

        /**
         * @brief 获取字典内部维护的std::map
         */
        std::map<TKey, TVal> &GetStdMap() const
        {
            return *this->_pMap;
        }

        /**
         * @brief 获取描述当前对象的字符串
         */
        std::wstring ToString() const
        {
            return Utils::BuildStr(*this->_pMap);
        }
    };
}

// DockLayout.h


namespace sw
{
    /**
     * @brief 停靠布局标记
     */
    class DockLayoutTag
    {
    public:
        enum : uint64_t {
            Left,   // 左边
            Top,    // 顶边
            Right,  // 右边
            Bottom, // 底边
        };

    private:
        /**
         * @brief Tag值
         */
        uint64_t _value;

    public:
        /**
         * @brief 创建DockLayoutTag
         */
        DockLayoutTag(uint64_t value = Left);

        /**
         * @brief 隐式转换uint64_t
         */
        operator uint64_t() const;

        /**
         * @brief 判断值是否相等
         */
        bool operator==(const DockLayoutTag &other) const;

        /**
         * @brief 判断值是否不相等
         */
        bool operator!=(const DockLayoutTag &other) const;

        /**
         * @brief 判断值是否相等
         */
        bool operator==(uint64_t value) const;

        /**
         * @brief 判断值是否不相等
         */
        bool operator!=(uint64_t value) const;
    };

    /**
     * @brief 停靠布局
     */
    class DockLayout : public LayoutHost
    {
    public:
        /**
         * @brief 最后一个子元素是否填充剩余空间
         */
        bool lastChildFill = true;

        /**
         * @brief 计算所需尺寸
         */
        virtual void MeasureOverride(Size &availableSize) override;

        /**
         * @brief 安排控件
         */
        virtual void ArrangeOverride(Size &finalSize) override;
    };
}

// WrapLayoutH.h


namespace sw
{
    /**
     * @brief 横向自动换行布局
     */
    class WrapLayoutH : virtual public LayoutHost
    {
    public:
        /**
         * @brief 计算所需尺寸
         */
        virtual void MeasureOverride(Size &availableSize) override;

        /**
         * @brief 安排控件
         */
        virtual void ArrangeOverride(Size &finalSize) override;
    };
}

// MsgBox.h


namespace sw
{
    /**
     * @brief 消息框回调
     */
    using MsgBoxCallback = Action<>;

    /**
     * @brief 消息框按钮类型
     */
    enum class MsgBoxButton {
        Ok          = MB_OK,
        OkCancel    = MB_OKCANCEL,
        YesNoCancel = MB_YESNOCANCEL,
        YesNo       = MB_YESNO,
    };

    /**
     * @brief 消息框结果
     */
    enum class MsgBoxResult {
        Ok     = IDOK,
        Cancel = IDCANCEL,
        Yes    = IDYES,
        No     = IDNO,
    };

    /**
     * @brief 处理消息框消息的帮助类
     */
    struct MsgBoxResultHelper {
        /**
         * @brief 消息框的结果
         */
        MsgBoxResult result;

        /**
         * @brief 构造MsgBoxResultHelper
         */
        MsgBoxResultHelper(MsgBoxResult result);

        /**
         * @brief 隐式转换MsgBoxResult
         */
        operator sw::MsgBoxResult() const;

        /**
         * @brief 指定按下“确定”按钮时的处理函数
         */
        MsgBoxResultHelper &OnOk(const MsgBoxCallback &callback);

        /**
         * @brief 指定按下“是”按钮时的处理函数
         */
        MsgBoxResultHelper &OnYes(const MsgBoxCallback &callback);

        /**
         * @brief 指定按下“否”按钮时的处理函数
         */
        MsgBoxResultHelper &OnNo(const MsgBoxCallback &callback);

        /**
         * @brief 指定按下“取消”按钮时的处理函数
         */
        MsgBoxResultHelper &OnCancel(const MsgBoxCallback &callback);

        /**
         * @brief 指定消息框结果的处理函数
         */
        template <MsgBoxResult RES>
        MsgBoxResultHelper &On(const MsgBoxCallback &callback)
        {
            if (this->result == RES) {
                if (callback) callback();
            }
            return *this;
        }
    };

    /**
     * @brief 消息框类
     */
    class MsgBox
    {
    private:
        MsgBox() = delete;

    public:
        /**
         * @brief 显示一个不含图标的消息框
         */
        static MsgBoxResultHelper Show(const WndBase *owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok);

        /**
         * @brief 显示一个不含图标的消息框
         */
        static MsgBoxResultHelper Show(const WndBase &owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok);

        /**
         * @brief 显示一个不含图标的消息框，将当前活动窗体作为Owner
         */
        static MsgBoxResultHelper Show(const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok);

        /**
         * @brief 显示一个含“消息”图标的消息框
         */
        static MsgBoxResultHelper ShowInfo(const WndBase *owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok);

        /**
         * @brief 显示一个含“消息”图标的消息框
         */
        static MsgBoxResultHelper ShowInfo(const WndBase &owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok);

        /**
         * @brief 显示一个含“消息”图标的消息框，将当前活动窗体作为Owner
         */
        static MsgBoxResultHelper ShowInfo(const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok);

        /**
         * @brief 显示一个含“错误”图标的消息框
         */
        static MsgBoxResultHelper ShowError(const WndBase *owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok);

        /**
         * @brief 显示一个含“错误”图标的消息框
         */
        static MsgBoxResultHelper ShowError(const WndBase &owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok);

        /**
         * @brief 显示一个含“错误”图标的消息框，将当前活动窗体作为Owner
         */
        static MsgBoxResultHelper ShowError(const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok);

        /**
         * @brief 显示一个含“警告”图标的消息框
         */
        static MsgBoxResultHelper ShowWarning(const WndBase *owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok);

        /**
         * @brief 显示一个含“警告”图标的消息框
         */
        static MsgBoxResultHelper ShowWarning(const WndBase &owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok);

        /**
         * @brief 显示一个含“警告”图标的消息框，将当前活动窗体作为Owner
         */
        static MsgBoxResultHelper ShowWarning(const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::Ok);

        /**
         * @brief 显示一个含“问题”图标的消息框
         */
        static MsgBoxResultHelper ShowQuestion(const WndBase *owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::YesNo);

        /**
         * @brief 显示一个含“问题”图标的消息框
         */
        static MsgBoxResultHelper ShowQuestion(const WndBase &owner, const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::YesNo);

        /**
         * @brief 显示一个含“问题”图标的消息框，将当前活动窗体作为Owner
         */
        static MsgBoxResultHelper ShowQuestion(const std::wstring &text = L"", const std::wstring &caption = L"", MsgBoxButton button = MsgBoxButton::YesNo);
    };
}

// ToolTip.h


namespace sw
{
    /**
     * @brief 提示信息图标类型
     */
    enum class ToolTipIcon {
        None         = TTI_NONE,                // 无图标
        Info         = TTI_INFO,                // 信息图标
        Warning      = TTI_WARNING,             // 警告图标
        Error        = TTI_ERROR,               // 错误图标
        LargeInfo    = 4 /*TTI_INFO_LARGE*/,    // 大错误图标
        LargeWarning = 5 /*TTI_WARNING_LARGE*/, // 大错误图标
        LargeError   = 6 /*TTI_ERROR_LARGE*/,   // 大错误图标
    };

    /**
     * @brief 信息提示，用于用户将指针移过关联控件时显示信息
     */
    class ToolTip : public WndBase
    {
    private:
        /**
         * @brief 图标
         */
        ToolTipIcon _icon{ToolTipIcon::None};

        /**
         * @brief 标题
         */
        std::wstring _title{};

    public:
        /**
         * @brief 触发提示信息的时间，以毫秒为单位，设置负数可恢复默认值
         */
        const Property<int> InitialDelay;

        /**
         * @brief 提示框中显示的图标，标题不为空时图标才会显示
         */
        const Property<ToolTipIcon> ToolTipIcon;

        /**
         * @brief 提示框中显示的标题
         */
        const Property<std::wstring> ToolTipTitle;

        /**
         * @brief 提示框的最大宽度，若未设置则为-1，设置负值可取消限制
         */
        const Property<double> MaxTipWidth;

    public:
        /**
         * @brief 初始化ToolTip
         */
        ToolTip();

        /**
         * @brief 初始化ToolTip，指定窗口样式
         */
        explicit ToolTip(DWORD style);

    public:
        /**
         * @brief         给指定句柄设置提示信息
         * @param hwnd    要设置提示信息的句柄
         * @param tooltip 提示信息
         */
        bool SetToolTip(HWND hwnd, const std::wstring &tooltip);

        /**
         * @brief         给指定窗口或控件设置提示信息
         * @param wnd     要设置提示信息的窗口或控件
         * @param tooltip 提示信息
         */
        bool SetToolTip(const WndBase &wnd, const std::wstring &tooltip);

        /**
         * @brief 移除所有关联的提示信息
         */
        void RemoveAll();

    private:
        /**
         * @brief 更新图标和标题
         */
        void _UpdateIconAndTitle();
    };

    /**
     * @brief 气泡样式的信息提示
     */
    class BallonToolTip : public ToolTip
    {
    public:
        /**
         * @brief 初始化BallonToolTip
         */
        BallonToolTip();
    };
}

// UIElement.h


namespace sw
{
    /**
     * @brief 通知布局更新的条件
     */
    enum class LayoutUpdateCondition : uint32_t {
        /**
         * @brief 表示不需要更新布局
         * @note  一旦设置了该标记，NotifyLayoutUpdated函数将不会触发WM_UpdateLayout消息
         * @note  框架内部使用该标记来抑制布局更新，可能会频繁被设置/取消，一般不建议用户直接使用
         */
        Supressed = 1,

        /**
         * @brief 尺寸改变时更新布局
         */
        SizeChanged = 2,

        /**
         * @brief 位置改变时更新布局
         */
        PositionChanged = 4,

        /**
         * @brief 添加子元素时更新布局
         */
        ChildAdded = 8,

        /**
         * @brief 移除子元素时更新布局
         */
        ChildRemoved = 16,

        /**
         * @brief 文本改变时更新布局
         */
        TextChanged = 32,

        /**
         * @brief 字体改变时更新布局
         */
        FontChanged = 64,
    };

    /**
     * @brief 标记LayoutUpdateCondition支持位操作
     */
    _SW_ENUM_ENABLE_BIT_OPERATIONS(LayoutUpdateCondition);

    /**
     * @brief 表示界面中的元素
     */
    class UIElement : public WndBase, public ILayout, public ITag
    {
    private:
        /**
         * @brief 布局更新条件
         */
        sw::LayoutUpdateCondition _layoutUpdateCondition =
            sw::LayoutUpdateCondition::SizeChanged |
            sw::LayoutUpdateCondition::ChildAdded |
            sw::LayoutUpdateCondition::ChildRemoved;

        /**
         * @brief 是否在不可见时不参与布局
         */
        bool _collapseWhenHide = true;

        /**
         * @brief 当前元素所需要占用的尺寸
         */
        Size _desireSize{};

        /**
         * @brief 当对齐方式为拉伸时用该字段存储原始大小
         */
        Size _origionalSize{};

        /**
         * @brief 边距
         */
        Thickness _margin{};

        /**
         * @brief 水平对齐方式
         */
        HorizontalAlignment _horizontalAlignment = HorizontalAlignment::Center;

        /**
         * @brief 垂直对齐方式
         */
        VerticalAlignment _verticalAlignment = VerticalAlignment::Center;

        /**
         * @brief 指向父元素的指针，在调用SetParent后会更新该值
         */
        UIElement *_parent = nullptr;

        /**
         * @brief 所有子窗口
         */
        std::vector<UIElement *> _children{};

        /**
         * @brief 参与布局的子窗口，在调用GetChildLayoutCount后会更新，不可见且CollapseWhenHide为true的控件会被忽略
         */
        std::vector<UIElement *> _childrenNotCollapsed{};

        /**
         * @brief 记录路由事件的map
         */
        std::map<RoutedEventType, RoutedEventHandler> _eventMap{};

        /**
         * @brief 储存用户自定义信息
         */
        uint64_t _tag = 0;

        /**
         * @brief 布局标记
         */
        uint64_t _layoutTag = 0;

        /**
         * @brief 上下文菜单
         */
        sw::ContextMenu *_contextMenu = nullptr;

        /**
         * @brief Arrange时子元素的水平偏移量
         */
        double _arrangeOffsetX = 0;

        /**
         * @brief Arrange时子元素的垂直偏移量
         */
        double _arrangeOffsetY = 0;

        /**
         * @brief 所有子元素在当前元素中最右边的位置
         */
        double _childRightmost = 0;

        /**
         * @brief 所有子元素在当前元素中最底边的位置
         */
        double _childBottommost = 0;

        /**
         * @brief 元素是否悬浮，若元素悬浮则该元素不会随滚动条滚动而改变位置
         */
        bool _float = false;

        /**
         * @brief 表示用户是否可以通过按下Tab键将焦点移动到当前元素
         */
        bool _tabStop = false;

        /**
         * @brief 背景颜色
         */
        Color _backColor{KnownColor::White};

        /**
         * @brief 文本颜色
         */
        Color _textColor{KnownColor::Black};

        /**
         * @brief 是否使用透明背景
         */
        bool _transparent = false;

        /**
         * @brief 是否继承父元素的文本颜色
         */
        bool _inheritTextColor = false;

        /**
         * @brief 是否使用默认的鼠标样式
         */
        bool _useDefaultCursor = true;

        /**
         * @brief 鼠标句柄
         */
        HCURSOR _hCursor = NULL;

    public:
        /**
         * @brief 边距
         */
        const Property<Thickness> Margin;

        /**
         * @brief 水平对齐方式
         */
        const Property<HorizontalAlignment> HorizontalAlignment;

        /**
         * @brief 垂直对齐方式
         */
        const Property<VerticalAlignment> VerticalAlignment;

        /**
         * @brief 子元素数量
         */
        const ReadOnlyProperty<int> ChildCount;

        /**
         * @brief 是否在不可见时不参与布局
         */
        const Property<bool> CollapseWhenHide;

        /**
         * @brief 指向父元素的指针，当前元素为顶级窗口时该值为nullptr
         */
        const ReadOnlyProperty<UIElement *> Parent;

        /**
         * @brief 储存用户自定义信息的标记
         */
        const Property<uint64_t> Tag;

        /**
         * @brief 布局标记，对于不同的布局有不同含义
         */
        const Property<uint64_t> LayoutTag;

        /**
         * @brief 右键按下时弹出的菜单
         */
        const Property<sw::ContextMenu *> ContextMenu;

        /**
         * @brief 元素是否悬浮，若元素悬浮则该元素不会随滚动条滚动而改变位置
         */
        const Property<bool> Float;

        /**
         * @brief 表示用户是否可以通过按下Tab键将焦点移动到当前元素
         */
        const Property<bool> TabStop;

        /**
         * @brief 背景颜色，修改该属性会同时将Transparent属性设为false，对于部分控件该属性可能不生效
         */
        const Property<Color> BackColor;

        /**
         * @brief 文本颜色，修改该属性会同时将InheritTextColor属性设为false，对于部分控件该属性可能不生效
         */
        const Property<Color> TextColor;

        /**
         * @brief 是否使用透明背景（此属性并非真正意义上的透明，将该属性设为true可继承父元素的背景颜色）
         */
        const Property<bool> Transparent;

        /**
         * @brief 是否继承父元素的文本颜色
         */
        const Property<bool> InheritTextColor;

        /**
         * @brief 触发布局更新的条件
         * @note  修改该属性不会立即触发布局更新
         */
        const Property<sw::LayoutUpdateCondition> LayoutUpdateCondition;

    protected:
        /**
         * @brief 初始化UIElement
         */
        UIElement();

    public:
        /**
         * @brief 析构函数，这里用纯虚函数使该类成为抽象类
         */
        virtual ~UIElement() = 0;

        /**
         * @brief           注册路由事件处理函数，当事件已注册时会覆盖已注册的函数
         * @param eventType 路由事件类型
         * @param handler   处理函数，当值为nullptr时可取消注册
         * @deprecated      建议使用AddHandler函数代替以避免覆盖已注册的事件处理函数
         */
        void RegisterRoutedEvent(RoutedEventType eventType, const RoutedEventHandler &handler);

        /**
         * @brief           注册成员函数作为路由事件处理函数，当事件已注册时会覆盖已注册的函数
         * @tparam T        成员函数所在的类
         * @param eventType 路由事件类型
         * @param obj       注册的成员函数所在的对象
         * @param handler   处理函数，当值为nullptr时可取消注册
         * @deprecated      建议使用AddHandler函数代替以避免覆盖已注册的事件处理函数
         */
        template <typename T>
        void RegisterRoutedEvent(RoutedEventType eventType, T &obj, void (T::*handler)(UIElement &, RoutedEventArgs &))
        {
            if (handler == nullptr) {
                this->UnregisterRoutedEvent(eventType);
            } else {
                this->RegisterRoutedEvent(eventType, RoutedEventHandler(obj, handler));
            }
        }

        /**
         * @brief             根据事件参数类型注册路由事件，当事件已注册时会覆盖已注册的函数
         * @tparam TEventArgs 路由事件的参数类型，必须继承自TypedRoutedEventArgs<...>
         * @param handler     事件的处理函数，当值为nullptr时可取消注册
         * @deprecated        建议使用AddHandler函数代替以避免覆盖已注册的事件处理函数
         */
        template <typename TEventArgs>
        typename std::enable_if<std::is_base_of<RoutedEventArgs, TEventArgs>::value && sw::_IsTypedRoutedEventArgs<TEventArgs>::value>::type
        RegisterRoutedEvent(const Action<UIElement &, TEventArgs &> &handler)
        {
            if (!handler) {
                this->UnregisterRoutedEvent(TEventArgs::EventType);
            } else {
                this->RegisterRoutedEvent(TEventArgs::EventType, RoutedEventHandlerWrapper<TEventArgs>(handler));
            }
        }

        /**
         * @brief             根据事件参数类型注册成员函数作为路由事件，当事件已注册时会覆盖已注册的函数
         * @tparam TEventArgs 路由事件的参数类型，必须继承自TypedRoutedEventArgs<...>
         * @tparam THandleObj 成员函数所在的类
         * @param obj         注册的成员函数所在的对象
         * @param handler     事件的处理函数，当值为nullptr时可取消注册
         * @deprecated        建议使用AddHandler函数代替以避免覆盖已注册的事件处理函数
         */
        template <typename TEventArgs, typename THandleObj>
        typename std::enable_if<std::is_base_of<RoutedEventArgs, TEventArgs>::value && sw::_IsTypedRoutedEventArgs<TEventArgs>::value>::type
        RegisterRoutedEvent(THandleObj &obj, void (THandleObj::*handler)(UIElement &, TEventArgs &))
        {
            if (handler == nullptr) {
                this->UnregisterRoutedEvent(TEventArgs::EventType);
            } else {
                this->RegisterRoutedEvent(TEventArgs::EventType, RoutedEventHandlerWrapper<TEventArgs>(Action<UIElement &, TEventArgs &>(obj, handler)));
            }
        }

        /**
         * @brief           添加路由事件处理函数
         * @param eventType 路由事件类型
         * @param handler   处理函数
         */
        void AddHandler(RoutedEventType eventType, const RoutedEventHandler &handler);

        /**
         * @brief           添加成员函数作为路由事件处理函数
         * @tparam T        成员函数所在的类
         * @param eventType 路由事件类型
         * @param obj       注册的成员函数所在的对象
         * @param handler   处理函数
         */
        template <typename T>
        void AddHandler(RoutedEventType eventType, T &obj, void (T::*handler)(UIElement &, RoutedEventArgs &))
        {
            if (handler) this->_eventMap[eventType].Add(obj, handler);
        }

        /**
         * @brief             根据事件参数类型添加路由事件处理函数
         * @tparam TEventArgs 路由事件的参数类型，必须继承自TypedRoutedEventArgs<...>
         * @param handler     事件的处理函数
         */
        template <typename TEventArgs>
        typename std::enable_if<std::is_base_of<RoutedEventArgs, TEventArgs>::value && sw::_IsTypedRoutedEventArgs<TEventArgs>::value>::type
        AddHandler(const Action<UIElement &, TEventArgs &> &handler)
        {
            if (handler) this->AddHandler(TEventArgs::EventType, RoutedEventHandlerWrapper<TEventArgs>(handler));
        }

        /**
         * @brief             根据事件参数类型添加成员函数作为路由事件处理函数
         * @tparam TEventArgs 路由事件的参数类型，必须继承自TypedRoutedEventArgs<...>
         * @tparam THandleObj 成员函数所在的类
         * @param obj         注册的成员函数所在的对象
         * @param handler     事件的处理函数
         */
        template <typename TEventArgs, typename THandleObj>
        typename std::enable_if<std::is_base_of<RoutedEventArgs, TEventArgs>::value && sw::_IsTypedRoutedEventArgs<TEventArgs>::value>::type
        AddHandler(THandleObj &obj, void (THandleObj::*handler)(UIElement &, TEventArgs &))
        {
            if (handler) this->AddHandler(TEventArgs::EventType, RoutedEventHandlerWrapper<TEventArgs>(Action<UIElement &, TEventArgs &>(obj, handler)));
        }

        /**
         * @brief             添加路由事件处理函数
         * @tparam TEventArgs 路由事件的参数类型，必须继承自RoutedEventArgs
         * @param eventType   路由事件类型
         * @param handler     处理函数
         */
        template <typename TEventArgs>
        typename std::enable_if<
            std::is_base_of<RoutedEventArgs, TEventArgs>::value &&
            !std::is_same<TEventArgs, RoutedEventArgs>::value &&
            !sw::_IsTypedRoutedEventArgs<TEventArgs>::value>::type
        AddHandler(RoutedEventType eventType, const Action<UIElement &, TEventArgs &> &handler)
        {
            if (handler) this->AddHandler(eventType, RoutedEventHandlerWrapper<TEventArgs>(handler));
        }

        /**
         * @brief             添加成员函数作为路由事件处理函数
         * @tparam TEventArgs 路由事件的参数类型，必须继承自RoutedEventArgs
         * @tparam THandleObj 成员函数所在的类
         * @param eventType   路由事件类型
         * @param obj         注册的成员函数所在的对象
         * @param handler     处理函数
         */
        template <typename TEventArgs, typename THandleObj>
        typename std::enable_if<
            std::is_base_of<RoutedEventArgs, TEventArgs>::value &&
            !std::is_same<TEventArgs, RoutedEventArgs>::value &&
            !sw::_IsTypedRoutedEventArgs<TEventArgs>::value>::type
        AddHandler(RoutedEventType eventType, THandleObj &obj, void (THandleObj::*handler)(UIElement &, TEventArgs &))
        {
            if (handler) this->AddHandler(eventType, RoutedEventHandlerWrapper<TEventArgs>(Action<UIElement &, TEventArgs &>(obj, handler)));
        }

        /**
         * @brief           移除已添加的路由事件处理函数
         * @param eventType 路由事件类型
         * @param handler   处理函数
         * @return          是否成功移除
         */
        bool RemoveHandler(RoutedEventType eventType, const RoutedEventHandler &handler);

        /**
         * @brief           移除已添加的类型为成员函数的路由事件处理函数
         * @tparam T        成员函数所在的类
         * @param eventType 路由事件类型
         * @param obj       注册的成员函数所在的对象
         * @param handler   处理函数
         * @return          是否成功移除
         */
        template <typename T>
        bool RemoveHandler(RoutedEventType eventType, T &obj, void (T::*handler)(UIElement &, RoutedEventArgs &))
        {
            return handler == nullptr ? false : this->_eventMap[eventType].Remove(obj, handler);
        }

        /**
         * @brief             移除已添加的路由事件处理函数
         * @tparam TEventArgs 路由事件的参数类型，必须继承自TypedRoutedEventArgs<...>
         * @param handler     事件的处理函数
         * @return            是否成功移除
         */
        template <typename TEventArgs>
        typename std::enable_if<std::is_base_of<RoutedEventArgs, TEventArgs>::value && sw::_IsTypedRoutedEventArgs<TEventArgs>::value, bool>::type
        RemoveHandler(const Action<UIElement &, TEventArgs &> &handler)
        {
            if (handler == nullptr) {
                return false;
            } else {
                return this->RemoveHandler(TEventArgs::EventType, RoutedEventHandlerWrapper<TEventArgs>(handler));
            }
        }

        /**
         * @brief             移除已添加的类型为成员函数的路由事件处理函数
         * @tparam TEventArgs 路由事件的参数类型，必须继承自TypedRoutedEventArgs<...>
         * @tparam THandleObj 成员函数所在的类
         * @param obj         注册的成员函数所在的对象
         * @param handler     事件的处理函数
         * @return            是否成功移除
         */
        template <typename TEventArgs, typename THandleObj>
        typename std::enable_if<std::is_base_of<RoutedEventArgs, TEventArgs>::value && sw::_IsTypedRoutedEventArgs<TEventArgs>::value, bool>::type
        RemoveHandler(THandleObj &obj, void (THandleObj::*handler)(UIElement &, TEventArgs &))
        {
            if (handler == nullptr) {
                return false;
            } else {
                return this->RemoveHandler(TEventArgs::EventType, RoutedEventHandlerWrapper<TEventArgs>(Action<UIElement &, TEventArgs &>(obj, handler)));
            }
        }

        /**
         * @brief             移除已添加的路由事件处理函数
         * @tparam TEventArgs 路由事件的参数类型，必须继承自RoutedEventArgs
         * @param eventType   路由事件类型
         * @param handler     处理函数
         * @return            是否成功移除
         */
        template <typename TEventArgs>
        typename std::enable_if<
            std::is_base_of<RoutedEventArgs, TEventArgs>::value &&
                !std::is_same<TEventArgs, RoutedEventArgs>::value &&
                !sw::_IsTypedRoutedEventArgs<TEventArgs>::value,
            bool>::type
        RemoveHandler(RoutedEventType eventType, const Action<UIElement &, TEventArgs &> &handler)
        {
            if (handler == nullptr) {
                return false;
            } else {
                return this->RemoveHandler(eventType, RoutedEventHandlerWrapper<TEventArgs>(handler));
            }
        }

        /**
         * @brief             移除已添加的类型为成员函数的路由事件处理函数
         * @tparam TEventArgs 路由事件的参数类型，必须继承自RoutedEventArgs
         * @tparam THandleObj 成员函数所在的类
         * @param eventType   路由事件类型
         * @param obj         注册的成员函数所在的对象
         * @param handler     处理函数
         * @return            是否成功移除
         */
        template <typename TEventArgs, typename THandleObj>
        typename std::enable_if<
            std::is_base_of<RoutedEventArgs, TEventArgs>::value &&
                !std::is_same<TEventArgs, RoutedEventArgs>::value &&
                !sw::_IsTypedRoutedEventArgs<TEventArgs>::value,
            bool>::type
        RemoveHandler(RoutedEventType eventType, THandleObj &obj, void (THandleObj::*handler)(UIElement &, TEventArgs &))
        {
            if (handler == nullptr) {
                return false;
            } else {
                return this->RemoveHandler(eventType, RoutedEventHandlerWrapper<TEventArgs>(Action<UIElement &, TEventArgs &>(obj, handler)));
            }
        }

        /**
         * @brief           取消对应类型路由事件的注册，该函数会移除对应事件所有的处理函数
         * @param eventType 路由事件类型
         */
        void UnregisterRoutedEvent(RoutedEventType eventType);

        /**
         * @brief           判断路由事件是否已存在事件处理函数
         * @param eventType 路由事件类型
         */
        bool IsRoutedEventRegistered(RoutedEventType eventType);

        /**
         * @brief 通过索引获取子元素
         */
        UIElement &operator[](int index) const;

        /**
         * @brief 获取子元素
         */
        UIElement &GetChildAt(int index) const;

        /**
         * @brief  添加子元素
         * @return 添加是否成功
         */
        bool AddChild(UIElement *element);

        /**
         * @brief  添加子元素
         * @return 添加是否成功
         */
        bool AddChild(UIElement &element);

        /**
         * @brief  添加子元素并设置布局标记
         * @return 添加是否成功
         */
        bool AddChild(UIElement *element, uint64_t layoutTag);

        /**
         * @brief  添加子元素并设置布局标记
         * @return 添加是否成功
         */
        bool AddChild(UIElement &element, uint64_t layoutTag);

        /**
         * @brief       移除指定索引处的子元素
         * @param index 要移除的索引
         * @return      移除是否成功
         */
        bool RemoveChildAt(int index);

        /**
         * @brief  移除子元素
         * @return 移除是否成功
         */
        bool RemoveChild(UIElement *element);

        /**
         * @brief  移除子元素
         * @return 移除是否成功
         */
        bool RemoveChild(UIElement &element);

        /**
         * @brief 移除所有子元素
         */
        void ClearChildren();

        /**
         * @brief         获取指定元素的索引
         * @param element 要查找的元素
         * @return        若找到指定元素则返回对应的索引，否则返回-1
         */
        int IndexOf(UIElement *element);

        /**
         * @brief         获取指定元素的索引
         * @param element 要查找的元素
         * @return        若找到指定元素则返回对应的索引，否则返回-1
         */
        int IndexOf(UIElement &element);

        /**
         * @brief       弹出当前元素的上下文菜单
         * @param point 弹出菜单左上角在屏幕中的位置
         */
        void ShowContextMenu(const Point &point);

        /**
         * @brief 移动到界面顶部
         */
        void MoveToTop();

        /**
         * @brief 移动到界面底部
         */
        void MoveToBottom();

        /**
         * @brief 判断当前元素是否为根节点
         */
        bool IsRootElement();

        /**
         * @brief 获取当前元素所在界面树的根节点
         */
        UIElement *GetRootElement();

        /**
         * @brief 获取当前元素在界面树上的下一个节点，若已是最后一个节点则返回根节点
         */
        UIElement *GetNextElement();

        /**
         * @brief 获取当前元素在界面树上的上一个节点，若元素为第一个节点则返回最后一个节点
         */
        UIElement *GetPreviousElement();

        /**
         * @brief 获取下一个TabStop属性为true的元素
         */
        UIElement *GetNextTabStopElement();

        /**
         * @brief 获取上一个TabStop属性为true的元素
         */
        UIElement *GetPreviousTabStopElement();

        /**
         * @brief 获取当前要显示的背景颜色：当Transparent为true时获取到祖先节点中首个Transparent为false的背景颜色，否则返回当前元素的背景颜色
         */
        Color GetRealBackColor();

        /**
         * @brief 获取当前要显示的文本颜色：当InheritTextColor为true时获取到祖先节点中首个InheritTextColor为false的文本颜色，否则返回当前元素的文本颜色
         */
        Color GetRealTextColor();

        /**
         * @brief         设置鼠标样式
         * @param hCursor 鼠标句柄
         */
        void SetCursor(HCURSOR hCursor);

        /**
         * @brief        设置鼠标样式
         * @param cursor 鼠标样式
         */
        void SetCursor(StandardCursor cursor);

        /**
         * @brief 将鼠标样式设置为默认样式
         */
        void ResetCursor();

        /**
         * @brief      设置对齐方式
         * @param horz 水平对齐方式
         * @param vert 垂直对齐方式
         */
        void SetAlignment(sw::HorizontalAlignment horz, sw::VerticalAlignment vert);

        /**
         * @brief 调整当前元素的尺寸，也可以用该函数更改默认Measure函数在当前横向或纵向对齐方式为拉伸时的DesireSize
         */
        void Resize(const Size &size);

        /**
         * @brief 判断指定的布局更新条件是否已设置
         */
        bool IsLayoutUpdateConditionSet(sw::LayoutUpdateCondition condition);

        /**
         * @brief 获取Tag
         */
        virtual uint64_t GetTag() override;

        /**
         * @brief 设置Tag
         */
        virtual void SetTag(uint64_t tag) override;

        /**
         * @brief 获取布局标记
         */
        virtual uint64_t GetLayoutTag() override;

        /**
         * @brief 获取参与布局的子控件数量
         */
        virtual int GetChildLayoutCount() override;

        /**
         * @brief 获取对应索引处的子控件，使用此函数前必须先调用GetChildLayoutCount
         */
        virtual ILayout &GetChildLayoutAt(int index) override;

        /**
         * @brief 获取控件所需尺寸
         */
        virtual Size GetDesireSize() override;

        /**
         * @brief 设置当前控件所需的尺寸
         */
        virtual void SetDesireSize(const Size &size) override;

        /**
         * @brief               测量控件所需尺寸
         * @param availableSize 可用的尺寸
         */
        virtual void Measure(const Size &availableSize) override;

        /**
         * @brief           安排控件位置
         * @param finalSize 最终控件所安排的位置
         */
        virtual void Arrange(const sw::Rect &finalPosition) override;

        /**
         * @brief  尝试将对象转换成UIElement
         * @return 若函数成功则返回UIElement指针，否则返回nullptr
         */
        virtual UIElement *ToUIElement() override;

    protected:
        /**
         * @brief           触发路由事件
         * @param eventType 事件类型
         */
        void RaiseRoutedEvent(RoutedEventType eventType);

        /**
         * @brief           触发路由事件
         * @param eventArgs 要触发事件的事件参数
         */
        void RaiseRoutedEvent(RoutedEventArgs &eventArgs);

        /**
         * @brief 通知顶级窗口布局改变
         */
        void NotifyLayoutUpdated();

        /**
         * @brief 获取Arrange时子元素的水平偏移量
         */
        double &GetArrangeOffsetX();

        /**
         * @brief 获取Arrange时子元素的垂直偏移量
         */
        double &GetArrangeOffsetY();

        /**
         * @brief        获取所有子元素在当前元素中最右边的位置（只考虑参与布局的子窗口且忽略悬浮的元素）
         * @param update 是否更字段
         * @return       _childRightmost字段
         */
        double GetChildRightmost(bool update);

        /**
         * @brief        获取所有子元素在当前元素中最底边的位置（只考虑参与布局的子窗口且忽略悬浮的元素）
         * @param update 是否更字段
         * @return       _childBottommost字段
         */
        double GetChildBottommost(bool update);

        /**
         * @brief 更新子元素的Z轴位置
         */
        void UpdateChildrenZOrder();

        /**
         * @brief 更新兄弟元素的Z轴位置
         */
        void UpdateSiblingsZOrder();

        /**
         * @brief 设置下一个TabStop属性为true的元素为焦点元素
         */
        void SetNextTabStopFocus();

        /**
         * @brief 设置上一个TabStop属性为true的元素为焦点元素
         */
        void SetPreviousTabStopFocus();

        /**
         * @brief        设置背景颜色
         * @param color  要设置的颜色
         * @param redraw 是否重绘
         */
        virtual void SetBackColor(Color color, bool redraw);

        /**
         * @brief        设置文本颜色
         * @param color  要设置的颜色
         * @param redraw 是否重绘
         */
        virtual void SetTextColor(Color color, bool redraw);

        /**
         * @brief         添加子元素后调用该函数
         * @param element 添加的子元素
         */
        virtual void OnAddedChild(UIElement &element);

        /**
         * @brief         移除子元素后调用该函数
         * @param element 移除的子元素
         */
        virtual void OnRemovedChild(UIElement &element);

        /**
         * @brief 通过tab键将焦点移动到当前元素时调用该函数
         */
        virtual void OnTabStop();

        /**
         * @brief  设置父窗口
         * @return 设置是否成功
         */
        virtual bool SetParent(WndBase *parent) override;

        /**
         * @brief           父窗口改变时调用此函数
         * @param newParent 新的父窗口
         */
        virtual void ParentChanged(WndBase *newParent) override;

        /**
         * @brief  接收到WM_CLOSE时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnClose() override;

        /**
         * @brief                   接收到WM_MOVE时调用该函数
         * @param newClientPosition 移动后用户区左上角的位置
         * @return                  若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMove(Point newClientPosition) override;

        /**
         * @brief               接收到WM_SIZE时调用该函数
         * @param newClientSize 改变后的用户区尺寸
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSize(Size newClientSize) override;

        /**
         * @brief Text属性更改时调用此函数
         */
        virtual void OnTextChanged() override;

        /**
         * @brief       字体改变时调用该函数
         * @param hfont 字体句柄
         */
        virtual void FontChanged(HFONT hfont) override;

        /**
         * @brief Visible属性改变时调用此函数
         */
        virtual void VisibleChanged(bool newVisible) override;

        /**
         * @brief            接收到WM_SETFOCUS时调用该函数
         * @param hPrevFocus 丢失焦点的hwnd，可能为NULL
         * @return           若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSetFocus(HWND hPrevFocus) override;

        /**
         * @brief            接收到WM_KILLFOCUS时调用该函数
         * @param hNextFocus 接收到焦点的hwnd，可能为NULL
         * @return           若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnKillFocus(HWND hNextFocus) override;

        /**
         * @brief       接收到WM_CHAR时调用该函数
         * @param ch    按键的字符代码
         * @param flags 附加信息
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnChar(wchar_t ch, KeyFlags flags) override;

        /**
         * @brief       接收到WM_KEYDOWN时调用该函数
         * @param key   虚拟按键
         * @param flags 附加信息
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnKeyDown(VirtualKey key, KeyFlags flags) override;

        /**
         * @brief       接收到WM_KEYUP时调用该函数
         * @param key   虚拟按键
         * @param flags 附加信息
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnKeyUp(VirtualKey key, KeyFlags flags) override;

        /**
         * @brief               接收到WM_MOUSEMOVE时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseMove(Point mousePosition, MouseKey keyState) override;

        /**
         * @brief  接收到WM_MOUSELEAVE时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseLeave() override;

        /**
         * @brief               接收到WM_MOUSEWHEEL时调用该函数
         * @param wheelDelta    滚轮滚动的距离，为120的倍数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseWheel(int wheelDelta, Point mousePosition, MouseKey keyState) override;

        /**
         * @brief               接收到WM_LBUTTONDOWN时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseLeftButtonDown(Point mousePosition, MouseKey keyState) override;

        /**
         * @brief               接收到WM_LBUTTONUP时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseLeftButtonUp(Point mousePosition, MouseKey keyState) override;

        /**
         * @brief               接收到WM_RBUTTONDOWN时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseRightButtonDown(Point mousePosition, MouseKey keyState) override;

        /**
         * @brief               接收到WM_RBUTTONUP时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseRightButtonUp(Point mousePosition, MouseKey keyState) override;

        /**
         * @brief               接收到WM_MBUTTONDOWN时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseMiddleButtonDown(Point mousePosition, MouseKey keyState) override;

        /**
         * @brief               接收到WM_MBUTTONUP时调用该函数
         * @param mousePosition 鼠标在用户区中的位置
         * @param keyState      指示某些按键是否按下
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnMouseMiddleButtonUp(Point mousePosition, MouseKey keyState) override;

        /**
         * @brief               接收到WM_CONTEXTMENU后调用目标控件的该函数
         * @param isKeyboardMsg 消息是否由按下快捷键（Shift+F10、VK_APPS）产生
         * @param mousePosition 鼠标在屏幕中的位置
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnContextMenu(bool isKeyboardMsg, Point mousePosition) override;

        /**
         * @brief    当WM_COMMAND接收到菜单命令时调用该函数
         * @param id 菜单id
         */
        virtual void OnMenuCommand(int id) override;

        /**
         * @brief           父窗口接收到WM_CTLCOLORxxx时调用对应控件的该函数
         * @param hdc       控件的显示上下文句柄
         * @param hRetBrush 要返回的画笔
         * @return          若返回true则将hRetBrush作为消息的返回值，否则使用DefaultWndProc的返回值
         */
        virtual bool OnColor(HDC hdc, HBRUSH &hRetBrush) override;

        /**
         * @brief         接收到WM_SETCURSOR消息时调用该函数
         * @param hwnd    鼠标所在窗口的句柄
         * @param hitTest hit-test的结果，详见WM_NCHITTEST消息的返回值
         * @param message 触发该事件的鼠标消息，如WM_MOUSEMOVE
         * @param result  消息的返回值，默认为false
         * @return        若返回true则将result作为消息的返回值，否则使用DefaultWndProc的返回值
         */
        virtual bool OnSetCursor(HWND hwnd, HitTestResult hitTest, int message, bool &result) override;

        /**
         * @brief       接收到WM_DROPFILES时调用该函数
         * @param hDrop 描述拖入文件的句柄
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnDropFiles(HDROP hDrop) override;

    private:
        /**
         * @brief       设置水平对齐方式
         * @param value 要设置的值
         * @return      值是否发生改变
         */
        bool _SetHorzAlignment(sw::HorizontalAlignment value);

        /**
         * @brief       设置垂直对齐方式
         * @param value 要设置的值
         * @return      值是否发生改变
         */
        bool _SetVertAlignment(sw::VerticalAlignment value);

        /**
         * @brief 循环获取界面树上的下一个节点
         */
        static UIElement *_GetNextElement(UIElement *element, bool searchChildren = true);

        /**
         * @brief 获取界面树上最深的最后一个节点
         */
        static UIElement *_GetDeepestLastElement(UIElement *element);

        /**
         * @brief 循环获取界面树上的上一个节点
         */
        static UIElement *_GetPreviousElement(UIElement *element);
    };
}

// FillLayout.h


namespace sw
{
    /**
     * @brief 一种将全部元素都铺满的布局，一般用于在只有一个子元素的时候将该元素铺满整个可用区域
     */
    class FillLayout : public LayoutHost
    {
    public:
        /**
         * @brief 计算所需尺寸
         */
        virtual void MeasureOverride(Size &availableSize) override;

        /**
         * @brief 安排控件
         */
        virtual void ArrangeOverride(Size &finalSize) override;
    };
}

// CanvasLayout.h


namespace sw
{
    /**
     * @brief 绝对位置布局方式的布局标记
     */
    struct CanvasLayoutTag {
        /**
         * @brief 左边
         */
        float left;

        /**
         * @brief 顶边
         */
        float top;

        /**
         * @brief 左边顶边均为0
         */
        CanvasLayoutTag();

        /**
         * @brief 指定左边和顶边
         */
        CanvasLayoutTag(float left, float top);

        /**
         * @brief 从LayoutTag创建
         */
        CanvasLayoutTag(uint64_t layoutTag);

        /**
         * @brief 隐式转换LayoutTag
         */
        operator uint64_t() const;
    };

    /**
     * @brief 绝对位置布局方式
     */
    class CanvasLayout : public LayoutHost
    {
    public:
        /**
         * @brief 计算所需尺寸
         */
        virtual void MeasureOverride(Size &availableSize) override;

        /**
         * @brief 安排控件
         */
        virtual void ArrangeOverride(Size &finalSize) override;
    };
}

// StackLayoutH.h


namespace sw
{
    /**
     * @brief 横向堆叠布局
     */
    class StackLayoutH : virtual public LayoutHost
    {
    public:
        /**
         * @brief 计算所需尺寸
         */
        virtual void MeasureOverride(Size &availableSize) override;

        /**
         * @brief 安排控件
         */
        virtual void ArrangeOverride(Size &finalSize) override;
    };
}

// WrapLayoutV.h


namespace sw
{
    /**
     * @brief 纵向自动换行布局
     */
    class WrapLayoutV : virtual public LayoutHost
    {
    public:
        /**
         * @brief 计算所需尺寸
         */
        virtual void MeasureOverride(Size &availableSize) override;

        /**
         * @brief 安排控件
         */
        virtual void ArrangeOverride(Size &finalSize) override;
    };
}

// Timer.h


namespace sw
{
    class Timer; // 向前声明

    /**
     * @brief 计时器触发事件类型
     */
    using TimerTickHandler = Action<Timer &>;

    /**
     * @brief 计时器
     */
    class Timer : public WndBase
    {
    private:
        /**
         * @brief 是否已启动
         */
        bool _started = false;

        /**
         * @brief 触发间隔
         */
        uint32_t _interval = 1000;

    public:
        /**
         * @brief 相对于上一次触发的Tick事件引发下一次Tick事件之间的时间（以毫秒为单位）
         */
        Property<uint32_t> Interval;

        /**
         * @brief 计时器触发事件
         */
        TimerTickHandler Tick;

    public:
        /**
         * @brief 初始化计时器
         */
        Timer();

        /**
         * @brief 开始计时器
         */
        void Start();

        /**
         * @brief 停止计时器
         */
        void Stop();

    protected:
        /**
         * @brief 计时器已启动并且达到间隔时间时调用该函数
         */
        virtual void OnTick();

    private:
        /**
         * @brief      通过窗口句柄获取Timer指针
         * @param hwnd 窗口句柄
         * @return     若函数成功则返回对象的指针，否则返回nullptr
         */
        static Timer *_GetTimerPtr(HWND hwnd);

        /**
         * @brief       关联窗口句柄与Timer对象
         * @param hwnd  窗口句柄
         * @param timer 与句柄关联的对象
         */
        static void _SetTimerPtr(HWND hwnd, Timer &timer);

        /**
         * @brief TimerProc回调函数
         */
        static void CALLBACK _TimerProc(HWND hwnd, UINT msg, UINT_PTR idTimer, DWORD time);
    };
}

// UniformGridLayout.h


namespace sw
{
    /**
     * @brief 均匀大小网格布局
     */
    class UniformGridLayout : public LayoutHost
    {
    public:
        /**
         * @brief 行数
         */
        int rows = 1;

        /**
         * @brief 列数
         */
        int columns = 1;

        /**
         * @brief 网格第一行中前导空白单元格的数量
         */
        int firstColumn = 0;

        /**
         * @brief 计算所需尺寸
         */
        virtual void MeasureOverride(Size &availableSize) override;

        /**
         * @brief 安排控件
         */
        virtual void ArrangeOverride(Size &finalSize) override;
    };
}

// StackLayoutV.h


namespace sw
{
    /**
     * @brief 纵向堆叠布局
     */
    class StackLayoutV : virtual public LayoutHost
    {
    public:
        /**
         * @brief 计算所需尺寸
         */
        virtual void MeasureOverride(Size &availableSize) override;

        /**
         * @brief 安排控件
         */
        virtual void ArrangeOverride(Size &finalSize) override;
    };
}

// GridLayout.h


namespace sw
{
    /**
     * @brief 网格布局方式的布局标记
     */
    struct GridLayoutTag {
        /**
         * @brief 所在行
         */
        uint16_t row;

        /**
         * @brief 所在列
         */
        uint16_t column;

        /**
         * @brief 所跨行数
         */
        uint16_t rowSpan;

        /**
         * @brief 所跨列数
         */
        uint16_t columnSpan;

        /**
         * @brief GridLayoutTag默认值
         */
        GridLayoutTag();

        /**
         * @brief 初始化GridLayoutTag
         */
        GridLayoutTag(uint16_t row, uint16_t column, uint16_t rowSpan, uint16_t columnSpan);

        /**
         * @brief 初始化GridLayoutTag
         */
        GridLayoutTag(uint16_t row, uint16_t column);

        /**
         * @brief 从LayoutTag创建
         */
        GridLayoutTag(uint64_t layoutTag);

        /**
         * @brief 隐式转换LayoutTag
         */
        operator uint64_t() const;
    };

    /**
     * @brief GridRow和GridColumn的类型
     */
    enum class GridRCType {
        FixSize,    // 固定大小
        AutoSize,   // 自动大小
        FillRemain, // 填充剩余空间
    };

    /**
     * @brief 网格中的行信息
     */
    struct GridRow {
        /**
         * @brief 类型
         */
        GridRCType type;

        /**
         * @brief 高度
         */
        double height;

        /**
         * @brief 创建一个FillRemain的GridRow
         */
        GridRow();

        /**
         * @brief 初始化GridRow
         */
        GridRow(GridRCType type, double height);

        /**
         * @brief 固定大小的行
         */
        GridRow(double height);
    };

    /**
     * @brief 固定高度的行
     */
    struct FixSizeGridRow : public GridRow {
        /**
         * @brief 初始化FixSizeGridRow
         */
        FixSizeGridRow(double height);
    };

    /**
     * @brief 自动高度的行
     */
    struct AutoSizeGridRow : public GridRow {
        /**
         * @brief 初始化AutoSizeGridRow
         */
        AutoSizeGridRow();
    };

    /**
     * @brief 填充剩余高度的行
     */
    struct FillRemainGridRow : public GridRow {
        /**
         * @brief 初始化FillRemainGridRow
         */
        FillRemainGridRow(double proportion = 1);
    };

    /**
     * @brief 网格中的列信息
     */
    struct GridColumn {
        /**
         * @brief 类型
         */
        GridRCType type;

        /**
         * @brief 宽度
         */
        double width;

        /**
         * @brief 创建一个FillRemain的GridColumn
         */
        GridColumn();

        /**
         * @brief 初始化GridColumn
         */
        GridColumn(GridRCType type, double width);

        /**
         * @brief 固定大小的列
         */
        GridColumn(double width);
    };

    /**
     * @brief 固定宽度的列
     */
    struct FixSizeGridColumn : public GridColumn {
        /**
         * @brief 初始化FixSizeGridColumn
         */
        FixSizeGridColumn(double width);
    };

    /**
     * @brief 自动宽度的列
     */
    struct AutoSizeGridColumn : public GridColumn {
        /**
         * @brief 初始化AutoSizeGridColumn
         */
        AutoSizeGridColumn();
    };

    /**
     * @brief 填充剩余宽度的列
     */
    struct FillRemainGridColumn : public GridColumn {
        /**
         * @brief 初始化FillRemainGridColumn
         */
        FillRemainGridColumn(double proportion = 1);
    };

    /**
     * @brief 网格布局方式
     */
    class GridLayout : public LayoutHost
    {
    private:
        /**
         * @brief 子元素的信息
         */
        struct _ChildInfo {
            ILayout *instance;         // 子元素对象
            GridLayoutTag layoutTag;   // 布局标记
            GridRCType rowMeasureType; // 元素measure行时的类型
            GridRCType colMeasureType; // 元素measure列时的类型
        };

        /**
         * @brief 行信息
         */
        struct _RowInfo {
            GridRow row;           // 行
            double size       = 0; // 所需空间大小
            double proportion = 0; // 类型为FillRemain时该字段保存该行的高度占比，范围为0~1
        };

        /**
         * @brief 列信息
         */
        struct _ColInfo {
            GridColumn col;        // 列
            double size       = 0; // 所需空间大小
            double proportion = 0; // 类型为FillRemain时该字段保存该列的宽度占比，范围为0~1
        };

        /**
         * @brief 一些内部数据
         */
        struct {
            std::vector<_RowInfo> rowsInfo;       // 行信息
            std::vector<_ColInfo> colsInfo;       // 列信息
            std::vector<_ChildInfo> childrenInfo; // 子元素信息
            std::vector<Rect> cells;              // 保存格信息
        } _internalData;

    public:
        /**
         * @brief 行定义
         */
        List<GridRow> rows;

        /**
         * @brief 列定义
         */
        List<GridColumn> columns;

        /**
         * @brief 计算所需尺寸
         */
        virtual void MeasureOverride(Size &availableSize) override;

        /**
         * @brief 安排控件
         */
        virtual void ArrangeOverride(Size &finalSize) override;

    private:
        /**
         * @brief 更新内部数据
         */
        void _UpdateInternalData();

        /**
         * @brief 获取指定行列处的网格信息
         */
        Rect &_GetCell(int row, int col);
    };
}

// Layer.h


namespace sw
{
    /**
     * @brief 表示可以设置布局方式的元素类型，如窗口、面板等
     */
    class Layer : virtual public UIElement
    {
    private:
        /**
         * @brief 是否关闭布局，当该字段为true时调用UpdateLayout不会更新布局，可以用DisableLayout和EnableLayout设置该字段
         */
        bool _layoutDisabled = false;

        /**
         * @brief 是否按照布局方式与子元素自动调整尺寸
         */
        bool _autoSize = true;

        /**
         * @brief 指向所自定义的布局方式对象的指针
         */
        LayoutHost *_customLayout = nullptr;

        /**
         * @brief 记录水平滚动条是否已被禁止
         */
        bool _horizontalScrollDisabled = true;

        /**
         * @brief 记录垂直滚动条是否已被禁止
         */
        bool _verticalScrollDisabled = true;

    public:
        /**
         * @brief 自定义的布局方式，赋值后将自动与所指向的布局关联，每个布局只能关联一个对象，设为nullptr可恢复默认布局
         */
        const Property<LayoutHost *> Layout;

        /**
         * @brief 是否按照布局方式与子元素自动调整尺寸，该属性仅在当前元素已设置布局方式并且非顶级元素时有效
         */
        const Property<bool> AutoSize;

        /**
         * @brief 是否显示横向滚动条
         */
        const Property<bool> HorizontalScrollBar;

        /**
         * @brief 是否显示纵向滚动条
         */
        const Property<bool> VerticalScrollBar;

        /**
         * @brief 横向滚动条位置
         */
        const Property<double> HorizontalScrollPos;

        /**
         * @brief 纵向滚动条位置
         */
        const Property<double> VerticalScrollPos;

        /**
         * @brief 横向滚动条可设置的最大位置
         */
        const ReadOnlyProperty<double> HorizontalScrollLimit;

        /**
         * @brief 纵向滚动条可设置的最大位置
         */
        const ReadOnlyProperty<double> VerticalScrollLimit;

    protected:
        /**
         * @brief 初始化Layer
         */
        Layer();

    public:
        /**
         * @brief 析构函数，这里用纯虚函数使该类成为抽象类
         */
        virtual ~Layer() = 0;

    private:
        /**
         * @brief 获取布局对象，若Layout属性被赋值则返回设置的对象，否则返回默认布局对象
         */
        LayoutHost *_GetLayout();

        /**
         * @brief 在没有设定布局方式时，使用该函数对子元素Measure和Arrange
         */
        void _MeasureAndArrangeWithoutLayout();

        /**
         * @brief 使用设定的布局方式对子元素进行Measure和Arrange，不改变当前的尺寸和DesireSize
         */
        void _MeasureAndArrangeWithoutResize();

    protected:
        /**
         * @brief 更新布局
         */
        void UpdateLayout();

        /**
         * @brief 获取默认布局对象
         */
        virtual LayoutHost *GetDefaultLayout();

        /**
         * @brief           触发滚动条相关事件时调用该函数
         * @param scrollbar 滚动条类型
         * @param event     滚动条事件
         * @param pos       当event为ThumbPosition或ThubmTrack时表示当前滚动条位置，其他情况固定为0
         */
        virtual void OnScroll(ScrollOrientation scrollbar, ScrollEvent event, double pos);

        /**
         * @brief       接收到WM_VSCROLL时调用目标控件的该函数
         * @param event 事件类型，即消息wParam的低字
         * @param pos   当前滚动条的位置，仅当event为SB_THUMBPOSITION或SB_THUMBTRACK时有效
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnVerticalScroll(int event, int pos) override;

        /**
         * @brief       接收到WM_HSCROLL时调用目标控件的该函数
         * @param event 事件类型，即消息wParam的低字
         * @param pos   当前滚动条的位置，仅当event为SB_THUMBPOSITION或SB_THUMBTRACK时有效
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnHorizontalScroll(int event, int pos) override;

    public:
        /**
         * @brief               测量控件所需尺寸
         * @param availableSize 可用的尺寸
         */
        virtual void Measure(const Size &availableSize) override;

        /**
         * @brief           安排控件位置
         * @param finalSize 最终控件所安排的位置
         */
        virtual void Arrange(const sw::Rect &finalPosition) override;

        /**
         * @brief 禁用布局，禁用布局后调用UpdateLayout不会更新布局
         */
        void DisableLayout();

        /**
         * @brief 启用布局，并立即更新布局
         */
        void EnableLayout();

        /**
         * @brief 获取一个值，表示当前控件是否已禁用布局
         */
        bool IsLayoutDisabled();

        /**
         * @brief        获取横向滚动条的范围
         * @param refMin 滚动范围最小值
         * @param refMax 滚动范围最大值
         */
        void GetHorizontalScrollRange(double &refMin, double &refMax);

        /**
         * @brief        获取纵向滚动条的范围
         * @param refMin 滚动范围最小值
         * @param refMax 滚动范围最大值
         */
        void GetVerticalScrollRange(double &refMin, double &refMax);

        /**
         * @brief     设置横向滚动条的范围
         * @param min 滚动范围最小值
         * @param max 滚动范围最大值
         */
        void SetHorizontalScrollRange(double min, double max);

        /**
         * @brief     设置纵向滚动条的范围
         * @param min 滚动范围最小值
         * @param max 滚动范围最大值
         */
        void SetVerticalScrollRange(double min, double max);

        /**
         * @brief 获取水平滚动条滚动页面大小
         */
        double GetHorizontalScrollPageSize();

        /**
         * @brief 获取垂直滚动条滚动页面大小
         */
        double GetVerticalScrollPageSize();

        /**
         * @brief 设置水平滚动条滚动页面大小
         */
        void SetHorizontalScrollPageSize(double pageSize);

        /**
         * @brief 设置垂直滚动条滚动页面大小
         */
        void SetVerticalScrollPageSize(double pageSize);

        /**
         * @brief 根据子元素更新滚动条范围，未设定布局方式时该函数无效
         */
        void UpdateScrollRange();

        /**
         * @brief 将垂直滚动条移动到顶部
         */
        void ScrollToTop();

        /**
         * @brief 将垂直滚动条移动到底部
         */
        void ScrollToBottom();

        /**
         * @brief 将水平滚动条移动到最左
         */
        void ScrollToLeft();

        /**
         * @brief 将水平滚动条移动到最右
         */
        void ScrollToRight();

        /**
         * @brief        水平滚动
         * @param offset 滚动的偏移量
         */
        void ScrollHorizontal(double offset);

        /**
         * @brief        垂直滚动
         * @param offset 滚动的偏移量
         */
        void ScrollVertical(double offset);
    };
}

// HwndWrapper.h


namespace sw
{
    /**
     * @brief 将Win32 window包装为SimpleWindow对象
     * @note  与HwndHost类似，不同的是HwndWrapper不会创建容器窗口并且会接管句柄的窗口过程函数
     */
    class HwndWrapper : public UIElement
    {
    protected:
        /**
         * @brief 子类需要调用该函数以初始化HwndWrapper，该函数会调用BuildWindowCore
         */
        void InitHwndWrapper();

        /**
         * @brief           初始化HwndWrapper时会调用该函数，需在该函数中创建并返回要被托管的窗口句柄
         * @param isControl 若被托管的窗口句柄是一个控件需要将该值置为true
         * @param controlId 当isControl被设为true时，该值提供一个可用的控件id，确保不会与其他控件的id相重复
         * @return          被托管窗口句柄
         */
        virtual HWND BuildWindowCore(bool &isControl, int controlId) = 0;
    };
}

// Control.h


namespace sw
{
    /**
     * @brief 控件
     */
    class Control : virtual public UIElement
    {
    private:
        /**
         * @brief 是否绘制虚线框
         */
        bool _drawFocusRect = false;

        /**
         * @brief 标记当前控件是否响应了NM_CUSTOMDRAW消息
         */
        bool _hasCustomDraw = false;

    public:
        /**
         * @brief 控件的标识符
         */
        const ReadOnlyProperty<int> ControlId;

    public:
        /**
         * @brief 初始化控件
         */
        Control();

        /**
         * @brief 析构函数，这里用纯虚函数使该类成为抽象类
         */
        virtual ~Control() = 0;

        /**
         * @brief  尝试将对象转换成Control
         * @return 若函数成功则返回Control指针，否则返回nullptr
         */
        virtual Control *ToControl() override;

    protected:
        /**
         * @brief         销毁控件句柄并重新初始化，该操作会创建新的句柄并设置样式、文本、字体等
         * @param lpParam 创建控件句柄时传给CreateWindowExW的参数
         */
        void ResetHandle(LPVOID lpParam = NULL);

        /**
         * @brief         销毁控件句柄并重新初始化，并修改样式，该操作会创建新的句柄并设置样式、文本、字体等
         * @param style   新的样式
         * @param exStyle 新的扩展样式
         * @param lpParam 创建控件句柄时传给CreateWindowExW的参数
         */
        void ResetHandle(DWORD style, DWORD exStyle, LPVOID lpParam = NULL);

    protected:
        /**
         * @brief        父窗口接收到WM_NOTIFY后且父窗口OnNotify函数返回false时调用发出通知控件的该函数
         * @param pNMHDR 包含有关通知消息的信息
         * @param result 函数返回值为true时将该值作为消息的返回值
         * @return       若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnNotified(NMHDR *pNMHDR, LRESULT &result) override;

        /**
         * @brief            接收到WM_KILLFOCUS时调用该函数
         * @param hNextFocus 接收到焦点的hwnd，可能为NULL
         * @return           若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnKillFocus(HWND hNextFocus) override;

        /**
         * @brief 通过tab键将焦点移动到当前元素时调用该函数
         */
        virtual void OnTabStop() override;

        /**
         * @brief 在OnPaint函数完成之后调用该函数
         */
        virtual void OnEndPaint() override;

        /**
         * @brief        接收到NM_CUSTOMDRAW后调用该函数
         * @param pNMCD  包含有关自定义绘制的信息
         * @param result 函数返回值为true时将该值作为消息的返回值
         * @return       若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnCustomDraw(NMCUSTOMDRAW *pNMCD, LRESULT &result);

        /**
         * @brief        擦除背景前调用该函数
         * @param hdc    绘制设备句柄
         * @param result 函数返回值为true时将该值作为NM_CUSTOMDRAW消息的返回值
         * @return       若已完成绘制则返回true，否则返回false以使用默认绘制
         */
        virtual bool OnPreErase(HDC hdc, LRESULT &result);

        /**
         * @brief        擦除背景后调用该函数
         * @param hdc    绘制设备句柄
         * @param result 函数返回值为true时将该值作为NM_CUSTOMDRAW消息的返回值
         * @return       若已完成绘制则返回true，否则返回false以使用默认绘制
         */
        virtual bool OnPostErase(HDC hdc, LRESULT &result);

        /**
         * @brief        绘制控件前调用该函数
         * @param hdc    绘制设备句柄
         * @param result 函数返回值为true时将该值作为NM_CUSTOMDRAW消息的返回值
         * @return       若已完成绘制则返回true，否则返回false以使用默认绘制
         */
        virtual bool OnPrePaint(HDC hdc, LRESULT &result);

        /**
         * @brief        绘制控件后调用该函数
         * @param hdc    绘制设备句柄
         * @param result 函数返回值为true时将该值作为NM_CUSTOMDRAW消息的返回值
         * @return       若已完成绘制则返回true，否则返回false以使用默认绘制
         */
        virtual bool OnPostPaint(HDC hdc, LRESULT &result);

        /**
         * @brief     绘制虚线框时调用该函数
         * @param hdc 绘制设备句柄
         */
        virtual void OnDrawFocusRect(HDC hdc);

        /**
         * @brief      控件句柄发生改变时调用该函数
         * @param hwnd 新的控件句柄
         */
        virtual void OnHandleChenged(HWND hwnd);
    };
}

// StackLayout.h


namespace sw
{
    /**
     * @brief 堆叠布局
     */
    class StackLayout : public StackLayoutH, public StackLayoutV
    {
    public:
        /**
         * @brief 排列方式
         */
        Orientation orientation = Orientation::Vertical;

        /**
         * @brief 计算所需尺寸
         */
        virtual void MeasureOverride(Size &availableSize) override;

        /**
         * @brief 安排控件
         */
        virtual void ArrangeOverride(Size &finalSize) override;
    };
}

// WrapLayout.h


namespace sw
{
    /**
     * @brief 自动换行布局
     */
    class WrapLayout : public WrapLayoutH, public WrapLayoutV
    {
    public:
        /**
         * @brief 排列方式
         */
        Orientation orientation = Orientation::Horizontal;

        /**
         * @brief 计算所需尺寸
         */
        virtual void MeasureOverride(Size &availableSize) override;

        /**
         * @brief 安排控件
         */
        virtual void ArrangeOverride(Size &finalSize) override;
    };
}

// Animation.h


namespace sw
{
    /**
     * @brief 动画控件，仅支持无音频的avi动画
     */
    class Animation : public Control
    {
    public:
        /**
         * @brief 是否将动画居中显示
         */
        const Property<bool> Center;

        /**
         * @brief 是否在打开avi文件后自动播放
         */
        const Property<bool> AutoPlay;

        /**
         * @brief 动画是否正在播放
         */
        const ReadOnlyProperty<bool> IsPlaying;

    public:
        /**
         * @brief 初始化动画控件
         */
        Animation();

        /**
         * @brief            从指定模块中打开avi动画
         * @param hInstance  DLL或EXE的模块句柄
         * @param resourceId 动画的资源序号
         * @return           若函数成功则返回true，否则返回false
         */
        bool Open(HINSTANCE hInstance, int resourceId);

        /**
         * @brief          从本地文件打开avi动画
         * @param fileName 动画的文件路径
         * @return         若函数成功则返回true，否则返回false
         */
        bool Open(const std::wstring &fileName);

        /**
         * @brief          从本地文件打开avi动画，传入nullptr可以关闭打开的动画
         * @param fileName 动画的文件路径
         * @return         若函数成功则返回true，否则返回false
         */
        bool Open(const wchar_t *fileName);

        /**
         * @brief            播放动画
         * @param times      循环播放次数，设为-1表示无限循环播放
         * @param beginFrame 从第几帧开始播放，值必须小于65536
         * @param endFrame   播放到第几帧，值必须小于65536，设为-1表示播放到动画结尾
         * @return           若函数成功则返回true，否则返回false
         */
        bool Play(int times, int beginFrame, int endFrame);

        /**
         * @brief            播放动画
         * @param times      循环播放次数，设为-1表示无限循环播放
         * @return           若函数成功则返回true，否则返回false
         */
        bool Play(int times = -1);

        /**
         * @brief  停止播放
         * @return 若函数成功则返回true，否则返回false
         */
        bool Stop();
    };
}

// PanelBase.h


namespace sw
{
    /**
     * @brief 面板类型的基类
     */
    class PanelBase : public Control, public Layer
    {
    public:
        /**
         * @brief 初始化PanelBase
         */
        PanelBase();

        /**
         * @brief 析构函数，这里用纯虚函数使该类成为抽象类
         */
        virtual ~PanelBase() = 0;

    protected:
        /**
         * @brief       接收到WM_VSCROLL时调用目标控件的该函数
         * @param event 事件类型，即消息wParam的低字
         * @param pos   当前滚动条的位置，仅当event为SB_THUMBPOSITION或SB_THUMBTRACK时有效
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnVerticalScroll(int event, int pos) override;

        /**
         * @brief       接收到WM_HSCROLL时调用目标控件的该函数
         * @param event 事件类型，即消息wParam的低字
         * @param pos   当前滚动条的位置，仅当event为SB_THUMBPOSITION或SB_THUMBTRACK时有效
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnHorizontalScroll(int event, int pos) override;

        /**
         * @brief        父窗口接收到WM_NOTIFY后且父窗口OnNotify函数返回false时调用发出通知控件的该函数
         * @param pNMHDR 包含有关通知消息的信息
         * @param result 函数返回值为true时将该值作为消息的返回值
         * @return       若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnNotified(NMHDR *pNMHDR, LRESULT &result) override;

        /**
         * @brief            接收到WM_KILLFOCUS时调用该函数
         * @param hNextFocus 接收到焦点的hwnd，可能为NULL
         * @return           若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnKillFocus(HWND hNextFocus) override;

        /**
         * @brief 通过tab键将焦点移动到当前元素时调用该函数
         */
        virtual void OnTabStop() override;

        /**
         * @brief 在OnPaint函数完成之后调用该函数
         */
        virtual void OnEndPaint() override;

    public:
        /**
         * @brief  尝试将对象转换成Control
         * @return 若函数成功则返回Control指针，否则返回nullptr
         */
        virtual Control *ToControl() override;

        /**
         * @brief               测量控件所需尺寸
         * @param availableSize 可用的尺寸
         */
        virtual void Measure(const Size &availableSize) override;

        /**
         * @brief           安排控件位置
         * @param finalSize 最终控件所安排的位置
         */
        virtual void Arrange(const sw::Rect &finalPosition) override;
    };
}

// TabControl.h


namespace sw
{
    /**
     * @brief TabControl标签的位置
     */
    enum class TabAlignment {
        Top,    // 顶部
        Bottom, // 底部
        Left,   // 左边
        Right   // 右边
    };

    /**
     * @brief 标签页控件
     */
    class TabControl : public Control
    {
    public:
        /**
         * @brief 内容区域位置与尺寸
         */
        const ReadOnlyProperty<sw::Rect> ContentRect;

        /**
         * @brief 当前页面的索引
         */
        const Property<int> SelectedIndex;

        /**
         * @brief 标签的位置
         */
        const Property<TabAlignment> Alignment;

        /**
         * @brief 是否开启多行标签
         */
        const Property<bool> MultiLine;

    public:
        /**
         * @brief 初始化标签页控件
         */
        TabControl();

        /**
         * @brief 获取标签项的数量
         */
        int GetTabCount();

        /**
         * @brief 更新标签项信息
         */
        void UpdateTab();

        /**
         * @brief       更新指定索引处页面项的文本
         * @param index 要更新的索引
         */
        void UpdateTabText(int index);

        /**
         * @brief               安排控件位置
         * @param finalPosition 最终控件所安排的位置
         */
        virtual void Arrange(const sw::Rect &finalPosition) override;

    protected:
        /**
         * @brief         添加子元素后调用该函数
         * @param element 添加的子元素
         */
        virtual void OnAddedChild(UIElement &element) override;

        /**
         * @brief         移除子元素后调用该函数
         * @param element 移除的子元素
         */
        virtual void OnRemovedChild(UIElement &element) override;

        /**
         * @brief        父窗口接收到WM_NOTIFY后且父窗口OnNotify函数返回false时调用发出通知控件的该函数
         * @param pNMHDR 包含有关通知消息的信息
         * @param result 函数返回值为true时将该值作为消息的返回值
         * @return       若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnNotified(NMHDR *pNMHDR, LRESULT &result) override;

        /**
         * @brief SelectedIndex属性更改时调用该函数
         */
        virtual void OnSelectedIndexChanged();

    private:
        /**
         * @brief 根据选中的tab更新子元素的Visible属性
         */
        void _UpdateChildVisible();

        /**
         * @brief 发送TCM_INSERTITEMW消息
         */
        int _InsertItem(int index, TCITEMW &item);

        /**
         * @brief 发送TCM_SETITEMW消息
         */
        bool _SetItem(int index, TCITEMW &item);

        /**
         * @brief 发送TCM_DELETEITEM消息
         */
        bool _DeleteItem(int index);

        /**
         * @brief 发送TCM_DELETEALLITEMS消息
         */
        bool _DeleteAllItems();
    };
}

// ProgressBar.h


namespace sw
{
    /**
     * @brief 进度条状态
     */
    enum class ProgressBarState {
        Normal = 0x0001, // 正常（PBST_NORMAL）
        Error  = 0x0002, // 错误（PBST_ERROR）
        Paused = 0x0003, // 暂停（PBST_PAUSED）
    };

    /**
     * @brief 进度条控件
     */
    class ProgressBar : public Control
    {
    public:
        /**
         * @brief 进度范围的下限
         */
        const Property<uint16_t> Minimum;

        /**
         * @brief 进度范围的上限
         */
        const Property<uint16_t> Maximum;

        /**
         * @brief 当前进度值
         */
        const Property<uint16_t> Value;

        /**
         * @brief 进度条状态
         */
        const Property<ProgressBarState> State;

        /**
         * @brief 是否显示为垂直进度条
         */
        const Property<bool> Vertical;

    public:
        /**
         * @brief 初始化进度条
         */
        ProgressBar();
    };
}

// SysLink.h


namespace sw
{
    /**
     * @brief 包含链接的文本区域
     */
    class SysLink : public Control
    {
    private:
        /**
         * @brief 文本所需的尺寸
         */
        sw::Size _textSize;

        /**
         * @brief 是否根据文本自动调整尺寸
         */
        bool _autoSize = true;

    public:
        /**
         * @brief 是否忽略回车按键，当该属性为true时按下回车键不会触发单击事件
         */
        const Property<bool> IgnoreReturn;

        /**
         * @brief 是否根据内容自动调整尺寸
         */
        const Property<bool> AutoSize;

    public:
        /**
         * @brief 初始化SysLink
         */
        SysLink();

    protected:
        /**
         * @brief Text属性更改时调用此函数
         */
        virtual void OnTextChanged() override;

        /**
         * @brief       字体改变时调用该函数
         * @param hfont 字体句柄
         */
        virtual void FontChanged(HFONT hfont) override;

        /**
         * @brief               测量控件所需尺寸
         * @param availableSize 可用的尺寸
         */
        virtual void Measure(const Size &availableSize) override;

        /**
         * @brief        父窗口接收到WM_NOTIFY后且父窗口OnNotify函数返回false时调用发出通知控件的该函数
         * @param pNMHDR 包含有关通知消息的信息
         * @param result 函数返回值为true时将该值作为消息的返回值
         * @return       若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnNotified(NMHDR *pNMHDR, LRESULT &result) override;

        /**
         * @brief         链接被单击时调用该函数
         * @param pNMLink 包含链接相关信息
         */
        virtual void OnClicked(NMLINK *pNMLink);

    private:
        /**
         * @brief 更新_textSize
         */
        void _UpdateTextSize();

        /**
         * @brief 调整尺寸为_textSize
         */
        void _ResizeToTextSize();
    };
}

// MonthCalendar.h


namespace sw
{
    /**
     * @brief 月历控件
     */
    class MonthCalendar : public Control
    {
    public:
        /**
         * @brief 是否显示当前日期
         */
        const Property<bool> ShowToday;

    public:
        /**
         * @brief 初始化月历控件
         */
        MonthCalendar();

        /**
         * @brief     获取当前控件的“今天”部分所表示的时间
         * @param out 输出的SYSTEMTIME结构体
         * @return    若获取成功则返回true，否则返回false
         */
        bool GetToday(SYSTEMTIME &out);

        /**
         * @brief       设置当前控件的“今天”部分所表示的时间
         * @param today 要设置的时间
         * @return      若设置成功则返回true，否则返回false
         */
        bool SetToday(const SYSTEMTIME &today);

        /**
         * @brief     获取当前控件表示的时间
         * @param out 输出的SYSTEMTIME结构体
         * @return    若获取成功则返回true，否则返回false
         */
        bool GetTime(SYSTEMTIME &out);

        /**
         * @brief      设置当前控件表示的时间
         * @param time 要设置的时间
         * @return     若设置成功则返回true，否则返回false
         */
        bool SetTime(const SYSTEMTIME &time);

        /**
         * @brief         设置可选的时间段
         * @param minTime 最早时间
         * @param maxTime 最晚时间
         * @return        若设置成功则返回true，否则返回false
         */
        bool SetRange(const SYSTEMTIME &minTime, const SYSTEMTIME &maxTime);

    protected:
        /**
         * @brief     绘制虚线框时调用该函数
         * @param hdc 绘制设备句柄
         */
        virtual void OnDrawFocusRect(HDC hdc) override;

        /**
         * @brief        设置背景颜色
         * @param color  要设置的颜色
         * @param redraw 是否重绘
         */
        virtual void SetBackColor(Color color, bool redraw) override;

        /**
         * @brief        设置文本颜色
         * @param color  要设置的颜色
         * @param redraw 是否重绘
         */
        virtual void SetTextColor(Color color, bool redraw) override;

        /**
         * @brief        父窗口接收到WM_NOTIFY后且父窗口OnNotify函数返回false时调用发出通知控件的该函数
         * @param pNMHDR 包含有关通知消息的信息
         * @param result 函数返回值为true时将该值作为消息的返回值
         * @return       若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnNotified(NMHDR *pNMHDR, LRESULT &result) override;

        /**
         * @brief       当前控件表示的时间改变时调用该函数
         * @param pInfo 发生改变的信息
         */
        virtual void OnTimeChanged(NMSELCHANGE *pInfo);
    };
}

// StatusBar.h


namespace sw
{
    /**
     * @brief 状态栏
     */
    class StatusBar : public Control
    {
    public:
        /**
         * @brief 是否在控件右下角显示大小调整手柄，修改该属性会重置当前控件
         */
        const Property<bool> SizingGrip;

        /**
         * @brief 获取当前控件中的分区数
         */
        const ReadOnlyProperty<int> PartsCount;

        /**
         * @brief 指定控件是否使用Unicode编码
         */
        const Property<bool> UseUnicode;

    public:
        /**
         * @brief 初始化状态栏
         */
        StatusBar();

        /**
         * @brief       设置控件中的分区信息
         * @param parts 每个部分的宽度，用-1表示延伸到最右边，不能多于256个
         * @return      若函数成功则返回true，否则返回false
         */
        bool SetParts(std::initializer_list<double> parts);

        /**
         * @brief       获取指定部分的文本
         * @param index 指定部分的索引
         * @param out   获取到的文本
         * @return      若函数成功则返回true，否则返回false
         */
        bool GetTextAt(uint8_t index, std::wstring &out);

        /**
         * @brief       设置指定部分的文本
         * @param index 指定部分的索引
         * @param text  要设置的文本
         * @return      若函数成功则返回true，否则返回false
         */
        bool SetTextAt(uint8_t index, const std::wstring &text);

        /**
         * @brief       获取指定部分的位置与尺寸
         * @param index 指定部分的索引
         * @param out   获取到的Rect
         * @return      若函数成功则返回true，否则返回false
         */
        bool GetRectAt(uint8_t index, sw::Rect &out);

    protected:
        /**
         * @brief        设置背景颜色
         * @param color  要设置的颜色
         * @param redraw 是否重绘
         */
        virtual void SetBackColor(Color color, bool redraw) override;
    };
}

// HotKeyControl.h


namespace sw
{
    /**
     * @brief 表示一个热键
     */
    struct HotKey {
        VirtualKey key;          // 按键
        HotKeyModifier modifier; // 辅助按键
    };

    /**
     * @brief 热键组合
     */
    enum class HotKeyCombination {
        Alt          = HKCOMB_A,    // ALT
        Ctrl         = HKCOMB_C,    // CTRL
        CtrlAlt      = HKCOMB_CA,   // CTRL+ALT
        None         = HKCOMB_NONE, // Unmodified keys
        Shift        = HKCOMB_S,    // SHIFT
        ShiftAlt     = HKCOMB_SA,   // SHIFT+ALT
        ShiftCtrl    = HKCOMB_SC,   // SHIFT+CTRL
        ShiftCtrlAlt = HKCOMB_SCA,  // SHIFT+CTRL+ALT
    };

    /**
     * @brief 标记HotKeyCombination枚举类型支持位运算
     */
    _SW_ENUM_ENABLE_BIT_OPERATIONS(HotKeyCombination);

    /**
     * @brief 热键框
     */
    class HotKeyControl : public Control
    {
    private:
        /**
         * @brief 热键的值
         */
        HotKey _value;

    public:
        /**
         * @brief 当前控件所表示的热键值
         */
        const Property<HotKey> Value;

    public:
        /**
         * @brief 初始化热键框
         */
        HotKeyControl();

        /**
         * @brief                 设置无效组合与默认值
         * @param invalidComb     无效的组合键
         * @param defaultModifier 用户输入无效组合时使用该组合键替换
         */
        void SetRules(HotKeyCombination invalidComb, HotKeyModifier defaultModifier);

    protected:
        /**
         * @brief      当父窗口接收到控件的WM_COMMAND时调用该函数
         * @param code 通知代码
         */
        virtual void OnCommand(int code) override;

        /**
         * @brief 控件表示的热键值发生改变时调用该函数
         */
        virtual void OnValueChanged(HotKey value);

    private:
        /**
         * @brief 更新_value
         */
        void _UpdateValue();
    };
}

// Window.h


namespace sw
{
    /**
     * @brief 窗口状态
     */
    enum class WindowState {
        Normal,    // 普通窗口
        Minimized, // 最小化窗口
        Maximized, // 最大化窗口
    };

    /**
     * @brief 窗口启动位置
     */
    enum class WindowStartupLocation {
        Manual,       // 使用系统默认或手动设置
        CenterScreen, // 屏幕中心
        CenterOwner,  // 所有者窗口中心
    };

    /**
     * @brief 窗口
     */
    class Window : public Layer
    {
    private:
        /**
         * @brief 是否为第一次显示，该字段使StartupLocation生效
         */
        bool _isFirstShow = true;

        /**
         * @brief 窗口的尺寸限制，当值不大于0时表示不限制
         */
        double _maxWidth = -1, _maxHeight = -1, _minWidth = -1, _minHeight = -1;

        /**
         * @brief 窗口初次启动的位置
         */
        WindowStartupLocation _startupLocation = WindowStartupLocation::Manual;

        /**
         * @brief 窗口顶部菜单
         */
        sw::Menu *_menu = nullptr;

        /**
         * @brief 窗口在失去焦点前保存句柄，用于窗口重新获得焦点时保留原先焦点控件的焦点
         */
        HWND _hPrevFocused = NULL;

        /**
         * @brief 窗口的默认布局方式
         */
        std::unique_ptr<LayoutHost> _layout;

        /**
         * @brief 当前窗口是否显示为模态窗口
         */
        bool _isModal = false;

        /**
         * @brief 以模态窗口显示时保存所有者窗口
         */
        HWND _hModalOwner = NULL;

        /**
         * @brief 窗口无边框
         */
        bool _isBorderless = false;

    public:
        /**
         * @brief 程序的当前活动窗体
         */
        static const ReadOnlyProperty<Window *> ActiveWindow;

        /**
         * @brief 当前已创建的窗口数
         */
        static const ReadOnlyProperty<int> WindowCount;

        /**
         * @brief 窗口初次启动的位置
         */
        const Property<WindowStartupLocation> StartupLocation;

        /**
         * @brief 窗口状态
         */
        const Property<WindowState> State;

        /**
         * @brief 窗口是否可调整大小
         */
        const Property<bool> SizeBox;

        /**
         * @brief 最大化按钮是否可用
         */
        const Property<bool> MaximizeBox;

        /**
         * @brief 最小化按钮是否可用
         */
        const Property<bool> MinimizeBox;

        /**
         * @brief 窗口是否置顶
         */
        const Property<bool> Topmost;

        /**
         * @brief 是否显示为ToolWindow (窄边框)
         */
        const Property<bool> ToolWindow;

        /**
         * @brief 最大宽度，当值不大于0时表示不限制
         */
        const Property<double> MaxWidth;

        /**
         * @brief 最大高度，当值不大于0时表示不限制
         */
        const Property<double> MaxHeight;

        /**
         * @brief 最小宽度，当值不大于0时表示不限制
         */
        const Property<double> MinWidth;

        /**
         * @brief 最小高度，当值不大于0时表示不限制
         */
        const Property<double> MinHeight;

        /**
         * @brief 窗口顶部的菜单栏
         */
        const Property<sw::Menu *> Menu;

        /**
         * @brief  窗口是否显示为模态窗口，当调用ShowDialog时该属性值为true，否则为false
         */
        const ReadOnlyProperty<bool> IsModal;

        /**
         * @brief 拥有者窗口
         */
        const Property<Window *> Owner;

        /**
         * @brief 窗口是否为分层窗口，即WS_EX_LAYERED样式是否被设置
         */
        const Property<bool> IsLayered;

        /**
         * @brief 窗口的透明度，范围为0.0~1.0
         * @note  只有将IsLayered设为true该属性才生效，初始值为0.0但需手动设置新值后才会生效
         */
        const Property<double> Opacity;

        /**
         * @brief 窗口无边框
         */
        const Property<bool> Borderless;

    public:
        /**
         * @brief 初始化窗口
         */
        Window();

    protected:
        /**
         * @brief 对WndProc的封装
         */
        virtual LRESULT WndProc(const ProcMsg &refMsg) override;

        /**
         * @brief 获取默认布局对象
         */
        virtual LayoutHost *GetDefaultLayout() override;

        /**
         * @brief  接收到WM_CLOSE时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnClose() override;

        /**
         * @brief  接收到WM_DESTROY时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnDestroy() override;

        /**
         * @brief        接收到WM_ERASEBKGND时调用该函数
         * @param hdc    设备上下文句柄
         * @param result 若已处理该消息则设为非零值，默认值为0
         * @return       若返回true则将result作为消息的返回值，否则使用DefaultWndProc的返回值
         */
        virtual bool OnEraseBackground(HDC hdc, LRESULT &result) override;

        /**
         * @brief  接收到WM_PAINT时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnPaint() override;

        /**
         * @brief    当OnCommand接收到菜单命令时调用该函数
         * @param id 菜单id
         */
        virtual void OnMenuCommand(int id) override;

        /**
         * @brief 窗口第一次显示时调用该函数
         */
        virtual void OnFirstShow();

        /**
         * @brief 窗口成为前台窗口时调用该函数
         */
        virtual void OnActived();

        /**
         * @brief 窗口成为后台窗口时调用该函数
         */
        virtual void OnInactived();

        /**
         * @brief      接收到WM_DPICHANGED时调用该函数
         * @param dpiX 横向DPI
         * @param dpiY 纵向DPI
         */
        virtual void OnDpiChanged(int dpiX, int dpiY);

    public:
        /**
         * @brief  尝试将对象转换成Window
         * @return 若函数成功则返回Window指针，否则返回nullptr
         */
        virtual Window *ToWindow() override;

        /**
         * @brief 显示窗口
         */
        void Show(int nCmdShow = SW_SHOW);

        /**
         * @brief       将窗口显示为模式对话框
         * @param owner 窗体的所有者，窗体显示期间该窗体的Enabled属性将被设为false，该参数不能设为自己
         * @note        该函数会创建一个新的消息循环并在窗口销毁时退出
         */
        void ShowDialog(Window &owner);

        /**
         * @brief 将窗口显示为模式对话框
         * @note  该函数会创建一个新的消息循环并在窗口销毁时退出
         */
        void ShowDialog();

        /**
         * @brief       设置图标
         * @param hIcon 图标句柄
         */
        void SetIcon(HICON hIcon);

        /**
         * @brief 重回窗口的菜单栏
         */
        void DrawMenuBar();

        /**
         * @brief 调整窗口尺寸以适应其内容大小，只对设置了布局方式的顶级窗口有效
         */
        void SizeToContent();

        /**
         * @brief 设置窗口的默认布局方式
         */
        template <typename TLayout>
        typename std::enable_if<std::is_base_of<LayoutHost, TLayout>::value>::type SetLayout()
        {
            this->_layout.reset(new TLayout);
            this->_layout->Associate(this);
            this->NotifyLayoutUpdated();
        }

        /**
         * @brief 取消通过SetLayout设置的布局方式
         */
        template <std::nullptr_t>
        void SetLayout()
        {
            this->_layout.reset(nullptr);
            this->NotifyLayoutUpdated();
        }
    };
}

// StaticControl.h


namespace sw
{
    /**
     * @brief 静态控件
     */
    class StaticControl : public Control
    {
    public:
        /**
         * @brief 获取或设置控件的SS_NOTIFY样式
         */
        const Property<bool> Notify;

    public:
        /**
         * @brief 初始化静态控件
         */
        StaticControl();
    };
}

// Slider.h


namespace sw
{
    /**
     * @brief 滑块控件
     */
    class Slider : public Control
    {
    public:
        /**
         * @brief 最小值
         */
        const Property<int> Minimum;

        /**
         * @brief 最大值
         */
        const Property<int> Maximum;

        /**
         * @brief 当前滑块的值
         */
        const Property<int> Value;

        /**
         * @brief 是否显示为垂直滑块
         */
        const Property<bool> Vertical;

        /**
         * @brief 是否在滑动时显示数值提示
         */
        const Property<bool> ValueTooltips;

    public:
        /**
         * @brief 初始化滑块控件
         */
        Slider();

    protected:
        /**
         * @brief       接收到WM_VSCROLL时调用目标控件的该函数
         * @param event 事件类型，即消息wParam的低字
         * @param pos   当前滚动条的位置，仅当event为SB_THUMBPOSITION或SB_THUMBTRACK时有效
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnVerticalScroll(int event, int pos) override;

        /**
         * @brief       接收到WM_HSCROLL时调用目标控件的该函数
         * @param event 事件类型，即消息wParam的低字
         * @param pos   当前滚动条的位置，仅当event为SB_THUMBPOSITION或SB_THUMBTRACK时有效
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnHorizontalScroll(int event, int pos) override;

        /**
         * @brief 滑块的值改变时调用该函数
         */
        virtual void OnValueChanged();

        /**
         * @brief 释放滑块时调用该函数
         */
        virtual void OnEndTrack();
    };
}

// ButtonBase.h


namespace sw
{
    /**
     * @brief 窗口类名为BUTTON的控件类型的基类
     */
    class ButtonBase : public Control
    {
    protected:
        /**
         * @brief 初始化ButtonBase
         */
        ButtonBase();

    public:
        /**
         * @brief 析构函数，这里用纯虚函数使该类成为抽象类
         */
        virtual ~ButtonBase() = 0;

    protected:
        /**
         * @brief 初始化控件
         */
        void InitButtonBase(LPCWSTR lpWindowName, DWORD dwStyle, DWORD dwExStyle);

        /**
         * @brief      当父窗口接收到控件的WM_COMMAND时调用该函数
         * @param code 通知代码
         */
        virtual void OnCommand(int code) override;

        /**
         * @brief 被单击时调用该函数
         */
        virtual void OnClicked();

        /**
         * @brief 被双击时调用该函数
         */
        virtual void OnDoubleClicked();
    };
}

// DateTimePicker.h


namespace sw
{
    /**
     * @brief 指定DateTimePicker控件显示的日期和时间格式
     */
    enum class DateTimePickerFormat {
        Short,  // 以短格式显示日期
        Long,   // 以长格式显示日期
        Custom, // 自定义格式
    };

    /**
     * @brief 日期和时间选取器
     */
    class DateTimePicker : public Control
    {
    private:
        /**
         * @brief 日期和时间格式
         */
        DateTimePickerFormat _format{DateTimePickerFormat::Short};

        /**
         * @brief 自定义格式字符串
         */
        std::wstring _customFormat{};

    public:
        /**
         * @brief 是否显示上下调整按钮
         */
        const Property<bool> ShowUpDownButton;

        /**
         * @brief 日期和时间格式
         */
        const Property<DateTimePickerFormat> Format;

        /**
         * @brief 自定义日期和时间格式字符串，空字符串表示默认格式
         */
        const Property<std::wstring> CustomFormat;

    public:
        /**
         * @brief 初始化DateTimePicker
         */
        DateTimePicker();

        /**
         * @brief     获取当前控件表示的时间
         * @param out 输出的SYSTEMTIME结构体
         * @return    若获取成功则返回true，否则返回false
         */
        bool GetTime(SYSTEMTIME &out);

        /**
         * @brief      设置当前控件表示的时间
         * @param time 要设置的时间
         * @return     若设置成功则返回true，否则返回false
         */
        bool SetTime(const SYSTEMTIME &time);

        /**
         * @brief         设置可选的时间段
         * @param minTime 最早时间
         * @param maxTime 最晚时间
         * @return        若设置成功则返回true，否则返回false
         */
        bool SetRange(const SYSTEMTIME &minTime, const SYSTEMTIME &maxTime);

    protected:
        /**
         * @brief        父窗口接收到WM_NOTIFY后且父窗口OnNotify函数返回false时调用发出通知控件的该函数
         * @param pNMHDR 包含有关通知消息的信息
         * @param result 函数返回值为true时将该值作为消息的返回值
         * @return       若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnNotified(NMHDR *pNMHDR, LRESULT &result) override;

        /**
         * @brief       当前控件表示的时间改变时调用该函数
         * @param pInfo 发生改变的信息
         */
        virtual void OnTimeChanged(NMDATETIMECHANGE *pInfo);

    private:
        /**
         * @brief       设置格式字符串
         * @param value 要设置的值
         */
        void _SetFormat(const std::wstring &value);

        /**
         * @brief       修改当前控件的样式，该函数会调用ResetHandle
         * @param style 新的样式
         */
        void _UpdateStyle(DWORD style);
    };
}

// TextBoxBase.h


namespace sw
{
    /**
     * @brief 窗口类名为EDIT的控件类型的基类
     */
    class TextBoxBase : public Control
    {
    private:
        /**
         * @brief 在读取Text属性时用于判断是否需要更新储存的文本
         */
        bool _isTextChanged = false;

        /**
         * @brief 是否允许输入制表符
         */
        bool _acceptTab = false;

    public:
        /**
         * @brief 是否只读
         */
        const Property<bool> ReadOnly;

        /**
         * @brief 文本的水平对齐方式，可设为左对齐、中心对齐、右对齐
         */
        const Property<sw::HorizontalAlignment> HorizontalContentAlignment;

        /**
         * @brief 是否可以撤销
         */
        const ReadOnlyProperty<bool> CanUndo;

        /**
         * @brief 是否允许输入制表符
         */
        const Property<bool> AcceptTab;

    protected:
        /**
         * @brief 初始化TextBoxBase
         */
        TextBoxBase();

    public:
        /**
         * @brief 析构函数，这里用纯虚函数使该类成为抽象类
         */
        virtual ~TextBoxBase() = 0;

    protected:
        /**
         * @brief 初始化控件
         */
        void InitTextBoxBase(DWORD dwStyle, DWORD dwExStyle);

        /**
         * @brief  获取窗口文本
         * @return 编辑框的文本内容
         */
        virtual std::wstring &GetText() override;

        /**
         * @brief      当父窗口接收到控件的WM_COMMAND时调用该函数
         * @param code 通知代码
         */
        virtual void OnCommand(int code) override;

        /**
         * @brief       接收到WM_CHAR时调用该函数
         * @param ch    按键的字符代码
         * @param flags 附加信息
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnChar(wchar_t ch, KeyFlags flags) override;

        /**
         * @brief       接收到WM_KEYDOWN时调用该函数
         * @param key   虚拟按键
         * @param flags 附加信息
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnKeyDown(VirtualKey key, KeyFlags flags) override;

        /**
         * @brief     绘制虚线框时调用该函数
         * @param hdc 绘制设备句柄
         */
        virtual void OnDrawFocusRect(HDC hdc) override;

    public:
        /**
         * @brief        选择指定文本内容
         * @param start  起始位置
         * @param length 选择文本的长度
         */
        void Select(int start, int length);

        /**
         * @brief 选中所有文本
         */
        void SelectAll();

        /**
         * @brief 将控件内容滚动到当前插入符号位置
         */
        void ScrollToCaret();

        /**
         * @brief  撤销
         * @return 操作是否成功
         */
        bool Undo();

        /**
         * @brief 清空内容
         */
        void Clear();
    };
}

// ItemsControl.h


namespace sw
{
    template <typename TItem>
    class ItemsControl; // 向前声明

    /**
     * @brief 表示可用于呈现一组字符串的控件
     */
    typedef ItemsControl<std::wstring> StrItemsControl;

    /**
     * @brief 表示可用于呈现一组项的控件
     */
    template <typename TItem>
    class ItemsControl : public Control
    {
    public:
        /**
         * @brief 项数
         */
        const ReadOnlyProperty<int> ItemsCount{
            // get
            [this]() -> int {
                return this->GetItemsCount();
            }};

        /**
         * @brief 选中项的索引，当无选中项时为-1
         */
        const Property<int> SelectedIndex{
            // get
            [this]() -> int {
                return this->GetSelectedIndex();
            },
            // set
            [this](const int &value) {
                this->SetSelectedIndex(value);
            }};

        /**
         * @brief 选中项
         */
        const ReadOnlyProperty<TItem> SelectedItem{
            // get
            [this]() -> TItem {
                return this->GetSelectedItem();
            }};

    protected:
        /**
         * @brief 选中项改变时调用该函数
         */
        virtual void OnSelectionChanged()
        {
            this->RaiseRoutedEvent(ItemsControl_SelectionChanged);
        }

    protected:
        /**
         * @brief  获取子项数
         */
        virtual int GetItemsCount() = 0;

        /**
         * @brief 选中项的索引，当无选中项时为-1
         */
        virtual int GetSelectedIndex() = 0;

        /**
         * @brief 设置选中项索引
         */
        virtual void SetSelectedIndex(int index) = 0;

        /**
         * @brief 获取选中项
         */
        virtual TItem GetSelectedItem() = 0;

    public:
        /**
         * @brief 清空所有子项
         */
        virtual void Clear() = 0;

        /**
         * @brief       获取指定索引处子项的值
         * @param index 子项的索引
         */
        virtual TItem GetItemAt(int index) = 0;

        /**
         * @brief      添加新的子项
         * @param item 要添加的子项
         * @return     是否添加成功
         */
        virtual bool AddItem(const TItem &item) = 0;

        /**
         * @brief       添加子项到指定索引
         * @param index 要插入的位置
         * @param item  要添加的子项
         * @return      是否添加成功
         */
        virtual bool InsertItem(int index, const TItem &item) = 0;

        /**
         * @brief          更新指定位置的子项
         * @param index    要更新子项的位置
         * @param newValue 子项的新值
         * @return         操作是否成功
         */
        virtual bool UpdateItem(int index, const TItem &newValue) = 0;

        /**
         * @brief       移除指定索引处的子项
         * @param index 要移除子项的索引
         * @return      操作是否成功
         */
        virtual bool RemoveItemAt(int index) = 0;
    };
}

// Button.h


namespace sw
{
    /**
     * @brief 按钮
     */
    class Button : public ButtonBase
    {
    public:
        /**
         * @brief 初始化按钮
         */
        Button();

    protected:
        /**
         * @brief     绘制虚线框时调用该函数
         * @param hdc 绘制设备句柄
         */
        virtual void OnDrawFocusRect(HDC hdc) override;

        /**
         * @brief           接收到WM_SETFOCUS时调用该函数
         * @param hPreFocus 丢失焦点的hwnd，可能为NULL
         * @return          若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSetFocus(HWND hPreFocus) override;

        /**
         * @brief            接收到WM_KILLFOCUS时调用该函数
         * @param hNextFocus 接收到焦点的hwnd，可能为NULL
         * @return           若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnKillFocus(HWND hNextFocus) override;

        /**
         * @brief       接收到WM_KEYDOWN时调用该函数
         * @param key   虚拟按键
         * @param flags 附加信息
         * @return      若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnKeyDown(VirtualKey key, KeyFlags flags) override;
    };
}

// ListView.h


namespace sw
{
    /**
     * @brief 列表视图的图像列表枚举
     */
    enum class ListViewImageList {
        Normal      = LVSIL_NORMAL,      // 包含大图标的图像列表
        Small       = LVSIL_SMALL,       // 包含小图标的图像列表
        State       = LVSIL_STATE,       // 包含状态图像的图像列表
        GroupHeader = LVSIL_GROUPHEADER, // 组标头的图像列表
    };

    /**
     * @brief 列表视图的列对齐方式
     */
    enum class ListViewColumnAlignment {
        Left   = LVCFMT_LEFT,   // 左对齐
        Right  = LVCFMT_RIGHT,  // 右对齐
        Center = LVCFMT_CENTER, // 居中
    };

    /**
     * @brief 列表视图的列信息
     */
    struct ListViewColumn {
        /**
         * @brief 列标题
         */
        std::wstring header;

        /**
         * @brief 列宽度
         */
        double width;

        /**
         * @brief 对齐方式
         */
        ListViewColumnAlignment alignment = ListViewColumnAlignment::Left;

        /**
         * @brief 构造函数
         */
        ListViewColumn(const std::wstring &header);

        /**
         * @brief 构造函数
         */
        ListViewColumn(const std::wstring &header, double width);

        /**
         * @brief 从LVCOLUMNW构造
         */
        ListViewColumn(const LVCOLUMNW &lvc);

        /**
         * @brief 隐式转换LVCOLUMNW
         */
        operator LVCOLUMNW() const;
    };

    /**
     * @brief 列表视图
     */
    class ListView : public ItemsControl<StrList>
    {
    public:
        /**
         * @brief 列数
         */
        const ReadOnlyProperty<int> ColumnsCount;

        /**
         * @brief 是否显示网格线
         */
        const Property<bool> GridLines;

        /**
         * @brief 是否允许多选
         */
        const Property<bool> MultiSelect;

        /**
         * @brief 多选状态下可通过该属性获取选中项的个数
         */
        const ReadOnlyProperty<int> SelectedCount;

        /**
         * @brief 是否在第一列显示复选框
         */
        const Property<bool> CheckBoxes;

        /**
         * @brief 当前列表框页面第一个子项的索引
         */
        const ReadOnlyProperty<int> TopIndex;

        /**
         * @brief 允许将同一图像列表与多个列表视图控件配合使用，控件销毁时若该属性为true则不会销毁图像列表
         */
        const Property<bool> ShareImageLists;

        /**
         * @brief 是否允许编辑
         */
        const Property<bool> Editable;

    public:
        /**
         * @brief 初始化ListView
         */
        ListView();

    protected:
        /**
         * @brief  获取子项数
         */
        virtual int GetItemsCount() override;

        /**
         * @brief 选中项的索引，当无选中项时为-1
         */
        virtual int GetSelectedIndex() override;

        /**
         * @brief 设置选中项索引
         */
        virtual void SetSelectedIndex(int index) override;

        /**
         * @brief 获取选中项
         */
        virtual StrList GetSelectedItem() override;

        /**
         * @brief        设置背景颜色
         * @param color  要设置的颜色
         * @param redraw 是否重绘
         */
        virtual void SetBackColor(Color color, bool redraw) override;

        /**
         * @brief        设置文本颜色
         * @param color  要设置的颜色
         * @param redraw 是否重绘
         */
        virtual void SetTextColor(Color color, bool redraw) override;

        /**
         * @brief        接收到WM_NOTIFY后调用该函数
         * @param pNMHDR 包含有关通知消息的信息
         * @param result 函数返回值为true时将该值作为消息的返回值，默认值为0
         * @return       若已处理该消息则返回true，否则调用发出通知控件的OnNotified函数，依据其返回值判断是否调用DefaultWndProc
         */
        virtual bool OnNotify(NMHDR *pNMHDR, LRESULT &result) override;

        /**
         * @brief        父窗口接收到WM_NOTIFY后且父窗口OnNotify函数返回false时调用发出通知控件的该函数
         * @param pNMHDR 包含有关通知消息的信息
         * @param result 函数返回值为true时将该值作为消息的返回值
         * @return       若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnNotified(NMHDR *pNMHDR, LRESULT &result) override;

        /**
         * @brief     绘制虚线框时调用该函数
         * @param hdc 绘制设备句柄
         */
        virtual void OnDrawFocusRect(HDC hdc) override;

        /**
         * @brief 列表项某些属性发生变化时调用该函数
         */
        virtual void OnItemChanged(NMLISTVIEW *pNMLV);

        /**
         * @brief       复选框选中状态发生改变
         * @param index 改变项的索引
         */
        virtual void OnCheckStateChanged(int index);

        /**
         * @brief 鼠标左键单击列标题时调用该函数
         */
        virtual void OnHeaderItemClicked(NMHEADERW *pNMH);

        /**
         * @brief 鼠标左键双击列标题时调用该函数
         */
        virtual void OnHeaderItemDoubleClicked(NMHEADERW *pNMH);

        /**
         * @brief 鼠标左键单击某一项时调用该函数
         */
        virtual void OnItemClicked(NMITEMACTIVATE *pNMIA);

        /**
         * @brief 鼠标左键双击某一项调用该函数
         */
        virtual void OnItemDoubleClicked(NMITEMACTIVATE *pNMIA);

        /**
         * @brief         当OnNotified接收到LVN_GETDISPINFOW通知时调用该函数
         * @param pNMInfo 包含有关通知消息的信息
         */
        virtual void OnGetDispInfo(NMLVDISPINFOW *pNMInfo);

        /**
         * @brief  编辑状态结束后调用该函数
         * @return 是否应用新文本
         */
        virtual bool OnEndEdit(NMLVDISPINFOW *pNMInfo);

    public:
        /**
         * @brief 清空所有子项
         */
        virtual void Clear() override;

        /**
         * @brief       获取指定索引处子项的值
         * @param index 子项的索引
         */
        virtual StrList GetItemAt(int index) override;

        /**
         * @brief      添加新的子项
         * @param item 要添加的子项
         * @return     是否添加成功
         */
        virtual bool AddItem(const StrList &item) override;

        /**
         * @brief       添加子项到指定索引
         * @param index 要插入的位置
         * @param item  要添加的子项
         * @return      是否添加成功
         */
        virtual bool InsertItem(int index, const StrList &item) override;

        /**
         * @brief          更新指定位置的子项
         * @param index    要更新子项的位置
         * @param newValue 子项的新值
         * @return         操作是否成功
         */
        virtual bool UpdateItem(int index, const StrList &newValue) override;

        /**
         * @brief       移除指定索引处的子项
         * @param index 要移除子项的索引
         * @return      操作是否成功
         */
        virtual bool RemoveItemAt(int index) override;

        /**
         * @brief     获取指定位置处文本
         * @param row 所在行
         * @param col 所在列
         * @return    对应位置的文本，若获取失败则返回空字符串
         */
        virtual std::wstring GetItemAt(int row, int col);

        /**
         * @brief          更新指定位置处文本
         * @param row      所在行
         * @param col      所在列
         * @param newValue 要设置的文本
         * @return         操作是否成功
         */
        virtual bool UpdateItem(int row, int col, const std::wstring &newValue);

        /**
         * @brief        添加新的列
         * @param column 要添加的列信息
         * @return       操作是否成功
         */
        bool AddColumn(const ListViewColumn &column);

        /**
         * @brief        添加新的列
         * @param header 要添加列的标题
         * @return       操作是否成功
         */
        bool AddColumn(const std::wstring &header);

        /**
         * @brief        添加新的列到指定索引
         * @param index  要插入的位置
         * @param column 要添加的列信息
         * @return       操作是否成功
         */
        bool InsertColumn(int index, const ListViewColumn &column);

        /**
         * @brief        添加新的列到指定索引
         * @param index  要插入的位置
         * @param header 要添加列的标题
         * @return       操作是否成功
         */
        bool InsertColumn(int index, const std::wstring &header);

        /**
         * @brief       设置指定列的标题
         * @param index 列的索引
         * @return      操作是否成功
         */
        bool SetColumnHeader(int index, const std::wstring &header);

        /**
         * @brief       获取指定列的宽度
         * @param index 列的索引
         * @return      列的宽度，若列不存在则返回-1
         */
        double GetColumnWidth(int index);

        /**
         * @brief       设置指定列的宽度
         * @param index 列的索引
         * @param width 要设置的宽度
         * @return      操作是否成功
         */
        bool SetColumnWidth(int index, double width);

        /**
         * @brief       移除指定列
         * @param index 列的索引
         * @return      操作是否成功
         */
        bool RemoveColumnAt(int index);

        /**
         * @brief 获取所有选中项的索引
         */
        List<int> GetSelectedIndices();

        /**
         * @brief 获取所有复选框选中的项的索引
         */
        List<int> GetCheckedIndices();

        /**
         * @brief 获取指定索引项的复选框是否选中
         */
        bool GetItemCheckState(int index);

        /**
         * @brief 设置指定索引项复选框的选中状态
         */
        void SetItemCheckState(int index, bool value);

        /**
         * @brief       获取指定点处子项的索引
         * @param point 相对于用户区左上角点的位置
         */
        int GetItemIndexFromPoint(const Point &point);

        /**
         * @brief           获取列表视图的图像列表
         * @param imageList 要获取的图像列表类型
         */
        ImageList GetImageList(ListViewImageList imageList);

        /**
         * @brief           设置列表视图的图像列表
         * @param imageList 要设置的图像列表类型
         * @param value     要设置的图像列表的句柄
         * @return          若函数成功则返回之前与控件关联的图像列表，否则返回NULL
         */
        HIMAGELIST SetImageList(ListViewImageList imageList, HIMAGELIST value);

        /**
         * @brief          设置指定子项的图像
         * @param index    子项的索引
         * @param imgIndex 图像在图像列表中的索引
         * @return         操作是否成功
         */
        bool SetItemImage(int index, int imgIndex);

        /**
         * @brief       进入编辑模式，调用该函数前需要将Editable属性设为true
         * @param index 编辑项的索引
         * @return      操作是否成功
         */
        bool EditItem(int index);

        /**
         * @brief 取消编辑
         */
        void CancelEdit();

    private:
        /**
         * @brief 获取行数
         */
        int _GetRowCount();

        /**
         * @brief 获取列数
         */
        int _GetColCount();

        /**
         * @brief 获取ListView扩展样式
         */
        DWORD _GetExtendedListViewStyle();

        /**
         * @brief  设置ListView扩展样式
         * @return 先前的样式
         */
        DWORD _SetExtendedListViewStyle(DWORD style);
    };
};

// CheckableButton.h


namespace sw
{
    /**
     * @brief 选中状态枚举类型
     */
    enum class CheckState {
        Unchecked     = BST_UNCHECKED,     // 未选中
        Checked       = BST_CHECKED,       // 已选中
        Indeterminate = BST_INDETERMINATE, // 不确定状态
    };

    /**
     * @brief 表示可选中的按钮类型（单选框、复选框等）
     */
    class CheckableButton : public ButtonBase
    {
    public:
        /**
         * @brief 选中状态
         */
        const Property<sw::CheckState> CheckState;

        /**
         * @brief 是否选中
         */
        const Property<bool> IsChecked;

    protected:
        /**
         * @brief 初始化CheckableButton
         */
        CheckableButton();

    public:
        /**
         * @brief 析构函数，这里用纯虚函数使该类成为抽象类
         */
        virtual ~CheckableButton() = 0;
    };
}

// PasswordBox.h


namespace sw
{
    /**
     * @brief 密码框
     */
    class PasswordBox : public TextBoxBase
    {
    public:
        /**
         * @brief 密码框显示的字符
         */
        const Property<wchar_t> PasswordChar;

    public:
        /**
         * @brief 初始化密码框
         */
        PasswordBox();
    };
}

// Panel.h


namespace sw
{
    /**
     * @brief 边框类型
     */
    enum class BorderStyle {
        None   = 0,           // 无边框
        Bump   = EDGE_BUMP,   // 突出的凸起边框
        Etched = EDGE_ETCHED, // 刻痕式边框
        Raised = EDGE_RAISED, // 凸起边框
        Sunked = EDGE_SUNKEN, // 凹陷边框
    };

    /**
     * @brief 面板
     */
    class Panel : public PanelBase
    {
    private:
        /**
         * @brief 边框类型，默认为无边框
         */
        BorderStyle _borderStyle = sw::BorderStyle::None;

    public:
        /**
         * @brief 边框样式
         */
        const Property<sw::BorderStyle> BorderStyle;

    public:
        /**
         * @brief 初始化面板
         */
        Panel();

    protected:
        /**
         * @brief  接收到WM_PAINT时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnPaint() override;

        /**
         * @brief               接收到WM_SIZE时调用该函数
         * @param newClientSize 改变后的用户区尺寸
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSize(Size newClientSize) override;
    };
}

// Label.h


namespace sw
{
    /**
     * @brief 文本过长时末尾的处理方式
     */
    enum class TextTrimming {
        None,         // 不处理
        EndEllipsis,  // 按字符截断并显示“...”
        WordEllipsis, // 按单词截断并显示“...”
    };

    /**
     * @brief 标签
     */
    class Label : public StaticControl
    {
    private:
        /**
         * @brief 文本所需的尺寸
         */
        Size _textSize;

        /**
         * @brief 是否根据文本自动调整尺寸
         */
        bool _autoSize = true;

    public:
        /**
         * @brief 文本的水平对齐方式，可设为左对齐、中心对齐、右对齐
         */
        const Property<sw::HorizontalAlignment> HorizontalContentAlignment;

        /**
         * @brief 文本的垂直对齐方式，仅支持顶部对齐和中心对齐，当使用中心对齐时会使自动换行失效
         */
        const Property<sw::VerticalAlignment> VerticalContentAlignment;

        /**
         * @brief 文本过长时末尾的处理方式，当使用截断时水平对齐和自动换行将失效
         */
        const Property<sw::TextTrimming> TextTrimming;

        /**
         * @brief 是否自动换行
         */
        const Property<bool> AutoWrap;

        /**
         * @brief 是否根据文本自动调整尺寸
         */
        const Property<bool> AutoSize;

    public:
        /**
         * @brief 初始化标签
         */
        Label();

    private:
        /**
         * @brief 更新_textSize
         */
        void _UpdateTextSize();

        /**
         * @brief 调整尺寸为_textSize
         */
        void _ResizeToTextSize();

    protected:
        /**
         * @brief               接收到WM_SIZE时调用该函数
         * @param newClientSize 改变后的用户区尺寸
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSize(Size newClientSize) override;

        /**
         * @brief Text属性更改时调用此函数
         */
        virtual void OnTextChanged() override;

        /**
         * @brief       字体改变时调用该函数
         * @param hfont 字体句柄
         */
        virtual void FontChanged(HFONT hfont) override;

        /**
         * @brief               测量控件所需尺寸
         * @param availableSize 可用的尺寸
         */
        virtual void Measure(const Size &availableSize) override;
    };
}

// FolderDialog.h


namespace sw
{
    /**
     * @brief https://learn.microsoft.com/en-us/windows/win32/api/shlobj_core/ns-shlobj_core-browseinfoa
     */
    enum class FolderDialogFlags : UINT {
        // Only return file system directories. If the user selects folders that are not part of the
        // file system, the OK button is grayed.
        // Note  The OK button remains enabled for "\\server" items, as well as "\\server\share" and
        // directory items. However, if the user selects a "\\server" item, passing the PIDL returned
        // by SHBrowseForFolder to SHGetPathFromIDList fails.
        ReturnOnlyFileSystemDirs = 0x00000001,

        // Do not include network folders below the domain level in the dialog box's tree view control.
        DontGoBelowDomain = 0x00000002,

        // Include a status area in the dialog box. The callback function can set the status text by
        // sending messages to the dialog box. This flag is not supported when BIF_NEWDIALOGSTYLE is
        // specified.
        StatusText = 0x00000004,

        // Only return file system ancestors. An ancestor is a subfolder that is beneath the root folder
        // in the namespace hierarchy. If the user selects an ancestor of the root folder that is not
        // part of the file system, the OK button is grayed.
        ReturnFileSystemAncestors = 0x00000008,

        // Version 4.71. Include an edit control in the browse dialog box that allows the user to type
        // the name of an item.
        EditBox = 0x00000010,

        // Version 4.71. If the user types an invalid name into the edit box, the browse dialog box calls
        // the application's BrowseCallbackProc with the BFFM_VALIDATEFAILED message. This flag is ignored
        // if BIF_EDITBOX is not specified.
        Validate = 0x00000020,

        // Version 5.0. Use the new user interface. Setting this flag provides the user with a larger
        // dialog box that can be resized. The dialog box has several new capabilities, including:
        // drag-and-drop capability within the dialog box, reordering, shortcut menus, new folders,
        // delete, and other shortcut menu commands.
        // Note  If COM is initialized through CoInitializeEx with the COINIT_MULTITHREADED flag set,
        // SHBrowseForFolder fails if BIF_NEWDIALOGSTYLE is passed.
        NewDialogStyle = 0x00000040,

        // Version 5.0. The browse dialog box can display URLs. The BIF_USENEWUI and BIF_BROWSEINCLUDEFILES
        // flags must also be set. If any of these three flags are not set, the browser dialog box rejects
        // URLs. Even when these flags are set, the browse dialog box displays URLs only if the folder that
        // contains the selected item supports URLs. When the folder's IShellFolder::GetAttributesOf method
        // is called to request the selected item's attributes, the folder must set the SFGAO_FOLDER attribute
        // flag. Otherwise, the browse dialog box will not display the URL.
        BrowseIncludeUrls = 0x00000080,

        // Version 5.0. Use the new user interface, including an edit box. This flag is equivalent to
        // BIF_EDITBOX | BIF_NEWDIALOGSTYLE.
        // Note  If COM is initialized through CoInitializeEx with the COINIT_MULTITHREADED flag set,
        // SHBrowseForFolder fails if BIF_USENEWUI is passed.
        UseNewUI = 0x00000010 | 0x00000040,

        // Version 6.0. When combined with BIF_NEWDIALOGSTYLE, adds a usage hint to the dialog box, in place
        // of the edit box. BIF_EDITBOX overrides this flag.
        UsageHint = 0x00000100,

        // Version 6.0. Do not include the New Folder button in the browse dialog box.
        NoNewFolderButton = 0x00000200,

        // Version 6.0. When the selected item is a shortcut, return the PIDL of the shortcut itself rather
        // than its target.
        NoTranslateTargets = 0x00000400,

        // Only return computers. If the user selects anything other than a computer, the OK button is grayed.
        BrowseForComputer = 0x00001000,

        // Only allow the selection of printers. If the user selects anything other than a printer, the OK
        // button is grayed.
        // In Windows XP and later systems, the best practice is to use a Windows XP-style dialog, setting
        // the root of the dialog to the Printers and Faxes folder (CSIDL_PRINTERS).
        BrowseForPrinter = 0x00002000,

        // Version 4.71. The browse dialog box displays files as well as folders.
        BrowseIncludeFiles = 0x00004000,

        // Version 5.0. The browse dialog box can display sharable resources on remote systems. This is intended
        // for applications that want to expose remote shares on a local system. The BIF_NEWDIALOGSTYLE flag must
        // also be set.
        Sharable = 0x00008000,

        // Windows 7 and later. Allow folder junctions such as a library or a compressed file with a .zip file
        // name extension to be browsed.
        BrowseFileJunctions = 0x00010000,
    };

    /**
     * @brief 标记FolderDialogFlags枚举支持位运算
     */
    _SW_ENUM_ENABLE_BIT_OPERATIONS(FolderDialogFlags);

    /**
     * @brief 选择文件夹对话框
     */
    class FolderBrowserDialog
    {
    private:
        /**
         * @brief BROWSEINFOW结构体
         */
        BROWSEINFOW _bi{};

        /**
         * @brief 储存文件名的缓冲区
         */
        std::vector<wchar_t> _buffer;

        /**
         * @brief 对话框上方显示的描述性文本
         */
        std::wstring _description;

    public:
        /**
         * @brief 储存文件名的缓冲区大小，值不能小于MAX_PATH
         */
        const Property<int> BufferSize;

        /**
         * @brief 对话框标志
         */
        const Property<FolderDialogFlags> Flags;

        /**
         * @brief 对话框上方显示的描述性文本
         */
        const Property<std::wstring> Description;

        /**
         * @brief 选中文件夹的路径
         */
        const ReadOnlyProperty<std::wstring> SelectedPath;

        /**
         * @brief 是否显示“新建文件夹”按钮
         */
        const Property<bool> NewFolderButton;

    public:
        /**
         * @brief 初始化FolderBrowserDialog
         */
        FolderBrowserDialog();

        /**
         * @brief  显示对话框，并指定当前活动窗口作为所有者窗口
         * @return 用户是否选择了文件夹
         */
        bool ShowDialog();

        /**
         * @brief  显示对话框，并指定所有者窗口
         * @return 用户是否选择了文件夹
         */
        bool ShowDialog(const Window &owner);

        /**
         * @brief  显示对话框，并指定所有者窗口
         * @return 用户是否选择了文件夹
         */
        bool ShowDialog(const Window *owner);

    protected:
        /**
         * @brief 获取BROWSEINFOW指针
         */
        BROWSEINFOW *GetBI();

        /**
         * @brief 获取指向缓冲区的指针
         */
        wchar_t *GetBuffer();

        /**
         * @brief 清空缓冲区
         */
        void ClearBuffer();
    };
}

// IconBox.h


namespace sw
{
    /**
     * @brief 用于显示一个图标的控件
     */
    class IconBox : public StaticControl
    {
    private:
        /**
         * @brief 图标句柄
         */
        HICON _hIcon{NULL};

    public:
        /**
         * @brief 当前控件显示的图标句柄，使用Load函数可以加载图标
         */
        const ReadOnlyProperty<HICON> IconHandle;

        /**
         * @brief 是否拉伸图标以填充整个控件，值为true时成功调用Load函数会自动调整尺寸为图标尺寸，反之则不会调整尺寸
         */
        const Property<bool> StretchIcon;

    public:
        /**
         * @brief 初始化IconBox
         */
        IconBox();

        /**
         * @brief       加载图标，该函数会复制一个图标句柄作为显示的图标
         * @param hIcon 要加载的图标
         * @return      加载到IconBox的图标句柄，若加载失败则返回NULL，该资源由IconBox内部管理，在加载新图标或控件销毁时会自动释放
         */
        HICON Load(HICON hIcon);

        /**
         * @brief      加载系统标准图标
         * @param icon 图标样式
         * @return     加载到IconBox的图标句柄，若加载失败则返回NULL，该资源由IconBox内部管理，在加载新图标或控件销毁时会自动释放
         */
        HICON Load(StandardIcon icon);

        /**
         * @brief            从指定模块中加载图标
         * @param hInstance  DLL或EXE的模块句柄
         * @param resourceId 图标的资源序号
         * @return           加载到IconBox的图标句柄，若加载失败则返回NULL，该资源由IconBox内部管理，在加载新图标或控件销毁时会自动释放
         */
        HICON Load(HINSTANCE hInstance, int resourceId);

        /**
         * @brief          从文件加载图标
         * @param fileName 图标文件的路径
         * @return         加载到IconBox的图标句柄，若加载失败则返回NULL，该资源由IconBox内部管理，在加载新图标或控件销毁时会自动释放
         */
        HICON Load(const std::wstring &fileName);

        /**
         * @brief 清除加载的图标
         */
        void Clear();

        /**
         * @brief 调整控件尺寸为图标尺寸，未加载图标时该函数不生效
         */
        void SizeToIcon();

    protected:
        /**
         * @brief  接收到WM_DESTROY时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnDestroy() override;

    private:
        /**
         * @brief       设置图标
         * @param hIcon 图标句柄
         */
        void _SetIcon(HICON hIcon);

        /**
         * @brief       传入的图标不为NULL时调用_SetIcon
         * @param hIcon 图标句柄
         * @return      传入的图标
         */
        HICON _SetIconIfNotNull(HICON hIcon);
    };
}

// BmpBox.h


namespace sw
{
    /**
     * @brief 指定位图在BmpBox中的定位方式
     */
    enum class BmpBoxSizeMode {
        Normal,       // 位图置于BmpBox左上角
        StretchImage, // 拉伸位图填充BmpBox
        AutoSize,     // 调整BmpBox大小，使其等于所包含位图的大小
        CenterImage,  // 将位图居中显示
        Zoom,         // 图像按照原来的比例缩放
    };

    /**
     * @brief 用于显示一个位图的控件
     */
    class BmpBox : public StaticControl
    {
    private:
        /**
         * @brief 位图句柄
         */
        HBITMAP _hBitmap{NULL};

        /**
         * @brief 位图尺寸（以像素为单位），调用_UpdateBmpSize更新
         */
        SIZE _bmpSize{0, 0};

        /**
         * @brief 位图定位方式
         */
        BmpBoxSizeMode _sizeMode{BmpBoxSizeMode::Normal};

    public:
        /**
         * @brief 当前控件显示的位图句柄，使用Load函数可以加载位图
         */
        const ReadOnlyProperty<HBITMAP> BmpHandle;

        /**
         * @brief 位图在BmpBox中的定位方式
         */
        const Property<BmpBoxSizeMode> SizeMode;

    public:
        /**
         * @brief 初始化BmpBox
         */
        BmpBox();

        /**
         * @brief         加载位图，该函数会复制一个位图句柄作为显示的位图
         * @param hBitmap 要加载的位图
         * @return        加载到BmpBox的位图句柄，若加载失败则返回NULL，该资源由BmpBox内部管理，在加载新位图或控件销毁时会自动释放
         */
        HBITMAP Load(HBITMAP hBitmap);

        /**
         * @brief            从指定模块中加载位图
         * @param hInstance  DLL或EXE的模块句柄
         * @param resourceId 位图的资源序号
         * @return           加载到BmpBox的位图句柄，若加载失败则返回NULL，该资源由BmpBox内部管理，在加载新位图或控件销毁时会自动释放
         */
        HBITMAP Load(HINSTANCE hInstance, int resourceId);

        /**
         * @brief          从文件加载位图
         * @param fileName 位图文件的路径
         * @return         加载到BmpBox的位图句柄，若加载失败则返回NULL，该资源由BmpBox内部管理，在加载新位图或控件销毁时会自动释放
         */
        HBITMAP Load(const std::wstring &fileName);

        /**
         * @brief 清除加载的位图
         */
        void Clear();

        /**
         * @brief 调整控件尺寸为位图尺寸，未加载位图时该函数不生效
         */
        void SizeToImage();

    protected:
        /**
         * @brief  接收到WM_DESTROY时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnDestroy() override;

        /**
         * @brief  接收到WM_PAINT时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnPaint() override;

        /**
         * @brief               接收到WM_SIZE时调用该函数
         * @param newClientSize 改变后的用户区尺寸
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSize(Size newClientSize) override;

        /**
         * @brief               测量控件所需尺寸
         * @param availableSize 可用的尺寸
         */
        virtual void Measure(const Size &availableSize) override;

    private:
        /**
         * @brief 更新_bmpSize
         */
        void _UpdateBmpSize();

        /**
         * @brief         设置位图
         * @param hBitmap 位图句柄
         */
        void _SetBmp(HBITMAP hBitmap);

        /**
         * @brief         传入的位图不为NULL时调用_SetBmp
         * @param hBitmap 位图句柄
         * @return        传入的位图
         */
        HBITMAP _SetBmpIfNotNull(HBITMAP hBitmap);
    };
}

// Splitter.h


namespace sw
{
    /**
     * @brief 分隔条
     */
    class Splitter : public StaticControl
    {
    private:
        /**
         * @brief 记录分隔条方向
         */
        Orientation _orientation = Orientation::Horizontal;

    public:
        /**
         * @brief 分隔条的方向，给该属性赋值同时会改变HorizontalAlignment和VerticalAlignment属性的值
         */
        const Property<Orientation> Orientation;

        /**
         * @brief 初始化分隔条
         */
        Splitter();

    protected:
        /**
         * @brief  接收到WM_PAINT时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnPaint() override;

        /**
         * @brief               接收到WM_SIZE时调用该函数
         * @param newClientSize 改变后的用户区尺寸
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSize(Size newClientSize) override;
    };
}

// ListBox.h


namespace sw
{
    /**
     * @brief 列表框
     */
    class ListBox : public StrItemsControl
    {
    public:
        /**
         * @brief 当前列表框页面第一个子项的索引
         */
        const Property<int> TopIndex;

        /**
         * @brief 是否允许多选，更新该属性会导致已添加的子项被清空
         */
        const Property<bool> MultiSelect;

        /**
         * @brief 多选状态下可通过该属性获取选中项的个数
         */
        const ReadOnlyProperty<int> SelectedCount;

    public:
        /**
         * @brief 初始化列表框
         */
        ListBox();

    protected:
        /**
         * @brief  获取子项数
         */
        virtual int GetItemsCount() override;

        /**
         * @brief 选中项的索引，当无选中项时为-1
         */
        virtual int GetSelectedIndex() override;

        /**
         * @brief 设置选中项索引
         */
        virtual void SetSelectedIndex(int index) override;

        /**
         * @brief 获取选中项
         */
        virtual std::wstring GetSelectedItem() override;

        /**
         * @brief               接收到WM_CONTEXTMENU后调用目标控件的该函数
         * @param isKeyboardMsg 消息是否由按下快捷键（Shift+F10、VK_APPS）产生
         * @param mousePosition 鼠标在屏幕中的位置
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnContextMenu(bool isKeyboardMsg, Point mousePosition) override;

        /**
         * @brief      当父窗口接收到控件的WM_COMMAND时调用该函数
         * @param code 通知代码
         */
        virtual void OnCommand(int code) override;

        /**
         * @brief     绘制虚线框时调用该函数
         * @param hdc 绘制设备句柄
         */
        virtual void OnDrawFocusRect(HDC hdc) override;

    public:
        /**
         * @brief 清空所有子项
         */
        virtual void Clear() override;

        /**
         * @brief       获取指定索引处子项的值
         * @param index 子项的索引
         */
        virtual std::wstring GetItemAt(int index) override;

        /**
         * @brief      添加新的子项
         * @param item 要添加的子项
         * @return     是否添加成功
         */
        virtual bool AddItem(const std::wstring &item) override;

        /**
         * @brief       添加子项到指定索引
         * @param index 要插入的位置
         * @param item  要添加的子项
         * @return      是否添加成功
         */
        virtual bool InsertItem(int index, const std::wstring &item) override;

        /**
         * @brief          更新指定位置的子项
         * @param index    要更新子项的位置
         * @param newValue 子项的新值
         * @return         操作是否成功
         */
        virtual bool UpdateItem(int index, const std::wstring &newValue) override;

        /**
         * @brief       移除指定索引处的子项
         * @param index 要移除子项的索引
         * @return      操作是否成功
         */
        virtual bool RemoveItemAt(int index) override;

        /**
         * @brief       获取指定点处子项的索引
         * @param point 相对于列表框用户区左上角点的位置
         */
        int GetItemIndexFromPoint(const Point &point);

        /**
         * @brief  多选状态下可通过该函数获取所有选中项的索引
         * @return 所有选中项的索引
         */
        List<int> GetSelectedIndices();

        /**
         * @brief  多选状态下可通过该函数获取所有选中项的内容
         * @return 所有选中项的内容
         */
        StrList GetSelectedItems();

        /**
         * @brief       获取指定索引处子项的选中状态
         * @param index 子项的索引
         * @return      若子项选中则返回true，否则返回false
         */
        bool GetItemSelectionState(int index);

        /**
         * @brief       多选状态下设置指定索引处子项的选中状态
         * @param index 子项的索引，输入-1可设置所有子项的选中状态
         * @param value 要设置的子项状态
         */
        void SetItemSelectionState(int index, bool value);
    };
}

// ComboBox.h


namespace sw
{
    /**
     * @brief 组合框
     */
    class ComboBox : public StrItemsControl
    {
    private:
        /**
         * @brief 在读取Text属性时用于判断是否需要更新储存的文本
         */
        bool _isTextChanged = false;

    public:
        /**
         * @brief 组合框内容是否可编辑，更新该属性会导致已添加的子项被清空
         */
        const Property<bool> IsEditable;

    public:
        /**
         * @brief 初始化组合框
         */
        ComboBox();

    protected:
        /**
         * @brief  获取子项数
         */
        virtual int GetItemsCount() override;

        /**
         * @brief 选中项的索引，当无选中项时为-1
         */
        virtual int GetSelectedIndex() override;

        /**
         * @brief 设置选中项索引
         */
        virtual void SetSelectedIndex(int index) override;

        /**
         * @brief 获取选中项
         */
        virtual std::wstring GetSelectedItem() override;

        /**
         * @brief  获取可编辑状态下的编辑框文本内容
         */
        virtual std::wstring &GetText() override;

        /**
         * @brief       设置Text属性时调用该函数
         * @param value 要设置的文本
         */
        virtual void SetText(const std::wstring &value) override;

        /**
         * @brief      当父窗口接收到控件的WM_COMMAND时调用该函数
         * @param code 通知代码
         */
        virtual void OnCommand(int code) override;

        /**
         * @brief 选中项改变时调用该函数
         */
        virtual void OnSelectionChanged() override;

    public:
        /**
         * @brief 清空所有子项
         */
        virtual void Clear() override;

        /**
         * @brief       获取指定索引处子项的值
         * @param index 子项的索引
         */
        virtual std::wstring GetItemAt(int index) override;

        /**
         * @brief      添加新的子项
         * @param item 要添加的子项
         * @return     是否添加成功
         */
        virtual bool AddItem(const std::wstring &item) override;

        /**
         * @brief       添加子项到指定索引
         * @param index 要插入的位置
         * @param item  要添加的子项
         * @return      是否添加成功
         */
        virtual bool InsertItem(int index, const std::wstring &item) override;

        /**
         * @brief          更新指定位置的子项
         * @param index    要更新子项的位置
         * @param newValue 子项的新值
         * @return         操作是否成功
         */
        virtual bool UpdateItem(int index, const std::wstring &newValue) override;

        /**
         * @brief       移除指定索引处的子项
         * @param index 要移除子项的索引
         * @return      操作是否成功
         */
        virtual bool RemoveItemAt(int index) override;

        /**
         * @brief 显示下拉列表
         */
        void ShowDropDown();

        /**
         * @brief 关闭下拉列表
         */
        void CloseDropDown();
    };
}

// FileDialog.h


namespace sw
{
    /**
     * @brief https://learn.microsoft.com/en-us/windows/win32/api/commdlg/ns-commdlg-openfilenamew
     */
    enum class FileDialogFlags : DWORD {
        // The File Name list box allows multiple selections. If you also set the OFN_EXPLORER flag,
        // the dialog box uses the Explorer-style user interface; otherwise, it uses the old-style
        // user interface.
        // If the user selects more than one file, the lpstrFile buffer returns the path to the
        // current directory followed by the file names of the selected files. The nFileOffset member
        // is the offset, in bytes or characters, to the first file name, and the nFileExtension
        // member is not used. For Explorer-style dialog boxes, the directory and file name strings
        // are NULL separated, with an extra NULL character after the last file name. This format
        // enables the Explorer-style dialog boxes to return long file names that include spaces.
        // For old-style dialog boxes, the directory and file name strings are separated by spaces
        // and the function uses short file names for file names with spaces. You can use the
        // FindFirstFile function to convert between long and short file names.
        // If you specify a custom template for an old-style dialog box, the definition of the File
        // Name list box must contain the LBS_EXTENDEDSEL value.
        AllowMultiSelect = 0x00000200,

        // If the user specifies a file that does not exist, this flag causes the dialog box to
        // prompt the user for permission to create the file. If the user chooses to create the
        // file, the dialog box closes and the function returns the specified name; otherwise,
        // the dialog box remains open. If you use this flag with the OFN_ALLOWMULTISELECT flag,
        // the dialog box allows the user to specify only one nonexistent file.
        CreatePrompt = 0x00002000,

        // Prevents the system from adding a link to the selected file in the file system directory
        // that contains the user's most recently used documents. To retrieve the location of this
        // directory, call the SHGetSpecialFolderLocation function with the CSIDL_RECENT flag.
        DontAddTorecent = 0x02000000,

        // Enables the hook function specified in the lpfnHook member.
        EnableHook = 0x00000020,

        // Causes the dialog box to send CDN_INCLUDEITEM notification messages to your OFNHookProc hook
        // procedure when the user opens a folder. The dialog box sends a notification for each item in
        // the newly opened folder. These messages enable you to control which items the dialog box
        // displays in the folder's item list.
        EnableIncludeNotify = 0x00400000,

        // Enables the Explorer-style dialog box to be resized using either the mouse or the keyboard.
        // By default, the Explorer-style Open and Save As dialog boxes allow the dialog box to be resized
        // regardless of whether this flag is set. This flag is necessary only if you provide a hook
        // procedure or custom template. The old-style dialog box does not permit resizing.
        EnableSizing = 0x00800000,

        // The lpTemplateName member is a pointer to the name of a dialog template resource in the module
        // identified by the hInstance member. If the OFN_EXPLORER flag is set, the system uses the specified
        // template to create a dialog box that is a child of the default Explorer-style dialog box. If the
        // OFN_EXPLORER flag is not set, the system uses the template to create an old-style dialog box that
        // replaces the default dialog box.
        EnableTemplate = 0x00000040,

        // The hInstance member identifies a data block that contains a preloaded dialog box template.
        // The system ignores lpTemplateName if this flag is specified. If the OFN_EXPLORER flag is set,
        // the system uses the specified template to create a dialog box that is a child of the default
        // Explorer-style dialog box. If the OFN_EXPLORER flag is not set, the system uses the template to
        // create an old-style dialog box that replaces the default dialog box.
        EnableTemplateHandle = 0x00000080,

        // Indicates that any customizations made to the Open or Save As dialog box use the Explorer-style
        // customization methods. For more information, see Explorer-Style Hook Procedures and Explorer-Style
        // Custom Templates.
        // By default, the Open and Save As dialog boxes use the Explorer-style user interface regardless of
        // whether this flag is set. This flag is necessary only if you provide a hook procedure or custom template,
        // or set the OFN_ALLOWMULTISELECT flag.
        // If you want the old-style user interface, omit the OFN_EXPLORER flag and provide a replacement old-style
        // template or hook procedure. If you want the old style but do not need a custom template or hook procedure,
        // simply provide a hook procedure that always returns FALSE.
        Explorer = 0x00080000,

        // The user typed a file name extension that differs from the extension specified by lpstrDefExt.
        // The function does not use this flag if lpstrDefExt is NULL.
        ExtensionDifferent = 0x00000400,

        // The user can type only names of existing files in the File Name entry field. If this flag is specified and
        // the user enters an invalid name, the dialog box procedure displays a warning in a message box. If this flag
        // is specified, the OFN_PATHMUSTEXIST flag is also used. This flag can be used in an Open dialog box. It cannot
        // be used with a Save As dialog box.
        FileMustExist = 0x00001000,

        // Forces the showing of system and hidden files, thus overriding the user setting to show or not show hidden
        // files. However, a file that is marked both system and hidden is not shown.
        ForceShowHidden = 0x10000000,

        // Hides the Read Only check box.
        HideReadOnly = 0x00000004,

        // For old-style dialog boxes, this flag causes the dialog box to use long file names. If this flag is not
        // specified, or if the OFN_ALLOWMULTISELECT flag is also set, old-style dialog boxes use short file names
        // (8.3 format) for file names with spaces. Explorer-style dialog boxes ignore this flag and always display
        // long file names.
        LongNames = 0x00200000,

        // Restores the current directory to its original value if the user changed the directory while searching for files.
        // This flag is ineffective for GetOpenFileName.
        NoChangeDir = 0x00000008,

        // Directs the dialog box to return the path and file name of the selected shortcut (.LNK) file. If this value
        // is not specified, the dialog box returns the path and file name of the file referenced by the shortcut.
        NoDereferenceLinks = 0x00100000,

        // For old-style dialog boxes, this flag causes the dialog box to use short file names (8.3 format). Explorer-style
        // dialog boxes ignore this flag and always display long file names.
        NoLongNames = 0x00040000,

        // Hides and disables the Network button.
        NoNetworkButton = 0x00020000,

        // The returned file does not have the Read Only check box selected and is not in a write-protected directory.
        NoReadOnlyReturn = 0x00008000,

        // The file is not created before the dialog box is closed. This flag should be specified if the application saves
        // the file on a create-nonmodify network share. When an application specifies this flag, the library does not
        // check for write protection, a full disk, an open drive door, or network protection. Applications using this flag
        // must perform file operations carefully, because a file cannot be reopened once it is closed.
        NoTestFileCreate = 0x00010000,

        // The common dialog boxes allow invalid characters in the returned file name. Typically, the calling application
        // uses a hook procedure that checks the file name by using the FILEOKSTRING message. If the text box in the edit
        // control is empty or contains nothing but spaces, the lists of files and directories are updated. If the text box
        // in the edit control contains anything else, nFileOffset and nFileExtension are set to values generated by parsing
        // the text. No default extension is added to the text, nor is text copied to the buffer specified by lpstrFileTitle.
        // If the value specified by nFileOffset is less than zero, the file name is invalid. Otherwise, the file name is valid,
        // and nFileExtension and nFileOffset can be used as if the OFN_NOVALIDATE flag had not been specified.
        NoValidate = 0x00000100,

        // Causes the Save As dialog box to generate a message box if the selected file already exists. The user must confirm
        // whether to overwrite the file.
        OverwritePrompt = 0x00000002,

        // The user can type only valid paths and file names. If this flag is used and the user types an invalid path and
        // file name in the File Name entry field, the dialog box function displays a warning in a message box.
        PathMustExist = 0x00000800,

        // Causes the Read Only check box to be selected initially when the dialog box is created. This flag indicates the
        // state of the Read Only check box when the dialog box is closed.
        ReadOnly = 0x00000001,

        // Specifies that if a call to the OpenFile function fails because of a network sharing violation, the error is ignored
        // and the dialog box returns the selected file name. If this flag is not set, the dialog box notifies your hook procedure
        // when a network sharing violation occurs for the file name specified by the user. If you set the OFN_EXPLORER flag,
        // the dialog box sends the CDN_SHAREVIOLATION message to the hook procedure. If you do not set OFN_EXPLORER, the dialog
        // box sends the SHAREVISTRING registered message to the hook procedure.
        ShareAware = 0x00004000,

        // Causes the dialog box to display the Help button. The hwndOwner member must specify the window to receive the
        // HELPMSGSTRING registered messages that the dialog box sends when the user clicks the Help button. An Explorer-style
        // dialog box sends a CDN_HELP notification message to your hook procedure when the user clicks the Help button.
        ShowHelp = 0x00000010,
    };

    /**
     * @brief 标记FileDialogFlags枚举支持位运算
     */
    _SW_ENUM_ENABLE_BIT_OPERATIONS(FileDialogFlags);

    /**
     * @brief 文件筛选器信息
     */
    struct FileFilterItem {
        /**
         * @brief 文本
         */
        std::wstring name;

        /**
         * @brief 筛选器字符串，有多个类型时用分号分隔
         */
        std::wstring filter;

        /**
         * @brief 默认扩展名，当SaveFileDialog用户没有填写扩展名时会使用该值作为扩展名
         */
        std::wstring defaultExt;
    };

    /**
     * @brief 文件筛选器
     */
    class FileFilter
    {
    private:
        /**
         * @brief 缓冲区
         */
        std::vector<wchar_t> _buffer;

        /**
         * @brief 默认扩展名
         */
        std::vector<std::wstring> _defaultExts;

    public:
        /**
         * @brief 默认构造函数
         */
        FileFilter() = default;

        /**
         * @brief 初始话并设置筛选器
         */
        FileFilter(std::initializer_list<FileFilterItem> filters);

        /**
         * @brief        添加筛选器
         * @param name   名称，示例：All Files
         * @param filter 筛选器，示例：*.*
         * @return       是否成功添加
         */
        bool AddFilter(const std::wstring &name, const std::wstring &filter, const std::wstring &defaultExt = L"");

        /**
         * @brief         清空现有筛选器并重新设置筛选器
         * @param filters 筛选器列表
         * @return        成功添加的筛选器个数
         */
        int SetFilter(std::initializer_list<FileFilterItem> filters);

        /**
         * @brief 清空所有已添加的筛选器
         */
        void Clear();

        /**
         * @brief 获取OPENFILENAMEW结构体lpstrFilter格式的字符串
         */
        wchar_t *GetFilterStr();

        /**
         * @brief  获取指定索引处筛选器的默认扩展名
         */
        const wchar_t *GetDefaultExt(int index);
    };

    /**
     * @brief “打开文件”对话框与“另存为”对话框的基类
     */
    class FileDialog
    {
    private:
        /**
         * @brief OPENFILENAMEW结构体
         */
        OPENFILENAMEW _ofn{};

        /**
         * @brief 储存文件名的缓冲区
         */
        std::vector<wchar_t> _buffer;

        /**
         * @brief 对话框标题
         */
        std::wstring _title;

        /**
         * @brief 初始目录
         */
        std::wstring _initialDir;

        /**
         * @brief 筛选器
         */
        FileFilter _filter;

    public:
        /**
         * @brief 储存文件名的缓冲区大小，值不能小于MAX_PATH
         */
        const Property<int> BufferSize;

        /**
         * @brief 对话框标志
         */
        const Property<FileDialogFlags> Flags;

        /**
         * @brief 对话框标题，设为空字符串可显示默认标题
         */
        const Property<std::wstring> Title;

        /**
         * @brief 初始目录
         */
        const Property<std::wstring> InitialDir;

        /**
         * @brief 筛选器
         */
        const ReadOnlyProperty<FileFilter *> Filter;

        /**
         * @brief 当前筛选器的索引，索引值从0开始
         */
        const Property<int> FilterIndex;

        /**
         * @brief 选中文件的路径
         */
        const ReadOnlyProperty<std::wstring> FileName;

        /**
         * @brief 是否允许多选
         */
        const Property<bool> MultiSelect;

        /**
         * @brief 所有选中的文件路径
         */
        const ReadOnlyProperty<sw::List<std::wstring>> FileNames;

    public:
        /**
         * @brief 初始化FileDialog
         */
        FileDialog();

        /**
         * @brief 默认虚析构函数
         */
        virtual ~FileDialog() = default;

        /**
         * @brief        设置筛选器
         * @param filter 筛选器
         */
        void SetFilter(const FileFilter &filter);

        /**
         * @brief  显示对话框，并指定当前活动窗口作为所有者窗口
         * @return 用户是否选择了文件
         */
        bool ShowDialog();

        /**
         * @brief  显示对话框，并指定所有者窗口
         * @return 用户是否选择了文件
         */
        bool ShowDialog(const Window &owner);

        /**
         * @brief  显示对话框，并指定所有者窗口
         * @return 用户是否选择了文件
         */
        virtual bool ShowDialog(const Window *owner) = 0;

    protected:
        /**
         * @brief 获取OPENFILENAMEW指针
         */
        OPENFILENAMEW *GetOFN();

        /**
         * @brief 获取指向缓冲区的指针
         */
        wchar_t *GetBuffer();

        /**
         * @brief 清空缓冲区，显示对话框前必须调用此函数
         */
        void ClearBuffer();

        /**
         * @brief          处理文件路径，获取文件路径时会先调用这个函数对返回值进行处理
         * @param fileName 获取到的文件路径，可通过修改该值改变FileName和FileNames属性获取到的内容
         */
        virtual void ProcessFileName(std::wstring &fileName);
    };

    /**
     * @brief “打开文件”对话框
     */
    class OpenFileDialog : public FileDialog
    {
    public:
        /**
         * @brief 初始化OpenFileDialog
         */
        OpenFileDialog();

        /**
         * @brief 继承ShowDialog的重载
         */
        using FileDialog::ShowDialog;

        /**
         * @brief  显示对话框，并指定所有者窗口
         * @return 用户是否选择了文件
         */
        virtual bool ShowDialog(const Window *owner) override;
    };

    /**
     * @brief “另存为”对话框
     */
    class SaveFileDialog : public FileDialog
    {
    private:
        /**
         * @brief 初始文件名
         */
        std::wstring _initialFileName;

    public:
        /**
         * @brief 初始文件名
         */
        const Property<std::wstring> InitialFileName;

        /**
         * @brief 初始化SaveFileDialog
         */
        SaveFileDialog();

        /**
         * @brief 继承ShowDialog的重载
         */
        using FileDialog::ShowDialog;

        /**
         * @brief  显示对话框，并指定所有者窗口
         * @return 用户是否选择了文件
         */
        virtual bool ShowDialog(const Window *owner) override;

    protected:
        /**
         * @brief          处理文件路径，获取文件路径时会先调用这个函数对返回值进行处理
         * @param fileName 获取到的文件路径，可通过修改该值改变FileName和FileNames属性获取到的内容
         */
        virtual void ProcessFileName(std::wstring &fileName) override;

    private:
        /**
         * @brief 设置初始文件名到缓冲区
         */
        void _SetInitialFileName();
    };
}

// HwndHost.h


namespace sw
{
    /**
     * @brief 将Win32 window托管为SimpleWindow控件
     */
    class HwndHost : public StaticControl
    {
    private:
        /**
         * @brief 托管的句柄
         */
        HWND _hWindowCore{NULL};

        /**
         * @brief 是否自动填充托管的内容
         */
        bool _fillContent = true;

    public:
        /**
         * @brief 是否自动填充托管的内容
         */
        const Property<bool> FillContent;

        /**
         * @brief 创建HwndHost对象
         */
        HwndHost();

    protected:
        /**
         * @brief 子类需要调用该函数以初始化HwndHost，该函数会调用BuildWindowCore
         */
        void InitHwndHost();

        /**
         * @brief               接收到WM_SIZE时调用该函数
         * @param newClientSize 改变后的用户区尺寸
         * @return              若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSize(Size newClientSize) override;

        /**
         * @brief  接收到WM_DESTROY时调用该函数
         * @return 若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnDestroy() override;

    protected:
        /**
         * @brief         初始化HwndHost时会调用该函数，需在该函数中创建要被托管的窗口句柄，设置其父窗口并返回被托管的句柄
         * @param hParent 需要给被托管窗口句柄设置的父窗口句柄
         * @return        被托管窗口句柄
         */
        virtual HWND BuildWindowCore(HWND hParent) = 0;

        /**
         * @brief      HwndHost被销毁时会调用该函数来销毁被托管的窗口句柄
         * @param hwnd 被托管窗口句柄
         */
        virtual void DestroyWindowCore(HWND hwnd) = 0;
    };
}

// TextBox.h


namespace sw
{
    /**
     * @brief 编辑框
     */
    class TextBox : public TextBoxBase
    {
    private:
        /**
         * @brief 是否自动换行
         */
        bool _autoWrap = false;

    public:
        /**
         * @brief 是否自动换行，仅在MultiLine属性为true时有效
         */
        const Property<bool> AutoWrap;

        /**
         * @brief 是否允许多行文本
         */
        const Property<bool> MultiLine;

        /**
         * @brief 是否显示横向滚动条
         */
        const Property<bool> HorizontalScrollBar;

        /**
         * @brief 是否显示纵向滚动条
         */
        const Property<bool> VerticalScrollBar;

    public:
        /**
         * @brief 初始化编辑框
         */
        TextBox();
    };
}

// GroupBox.h


namespace sw
{
    /**
     * @brief 组合框
     */
    class GroupBox : public PanelBase
    {
    public:
        /**
         * @brief 初始化组合框
         */
        GroupBox();
    };
}

// CheckBox.h


namespace sw
{
    /**
     * @brief 复选框
     */
    class CheckBox : public CheckableButton
    {
    public:
        /**
         * @brief 是否为三态复选框
         */
        const Property<bool> ThreeState;

    public:
        /**
         * @brief 初始化复选框
         */
        CheckBox();
    };
}

// Canvas.h


namespace sw
{
    /**
     * @brief 一种可以为子元素设置绝对位置的面板，与普通Panel不同的是Canvas支持自动滚动条
     */
    class Canvas : public Panel
    {
    private:
        /**
         * @brief 默认布局对象
         */
        CanvasLayout _canvasLayout = CanvasLayout();

    public:
        /**
         * @brief 初始化Canvas
         */
        Canvas();

        /**
         * @brief 获取指定元素的布局标记
         */
        static CanvasLayoutTag GetCanvasLayoutTag(UIElement &element);

        /**
         * @brief 给指定元素设置布局标记
         */
        static void SetCanvasLayoutTag(UIElement &element, const CanvasLayoutTag &tag);

    protected:
        /**
         * @brief 获取默认布局对象
         */
        virtual LayoutHost *GetDefaultLayout() override;
    };
}

// UniformGrid.h


namespace sw
{
    /**
     * @brief 提供一种在网格（网格中的所有单元格都具有相同的大小）中排列内容的方法
     */
    class UniformGrid : public Panel
    {
    private:
        /**
         * @brief 默认布局对象
         */
        UniformGridLayout _uniformGridLayout = UniformGridLayout();

    public:
        /**
         * @brief 行数
         */
        const Property<int> Rows;

        /**
         * @brief 列数
         */
        const Property<int> Columns;

        /**
         * @brief 网格第一行中前导空白单元格的数量
         */
        const Property<int> FirstColumn;

    public:
        /**
         * @brief 初始化UniformGrid
         */
        UniformGrid();

    protected:
        /**
         * @brief 获取默认布局对象
         */
        virtual LayoutHost *GetDefaultLayout() override;
    };
}

// RadioButton.h


namespace sw
{
    /**
     * @brief 单选框
     */
    class RadioButton : public CheckableButton
    {
    public:
        /**
         * @brief 初始化单选框
         */
        RadioButton();
    };
}

// WrapPanel.h


namespace sw
{
    /**
     * @brief 自动换行面板
     */
    class WrapPanel : public Panel
    {
    private:
        /**
         * @brief 默认布局对象
         */
        WrapLayout _wrapLayout = WrapLayout();

    public:
        /**
         * @brief 排列方式
         */
        const Property<sw::Orientation> Orientation;

    public:
        /**
         * @brief 初始化WrapPanel
         */
        WrapPanel();

    protected:
        /**
         * @brief 获取默认布局对象
         */
        virtual LayoutHost *GetDefaultLayout() override;
    };
}

// IPAddressControl.h


namespace sw
{
    /**
     * @brief IP地址框
     */
    class IPAddressControl : public HwndHost
    {
    private:
        /**
         * @brief IP地址框的句柄
         */
        HWND _hIPAddrCtrl{NULL};

    public:
        /**
         * @brief 内容是否为空
         */
        const ReadOnlyProperty<bool> IsBlank;

        /**
         * @brief 当前内容所表示的IP地址
         */
        const Property<uint32_t> Address;

    public:
        /**
         * @brief 初始化IP地址框
         */
        IPAddressControl();

        /**
         * @brief 初始化IP地址框，并设置控件尺寸
         */
        explicit IPAddressControl(sw::Size size);

        /**
         * @brief 清空输入的内容
         */
        void Clear();

        /**
         * @brief       限制某个IP段的值范围
         * @param field 要限制的IP段的索引
         * @param min   最小值
         * @param max   最大值
         * @return      若函数成功则返回true，否则返回false
         */
        bool SetRange(int field, uint8_t min, uint8_t max);

    protected:
        /**
         * @brief         初始化HwndHost时会调用该函数，需在该函数中创建要被托管的窗口句柄，设置其父窗口并返回被托管的句柄
         * @param hParent 需要给被托管窗口句柄设置的父窗口句柄
         * @return        被托管窗口句柄
         */
        virtual HWND BuildWindowCore(HWND hParent) override;

        /**
         * @brief      HwndHost被销毁时会调用该函数来销毁被托管的窗口句柄
         * @param hwnd 被托管窗口句柄
         */
        virtual void DestroyWindowCore(HWND hwnd) override;

        /**
         * @brief       字体改变时调用该函数
         * @param hfont 字体句柄
         */
        virtual void FontChanged(HFONT hfont) override;

        /**
         * @brief            接收到WM_SETFOCUS时调用该函数
         * @param hPrevFocus 丢失焦点的hwnd，可能为NULL
         * @return           若已处理该消息则返回true，否则返回false以调用DefaultWndProc
         */
        virtual bool OnSetFocus(HWND hPrevFocus) override;

        /**
         * @brief        接收到WM_NOTIFY后调用该函数
         * @param pNMHDR 包含有关通知消息的信息
         * @param result 函数返回值为true时将该值作为消息的返回值，默认值为0
         * @return       若已处理该消息则返回true，否则调用发出通知控件的OnNotified函数，依据其返回值判断是否调用DefaultWndProc
         */
        virtual bool OnNotify(NMHDR *pNMHDR, LRESULT &result) override;

        /**
         * @brief 地址改变时调用该函数
         */
        virtual void OnAddressChanged();
    };
}

// Grid.h


namespace sw
{
    /**
     * @brief 由列和行组成的灵活的网格区域
     */
    class Grid : public Panel
    {
    private:
        /**
         * @brief 默认布局对象
         */
        GridLayout _gridLayout = GridLayout();

    public:
        /**
         * @brief 初始化Grid
         */
        Grid();

        /**
         * @brief 添加行
         */
        void AddRow(const GridRow &row);

        /**
         * @brief 设置行信息
         */
        void SetRows(std::initializer_list<GridRow> rows);

        /**
         * @brief 添加列
         */
        void AddColumn(const GridColumn &col);

        /**
         * @brief 设置列信息
         */
        void SetColumns(std::initializer_list<GridColumn> cols);

        /**
         * @brief 清空行
         */
        void ClearRows();

        /**
         * @brief 清空列
         */
        void ClearColumns();

        /**
         * @brief 获取指定元素的网格布局标记
         */
        static GridLayoutTag GetGridLayoutTag(UIElement &element);

        /**
         * @brief 给指定元素设置网格布局标记
         */
        static void SetGridLayoutTag(UIElement &element, const GridLayoutTag &tag);

    protected:
        /**
         * @brief 获取默认布局对象
         */
        virtual LayoutHost *GetDefaultLayout() override;
    };
}

// DockPanel.h


namespace sw
{
    /**
     * @brief 停靠面板
     */
    class DockPanel : public Panel
    {
    private:
        /**
         * @brief 默认布局对象
         */
        DockLayout _dockLayout = DockLayout();

    public:
        /**
         * @brief 最后一个子元素是否填充剩余空间
         */
        const Property<bool> LastChildFill;

    public:
        /**
         * @brief 初始化DockPanel
         */
        DockPanel();

        /**
         * @brief 获取指定元素的Dock
         */
        static DockLayoutTag GetDock(UIElement &element);

        /**
         * @brief 设置指定元素的Dock
         */
        static void SetDock(UIElement &element, DockLayoutTag dock);

    protected:
        /**
         * @brief 获取默认布局对象
         */
        virtual LayoutHost *GetDefaultLayout() override;
    };
}

// StackPanel.h


namespace sw
{
    /**
     * @brief 堆叠面板
     */
    class StackPanel : public Panel
    {
    private:
        /**
         * @brief 默认布局对象
         */
        StackLayout _stackLayout = StackLayout();

    public:
        /**
         * @brief 排列方式
         */
        const Property<sw::Orientation> Orientation;

    public:
        /**
         * @brief 初始化StackPanel
         */
        StackPanel();

    protected:
        /**
         * @brief 获取默认布局对象
         */
        virtual LayoutHost *GetDefaultLayout() override;
    };
}

// SimpleWindow.h

// 包含SimpleWindow所有头文件

// 启用视觉样式
#pragma comment(linker, "\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

// Comctl32
#pragma comment(lib, "comctl32.lib")
