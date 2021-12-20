#include "../src/include/code.h"
#include "../src/include/lex.h"
#include "test.h"

START_TEST();

void
push(){
    Code c = Code_new();
    Vec toks = Vec_from(
            Tok, 
            Tok(Eof, 0),
            Tok(Eof, 0),
            Tok(Num, 10.0));
    size_t opcode = 10;
    size_t len = Code_push(&c, &toks, opcode);
    REQUIRE(len == 1);
    Line l = *Vec_at(&c.lines, 0, Line);
    REQUIRE(Tok_eq(Vec_at(&l.toks, 0, Tok), &Tok(Eof, 0)));
    REQUIRE(Tok_eq(Vec_at(&l.toks, 1, Tok), &Tok(Eof, 0)));
    REQUIRE(Tok_eq(Vec_at(&l.toks, 2, Tok), &Tok(Num, 10.0)));
    REQUIRE(l.opcode == opcode);
}

int
main()
{

    push();

    SUM_UP();
    return 0;
}
