# CppTextSearch



## 1. 前言

本项目为**WHU 2025**信息检索课程大作业的**轻量实现框架**，旨在提供一个**可复用、可扩展的起点**，供本学期及后续学期的同学参考使用。

当前版本实现了基础的**索引构建模块（IndexRepository）**，支持从**TDT3**语料中提取文本并建立倒排索引。尚未覆盖查询处理、评分排序等完整功能，留有进一步拓展的空间（如 TF-IDF、BM25等），便于根据个人需求进行演进开发。

---



## 2. 项目介绍



### 2.1 环境说明

本工程的**初始环境**为**CLion**创建的空工程，实现过程中引入了**Flex工具**做词法分析。具体可阅读工程的**CMakeLists.txt**

如果要测试，请自行导入数据，在main.cpp里修改数据的目录：

```cpp
#include <iostream>

#include "IRText.h"
#include "utils.h"
#include "IndexRepository.h"

IndexRepository INDEX_REPO;
int main() {
    const IROptions ops;
    build_idx_repo_from("../data/tdt3", ops);
    std::cout << "Generated index successfully!" << std::endl;
    INDEX_REPO.dumpSummaryToDisk();
}
```
`dumpSummaryToDisk` 会把内存中的IndexRepository读到项目根目录的index目录下。


### 2.2 目录说明

`include` 目录存放头文件，主要是c++类型和一些函数的定义和向前声明。

**不要在头文件里写独立于class的函数实现，除非你确信你能处理好模块间的关系。**



`src` 目录结构如下：

```shel
➜  src git:(master) tree .
.
├── IRText.cpp
├── lexer
│   └── irtokengenerator.ll
├── lexgenerated
│   └── irtokengenerator.cpp
├── main.cpp
└── utils.cpp

3 directories, 5 files

```

`lexer` 目录下是Flex工具读取的词法分析的规则输入， `lexgenerated` 下是Flex工具生成的cpp文件。如果修改`irtokengenerator.ll` 在构建工程的时候会自动生成新的`irtokengenerator.cpp` 。
如果需要修改词法分析的逻辑，请修改.ll文件，不要直接修改生成的cpp文件。



### 2.3 代码中需要强调的点



#### 2.3.1 向前声明

C工程的编译向来比较混乱，所以在代码里需要写一堆向前声明。

在这个工程里面，我用**Flex工具**给我生成了一个cpp文件，里面包含了词法分析的实现。但是我在某个文件里使用这些函数的时候，他还不知道有这些函数的存在，所以编译会报错。这个时候就需要向前声明了。

对词法分析需要用到的变量和函数的向前声明在文件`irgenerator.h` 里：

```c
//
// Created by ByteDance on 25-5-21.
//

#ifndef IRGENERATOR_H
#define IRGENERATOR_H

extern FILE* yyin;
extern char* yytext;
extern int in_text;

extern int yylex();
extern void yyrestart(FILE* input_file);
extern int yylex_destroy();
#endif //IRGENERATOR_H
```



#### 2.3.2 IndexRepository实体

前言有谈到，代码框架已经实现了建立一个索引仓库。目前的索引仓库是存在在内存中的，是一个全局的实体，类似单例模式，但是实现更简单，只需要一个全局变量就可以做到。

```cpp
// 这是索引仓库对应的class
struct IndexRepository {
    // very useful
  	// 单词到文档位置的map
    std::unordered_map<Word, std::vector<Posting>> word2posting;
  	// 文档位置到单词的map
    std::unordered_map<Posting, Word> posting2word;
  	// item到索引仓库中单词实体的map
    std::unordered_map<std::string, Word> name2word;

    // maybe useful
  	// 所有的文档实体
    std::vector<IRText> irs;

  	// 进行词法分析的时候做的操作，存储单词和文档的关系到索引仓库。
    void saveItem(const std::string& item, const Posting& posting) {
        if (!name2word.contains(item)) {
            name2word[item] = Word(item);
        }
        Word& word = name2word[item];
        word.where[posting.doc_path] += 1;
        ++word.total_hit;

        word2posting[word].push_back(posting);
        posting2word[posting] = word;
    }
};
```

