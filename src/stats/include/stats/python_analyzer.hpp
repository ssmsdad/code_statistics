// 2024/1/5
// zhangzhong

#pragma once

#include "stats/code_analyzer.hpp"

namespace stats {

class PythonAnalyzer : public CodeAnalyzer {
  public:
    PythonAnalyzer();
    // 确保通过基类指针删除派生类对象时，能够正确调用派生类的析构函数
    virtual ~PythonAnalyzer() = default;

  private:
    virtual auto IsStringHead(std::string_view const& line, size_t offset)
        -> bool override;
    virtual auto IsRawStringHead(std::string_view const& line, size_t offset)
        -> bool override;
    virtual auto IsBlockCommentHead(std::string_view const& line, size_t offset)
        -> bool override;
    virtual auto SkipRawString(std::istream& is, std::string& line,
                               size_t offset) -> size_t override;
    virtual auto SkipBlockComment(std::istream& is, std::string& line,
                                  size_t offset) -> size_t override;
};

auto MakePythonAnalyzer() -> std::shared_ptr<PythonAnalyzer>;

} // namespace stats
