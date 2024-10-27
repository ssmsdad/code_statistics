# 将src目录下的conf.cpp编译成静态库，名为conf
# 静态库在链接时会将库中的所有目标文件都链接到可执行文件中，动态库则不会，只有在运行时才会加载动态库中的目标文件
add_library(conf STATIC 
    src/conf.cpp
)
# 设置conf库的头文件路径，public表示该路径会被链接到依赖conf库的目标中
target_include_directories(conf 
    PUBLIC include
)
