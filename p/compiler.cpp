// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

module p.compiler;

import p.parsing_context;
import p.lexer;
import p.main_parser;
import p.block;
import p.code;
import p.mod;
import p.execute;
import util;

namespace p {

void Compile(const std::string& filePath)
{
    ModuleMap moduleMap;
    std::string s = util::ReadFile(filePath);
    auto lexer = p::lexer::MakeLexer(s.c_str(), s.c_str() + s.length(), filePath);
    using LexerType = decltype(lexer);
    ParsingContext context;
    context.SetModuleMap(&moduleMap);
    p::main_parser::MainParser<LexerType>::Parse(lexer, &context);
    Module* mod = context.GetModule();
    moduleMap.AddModule(mod);
    mod->CheckInterface();
    mod->MakeVmts(&context);
    mod->Save();
}

} // namespace p
