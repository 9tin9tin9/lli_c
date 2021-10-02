#include "../src/include/op.h"
#include "test.h"

START_TEST();

void
passLexAndExec()
{
    Vec line = Vec_from(Tok, Tok(Sym, Str_fromLtl("nop")));
    Code c = Code_new();
    Code_push(&c, line, 0);
    Mem m = Mem_new();
    Signal s;
    Error r = exec(&m, c, &s);
    REQUIRE(r == Ok);
    REQUIRE(s.type == None);
}

int main(){

    passLexAndExec();

    SUM_UP();
    return 0;
}
