#pragma once

#include <unordered_map>
#include <hscpp/Config.h>

#include "ICompilerCmdLine.h"

namespace hscpp {

    class CompilerCmdLine_compile_commands : virtual public ICompilerCmdLine
    {
    public:
        explicit CompilerCmdLine_compile_commands(CompilerConfig *pConfig);

        bool GenerateCommandFile(const fs::path &commandFilePath,
                                 const fs::path &moduleFilePath,
                                 const ICompiler::Input &input) override;

    private:
        CompilerConfig *m_pConfig = nullptr;
        std::unordered_map<std::string, std::string> m_commands_map;
    };

}

