//
// Created by ByteDance on 25-5-21.
//

#ifndef UTILS_H
#define UTILS_H
#include <string>

#include "IRText.h"

std::string extract_between_and_check(const char *c_str);
void reset_lexer();
void traverse_all_files(const Path& root, const IROptions& ops);
void build_idx_repo_from(const Path& root, const IROptions& ops);
#endif //UTILS_H
