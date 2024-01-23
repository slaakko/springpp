// =================================
// Copyright (c) 2024 Seppo Laakko
// Distributed under the MIT license
// =================================

module;
#include <Windows.h>

export module wing.dialog;

import std.core;

export namespace wing {

// OFN_PATHMUSTEXIST, OFN_FILEMUSTEXIST

bool OpenFileName(HWND windowHandle, const std::vector<std::pair<std::string, std::string>>& descriptionFilterPairs, const std::string& initialDirectory,
    const std::string& defaultFilePath, const std::string& defaultExtension,
    uint32_t flags, std::string& firstPath, std::string& currentDirectory, std::vector<std::string>& fileNames);

bool SaveFileName(HWND windowHandle, const std::vector<std::pair<std::string, std::string>>& descriptionFilterPairs, const std::string& initialDirectory,
    uint32_t flags, const std::string& defaultExtension, std::string& fileName);

} // wing
