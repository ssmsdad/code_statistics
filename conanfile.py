from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
import os

# 这个文件的layout、generate、build、package方法是ConanFile类的方法，用于定义构建和打包的流程
# 这四个函数几乎不用任何的修改，适用于所有的项目，只需要在类的属性中定义项目的名称、版本、依赖项等信息即可

class mycppRecipe(ConanFile):
    name = "CodeStatistics"
    version = "0.2.0"
    package_type = "application"

    # Optional metadata
    license = "GPL-3.0"
    author = "im.zhong@outlook.com"
    url = "https://github.com/im-zhong/code_statistics"
    description = "A simple code statistics tool"
    topics = ("C++", "Code", "Statistics")

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = "CMakeLists.txt", "src/*"

    # "ConanFile" 是类型注释，用于告诉编辑器这是一个 ConanFile 类型的对象
    def requirements(self: ConanFile):
        self.requires("cli11/2.3.2")
        self.requires("doctest/2.4.11")
        self.requires("fmt/10.2.1")

    def layout(self):
        # 根据 CMake 的惯例设置源代码、构建和包的目录结构，会自动创建 build 目录
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        # 生成 CMake 的依赖文件，这些文件通常包含项目所需的外部库和依赖项的信息
        deps.generate()
        tc = CMakeToolchain(self)
        # 生成 CMake 的工具链文件，这些文件通常包含项目所需的编译器和构建选项
        tc.generate()

    def build(self):
        cmake = CMake(self)
        # 相当于cmake ..，生成makefile文件
        cmake.configure()
        # 相当于cmake --build .，编译
        cmake.build()
        # Create a symlink to the compile_commands.json file
        # clangd 使用 compile_commands.json 文件来提供代码补全、跳转和静态分析功能
        db_path = os.path.join(
            "build", str(self.settings.build_type), "compile_commands.json")
        if os.path.exists("../../compile_commands.json"):
            os.unlink("../../compile_commands.json")
        os.symlink(db_path, "../../compile_commands.json")

    def package(self):
        cmake = CMake(self)
        # 相当于cmake --install .，安装构建生成的文件，以便于分发，部署或使用
        cmake.install()
