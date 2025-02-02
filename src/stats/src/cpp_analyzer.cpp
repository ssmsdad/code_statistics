// 2024/1/3
// zhangzhong

#include "stats/cpp_analyzer.hpp"
#include "stats/code_analyzer.hpp"

namespace stats {

CppAnalyzer::CppAnalyzer() : CodeAnalyzer("//", "/*", "*/") {}

auto CppAnalyzer::IsLineCommentHead(std::string_view const& line, size_t offset)
    -> bool {
    return line[offset] == '/' && line[offset + 1] == '/';
}

auto CppAnalyzer::IsBlockCommentHead(std::string_view const& line,
                                     size_t offset) -> bool {
    return line[offset] == '/' && line[offset + 1] == '*';
}

auto CppAnalyzer::SkipRawString(std::istream& is, std::string& line,
                                size_t offset) -> size_t {
    auto raw_string_tail = GetRawStringTail(line, offset);
    return SkipUntilFindDelimiter(is, line, offset, raw_string_tail,
                                  LineCategory::kCode);
}

auto CppAnalyzer::GetRawStringTail(std::string_view const& line, size_t offset)
    -> std::string {
    // R"de( ... )de";
    auto raw_string_tail = std::string{};
    // offset -> R, begin = d
    auto begin = offset + 2;
    // end ->（
    auto end = line.find('(', begin);

    // construct raw_delimiter: )de"
    raw_string_tail.push_back(')');
    // 将de加入到raw_string_tail中
    raw_string_tail += line.substr(begin, end - begin);
    raw_string_tail.push_back('"');
    // 最终的raw_string_tail为)de"
    return raw_string_tail;
}

auto MakeCppAnalyzer() -> std::shared_ptr<CppAnalyzer> {
    return std::make_shared<CppAnalyzer>();
}

} // namespace stats
