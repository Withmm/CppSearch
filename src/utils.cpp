//
// Created by ByteDance on 25-5-21.
//
#include <filesystem>

#include "utils.h"

#include "IndexRepository.h"
#include "irgenerator.h"
#include "IRText.h"

extern IndexRepository INDEX_REPO;

std::string extract_between_and_check(const char *c_str) {
    const std::string s = c_str;
    const auto start = s.find('>') + 1;
    const auto end = s.rfind('<');
    if (start == std::string::npos || end == std::string::npos || end <= start)
        return "";
    const std::string result = s.substr(start, end - start);

    const size_t first = result.find_first_not_of(" \r\t\n");
    const size_t last  = result.find_last_not_of(" \t\r\n");

    if (first == std::string::npos)
        return "";

    std::string trimmed = result.substr(first, last - first + 1);
    if (trimmed.find(' ') != std::string::npos || trimmed.find('\n') != std::string::npos) {
        return "";
    }
    return trimmed;
}

void reset_lexer() {
    yyin = nullptr;
    yyrestart(nullptr);
    yylex_destroy();
    in_text = 0;
}

void traverse_all_files(const Path& root, const IROptions& ops) {
    for (const auto& entry: std::filesystem::recursive_directory_iterator(root)) {
        if (entry.is_regular_file()) {
            Path path = entry.path();
            INDEX_REPO.irs.emplace_back(path, ops);
        }
    }
}

void build_idx_repo_from(const Path& root, const IROptions& ops) {
    traverse_all_files(root, ops);
}