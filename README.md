# Code Statistics
A simple code statistics tool

# Example Usage
`code_statistics src`

running this command, tool will recursively traverse all cpp files and do statistics under src directory.

for example, like this code

src/main.cpp
```
#include <iostream> // include for std::cout and std::endl

int main(int argc, char* argv[]) {
    // print hello world
    std::cout << "hello, world" << std::endl;
    return 0;
}
```
the second line is blank line, and the forth line is comment line, and the rest lines are all code lines. so the output of tool is something like this:
```
stats... src/main.cpp

files: 1
lines: 7
codes: 5 (71.43%)
comments: 2 (28.57%)
```

for more infomation, use `code_statistics -h` to see help.

# Build

1. install conan and configure it well, `pip install conan`. for more infomation, see [conan](https://conan.io/)
2. download and install my conan tools: [conan tools](https://github.com/im-zhong/conan.git)
3. clone this repo to your local machine `git clone https://github.com/im-zhong/code_statistics.git`
4. `cd code_statistics`
5. `conan install . --build=missing`
6. `conan build .`

# Personal Mark

这是第一个完全读懂的c++项目，甚至还参与了一小部分的开发

虽然是个小项目，但是也值得记录一下，希望以后能读懂大项目，到最后甚至可以参与大项目的开发

# 有个问题

该怎么调试？cmakelists文件中只要写了#set(CMAKE_BUILD_TYPE Debug)，就报错：
'''cpp
/home/zhangwenzhe/learning_c++/code_statistics/src/stats/src/code_analyzer.cpp:11:10: fatal error: fmt/core.h: No such file or directory
11 | #include <fmt/core.h>
'''
这是为什么呢？

# 个人总结

**如何判断此行的代码类型？**

    -1、遍历这一行中的所有字符，对于每一个字符，都会设置一次result_->line_categories[line]的值，如果是什么都没有，那么计算为空白行
    -2、如果当前的字符不属于字符串，也不是任何的注释符号，那么就设置为代码行（这里走的是else），如果后边还有注释（走IsLineCommentHead），就设置注释行，二者按位取或，变为3，即代码行和注释行
    -3、如果当前的字符属于字符串（走IsStringHead），那么就跳过这个字符串，设置为代码行，
    -4、如果当前的字符是原始字符串（走IsRawStringHead），那么就跳过这个原始字符串，设置为代码行，
    -5、如果当前的字符是行注释（走IsLineCommentHead），那么就跳过这个行注释，设置为行注释，
    -6、如果当前的字符是块注释（走IsBlockCommentHead），那么就跳过这个块注释，设置为块注释，
    -7、以上都添加了多行的处理，如果当前行找不到指定的结束符号，那么就继续读取下一行
    -8、但是对于每个字符都要更新一次result_->line_categories[line]的值，会不会很慢？