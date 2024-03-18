这个目录包含了TSDuck库的所有源代码。

所有头文件都是库的公共API的一部分，但有少数例外（见下文）。它们随TSDuck开发环境一起安装。

主要子目录包括：

  - base   ：基础C++库，数据结构，系统支持。
  - crypto ：加密代码，大部分提取自 LibTomCrypt。
  - dtv    ：数字电视类，传输流，信号，调谐。
  - plugin ：插件在TSDuck中的集成。

这些目录可能包含额外的子目录，用于功能分类或系统特定目的。在后一种情况下，标准的子目录命名规则如下：

  - private ：库内部使用，不是公共接口的一部分。
  - windows ：Windows特定。
  - unix    ：Unix特定，包括所有Unix平台，如Linux和macOS。
  - linux   ：Linux特定。
  - mac     ：macOS特定。
