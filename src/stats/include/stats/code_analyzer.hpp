// 2024/1/2
// zhangzhong

#pragma once

#include "stats/analysis_result.hpp"
#include <cstddef>
#include <ctime>
#include <iostream>
#include <istream>
#include <memory>
#include <string>

namespace stats {

class CodeAnalyzer {
  public:
    CodeAnalyzer() = default;
    CodeAnalyzer(std::string const& line_comment_head,
                 std::string const& block_comment_head,
                 std::string const& block_comment_tail);
    virtual ~CodeAnalyzer() = default;

    // 这些是虚函数，子类可以重写，并且是暴露给外部的接口
    virtual auto Analyze(std::string const& path)
        -> std::shared_ptr<AnalysisResult>;
    virtual auto Init() -> void;
    virtual std::shared_ptr<AnalysisResult> AnalyzeFile(std::istream& is);

  protected:
    // 这些是内部函数，子类不应该重写，并且是不暴露给外部的接口
    auto GetLineAndResetOffset(std::istream& is, std::string& line,
                               size_t& offset) -> std::istream&;
    auto FindFirstNotBlank(std::string const& line, size_t offset) -> size_t;
    auto SetLineCategory(size_t line, LineCategory category) -> void;
    auto SetMultiLineCategory(size_t begin, size_t end, LineCategory category)
        -> void;

  protected:
    // 这些是虚函数，子类可以重写，并且是不暴露给外部的接口
    virtual auto IsStringHead(std::string_view const& line, size_t offset)
        -> bool;
    virtual auto IsRawStringHead(std::string_view const& line, size_t offset)
        -> bool;
    virtual auto IsLineCommentHead(std::string_view const& line, size_t offset)
        -> bool;
    virtual auto IsBlockCommentHead(std::string_view const& line, size_t offset)
        -> bool;

    virtual auto SkipString(std::istream& is, std::string& line, size_t offset)
        -> size_t;
    virtual auto SkipRawString(std::istream& is, std::string& line,
                               size_t offset) -> size_t;
    virtual auto SkipLineComment(std::istream& is, std::string& line,
                                 size_t offset) -> size_t;
    virtual auto SkipBlockComment(std::istream& is, std::string& line,
                                  size_t offset) -> size_t;
    virtual auto SkipUntilFindDelimiter(std::istream& is, std::string& line,
                                        size_t offset,
                                        std::string_view const& delimiter,
                                        LineCategory line_category) -> size_t;

  public:
    std::shared_ptr<AnalysisResult> result_;

  private:
    uint32_t line_begin_;
    uint32_t line_end_;
    std::string line_comment_head_;
    std::string block_comment_head_;
    std::string block_comment_tail_;
};

auto MakeCodeAnalyzer(std::string const& language)
    -> std::shared_ptr<CodeAnalyzer>;

} // namespace stats
