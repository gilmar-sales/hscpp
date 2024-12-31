#include <fstream>
#include <regex>

#include "hscpp/compiler/CompilerCmdLine_compile_commands.h"

#include <iostream>

#include "hscpp/Log.h"
#include "hscpp/Util.h"

namespace hscpp {
    CompilerCmdLine_compile_commands::CompilerCmdLine_compile_commands(CompilerConfig *pConfig)
        : m_pConfig(pConfig), m_commands_map({}) {
        std::ifstream compileCommandsFile(m_pConfig->compile_commands_json_path.u8string().c_str());

        if (!compileCommandsFile.is_open()) {
            log::Error() << HSCPP_LOG_PREFIX <<
                    "Failed to open compile_commands.json file, use set(CMAKE_EXPORT_COMPILE_COMMANDS ON)"
                    << m_pConfig->compile_commands_json_path.u8string().c_str() << log::End(".");
            throw std::runtime_error("Failed to open compile_commands.json");
        }

        auto buffer = std::string(1024, '\0');

        while (compileCommandsFile.good()) {
            compileCommandsFile.getline(buffer.data(), buffer.size(), '\n');

            auto regex = std::regex(R"(\s{2}.command.:\s"(.+?)\s(.+)(-o.+)(.+-c.(.+))\".+)");
            std::smatch matches;

            if (std::regex_match(buffer, matches, regex)) {
                m_commands_map[util::UnixSlashes(matches[5])] = util::UnixSlashes(matches[2]) + util::UnixSlashes( matches[5]);
            }

            buffer = std::string(1024, '\0');
        }
    }

    bool CompilerCmdLine_compile_commands::GenerateCommandFile(const fs::path &commandFilePath,
                                                               const fs::path &moduleFilePath,
                                                               const ICompiler::Input &input) {
        std::ofstream commandFile(commandFilePath.u8string().c_str());

        std::stringstream command;

        if (!commandFile.is_open()) {
            log::Error() << HSCPP_LOG_PREFIX << "Failed to open command file "
                    << commandFilePath << log::End(".");
            return false;
        }

        const auto it = m_commands_map.find(input.sourceFilePaths.at(0));

        if (it == m_commands_map.end()) {
            return false;
        }

        command << it->second;

        // Output module name.
        command << " -o \"" << util::UnixSlashes(moduleFilePath.u8string()) << "\"";

        command << " -shared -fPIC -fvisibility=hidden /home/gilmar/dev/hscpp/src/module/Module.cpp";

        log::Build() << command.str() << log::End();
        commandFile << command.str();
        return true;
    }
}
