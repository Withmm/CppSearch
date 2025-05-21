%option noyywrap

%{
#include "TokenType.h"
int in_text = 0;

%}

%%

"<DOCNO>"[^<]*"</DOCNO>"        { return TOKEN_DOCNO; }
"<DOCTYPE>"[^<]*"</DOCTYPE>"    { return TOKEN_DOCTYPE; }
"<TXTTYPE>"[^<]*"</TXTTYPE>"    { return TOKEN_TXTTYPE; }

"<TEXT>"                          { in_text = 1; return TOKEN_TEXT_OPEN; }
"</TEXT>"                         { in_text = 0; return TOKEN_TEXT_CLOSE; }

[a-zA-Z0-9_.\-\"\!\.]+            { if (in_text) return TOKEN_TEXT_CONTENT; }

.|\n                              { /* ignore */ }

%%
