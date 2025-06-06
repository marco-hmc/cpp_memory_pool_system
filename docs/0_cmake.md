---
layout: post
title: cmake 那些事儿（一）：基本概念
categories: cpp
related_posts: True
tags: cpp cmake
toc:
  sidebar: right
---

## cmake 那些事儿（一）：基本概念

### cheat-sheet

- **与源文件和目录相关的变量**
  - `${CMAKE_CURRENT_SOURCE_DIR}`：表示当前处理的 CMakeLists.txt 文件所在的源文件目录。
  - `${CMAKE_CURRENT_BINARY_DIR}`：对应于当前 CMakeLists.txt 文件对应的构建目录，也就是编译生成目标文件、中间文件等所在的目录。
  - `${CMAKE_SOURCE_DIR}`：整个项目最顶层的源文件目录，是所有源文件目录的根目录，对于定位项目中全局的一些公共源文件、资源文件等非常有用。
  - `${CMAKE_BINARY_DIR}`：整个项目的构建根目录，所有的构建产物最终都会放置在这个目录或者其下的子目录中，类似构建过程中生成的库文件、可执行文件等都会在这个目录结构下，并且可以利用它来设置一些全局的输出布局。
  - `${PROJECT_SOURCE_DIR`\* 最近的定义`project()`的那个`CMakeLists.txt`所在的目录。
  - `${PROJECT_BINARY_DIR`\* 当前项目的构建目录。
  - `${CMAKE_PREFIX_PATH`\* 用于告诉 CMake 去哪里查找一些外部的项目、库或者工具链等相关文件。
  - `${CMAKE_LIBRARY_PATH`\* 指定 CMake 查找库文件的路径。
  - `${CMAKE_FIND_ROOT_PATH}`：在跨平台交叉编译或者查找特定路径下的依赖库时很有用，它定义了一个搜索路径的根路径，CMake 会基于这个根路径去查找库文件、头文件等，比如在嵌入式开发中，指定交叉编译工具链对应的系统根目录路径，通过 `set(CMAKE_FIND_ROOT_PATH /path/to/embedded/root)` 来辅助查找相关的库和头文件。
- **与编译选项相关的变量**
  - `${CMAKE_C_FLAGS}`：用于设置 C 语言编译器的编译选项，比如可以添加优化级别选项 `-O2` ，调试相关选项 `-g` 等，像 `set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -g")`。
  - `${CMAKE_CXX_FLAGS}`：和上面类似，不过是针对 C++ 语言编译器的编译选项，同样可以添加诸如优化、调试、警告处理等相关的参数。
  - `${CMAKE_BUILD_TYPE}`：表示当前的构建类型，常见的值有 `Debug`、`Release`、`RelWithDebInfo`（发布版本带调试信息）、`MinSizeRel`（最小化大小发布版）等，不同的构建类型会应用不同的编译选项，例如在 `Debug` 类型下会默认开启更多的调试相关的编译参数，方便调试程序，而 `Release` 类型则侧重于优化生成的二进制文件性能和减小文件大小等，可以通过 `set(CMAKE_BUILD_TYPE Release)` 来显式指定构建类型。
- **与目标属性相关的变量**
  - `${PROJECT_NAME}`：代表当前项目的名称，这个名称通常是通过 `project()` 命令在 CMakeLists.txt 中定义的，在后续指定生成的目标文件（如库文件、可执行文件）的名称、安装目录等场景中会经常用到，比如 `add_library(${PROJECT_NAME}_lib mylib.cpp)` ，就会以项目名称结合后缀的形式来命名生成的库文件。
  - `${CMAKE_INSTALL_PREFIX}`：用于指定项目安装的根目录，当执行 `make install` 命令时，生成的可执行文件、库文件、头文件等会按照设定的安装规则安装到这个指定的目录及其子目录下，例如 `set(CMAKE_INSTALL_PREFIX /usr/local)` ，那么最终的安装产物就会放置到 `/usr/local` 对应的合适位置（可执行文件到 `bin` 目录、库文件到 `lib` 目录等）。

