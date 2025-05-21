#include "../include/IRText.h"

#include <iostream>

#include "irgenerator.h"
#include "TokenType.h"
#include "utils.h"
#include "IndexRepository.h"
//
// Created by ByteDance on 25-5-21.
//
extern IndexRepository INDEX_REPO;

void IRText::resolve_raw_text(const std::string& path, const IROptions& ops) {
    FILE* file = fopen(path.c_str(), "r");
    if (!file) {
        throw std::runtime_error("Failed to open file: " + path);
    }
    int count = 0;
    yyin = file;
    while (const int token = yylex()) {
        if (token == TOKEN_DOCNO)
            doc_no = extract_between_and_check(yytext);
        else if (token == TOKEN_DOCTYPE)
            doc_type = extract_between_and_check(yytext);
        else if (token == TOKEN_TXTTYPE)
            txt_type = extract_between_and_check(yytext);
        else if (token == TOKEN_TEXT_CONTENT) {
            std::string cpp_yytext = yytext;
            tokens.emplace_back(cpp_yytext);
            INDEX_REPO.saveItem(cpp_yytext, Posting(path, count++));
        }
    }
    reset_lexer();
}


