// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

module;
#include <Windows.h>
#include <gdiplus.h>

module wing.graphics;

import util;

namespace wing {

std::string GdiplusStatusStr(Gdiplus::Status status)
{
    switch (status)
    {
    case Gdiplus::Status::Ok: return "ok";
    case Gdiplus::Status::GenericError: return "generic error";
    case Gdiplus::Status::InvalidParameter: return "invalid parameter";
    case Gdiplus::Status::OutOfMemory: return "out of memory";
    case Gdiplus::Status::ObjectBusy: return "object busy";
    case Gdiplus::Status::InsufficientBuffer: return "insufficient buffer";
    case Gdiplus::Status::NotImplemented: return "not implemented";
    case Gdiplus::Status::Win32Error: return "WIN32 error";
    case Gdiplus::Status::WrongState: return "wrong state";
    case Gdiplus::Status::Aborted: return "aborted";
    case Gdiplus::Status::FileNotFound: return "file not found";
    case Gdiplus::Status::ValueOverflow: return "value overflow";
    case Gdiplus::Status::AccessDenied: return "access denied";
    case Gdiplus::Status::UnknownImageFormat: return "unknown image format";
    case Gdiplus::Status::FontFamilyNotFound: return "font family not found";
    case Gdiplus::Status::NotTrueTypeFont: return "not TrueType font";
    case Gdiplus::Status::UnsupportedGdiplusVersion: return "unsupported GDI+ version";
    case Gdiplus::Status::GdiplusNotInitialized: return "GDI+ not initialized";
    case Gdiplus::Status::PropertyNotFound: return "property not found";
    case Gdiplus::Status::PropertyNotSupported: return "property not supported";
    default: return "unknown GDI+ error";
    }
}

GdiPlusException::GdiPlusException(Gdiplus::Status status_) : std::runtime_error("GDI+ error: " + GdiplusStatusStr(status_)), status(status_)
{
}

void CheckGraphicsStatus(Gdiplus::Status status)
{
    if (status != Gdiplus::Status::Ok)
    {
        throw GdiPlusException(status);
    }
}

Font::Font(const FontFamily& family, float size, FontStyle style, Unit unit) : nativeFont(new Gdiplus::Font(&family, size, style, unit))
{
}

Font::~Font()
{
    if (nativeFont)
    {
        delete nativeFont;
    }
}

Gdiplus::Font* CloneFont(Gdiplus::Font* nativeFont)
{
    if (nativeFont)
    {
        return nativeFont->Clone();
    }
    return nullptr;
}

Font::Font(const Font& that) : nativeFont(CloneFont(that.nativeFont))
{
}

Font::Font(Font&& that) noexcept : nativeFont(that.nativeFont)
{
    that.nativeFont = nullptr;
}

Font& Font::operator=(const Font& that)
{
    if (nativeFont != that.nativeFont)
    {
        if (nativeFont != nullptr)
        {
            delete nativeFont;
        }
        nativeFont = CloneFont(that.nativeFont);
    }
    return *this;
}

Font& Font::operator=(Font&& that) noexcept
{
    std::swap(nativeFont, that.nativeFont);
    return *this;
}

FontHandle::FontHandle(HFONT handle_) : handle(handle_)
{
}

FontHandle::~FontHandle()
{
    if (handle)
    {
        DeleteObject(handle);
    }
}

FontHandle::FontHandle(FontHandle&& that) noexcept : handle(that.handle)
{
    that.handle = nullptr;
}

FontHandle& FontHandle::operator=(FontHandle&& that) noexcept
{
    std::swap(handle, that.handle);
    return *this;
}

FontHandle ToFontHandle(Graphics& graphics, const Font& font)
{
    LOGFONTW logFont;
    CheckGraphicsStatus(font.NativeFont()->GetLogFontW(&graphics, &logFont));
    return FontHandle(CreateFontIndirectW(&logFont));
}

Point GetMessagePos()
{
    DWORD pos = ::GetMessagePos();
    int x = pos & 0xFFFF;
    int y = (pos >> 16) & 0xFFFF;
    return Point(x, y);
}

Rect ToRect(const RECT& winRect)
{
    return Rect(winRect.left, winRect.top, winRect.right - winRect.left, winRect.bottom - winRect.top);
}

RECT ToWinRect(const Rect& rect)
{
    RECT r;
    r.left = rect.X;
    r.right = rect.X + rect.Width;
    r.top = rect.Y;
    r.bottom = rect.Y + rect.Height;
    return r;
}

Size LParamSize(Message& msg)
{
    uint32_t s = msg.LParamLoDWord();
    uint16_t sw = static_cast<uint16_t>(s);
    uint16_t sh = static_cast<uint16_t>(s >> 16);
    return Size(sw, sh);
}

Point LParamLocation(Message& msg)
{
    uint32_t s = msg.LParamLoDWord();
    uint16_t sx = static_cast<uint16_t>(s);
    uint16_t sy = static_cast<uint16_t>(s >> 16);
    return Point(sx, sy);
}

void DrawString(Graphics& graphics, const std::string& text, const Font& font, const PointF& origin, const Brush& brush)
{
    std::u16string txt = util::ToUtf16(text);
    CheckGraphicsStatus(graphics.DrawString((const WCHAR*)txt.c_str(), txt.length(), font.NativeFont(), origin, &brush));
}

void DrawString(Graphics& graphics, const std::string& text, const Font& font, const PointF& origin, const StringFormat& stringFormat, const Brush& brush)
{
    std::u16string txt = util::ToUtf16(text);
    CheckGraphicsStatus(graphics.DrawString((const WCHAR*)txt.c_str(), txt.length(), font.NativeFont(), origin, &stringFormat, &brush));
}

void DrawString(Graphics& graphics, const std::string& text, const Font& font, const RectF& rect, const StringFormat& stringFormat, const Brush& brush)
{
    std::u16string txt = util::ToUtf16(text);
    CheckGraphicsStatus(graphics.DrawString((const WCHAR*)txt.c_str(), txt.length(), font.NativeFont(), rect, &stringFormat, &brush));
}

RectF MeasureString(Graphics& graphics, const std::string& text, const Font& font, const PointF& origin, const StringFormat& stringFormat)
{
    std::u16string txt = util::ToUtf16(text);
    RectF boundingBox;
    CheckGraphicsStatus(graphics.MeasureString((const WCHAR*)txt.c_str(), txt.length(), font.NativeFont(), origin, &stringFormat, &boundingBox));
    return boundingBox;
}

Color ToGray(const Color& color, const Color& transparentColor)
{
    if (color == transparentColor) return color;
    std::string theme = "light"; // = GetTheme(); // todo
    if (theme == "light")
    {
        int32_t gray = (int32_t(color.GetRed()) + int32_t(color.GetGreen()) + int32_t(color.GetBlue()) + 2 * 255) / 5;
        uint8_t g = static_cast<uint8_t>(gray);
        return Color(color.GetAlpha(), g, g, g);
    }
    else if (theme == "dark")
    {
        float y = (color.GetRed() * 0.3 + color.GetGreen() * 0.59 + color.GetBlue() * 0.11) / 3.0;
        uint8_t g = static_cast<uint8_t>(y * 255);
        return Color(color.GetAlpha(), g, g, g);
    }
    else
    {
        int32_t gray = (int32_t(color.GetRed()) + int32_t(color.GetGreen()) + int32_t(color.GetBlue()) + 2 * 255) / 5;
        uint8_t g = static_cast<uint8_t>(gray);
        return Color(color.GetAlpha(), g, g, g);
    }
}

std::unique_ptr<Bitmap> ToGrayBitmap(Bitmap* bm, const Color& transparentColor)
{
    int w = bm->GetWidth();
    int h = bm->GetHeight();
    std::unique_ptr<Bitmap> grayBitmap(bm->Clone(0, 0, w, h, PixelFormat24bppRGB));
    for (int y = 0; y < h; ++y)
    {
        for (int x = 0; x < w; ++x)
        {
            Color color;
            CheckGraphicsStatus(grayBitmap->GetPixel(x, y, &color));
            Color gray = ToGray(color, transparentColor);
            grayBitmap->SetPixel(x, y, gray);
        }
    }
    return grayBitmap;
}

std::string GetImageFormatStr(ImageFormat imageFormat)
{
    switch (imageFormat)
    {
        case ImageFormat::bmp: return "image/bmp";
        case ImageFormat::jpeg: return "image/jpeg";
        case ImageFormat::gif: return "image/gif";
        case ImageFormat::tiff: return "image/tiff";
        case ImageFormat::png: return "image/png";
    }
    return std::string();
}

std::string ColorStr(const Color& color)
{
    std::string s = "#";
    s.append(util::ToHexString(color.GetAlpha()));
    s.append(util::ToHexString(color.GetRed()));
    s.append(util::ToHexString(color.GetGreen()));
    s.append(util::ToHexString(color.GetBlue()));
    return s;
}

Color ParseColorStr(const std::string& s)
{
    if (s.length() != 9)
    {
        throw std::runtime_error("invalid color string length, format is hexadecimal '#AARRGGBB'");
    }
    std::string aa = s.substr(1, 2);
    uint8_t alpha = util::ParseHexByte(aa);
    std::string rr = s.substr(3, 2);
    uint8_t red = util::ParseHexByte(rr);
    std::string gg = s.substr(5, 2);
    uint8_t green = util::ParseHexByte(gg);
    std::string bb = s.substr(7, 2);
    uint8_t blue = util::ParseHexByte(bb);
    return Color(alpha, red, green, blue);
}

std::string MakePenKey(const Color& color, float width, DashStyle dashStyle)
{
    std::string penKey = ColorStr(color);
    penKey.append(1, '.').append(std::to_string(width)).append(1, '.').append(std::to_string(int(dashStyle)));
    return penKey;
}

FontKey::FontKey(const std::string& family_, float size_, FontStyle style_) : family(family_), size(size_), style(style_)
{
}

std::string FontKey::ToString() const
{
    std::string fontKeyStr = family;
    fontKeyStr.append(1, '.').append(std::to_string(size)).append(1, '.').append(std::to_string(int(style)));
    return fontKeyStr;
}

std::string FontStyleStr(FontStyle style)
{
    std::string styleStr;
    if (style == Gdiplus::FontStyleRegular)
    {
        styleStr = "regular";
    }
    else
    {
        if ((style & Gdiplus::FontStyleBold) != 0)
        {
            if (!styleStr.empty())
            {
                styleStr.append(1, '.');
            }
            styleStr.append("bold");
        }
        if ((style & Gdiplus::FontStyleItalic) != 0)
        {
            if (!styleStr.empty())
            {
                styleStr.append(1, '.');
            }
            styleStr.append("italic");
        }
        if ((style & Gdiplus::FontStyleUnderline) != 0)
        {
            if (!styleStr.empty())
            {
                styleStr.append(1, '.');
            }
            styleStr.append("underline");
        }
        if ((style & Gdiplus::FontStyleStrikeout) != 0)
        {
            if (!styleStr.empty())
            {
                styleStr.append(1, '.');
            }
            styleStr.append("strikeout");
        }
    }
    return styleStr;
}

FontStyle ParseFontStyle(const std::string& fontStyleStr)
{
    FontStyle style = Gdiplus::FontStyleRegular;
    std::vector<std::string> components = util::Split(fontStyleStr, '.');
    int n = components.size();
    for (int i = 0; i < n; ++i)
    {
        const std::string& component = components[i];
        if (component == "bold")
        {
            style = static_cast<FontStyle>(style | Gdiplus::FontStyleBold);
        }
        else if (component == "italic")
        {
            style = static_cast<FontStyle>(style | Gdiplus::FontStyleItalic);
        }
        else if (component == "underline")
        {
            style = static_cast<FontStyle>(style | Gdiplus::FontStyleUnderline);
        }
        else if (component == "strikeout")
        {
            style = static_cast<FontStyle>(style | Gdiplus::FontStyleStrikeout);
        }
    }
    return style;
}

int GetEncoderClsId(ImageFormat imageFormat, CLSID& guid) 
{
    std::string imageFormatName = GetImageFormatStr(imageFormat);
    std::u16string imageFormatStr = util::ToUtf16(imageFormatName);
    Gdiplus::ImageCodecInfo* imageCodecInfo = nullptr;
    uint32_t n = 0;
    uint32_t size = 0;
    int result = Gdiplus::GetImageEncodersSize(&n, &size);
    if (result != 0 || size == 0)
    {
        return result;
    }
    imageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
    if (!imageCodecInfo) return 3;
    result = Gdiplus::GetImageEncoders(n, size, imageCodecInfo);
    if (result != 0)
    {
        return result;
    }
    for (uint32_t i = 0; i < n; ++i)
    {
        std::u16string mimetype = reinterpret_cast<const char16_t*>(imageCodecInfo[i].MimeType);
        if (mimetype == imageFormatStr)
        {
            guid = imageCodecInfo[i].Clsid;
            free(imageCodecInfo);
            return 0;
        }
    }
    free(imageCodecInfo);
    return -1;
}

} // wing