### 0. 基本概念

- **什么是构建工具？** 需要有一个文件指定编译路径，怎么调用编译器，生成什么对象（库还是可执行文件）等等，这个文件叫构建文件。根据构建文件，调用编译器和连接器生成目标对象的就是构建工具，`make`就是一种构建工具。而生成目标对象`target`可以是静态库、动态库和可执行文件（甚至进一步的，生成对象可以为空，用于表示执行脚本。比如比如 make 工具中的 make clean，资源化，代码检查等等）。

  > 构建工具-构建文件： make-makefile, MSBuild-.vcxproj, Ninja-build.ninja，就是 c++下常见构建工具和构建文件。构建文件描述的生成有三种方式：手工写、gui 操作、脚本生成。比如 makefile 都是手写；而.vcxproj 则是 gui 点击操作；而 CMake 就是脚本生成 make 和.vcxproj 等等。

- **构建工具和文件的生态位置**

  - **Make**：Make 是一个构建工具，它根据 Makefile 调用编译器和链接器来构建项目。
  - **Makefile**：Makefile 是 Make 使用的构建文件，用于描述如何构建项目。Make 读取 Makefile，并根据其中定义的规则调用编译器和链接器。

  - **MSBuild**：MSBuild 是 Microsoft 的构建工具，它根据 .vcxproj 文件（Visual C++ 项目文件）和 .csproj 文件（C# 项目文件）调用编译器和链接器来构建项目。
  - **.vcxproj**：.vcxproj 文件是 Visual C++ 项目的项目文件，用于描述如何构建 Visual C++ 项目。MSBuild 读取 .vcxproj 文件，并根据其中定义的规则调用编译器和链接器。

  - **Ninja**：Ninja 是一个高效的构建工具，它根据 build.ninja 文件调用编译器和链接器来构建项目。
  - **build.ninja**：build.ninja 文件是 Ninja 使用的构建文件，用于描述如何构建项目。Ninja 读取 build.ninja 文件，并根据其中定义的规则调用编译器和链接器。

* **cmake 和构建工具的关系** cmake 是一个工具，就是用来**自动生成构建文件**的，构建工具会根据构建文件调用编译器，链接器等等生成可执行文件、动态库、静态库等等。那 cmake 如何生成构建文件呢？通过写脚本的方式，指定生成目标对象（可执行文件、动态库等等），指定路径，外加一些基本的脚本函数能力便于做一些逻辑处理。

- **发展演化的描述** 我未经考察地做一些发展演化的描述，以便串起这些工具和文件的关系。一开始，构建工具主要是 Make，它使用 Makefile 来描述构建过程。然而，Makefile 的语法和功能有限，针对大型工程非常吃力。于是出现了 CMake，它通过 CMakeLists.txt 文件生成 Makefile。

  与此同时，微软通过 Visual Studio IDE 提供了项目管理功能，允许用户通过 GUI 操作指定哪些文件属于同一个模块，并生成静态库、动态库或可执行文件。相应的数据存储在 `.vcxproj` 文件中。 CMake 的设计目标是生成构建文件描述，因此它不仅可以生成 Makefile，还可以生成 .vcxproj 文件。CMake 支持.vcxproj 也只是顺手之举。

  后来，Google 推出了 Ninja 构建工具，旨在提高构建效率。其实就是改进 make 的。但是 make 的所有权又不是 Google 的。便直接另起炉灶了。

### 1. 生成

#### 1.1 如何生目标？

```cmake
add_library(my_static_lib STATIC source1.cpp) # 定义静态库
add_library(my_shared_lib SHARED source1.cpp) # 定义动态库
add_executable(EXECUTABLE_NAME main.cpp)
add_custom_target()
add_dependencies(<target> <dependency> [<dependency>...])
add_test()
```

