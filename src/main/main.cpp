// 2024/1/2
// zhangzhong

#include "conf/conf.hpp"
#include "driver/driver.hpp"
#include <CLI/App.hpp>
#include <CLI/Config.hpp>
#include <CLI/ConfigFwd.hpp>
#include <CLI/Formatter.hpp>
#include <iostream>
#include <string>

int main(int argc, const char* argv[]) {
    // CLI::App是一个命令行解析器，用于解析命令行参数
    auto app = CLI::App{"A simple code statistics tool"};

    auto path = std::string{};
    app.add_option("path", path, "Path to analyze")->required();

    auto language = std::string{"cpp"};
    app.add_option("-l,--language", language,
                   "Language to analyze, support [cpp], [rust], [python], and "
                   "default is cpp");

    try {
        app.parse(argc, argv);

        auto conf = conf::MakeConf();
        conf->AddLoadPath(path);
        conf->AddLanguage(language);

        auto driver = driver::Driver(conf, stats::MakeCodeAnalyzer(language));
        driver.Run();
    } catch (const CLI::ParseError& e) {
        // app.exit(e)会打印错误信息并退出程序
        return app.exit(e);
    } catch (const std::exception& e) {
        // e.what()返回异常信息
        std::cerr << e.what() << std::endl;
        return -1;
    }
    return 0;
}
