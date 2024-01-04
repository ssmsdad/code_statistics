// 2024/1/2
// zhangzhong

#include "CLI/App.hpp"
#include "CLI/Config.hpp"
#include "CLI/Formatter.hpp"
#include "conf/conf.hpp"
#include "driver/driver.hpp"
#include <CLI/ConfigFwd.hpp>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

int main(int argc, const char* argv[]) {
    CLI::App app{"A simple code statistics tool"};

    // positional option
    std::string path;
    app.add_option("path", path, "Path to analyze")->required();
    // 怎么描述这个是required

    // container options
    // -i path1 path2 path3 ...
    // std::vector<std::string> ignored_paths;
    // app.add_option("-i,--ignore", ignored_paths, "ignored paths");

    std::string language = "cpp";
    app.add_option(
        "-l,--language", language,
        "Language to analyze, support [cpp], [rust], and default is cpp");

    try {
        // 在windows上不能这么用
        app.parse(argc, argv);

        auto conf = conf::MakeConf();
        conf->AddLoadPath(path);
        conf->AddLanguage(language);

        auto driver = driver::Driver(conf, stats::MakeCodeAnalyzer(language));
        driver.Run();

    } catch (const CLI::ParseError& e) {
        return app.exit(e);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    return 0;
}
