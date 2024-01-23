// =================================
// Copyright (c) 2024 Seppo Laakko
// Distributed under the MIT license
// =================================

module;
#ifdef _WIN32
#include <Windows.h>
#endif

module util.system;

namespace util {

std::string GetPathToExecutable()
{
    char buf[4096];
    DWORD result = GetModuleFileNameA(NULL, buf, sizeof(buf));
    if (result == 0)
    {
        throw std::runtime_error("could not get path to current executable: GetModuleFileName failed");
    }
    return std::string(buf);
}

} // util