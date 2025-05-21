//
// Created by ByteDance on 25-5-21.
//

#ifndef IRTEXT_H
#define IRTEXT_H
#include <vector>
#include "DocNo.h"
#include "DocType.h"
#include "TxtType.h"
using Path = std::string;

struct IROptions {
    bool ignore_stemming;
    bool ignore_upper_lower_case;
    bool ignore_nums;
    bool ignore_punctuations;

    IROptions():
    ignore_stemming(false),
    ignore_upper_lower_case(false),
    ignore_nums(false),
    ignore_punctuations(false) {}
};

struct IRText{
    std::vector<std::string> tokens;

    DocNo doc_no;
    DocType doc_type;
    TxtType txt_type;
    Path path;
    explicit IRText(const std::string& path, const IROptions& ops) : path(path) {
        resolve_raw_text(path, ops);
    }

    void resolve_raw_text(const std::string& path, const IROptions& ops);
};
#endif //IRTEXT_H
