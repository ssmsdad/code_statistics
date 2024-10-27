// 2024/1/2
// zhangzhong

#include "stats/code_analyzer.hpp"
#include "stats/all_analyzer.hpp"
#include "stats/cpp_analyzer.hpp"
#include "stats/python_analyzer.hpp"
#include "stats/rust_analyzer.hpp"
#include <cstddef>
#include <fmt/core.h>
#include <fstream>
#include <memory>
#include <string>
#include <string_view>

namespace stats {

CodeAnalyzer::CodeAnalyzer(std::string const& line_comment_head,
                           std::string const& block_comment_head,
                           std::string const& block_comment_tail)
    : result_{std::make_shared<AnalysisResult>()}, line_begin_{0}, line_end_{0},
      line_comment_head_{line_comment_head},
      block_comment_head_{block_comment_head}, block_comment_tail_{
                                                   block_comment_tail} {}

auto CodeAnalyzer::Analyze(std::string const& path)
    -> std::shared_ptr<AnalysisResult> {
    fmt::println("stats... {}", path);
    Init();
    auto fin = std::ifstream{path};
    if (fin.is_open()) {
        result_->path = path;
        auto result = AnalyzeFile(fin);
        fin.close();
        return result;
    }
    return nullptr;
}

auto CodeAnalyzer::Init() -> void {
    line_begin_ = 0;
    line_end_ = 0;
    result_ = std::make_shared<AnalysisResult>();
}

//分析一个文件的内容
auto CodeAnalyzer::AnalyzeFile(std::istream& is)
    -> std::shared_ptr<AnalysisResult> {
    auto line = std::string{};
    auto offset = size_t{0};

    // 读取文件的每一行\
       判断当前行是否为空白行，如果不是空白行，那么就判断这行代码的类型
    while (GetLineAndResetOffset(is, line, offset)) {
        while ((offset = FindFirstNotBlank(line, offset)) !=
               std::string::npos) {
            if (IsStringHead(line, offset)) {
                offset = SkipString(is, line, offset);
            } else if (IsRawStringHead(line, offset)) {
                offset = SkipRawString(is, line, offset);
            } else if (IsLineCommentHead(line, offset)) {
                offset = SkipLineComment(is, line, offset);
            } else if (IsBlockCommentHead(line, offset)) {
                offset = SkipBlockComment(is, line, offset);
            } else {
                SetLineCategory(line_begin_, LineCategory::kCode);
                ++offset;
            }
        }
        line_begin_ = line_end_;
    }
    return result_;
}

//获取一行内容，并将offset置为0，这个函数是用来协助处理多行字符串的
auto CodeAnalyzer::GetLineAndResetOffset(std::istream& is, std::string& line,
                                         size_t& offset) -> std::istream& {
    // 如果成功获取一行内容，那么is为true
    // 先将line_category置为kBlank，然后将这行真正的类型与kBlank进行或操作，得到这行的类型，0或任何数都是这个数
    if (std::getline(is, line)) {
        ++line_end_;
        offset = 0;
        result_->line_categories.push_back(LineCategory::kBlank);
    }
    return is;
}

// 找到每行中第一个非空白字符的位置
auto CodeAnalyzer::FindFirstNotBlank(std::string const& line, size_t offset)
    -> size_t {
    for (; offset < line.size(); ++offset) {
        if (!std::isspace(line[offset]))
            return offset;
    }
    return std::string::npos;
}

auto CodeAnalyzer::SetLineCategory(size_t line, LineCategory category) -> void {
    // 将之前push_back的kBlank与category进行或操作，得到这行的类型
    result_->line_categories[line] |= static_cast<uint>(category);
}

auto CodeAnalyzer::SetMultiLineCategory(size_t begin, size_t end,
                                        LineCategory category) -> void {
    for (auto i = begin; i != end; ++i) {
        SetLineCategory(i, category);
    }
}

// 如果这行代码是字符串的开头，那么就判断其是代码行
auto CodeAnalyzer::IsStringHead(std::string_view const& line, size_t offset)
    -> bool {
    // construct the sub string and compare it is very slow
    return line[offset] == '\'' || line[offset] == '\"';
}

// 检查是否是原始字符串的开头，原始字符串是以r"..."或者R"..."开头的，并且允许包含特殊字符而不需要转义
auto CodeAnalyzer::IsRawStringHead(std::string_view const& line, size_t offset)
    -> bool {
    if (line[offset] == 'r' || line[offset] == 'R') {
        if (line[offset + 1] == '\'' || line[offset + 1] == '\"') {
            // the character before r must be a blank or at the front the line
            // i.e. operator"" is not raw string head
            if (offset == 0 || std::isspace(line[offset - 1])) {
                return true;
            }
        }
    }
    return false;
}

auto CodeAnalyzer::IsLineCommentHead(std::string_view const& line,
                                     size_t offset) -> bool {
    return line.substr(offset, line_comment_head_.size()) == line_comment_head_;
}

auto CodeAnalyzer::IsBlockCommentHead(std::string_view const& line,
                                      size_t offset) -> bool {
    return line.substr(offset, block_comment_head_.size()) ==
           block_comment_head_;
}

auto CodeAnalyzer::SkipString(std::istream& is, std::string& line,
                              size_t offset) -> size_t {
    // delimiter is ' or "
    // std::string_view 提供了对字符串数据的只读访问，而不需要复制字符串数据，提高性能
    // std::string_view 是轻量级的，不涉及内存分配和释放操作
    auto delimiter = std::string_view{line.substr(offset, 1)};
    auto is_find_delimiter = false;
    // size_t是无符号整数，通常用于数组索引、循环计数与大小等
    auto current_index = size_t{0};
    auto slash_size = size_t{0};

    while (true) {
        current_index = 0;
        slash_size = 0;
        // 查找下一个delimiter的位置，也就是字符串的结束位置
        offset = line.find(delimiter, offset + delimiter.size());

        // while循环结束，offset指向的是字符串的结束位置
        while (offset < line.size()) {
            // 检查字符串中是否有\，如果有，就是转义字符
            current_index = offset - 1;
            slash_size = 0;
            // look back to find and count slash
            // i.e. "....\\\", offset point to the last " now
            while (line[current_index] == '\\') {
                ++slash_size;
                --current_index;
            }
            
            // 如果\的数量是偶数，那么说明当前找到的这个delimiter是字符串的结束符，否则其被转义了，不是字符串的结束符
            if (slash_size % 2 == 0) {
                // if slash_size is even, then it is not escape sequence
                // and we find the end of string, make offset point to the
                // next position of delimiter
                ++offset;
                is_find_delimiter = true;
                break;
            } else {
                // otherwise it is escape sequence
                // and we need to find the next " at the next position
                offset = line.find(delimiter, offset + delimiter.size());
            }
        }
        if (is_find_delimiter) {
            break;
        }
        // if we reach here, it means we cannot find the delimiter in this line
        // 如果当前行没有找到结束符，那么就继续读取下一行，可用于处理多行字符串
        if (!GetLineAndResetOffset(is, line, offset)) {
            break;
        }
    }

    SetMultiLineCategory(line_begin_, line_end_, LineCategory::kCode);
    line_begin_ = line_end_ - 1;
    return offset;
}

auto CodeAnalyzer::SkipRawString(std::istream& is, std::string& line,
                                 size_t offset) -> size_t {
    // raw string: r"..."
    // offset -> r, but r should not couat as a part of raw string
    auto raw_string_head = std::string{line.substr(offset + 1, 1)};
    // 翻转字符串 
    auto raw_string_tail =
        std::string{raw_string_head.rbegin(), raw_string_head.rend()};
    return SkipUntilFindDelimiter(is, line, offset + 1 + raw_string_head.size(),
                                  raw_string_tail, LineCategory::kCode);
}

auto CodeAnalyzer::SkipLineComment(std::istream& is, std::string& line,
                                   size_t offset) -> size_t {
    // deal with '\' at the end of line
    // 反斜杠在许多编程语言中用作续行符，表示当前行与下一行是连续的
    // 0 fkodlk\
    1 f,l;'sd\
    2 kdsgl;\
    3 lk;dfsg
    // line_begin = 0 , line_end = 4
    // line_begin = line_end - 1 \
        line_begin = 3 , line_end = 4
    // line_begin = line_end \
        line_begin = 4 , line_end = 4
    // 下一轮时：\
        line_begin = 4 , line_end = 5
    while (line.back() == '\\') {
        GetLineAndResetOffset(is, line, offset);
    }

    SetMultiLineCategory(line_begin_, line_end_, LineCategory::kLineComment);
    line_begin_ = line_end_ - 1;
    return std::string::npos;
}

auto CodeAnalyzer::SkipBlockComment(std::istream& is, std::string& line,
                                    size_t offset) -> size_t {
    return SkipUntilFindDelimiter(is, line, offset + block_comment_head_.size(),
                                  block_comment_tail_,
                                  LineCategory::kBlockComment);
}

// 跳过原始字符串、块注释，因为原始字符串和块注释是多行的，所以需要一直读取直到找到结束符
// 原始字符串中一定有(),可能是R"(This is a "raw" "string" literal)"或者R"delimiter(This is a raw string with a delimiter)delimiter"
auto CodeAnalyzer::SkipUntilFindDelimiter(std::istream& is, std::string& line,
                                          size_t offset,
                                          std::string_view const& delimiter,
                                          LineCategory line_category)
    -> size_t {
    // find delimiter, and set offset to the next position of delimiter
    while ((offset = line.find(delimiter, offset)) == std::string::npos) {
        GetLineAndResetOffset(is, line, offset);
    }
    offset += delimiter.size();

    // initial:
    // begin -> ... head ...
    //			...
    //			... tail ...
    // end   -> ...
    SetMultiLineCategory(line_begin_, line_end_, line_category);
    line_begin_ = line_end_ - 1;
    // after:
    //			... head ...
    //			...
    // begin -> ... tail ...
    // end   -> ...
    return offset;
}

auto MakeCodeAnalyzer(std::string const& language)
    -> std::shared_ptr<CodeAnalyzer> {
    if (language == "cpp") {
        return MakeCppAnalyzer();
    } else if (language == "rust") {
        return MakeRustAnalyzer();
    } else if (language == "python") {
        return MakePythonAnalyzer();
    } else if (language == "all") {
        return MakeAllAnalyzer();
    }
    return nullptr;
}

} // namespace stats


/*
如何判断此行的代码类型？
1. 如果是什么都没有，那么计算为空白行
2. 如果是字符串的开头，那么就计算为代码行，这里加了多行字符串的处理，如果当前行找不到字符串的结束符号，那么就继续读取下一行
3. 如果是原始字符串的开头，那么就计算为代码行，同样加了多行字符串处理，即检测原始字符串的结束符号是否在这一行，如果不在，那么就继续读取下一行
4. 如果是行注释的开头，那么就计算为行注释，加了多行注释处理，即在有末尾有“\”的行也都计算为注释行
5. 如果是块注释的开头，那么就计算为块注释，这个也加了多行注释处理，即检测他的段注释结束符号是否在这一行，如果不在，那么就继续读取下一行
6. 如果都不是，那么就是代码行
*/