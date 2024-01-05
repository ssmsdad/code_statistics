// 2024/1/3
// zhangzhong

#pragma once

#include "code_analyzer.hpp"

namespace stats {

class CppAnalyzer : public CodeAnalyzer {

  public:
    CppAnalyzer();
    virtual ~CppAnalyzer() = default;

  private:
    virtual auto IsLineCommentHead(std::string_view const& line, size_t offset)
        -> bool override;
    virtual auto IsBlockCommentHead(std::string_view const& line, size_t offset)
        -> bool override;
    virtual auto SkipRawString(std::istream& is, std::string& line,
                               size_t offset) -> size_t override;
    auto GetRawStringTail(std::string_view const& line, size_t offset)
        -> std::string;
};

auto MakeCppAnalyzer() -> std::shared_ptr<CppAnalyzer>;

} // namespace stats
