---
layout: post
title: cmake 那些事儿（三）：提速
categories: cpp
related_posts: True
tags: cpp cmake todo
toc:
  sidebar: right
---

## cmake 那些事儿（三）：提速

### 1. 并行

### 2. pimpl

### 3. pch

### 4. ccache

### 5. unity build

- unity build - supported by cmake

  - 为什么会变快？因为编译单元少了，减少了编译的初始化开销。
  - 减少了头文件的重复解析
  - 更大的编译单元，利于内联函数，常量传播等编译器优化方式。
  - 缺点：
    - 调试：
    - 命名空间

- include what you used
- clang build analyzer
- module c++ 20

1、Windows 平台，使用 increase build ，缺点是无法产生 pdb 符号文件。速度有保障 2、Linux 平台，最近发现一个很有趣的现象，我们写了一个脚本把所有的 cpp 文件写到了一个 CPP 文件中,可以大幅度提高编译速度，缺点是 gdb 调试的时候不方便。使用 distcc 和 ccache 并不能显著提高速度，除非机器数量特别庞大，而且要保证机器环境一致，相比之下更推荐写脚本把 CPP 文件数量减少。
