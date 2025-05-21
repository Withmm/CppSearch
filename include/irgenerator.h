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