我目前在main.cpp里对索引仓库进行定义：

```cpp
#include <iostream>

#include "IRText.h"
#include "utils.h"
#include "IndexRepository.h"

IndexRepository INDEX_REPO;
int main() {
    const IROptions ops;
    build_idx_repo_from("../data/tdt3", ops);
    std::cout << "Generated index successfully!" << std::endl;
    INDEX_REPO.dumpSummaryToDisk();
}
```

这里相当于用C写的单例，你可以换成更c++的写法，在class用一个静态变量当单例用。**更规范，更现代**。但是我要快速把框架搭建起来就写得比较随意。



## 3. 拓展思路



### 3.1 On-Disk IndexRepository（难度：3星）

前面提到，我们当前实现的索引仓库是以**进程内全局变量的形式**存在的，它完全驻留在内存中。因此在程序退出时，所有索引数据都会随之销毁，不具备持久化能力。

尽管这种内存索引结构实现简单、访问速度快，但它也带来了两个明显的局限：

1. **无法持久化**：每次启动程序都必须重新构建索引，代价高，启动慢；

2. **受限于内存大小**：大规模语料下，容易触发内存瓶颈甚至崩溃。

为了提升系统可扩展性和实用性，我们可以考虑将IndexRepository**从纯内存结构迁移为磁盘存储结构**。这通常需要设计：

1. 更复杂的数据结构（如倒排文件、跳表、压缩编码）

2. IO 缓存策略

3. 数据格式与编码方案（如基于段的分片存储）

对于期末作业的要求，这个是加分项不是必须项，但是作为搬砖练手，培养编程能力还是值得一做的。

---



### 3.2 并行化设计 (难度：2星)

在完成这个工程之后，我和PDF中推荐的Java的库`Lucene`生成索引的效率对比了一下。尽管这种成熟的库经过优化之后肯定会比我们手写的简易实现更快，但是还是快的有点太多了。 理论上来说我做的事情就是把文件遍历了一遍，一边遍历一边生成一些Index，不会有太高的性能开销，就算有也是STL的性能开销（~~反正不是我写的菜~~）。

所以我认为主要是并发和IO的问题。

来看我读取文件的逻辑：

```cpp
void traverse_all_files(const Path& root, const IROptions& ops) {
    for (const auto& entry: std::filesystem::recursive_directory_iterator(root)) {
        if (entry.is_regular_file()) {
          	// 这里不可避免的要触发IO逻辑，而且每一个doc都会触发一次，是典型的IO密集场景
          	// 所以这里要考虑并行。
            Path path = entry.path();
            INDEX_REPO.irs.emplace_back(path, ops);
        }
    }
}
```

诶！到这里是不是发现非常简单，为什么我不顺便做一下呢？ 当然不是这么简单，我们通过Flex提供的变量和函数做词法分析，当然要考虑可重入的问题。好消息是Flex是支持可重入的，坏消息是默认是不可重入的，如果要让Flex可重入，支持并行，**有些接口需要变化**。

---



### 3.3 模式选择（难度： 1星）

这个是期末大作业要求， PDF里提供了几种模式，

1. Perform or ignore stemming

2. Distinguish or ignore upper/lower case characters

3. Process or ignore numbers

4. Process or ignore punctuations including hyperns

后面几个都很好做， 因为tokens已经暴露给我们了，我们收到token的时候做相应的处理就行了。

第一个需要提取词干， 这个逻辑我们可以引入一些轻量级的实现库。

比如： [一百来行实现stemming](https://tartarus.org/martin/PorterStemmer/)， 直接copy过来用就可以了

---



### 3.4 score计算（难度：？）

这个也是期末大作业要求， 原文是： *implement the document relevance score function by yourself*

score计算是个算法问题， 前提是你遍历doc的时候能够取到你需要的数据，所以是个综合的过程。目前框架提取到的数据。可以实现

Term Frequency (TF) 和 TF-IDF算法。如果要实现更复杂的算法，可能需要修改词法分析的时候的逻辑。

