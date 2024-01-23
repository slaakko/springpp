// =================================
// Copyright (c) 2024 Seppo Laakko
// Distributed under the MIT license
// =================================

export module wing.text.view;

import wing.base;
import std.core;

export namespace wing {

Color DefaultSelectionBackgroundColor();
std::vector<std::u32string> SplitTextIntoLines(const std::u32string& text);

} // wing
