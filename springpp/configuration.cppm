// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

export module springpp.configuration;

import springpp.layout;
import wing;
import std.core;

export namespace springpp {

class Configuration
{
public:
    static Configuration& Instance();
    void SetLayout(Layout* layout_);
    Layout* GetLayout() const { return layout.get(); }
private:
    std::unique_ptr<Layout> layout;
};

} // namespace springpp
