---
layout: post
title: cmake 那些事儿（二）：外部库管理和使用
categories: cpp
related_posts: True
tags: cpp cmake
toc:
  sidebar: right
---

## cmake 那些事儿（二）：外部库管理和使用

### 1. 系统路径引入

- find_package
- find_library

```cmake
find_package(<PackageName> [version] [REQUIRED] [COMPONENTS components...])
```

- `<PackageName>`：要查找的包的名称。
- `[version]`：可选，指定所需的包版本。
- `[REQUIRED]`：可选，如果指定，找不到包时会导致配置失败。
- `[COMPONENTS components...]`：可选，指定要查找的包组件。

`find_package` 命令用于查找和加载外部库或包。它的本质是在模块路径下查找名为 `Find<PackageName>.cmake` 的脚本文件，并执行该脚本文件。

- **变量设置** 这个脚本文件通常会设置一些变量，以便在后续的构建过程中使用。

* `<PackageName>_FOUND`：指示是否找到指定的包。
* `<PackageName>_LIBRARIES`：指定包的库文件路径。
* `<PackageName>_INCLUDE_DIRS`：指定包的头文件路径。基于这些变量去设置库文件路径和头文件即可。

      ```cmake
      target_include_directories(my_executable PRIVATE ${MyCustomLib_INCLUDE_DIRS})
      target_link_libraries(my_executable PRIVATE ${MyCustomLib_LIBRARIES})
      ```

- **别名和导入机制** 新的 `Find<PackageName>.cmake` 脚本文件通常会提供别名和导入机制：

* **别名（Alias）**：可以重命名生成对象目标的名字，使引用目标更加直观和一致。
* **导入机制（Imported Targets）**：将指定库的 `include_directories` 和 `link_directories` 结合起来，具体表现为库的生成对象添加一个 `IMPORTED` 关键词，并设置导入属性（如 `INTERFACE_INCLUDE_DIRECTORIES` 和 `IMPORTED_LOCATION`）。

- **查找.cmake 文件路径** `find_package` 命令会在以下位置查找包：
  1.  **CMake 模块路径**：CMake 自带的模块路径，通常位于 CMake 安装目录下的 `Modules` 目录。
  2.  **用户定义的模块路径**：可以通过设置 `CMAKE_MODULE_PATH` 变量来指定额外的模块路径。

#### 1.1 例子

自己的 CMakeLists.txt 需要声明使用 boost 库 ```cmake # CMakeLists.txt cmake_minimum_required(VERSION 3.10) project(MyProject)

    find_package(Boost 1.70 REQUIRED COMPONENTS filesystem system) # 查找 Boost 库
    add_executable(my_executable main.cpp) # 添加可执行文件
    target_link_libraries(my_executable PRIVATE Boost::filesystem Boost::system) # 链接 Boost 库到可执行文件
    ```

那么在 cmake 安装目录的 modules 目录下，一般会有一个 FindBoost.cmake 脚本文件。这个脚本文件会有这些 ```cmake # FindBoost.cmake

    # 查找 Boost 库文件
    find_library(Boost_FILESYSTEM_LIBRARY NAMES boost_filesystem)

    # 查找 Boost 头文件路径
    find_path(Boost_INCLUDE_DIR boost/filesystem.hpp)

    # 定义导入目标
    add_library(Boost::filesystem SHARED IMPORTED)
    set_target_properties(Boost::filesystem PROPERTIES
        IMPORTED_LOCATION ${Boost_FILESYSTEM_LIBRARY}
        INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR}
    )
    ```

CMakeLists.txt.in 有什么用

### 2. 工程路径引入

把第三方库的源码或者预编译文件直接放在项目目录里，当作项目的一部分处理。 **术语**：

- **Submodule**：在 Git 里，可使用子模块（Submodule）把第三方库作为一个独立的仓库引入到项目里。
- **Vendor**：把第三方库的代码复制到项目中，这种方式也被叫做“Vendor”（供应商）。

借助 CMake 脚本的功能，编写自定义脚本来处理第三方库的下载、编译和安装。 **术语**：

- **ExternalProject**：CMake 提供了`ExternalProject`模块，能够在构建项目时自动下载、编译和安装第三方库。
- **FetchContent**：从 CMake 3.11 开始，提供了`FetchContent`模块，让你能在 CMake 脚本里直接下载和使用第三方库。

### 3. 第三方工具引入 - conan

利用第三方包管理工具，像 Conan、vcpkg 等，来管理项目的依赖。 **术语**：

- **Conan**：是一个开源的 C/C++包管理工具，有自己的包仓库，能帮助你管理和分发第三方库。
- **vcpkg**：是微软开发的开源 C++包管理工具，支持多平台，能简化第三方库的获取和使用。

