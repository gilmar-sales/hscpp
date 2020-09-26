#pragma once

#include <vector>
#include <string>
#include <memory>

#include "hscpp/Platform.h"
#include "hscpp/ICompiler.h"
#include "hscpp/ICmdShell.h"

namespace hscpp
{

    class Compiler_msvc : public ICompiler
    {
    public:
        Compiler_msvc();

        bool IsInitialized() override;

        bool StartBuild(const Input& input) override;
        void Update() override;

        bool IsCompiling() override;

        bool HasCompiledModule() override;
        fs::path PopModule() override;

    private:
        enum class CompilerTask
        {
            GetVsPath,
            SetVcVarsAll,
            Build,
        };

        bool m_bInitialized = false;
        std::unique_ptr<ICmdShell> m_pCmdShell;

        size_t m_iCompileOutput = 0;
        fs::path m_CompilingModulePath;
        fs::path m_CompiledModulePath;

        bool CreateCommandFile(const Input& input);

        void StartVsPathTask();
        bool HandleTaskComplete(CompilerTask task);
        bool HandleGetVsPathTaskComplete(const std::vector<std::string>& output);
        bool HandleSetVcVarsAllTaskComplete(std::vector<std::string> output);
        bool HandleBuildTaskComplete();
    };

}