- **cmake 中的目标是什么意思？** 目标（Target）是构建系统中的一个基本概念，表示一个可以构建的实体，如可执行文件、库文件。也可以特殊地表示一个自定义命令。

- **如何理解 CMakePredefinedTargets？** 就是 add_custom_target()得到的生成目标在，这个就是搭配自定义脚本使用的，而一般 CMake 会通过这些默认的脚本。
  - **`all_build`**
    - **描述**：`all_build` 目标用于构建项目中的所有目标。
    - **用途**：这是默认的构建目标，通常在执行 `make` 或 `cmake --build .` 时被调用。
  - **`install`**
    - **描述**：`install` 目标用于将构建生成的文件安装到指定的安装目录。
    - **用途**：执行安装步骤，将可执行文件、库文件、头文件等复制到预定义的安装路径。
  - **`package`**
    - **描述**：`package` 目标用于创建项目的分发包（如 `.tar.gz`、`.zip`、`.deb`、`.rpm` 等）。
    - **用途**：生成可分发的安装包，便于发布和分发项目。
  - **`zero_check`**
    - **描述**：`zero_check` 目标用于确保生成的构建系统是最新的。
    - **用途**：在每次构建之前检查 CMakeLists.txt 文件和其他配置文件是否有变化，如果有变化，则重新运行 CMake 以更新构建

#### 1.2 如何如何指定生成路径

```cmake
# 指定头文件路径
target_include_directories(TARGET [PRIVATE|PUBLIC|INTERFACE] INCLUDE_DIRECTORY ...)

# 指令库文件
target_link_libraries(TARGET [PRIVATE|PUBLIC|INTERFACE] LIBRARY_NAME ...)

# 指定找库路径
find_package(LIBRARY_NAME)
find_library(MYLIBRARY_LIB NAMES mylibrary PATHS /path/to/mylibrary)
link_directories(/path/to/libDir)
# CMAKE_PREFIX_PATH 变量用于设置库和头文件的搜索路径。通常用于设置多个库和头文件的搜索路径。
```

#### 1.3 如何设置编译器选项

```cmake
add_definitions(-DDEBUG) # 添加全局宏定义
target_compile_definitions(TARGET_OBJECT PRIVATE DEBUG) # 为特定目标添加宏定义
```

```cmake
set(CMAKE_CXX_STANDARD 17)  # 设置 C++ 标准为 C++17
set(CMAKE_CXX_STANDARD_REQUIRED ON)  # 要求必须支持该标准，否则报错
set(CMAKE_BUILD_TYPE Debug)  # 设置构建类型为 Debug，也可以是 Release、RelWithDebInfo 等
```

- **什么是编译器的编译选项?** `g++ -o2`其中`-o2`就是一个编译器选项,一般还包括优化级别/警告选项/目标平台选项/调试选项等.

- **为什么有时候看见的`ADD_DEFINITIONS`添加的不太像是一个编译器选项?** 那说明编译器本身并不直接识别或处理这个命令，而是将其作为预定义的宏定义传递给代码的预处理阶段。

### 2. cmake 脚本语法

#### 2.1 设置变量 - set

- **SET**

  ```cmake
  SET(VAR_NAME "${AB}") # 第一种写法
  SET(VAR_NAME ${CD}) # 第二种写法
  set(ENV{VAR_NAME} "Hello!")
  set(ENV{VAR_NAME}_xxx "Hello!") # 可拼接
  # 使用的时候，要使用${VAR_NAME}
  ```

  第一种写法和第二种写法是等价的，都是用来设置变量的值。区别在于第一种写法使用了双引号将变量的值括起来，而第二种写法没有使用双引号。使用双引号可以确保变量的值被视为一个整体，而不会被解释为多个独立的字符串。在这个例子中，由于 `${AB}` 和 `${CD}` 是变量，它们的值将被替换为实际的值，然后赋给 `VAR_NAME` 变量。所以在这种情况下，这两种写法是等效的。

