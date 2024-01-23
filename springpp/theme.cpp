// =================================
// Copyright (c) 2024 Seppo Laakko
// Distributed under the MIT license
// =================================

module springpp.theme;

namespace springpp {

wing::Color DefaultToolBarBorderColor()
{
    return wing::Color(204, 206, 219);
}

wing::Color DefaultToolBarToolTipColor()
{
    return wing::Color(241, 242, 247);
}

void MakeLightTheme()
{
    std::unique_ptr<wing::Theme> lightTheme(new wing::Theme("light", "light-theme.xml"));

    lightTheme->SetColor("window.background", wing::Color(238, 238, 242));
    lightTheme->SetColor("window.title.bar", wing::Color(238, 238, 242));
    lightTheme->SetColor("window.title.text", wing::Color(0, 0, 0));
    lightTheme->SetColor("window.title.disabled.text", wing::Color(67, 67, 67));
    lightTheme->SetColor("window.sys.button", wing::Color(30, 30, 30));
    lightTheme->SetColor("window.sys.button.selected", wing::Color(0, 0, 0));
    lightTheme->SetColor("window.sys.button.hover.background", wing::Color(252, 252, 253));

    lightTheme->SetColor("tool.bar.background", wing::DefaultControlBackgroundColor());
    lightTheme->SetColor("tool.bar.frame", DefaultToolBarBorderColor());
    lightTheme->SetColor("tool.bar.tool.tip", DefaultToolBarToolTipColor());
    lightTheme->SetColor("tool.bar.tool.button.dark", wing::DefaultToolButtonDarkColor());
    lightTheme->SetColor("tool.bar.tool.button.light", wing::DefaultToolButtonLightColor());
    lightTheme->SetColor("tool.bar.tool.button.medium.light", wing::DefaultToolButtonMediumLightColor());
    lightTheme->SetColor("tool.bar.tool.button.medium.dark", wing::DefaultToolButtonMediumDarkColor());
    lightTheme->SetColor("tool.bar.tool.button.text", wing::DefaultToolButtonTextColor());
    lightTheme->SetColor("tool.bar.tool.button.disabled", wing::DefaultToolButtonDisabledColor());
    lightTheme->SetColor("tool.bar.tool.button.separator1", wing::DefaultToolButtonSeparatorColor1());
    lightTheme->SetColor("tool.bar.tool.button.separator2", wing::DefaultToolButtonSeparatorColor2());

    lightTheme->SetColor("selection.background", wing::DefaultSelectionBackgroundColor());

    lightTheme->SetColor("text", wing::Color::Black);

    lightTheme->SetColor("menu.background", wing::DefaultMenuBackgroundColor());
    lightTheme->SetColor("menu.text", wing::DefaultMenuTextColor());
    lightTheme->SetColor("menu.disabled.text", wing::DefaultDisabledMenuTextColor());
    lightTheme->SetColor("menu.mouse.over.background", wing::DefaultMenuMouseOverColor());
    lightTheme->SetColor("menu.open.background", wing::DefaultMenuOpenColor());
    lightTheme->SetColor("menu.shadow", wing::DefaultMenuShadowColor());
    lightTheme->SetColor("menu.child.indicator", wing::Color::Black);
    lightTheme->SetColor("menu.frame", wing::Color::Black);
    lightTheme->SetColor("menu.item.separator", wing::Color::DarkGray);

    lightTheme->SetColor("status.bar.background", wing::DefaultControlBackgroundColor());
    lightTheme->SetColor("status.bar.text", wing::DefaultTextColor());
    lightTheme->SetColor("status.bar.top.line", wing::DefaultTopLineColor());
    lightTheme->SetColor("status.bar.sunken.border.outer.top.left", wing::DefaultSunkenBorderOuterTopLeftColor());
    lightTheme->SetColor("status.bar.sunken.border.inner.top.left", wing::DefaultSunkenBorderInnerTopLeftColor());
    lightTheme->SetColor("status.bar.sunken.border.outer.right.bottom", wing::DefaultSunkenBorderOuterRightBottomColor());
    lightTheme->SetColor("status.bar.sunken.border.inner.right.bottom", wing::DefaultSunkenBorderInnerRightBottomColor());
    lightTheme->SetColor("status.bar.raised.border.outer.top.left", wing::DefaultRaisedBorderOuterTopLeftColor());
    lightTheme->SetColor("status.bar.raised.border.outer.right.bottom", wing::DefaultRaisedBorderOuterRightBottomColor());
    lightTheme->SetColor("status.bar.raised.border.inner.top.left", wing::DefaultRaisedBorderInnerTopLeftColor());
    lightTheme->SetColor("status.bar.raised.border.inner.right.bottom", wing::DefaultRaisedBorderInnerRightBottomColor());

    lightTheme->SetCursorName("horizontal.splitter.cursor", "horizontal.splitter.light.wing.cursor");
    lightTheme->SetCursorName("vertical.splitter.cursor", "vertical.splitter.light.wing.cursor");

    wing::AddTheme(lightTheme.release());
}

void InitThemes()
{
    if (!wing::ThemesFileExists())
    {
        MakeLightTheme();
        wing::SaveThemes();
    }
    wing::LoadThemes();
    wing::SetTheme("light");
}

} // namespace springpp
