// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

module springpp.configuration;

namespace springpp {

Configuration& Configuration::Instance()
{
    static Configuration instance;
    return instance;
}

void Configuration::SetLayout(Layout* layout_)
{
    layout.reset(layout_);
}

} // namespace springpp
