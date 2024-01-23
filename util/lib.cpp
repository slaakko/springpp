// =================================
// Copyright (c) 2024 Seppo Laakko
// Distributed under the MIT license
// =================================

module util.lib;

namespace util {

std::string SpringPPVersionStr()
{
    return "0.2.0";
}

std::string SpringPPRoot()
{
    std::string springPPRoot;
#pragma warning(suppress : 4996)
    const char* springPPRootEnv = std::getenv("SPRINGPP_ROOT");
    if (springPPRootEnv)
    {
        springPPRoot = springPPRootEnv;
    }
    if (springPPRoot.empty())
    {
        throw std::runtime_error("please set 'SPRINGPP_ROOT' environment variable to contain /path/to/springpp directory.");
    }
    return springPPRoot;
}

} // util
