#pragma once

#include "hscpp/ICompilerCmdLine.h"
#include "hscpp/Config.h"

namespace hscpp
{

    class CompilerCmdLine_msvc : public ICompilerCmdLine
    {
    public:
        explicit CompilerCmdLine_msvc(const CompilerConfig& config);

        bool GenerateCommandFile(const fs::path& commandFilePath,
                                 const fs::path& moduleFilePath,
                                 const ICompiler::Input& input) override;

    private:
        CompilerConfig m_Config;
    };

}