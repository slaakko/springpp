// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <Windows.h>

import p;
import std.core;
import util;

void InitApplication()
{
    util::Init();
}

void TerminateApplication()
{
    util::Done();
}

void Run(const std::string& filePath)
{
    p::Init();
    p::Heap heap;
    p::ModuleMap moduleMap;
    p::Module mod;
    p::ExecutionContext context;
    context.SetModuleMap(&moduleMap);
    context.SetHeap(&heap);
    mod.Load(filePath, &context);
    moduleMap.AddModule(&mod);
    if (!mod.GetImplementationPart())
    {
        throw std::runtime_error("error: module '" + filePath + "' does not contain implementation part");
    }
    p::Block* block = mod.GetImplementationPart()->GetBlock();
    p::Procedure* program = block->GetProcedure("@program");
    if (!program)
    {
        throw std::runtime_error("error: module '" + filePath + "' does not contain procedure '@program'");
    }
    context.SetGlobalVariableMap(block->GetGlobalVariableMap());
    context.GetGlobalVariableMap()->AllocateFrame();
    context.PushBlock(block);
    program->Resolve(&context);
    mod.RunInits(&context);
    program->Execute(&context);
    context.PopBlock();
    p::Done();
}

void PrintHelp()
{
    std::cout << "Usage: rp [options] FILE.pcode" << "\n";
    std::cout << "Run p-language pcode program file FILE.pcode" << "\n";
    std::cout << "Options:" << "\n";
    std::cout << "--help | -h" << "\n";
    std::cout << "  Print help and exit." << "\n";
    std::cout << "\n";
}

int main(int argc, const char** argv)
{
    try
    {
        InitApplication();
        std::string filePath;
        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];
            if (arg.starts_with("--"))
            {
                if (arg == "--help")
                {
                    PrintHelp();
                    return 0;
                }
                else
                {
                    throw std::runtime_error("unknown option '" + arg + "'");
                }
            }
            else if (arg.starts_with("-"))
            {
                std::string options(arg.substr(1));
                for (char o : options)
                {
                    switch (o)
                    {
                        case 'h':
                        {
                            PrintHelp();
                            return 0;
                        }
                        default:
                        {
                            throw std::runtime_error("unknown option '-" + std::string(1, o) + "'");
                        }
                    }
                }
            }
            else
            {
                filePath = arg;
            }
        }
        if (!filePath.empty())
        {
            Run(filePath);
        }
        else
        {
            throw std::runtime_error("file path argument not given");
        }
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    TerminateApplication();
    return 0;
}