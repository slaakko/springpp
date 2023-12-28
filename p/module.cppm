// =================================
// Copyright (c) 2023 Seppo Laakko
// Distributed under the MIT license
// =================================

export module p.mod;

import p.block;
import soul.lexer;
import util;
import std.core;

export namespace p {

class Routine;
class Writer;
class Reader;
class Context;
class ParsingContext;
class ModuleMap;
class Subroutine;

enum class ModuleKind
{
    unit, program
};


enum class ModulePartKind
{
    interfacePart, implementationPart
};

class Module;

class ModulePart
{
public:
    ModulePart(ModulePartKind kind_);
    virtual ~ModulePart();
    ModulePartKind Kind() const { return kind; }
    void SetModule(Module* mod_) { mod = mod_; }
    Module* GetModule() const { return mod; }
    const std::set<std::string>& ImportedModulesNames() const { return importedModuleNames; }
    void AddImportedModuleName(const std::string& moduleName);
    virtual void Write(Writer& writer);
    virtual void Read(Reader& reader, Context* context);
    void CheckInterface();
    void MakeVmts(Context* context);
    void SetBlock(Block* block_);
    Block* GetBlock() const { return block.get(); }
    void LoadModules(Context* context);
    const std::vector<Module*>& ImportedModules() const { return importedModules; }
    void AddSubroutine(Subroutine* subroutine);
    void AddSubroutineRead(Subroutine* subroutine);
    Subroutine* GetSubroutine(int32_t subroutineId) const;
    bool IsInterfacePart() const { return kind == ModulePartKind::interfacePart; }
    bool IsImplementationPart() const { return kind == ModulePartKind::implementationPart; }
    const std::map<int32_t, Subroutine*>& SubroutineMap() const { return subroutineMap; }
    void Resolve(Context* context);
    void Print(util::CodeFormatter& formatter, ExecutionContext* context);
private:
    ModulePartKind kind;
    Module* mod;
    Module* systemModule;
    std::set<std::string> importedModuleNames;
    std::vector<Module*> importedModules;
    std::unique_ptr<Block> block;
    std::map<int32_t, Subroutine*> subroutineMap;
};

class InterfacePart : public ModulePart
{
public:
    InterfacePart();
};

class ImplementationPart : public ModulePart
{
public:
    ImplementationPart();
};

class Module
{
public:
    Module();
    ModuleKind Kind() const { return kind; }
    void SetKind(ModuleKind kind_) { kind = kind_; }
    const std::string& Name() const { return name; }
    void SetName(const std::string& name_);
    int32_t Id() const { return id; }
    void SetId();
    const std::string& FilePath() const { return filePath; }
    void SetFilePath();
    const std::string& SourceFilePath() const { return sourceFilePath; }
    void SetSourceFilePath(const std::string& sourceFilePath_);
    ModulePart* CreateInterfacePart();
    ModulePart* GetInterfacePart() const { return interfacePart.get(); }
    ModulePart* CreateImplementationPart();
    ModulePart* GetImplementationPart() const { return implementationPart.get(); }
    bool IsUpToDate() const;
    void Init(ParsingContext* context);
    void Term(ParsingContext* context);
    void Save();
    void Load(const std::string& fromFilePath, Context* context);
    Block* RootBlock() const { return rootBlock.get(); }
    void CheckInterface();
    void MakeVmts(Context* context);
    void Resolve(Context* context);
    void Print(const std::string& textFilePath, ExecutionContext* context);
    void SetInit(ParsingContext* context, soul::lexer::LexerBase<char>& lexer, int64_t pos);
    Subroutine* InitProc() const { return init; }
    void MakeInitBlock(Block* parent);
    Block* InitBlock() const { return initBlock.get(); }
    void RunInits(ExecutionContext* context);
private:
    ModuleKind kind;
    std::string name;
    int32_t id;
    std::string filePath;
    std::string sourceFilePath;
    std::unique_ptr<ModulePart> interfacePart;
    std::unique_ptr<ModulePart> implementationPart;
    std::unique_ptr<Block> rootBlock;
    std::unique_ptr<Block> initBlock;
    bool initRun;
    Subroutine* init;
};

class ModuleMap
{
public:
    ModuleMap();
    void AddModule(Module* mod);
    Module* GetModule(const std::string& moduleName, Context* context);
    Module* GetModule(int32_t moduleId) const;
private:
    std::map<std::string, Module*> moduleMap;
    std::map<int32_t, Module*> moduleIdMap;
    std::vector<std::unique_ptr<Module>> modules;
};

} // namespace p