#### 2.2 条件操作 - if

- **IF**
  ```cmake
  IF(CONDITION)
    # ...
  ELSE()
    # ...
  ENDIF()
  ```

#### 2.3 循环操作 - while-foreach

- **while**

  ```cmake
  WHILE(CONDITION)
    # ...
  ENDWHILE()
  ```

- **foreach**
  ```cmake
  FOREACH(item IN LISTS list)
    # ...
  ENDFOREACH()
  ```

#### 2.4 宏和函数 - macro-func

    ```cmake
    macro(MacroName arg1 arg2 ...)
      # ...
    endmacro()

    function(FunctionName arg1 arg2 ...)
      # ...
      # return() 可选，用于返回结果
    endfunction()
    ```

#### 2.5 其他 - file

- **file**

  ```cmake
  file(GLOB SOURCES "src/*.cpp")
  ```

  `file(GLOB ...)` 命令用于查找符合指定模式的文件，并将它们的路径存储在变量中。在这个例子中，`SOURCES` 变量将包含 `src` 目录下所有 `.cpp` 文件的路径。

#### 2.6 脚本引用 .cmake

当脚本常用的逻辑函数写好了，想给其他文件用，怎么处理？ `.cmake`就是 Cmake 脚本函数封装文件，用于引入脚本函数。

### 3. 测试 - ctest

而 CTest 是 CMake 自带的测试驱动工具，它能与 CMake 集成，用来管理和运行测试。借助 CMake 生成的构建系统，CTest 可以发现、组织并执行测试用例。

换句话说如果不用 CTest，CMake 指定测试文件生成一个可运行对象，然后和运行程序一样，去运行这个测试的可运行对象也行。这样总归麻烦一些，因此一个 CMake 提供了 CTest，可简化操作，并便于生成测试报告。

### 4. 第三方库引入

随着计算机的发展，很少工程是完全独立的，一定都会引入第三方库。那如何处理第三方库呢？其实也就四种思路：

- A. 下载到系统目录下，cmake 会默认根据系统路径查找。
- A. 直接放到当前工程，第三方库和自己的库一样处理。
- B. 运用 cmake 脚本能力，第三方库写一个脚本用于处理下载和文件移动位置。
- C. 借助第三方工具，如 conan。这些工具会有一个下载中心，统一提供第三方库下载，统一处理文件链接，不会找不到路径。

限于篇幅关系，本文不展开。

### 5. 构建

#### 5.1 cpack

CPack 是 CMake 的一个扩展工具，主要用于创建软件的安装包。它支持多种不同的打包格式，如 ZIP、TGZ、DEB、RPM 等，适用于不同的操作系统和发行版。CPack 可以将编译好的可执行文件、库文件、配置文件等打包成一个或多个安装包，方便用户进行分发和安装。

CMake 提供了与 CPack 相关的 API 和命令，允许你在 CMakeLists.txt 文件中直接配置和调用 CPack。通过在 CMakeLists.txt 中添加相关的 CPack 配置，你可以指定要打包的文件、安装包的名称、版本号、作者信息等，然后使用 cpack 命令或在 CMake 生成的构建系统中调用 CPack 来完成软件的打包工作。

通过 CPack 打包后的安装包，用户不需要获取源码，只需要运行安装程序，就可以将软件安装到自己的系统中并使用。

#### 5.2 安装

CMake 提供了一系列的 `install` 指令，用于将构建生成的目标文件、库文件、目录和配置文件安装到指定的目标位置。以下是对常用 `install` 指令的总结：

- **1. 安装目标文件**

  ```cmake
  install(TARGETS cmake_examples_inst_bin
      DESTINATION bin)
  ```

  - **作用**：将目标文件 `cmake_examples_inst_bin` 安装到 `bin` 目录。

- **2. 安装库文件**

  ```cmake
  install(TARGETS cmake_examples_inst
      LIBRARY DESTINATION lib)
  ```

  - **作用**：将库文件 `cmake_examples_inst` 安装到 `lib` 目录。