Conan 是一个开源的 C/C++ 包管理器，旨在帮助开发者管理和分发库。它可以用于解决依赖关系、版本控制和跨平台构建等问题。应用场景包括但不限于：

- 管理项目的第三方库依赖
- 构建和发布内部库
- 支持跨平台开发

#### 3.1 conan 怎么用？

- **基本使用步骤**

1. `创建conanfile.txt`

```ini
# `conanfile.txt`
[requires]
fmt/5.3.0@bincrafters/stable

[generators]
cmake
```

2. `安装依赖项`

```shell
    conan install .
```

这将下载并安装 fmt 库及其依赖项，并生成一个 `conanbuildinfo.cmake` 文件，用于集成到 CMake 构建系统中。

3. `集成到cmake构建系统`

```cmake
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
```

- **相关文件有哪些？**

* `conanfile.txt`：简化的包定义文件，主要用于列出依赖。
* `conanfile.py`：定义包的配方，包括依赖、构建选项、源代码等。
* `conan.lock`：锁定依赖版本，确保构建的一致性。
* `conan-profile`：Conan 的 profile 是用于定义包构建和运行时的配置参数的文件。它包含了编译器设置、编译选项、环境变量等信息。使用 profile 可以确保在不同的环境中构建和运行包时的一致性和可重复性。

- **常用命令有哪些？**

* `conan install`：安装依赖包。
* `conan create`：创建并打包库。
* `conan upload`：上传包到远程仓库。
* `conan search`：搜索包。
* `conan remove`：删除本地或远程包。

- **conan-profile 怎么用？**

* **一致性**：确保在不同的机器和环境中构建包时使用相同的配置。
* **可重复性**：可以轻松地重现构建环境，方便调试和测试。
* **简化配置**：通过 profile 文件，可以避免在命令行中重复输入配置参数。

你可以通过 `conan profile` 命令来管理 profile，例如创建、编辑和列出 profile。

Conan 的 profile 是全局的，默认情况下存储在用户主目录下的 `.conan2/profiles` 目录中。因此，所有工作空间都可以访问和使用这些 profile。

不过，你也可以在项目级别创建和使用自定义的 profile。只需在项目目录中创建一个 profile 文件，然后在运行 Conan 命令时指定该 profile。例如：

#### 3.2 attach files

```ini
    ## conanfile.txt
    [requires]
    # 列出项目所需的依赖包
    boost/1.75.0
    poco/1.10.1

    [generators]
    # 指定生成的构建文件类型
    cmake

    [options]
    # 定义包的可选配置
    boost:shared=True
    poco:shared=True

    [imports]
    # 指定要导入的文件
    bin, *.dll -> ./bin
    lib, *.dylib* -> ./lib
```

```ini
    ## conanfile-profile
    [settings]
    # 指定编译器、操作系统、架构等信息
    os=Linux
    arch=x86_64
    compiler=gcc
    compiler.version=10
    compiler.libcxx=libstdc++11
    build_type=Release

    [options]
    # 定义包的可选配置
    boost:shared=True
    poco:shared=True

    [build_requires]
    # 列出构建所需的工具包
    cmake/3.19.2

    [env]
    # 设置环境变量
    CC=/usr/bin/gcc
    CXX=/usr/bin/g++
```

### 99. quiz

#### 1. 为什么 Conan 推广不开，或者说 C++ 的包管理工具统一起来这么难？

C++ 的包管理工具推广困难，主要原因在于以下几个方面：

1. **平台差异**：
   - C++ 的包不像 Java 是平台无关的。每个平台（如 Windows、Linux、macOS）都有其特定的库和依赖，这导致需要为每个平台提供不同的包版本。
   - 操作系统层面的平台差异是情有可原的，但这只是问题的一部分。
2. **编译器差异**：
   - 不同的 C++ 编译器（如 GCC、Clang、MSVC）之间互不兼容。不同编译器的 ABI（应用二进制接口）不一样，库内部使用的编译器指令不同，内部调用的库指令也不同。
   - 这意味着一个包需要提供平台数与编译器数的乘积那么多个版本，才能覆盖所有可能的组合。
3. **依赖管理复杂**：
   - 由于平台和编译器的多样性，一个包的依赖管理变得非常复杂。每个依赖项也需要考虑平台和编译器的兼容性，这进一步增加了管理的难度。
4. **现有工具的局限性**：
   - Windows 平台一般使用 vcpkg，它主要支持 Windows 平台和 MSVC 编译器。
   - 如果是自己发布和管理包，通常使用 Conan，可以自己配置源服务器和包管理策略。
