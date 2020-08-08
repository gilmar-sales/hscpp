#include <fstream>
#include <sstream>

#include "hscpp/FileParser.h"
#include "hscpp/Log.h"

namespace hscpp
{

    bool FileParser::ParseFile(const std::filesystem::path& path, ParseInfo& info)
    {
        std::ifstream file(path.native().c_str());
        if (!file.is_open())
        {
            Log::Write(LogLevel::Error, "%s: Failed to open file %s.\n", __func__, path.u8string().c_str());
            return false;
        }

        std::stringstream buf;
        buf << file.rdbuf();

        m_Filepath = path;
        m_iChar = 0;
        m_Content = buf.str();

        Parse(info);
        return true;
    }

    void FileParser::Parse(ParseInfo& info)
    {
        // Barebones lexer/parser. There are very few things we need to match, and if
        // hscpp::Feature::DependentCompilation is enabled, all files in the project will need to be
        // parsed on startup. Prefer speed over elegance.
        while (!IsAtEnd())
        {
            // We only care to find:
            //    hscpp_require_source
            //    hscpp_require_include
            //    hscpp_require_lib
            //    #include
            switch (Peek())
            {
            case 'h':
            {
                bool bDependency = false;
                RuntimeDependency::Type dependencyType = RuntimeDependency::Type::Source;

                if (Match("hscpp_require_"))
                {
                    if (Match("source"))
                    {
                        bDependency = true;
                        dependencyType = RuntimeDependency::Type::Source;
                    }
                    else if (Match("include"))
                    {
                        bDependency = true;
                        dependencyType = RuntimeDependency::Type::Include;
                    }
                    else if (Match("lib"))
                    {
                        bDependency = true;
                        dependencyType = RuntimeDependency::Type::Library;
                    }

                    if (bDependency)
                    {
                        RuntimeDependency dependency;
                        dependency.type = dependencyType;

                        if (ParseRuntimeRequirement(dependency))
                        {
                            info.dependencies.push_back(dependency);
                        }
                    }
                }

                break;
            }
            case '#':
            {
                if (Match("#include"))
                {
                    std::filesystem::path include;
                    if (ParseInclude(include))
                    {
                        info.includes.push_back(include);
                    }
                }

                break;
            }
            default:
                break;
            }

            Advance();
        }
    }

    bool FileParser::ParseRuntimeRequirement(RuntimeDependency& dependency)
    {
        SkipWhitespace();      

        if (Peek() != '(')
        {
            // Not a true error, in case user defined something like hscpp_require_source_custom.
            return false;
        }

        do
        {
            // Parse argument list (ex. hscpp_require_source(file1.cpp, file2.cpp) ).
            Advance();
            SkipWhitespace();

            std::string path;
            if (!ParseString(path))
            {
                return false;
            }

            dependency.paths.push_back(path);

            SkipWhitespace();
        } while (Peek() == ',');

        if (Peek() != ')')
        {
            LogParseError("Runtime dependency missing closing ')'.");
            return false;
        }

        Advance();
        return true;
    }

    bool FileParser::ParseInclude(std::filesystem::path& include)
    {
        if (!std::isspace(Peek()))
        {
            // Not treated as a true error, though in practice #include should never have a suffix.
            return false;
        }
        SkipWhitespace();

        std::string includeStr;
        if (!ParseString(includeStr))
        {
            return false;
        }

        include = includeStr;
        return true;
    }

    bool FileParser::ParseString(std::string& strContent)
    {
        if (Peek() != '"')
        {
            LogParseError("Missing opening '\"'.");
            return false;
        }
        Advance();

        bool bEscaped = false;
        while (!IsAtEnd())
        {
            if (!bEscaped && Peek() == '"')
            {
                break;
            }

            bEscaped = false;
            if (Peek() == '\\')
            {
                bEscaped = true;
            }

            strContent += Peek();
            Advance();
        }

        if (Peek() != '"')
        {
            LogParseError("Unterminated string, expected a '\"'.");
            return false;
        }

        Advance();
        return true;
    }

    bool FileParser::Match(const std::string& str)
    {
        size_t iChar = m_iChar;
        size_t iOffset = 0;

        while (iChar < m_Content.size() && iOffset < str.size())
        {
            if (str.at(iOffset) != m_Content.at(iChar))
            {
                return false;
            }
            
            ++iChar;
        }

        if (iOffset != str.size())
        {
            // Hit end of content before matching str.
            return false;
        }

        // Success, consume the match.
        m_iChar = iChar;
        return true;
    }

    bool FileParser::Expect(char c, const std::string& error)
    {
        if (Peek() == c)
        {
            return true;
        }

        Log::Write(LogLevel::Error, "%s\n", error.c_str());
        return false;
    }

    void FileParser::SkipWhitespace()
    {
        while (std::isspace(Peek()))
        {
            Advance();
        }
    }

    bool FileParser::IsAtEnd()
    {
        return m_iChar >= m_Content.size();
    }

    char FileParser::Peek()
    {
        if (IsAtEnd())
        {
            return 0;
        }

        return m_Content.at(m_iChar);
    }

    void FileParser::Advance()
    {
        m_iChar++;
    }

    void FileParser::LogParseError(const std::string& error)
    {
        Log::Write(LogLevel::Error, "%s: Failed to parse file %s: %s\n",
            __func__, m_Filepath.u8string().c_str(), error.c_str());
    }

}