- **3. 安装库文件和运行时文件**

  ```cmake
  install(TARGETS cmake_examples_inst
      LIBRARY DESTINATION lib
      RUNTIME DESTINATION bin)
  ```

  - **作用**：将库文件 `cmake_examples_inst` 安装到 `lib` 目录，将运行时文件安装到 `bin` 目录。

- **4. 安装目录**

  ```cmake
  install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/
      DESTINATION include)
  ```

  - **作用**：将目录 `${PROJECT_SOURCE_DIR}/include/` 安装到 `include` 目录。

- **5. 安装文件**

  ```cmake
  install(FILES cmake-examples.conf
      DESTINATION etc)
  ```

  - **作用**：将文件 `cmake-examples.conf` 安装到 `etc` 目录。

- **默认安装位置**

  - **默认安装位置**：默认安装位置由 `CMAKE_INSTALL_PREFIX` 设置，默认值为 `/usr/local/`。
  - **更改默认安装位置**：如果要为所有用户更改默认安装位置，可以在顶层 `CMakeLists.txt` 文件中添加以下代码：

  ```cmake
  set(CMAKE_INSTALL_PREFIX "/desired/install/path" CACHE PATH "Installation Directory" FORCE)
  ```

### 6. cmake 进阶使用-大型工程构建

#### 6.1 什么是可见性？

这个`[PUBLIC|PRIVATE|INTERFACE]`是可见性属性

1. `PRIVATE` include 目录仅应用于目标自身.
2. `PUBLIC` include 目录将应用于目标自身，以及使用目标自己的其他目标
3. `INTERFACE` include 目录仅应用于使用目标的其他目标，不应用于目标自身.

```cmake
add_library(my_lib src.cpp)
target_include_directories(my_lib XXX_ATTR
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(my_executable private my_lib)
```

- 如果`XXX_ATTR`为 private，`my_executable`不可以使用这个头文件路径
- 如果`XXX_ATTR`为 public，`my_executable`也可以使用这个头文件路径
- 如果`XXX_ATTR`为 interface，`my_executable`可以使用这个头文件路径，但是自己不使用。
  - header-only 文件库，实现和定义都在头文件。
  - 接口库，我给了头文件，我不负责实现。
  - 传递编译选项。

#### 6.2 什么是`add_subdirectory()`？

它的作用是将名为`subdir`的子目录添加到当前项目的构建过程中.

- **`add_subdirectory()`的使用场景是什么?** 当你的项目由多个模块或子项目组成时,每个模块有自己的源文件/依赖关系和构建配置,你可以使用`add_subdirectory()`来将每个模块作为子目录添加到主项目中.这样可以将项目的构建过程分解为更小的部分,每个部分有自己的`CMakeLists.txt`文件来管理.例如以下场景

  - 多模块项目:
  - 第三方库或模块:共享功能库:如果你有一些通用的功能或工具代码,你可以将其封装为一个独立的库,并将其作为子目录添加到不同的项目中.
  - 构建和测试工具:

#### 6.3 cmake 的 build 文件说明

- **CMake 中的 cache 有什么用?** 首先 cache 中存储的变量是全局的，父目录子目录都可以查看。

- **.cmake 是什么文件?** `.cmake`文件是一种常见的文件扩展名,用于存储 CMake 相关的配置/宏定义/函数或其他辅助功能.

#### 6.4 cmakePresets.json 使用

### 99. quiz

#### 1. CMakeLists 是大小写敏感的吗?

不完全是,CMake 的语法和关键词是大小写不敏感的.但是变量和函数名是大小写敏感的

