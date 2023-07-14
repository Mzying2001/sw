#pragma once

#include <Windows.h>

namespace sw
{
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
}