- **`set`变量的作用域有多大?**

  - 在根目录的 CMakeLists.txt 文件中使用`set`命令设置的变量默认情况下是全局变量,对整个项目都是可见的.
  - 在子目录的 CMakeLists.txt 文件中设置的变量将在该子目录及其子目录中生效,但不会影响父目录或其他兄弟目录中的变量.
  - 在函数或宏中使用`set`命令设置的变量的作用域仅限于该函数或宏内部,不会影响调用它们的上下文.这意味着这些变量在函数或宏之外是不可见的.

- **`IF(A)`和`IF(${A})`的区别是什么?**

- **`SET(A ${A})`和`SET(A "${A}")`的区别是什么?**

#### 2. cmake 的顺序问题和依赖问题

- cmake 顺序一般是先生成 target，再设置 target 的条件，例如 include 目录，属性等等。
- cmake 依赖
  - add_dependencies() 显式指定
  - target_link_libraries() 隐式指定
  - find_package() 隐式指定

#### 3. 宏和函数在 CMake 的区别是什么？

1. 参数传递方式:宏和函数在参数传递方式上有所不同.在 CMake 中,宏的参数是通过简单的文本替换来传递的,而函数的参数则是通过变量的赋值和引用传递的.
2. 变量作用域:宏和函数在变量作用域上有所不同.宏的定义和调用是在同一个变量作用域内进行的,宏内部的变量对于调用宏的上下文没有影响.而函数具有独立的变量作用域,函数内部的变量只在函数内部有效,不会影响调用函数的上下文.
3. 返回值:宏没有返回值的概念,而函数可以使用`return()`命令来显式返回一个结果.

#### 4. 注意事项

- 不要设置`CMAKE_CXX_FLAGS`，会被覆盖
  - `set(CMAKE_CXX_FLAGS "-std=c++17") ` -> `set(CMAKE_CXX_STANDARD 17)`
  - `set(CMAKE_CXX_FLAGS "-O3") ` -> `set(CMAKE_BUILD_TYPE Release)`

#### 5. ccache

ccache 是一个用来加速 build 阶段的工具。但用处不大。cache 是作为 build 过程的前处理阶段，判断是否有无 cache，有就用上的。那这个阶段为什么不是内置到 make 里面，而是单独作为一个工具呢？如果有用，make 打个补丁加上去不就好了吗。

CMake、GNU Make（gmake）和 ccache 并非相互排斥，通常会一起使用。当由于某种原因需要重新构建整个源代码树时，ccache 就会发挥作用。CMake 和 gmake 只会重新构建已更改的文件，但也存在需要重新编译整个源代码树的情况。如果这种情况反复出现，ccache 就会启动并绕过编译器的部分工作。C++编译器速度慢是出了名的，而 ccache 在这种情况下往往能起到很大的帮助作用。

举几个例子：当你需要反复在开启和关闭优化的情况下进行构建时，ccache 就会发挥作用。当你编辑 Makefile 并调整编译标志时，CMake 和 gmake 帮不上忙，因为实际上没有源文件发生更改，所以 CMake 和 gmake 认为无需进行任何操作，这时你必须明确执行“make clean”并从头开始重新编译。如果你反复进行这样的操作，ccache 将避免对整个源代码运行编译器，而是直接取出合适的目标模块，而不是从头编译源代码。

另一种常见情况是，当你运行一个脚本为你的代码准备可安装包时。这通常涉及使用特定于实现的工具，将源代码从头开始重新构建为可安装包。

再考虑这样一种情况，当你切换到项目的某个较旧分支时——你过去编译过这个分支且 ccache 已经缓存了相关内容，但 CMake 却认为“几乎所有文件都已更改，必须重新编译”——在这种情况下，你就能看到 ccache 带来的巨大好处。

还有一种情况是，当你（出于合理原因）删除了构建目录，现在必须重新构建所有内容时，ccache 也能提供很大帮助。

此外，ccache 的设置非常简单，而且一旦设置好就完全不可见、透明，所以没有理由不使用它。当它发挥作用时，通常效果显著；当它不起作用时，也不会造成任何负面影响。

#### 6. cmake 生成的 build 目录说明
