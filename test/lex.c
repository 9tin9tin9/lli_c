#include "../src/include/lex.h"
#include "test.h"

START_TEST();

void 
fromStr()
{
    Tok t;

    Tok_fromStr(&t, &Str_fromLtl("$asd"));
    REQUIRE(Tok_eq(&t, &(Tok){
                Var,
                .Var = HashIdx_new(&Str_fromLtl("asd"), 0)
                }));

    Tok_fromStr(&t, &Str_fromLtl("-123.345"));
    REQUIRE(Tok_eq(&t, &(Tok){
                Num,
                .Num = -123.345
                }));

    Tok_fromStr(&t, &Str_fromLtl("[-123]"));
    REQUIRE(Tok_eq(&t, &(Tok){
                Idx,
                .Idx = (struct Idx){.type = Idx_Type_Num, .Num = -123},
                }));

    Tok_fromStr(&t, &Str_fromLtl("[$asd]"));
    REQUIRE(Tok_eq(&t, &(Tok){
                Idx,
                .Idx = (struct Idx){
                    .type = Idx_Type_Var,
                    .Var = HashIdx_new(&Str_fromLtl("asd"), 0)},
                }));

    Tok_fromStr(&t, &Str_fromLtl("[[-123]]"));
    REQUIRE(Tok_eq(&t, &(Tok){
                Idx,
                .Idx = (struct Idx){
                    .type = Idx_Type_Idx,
                    .Idx = memcpy(
                        malloc(sizeof(struct Idx)),
                        &(struct Idx){.type = Idx_Type_Num, .Num = -123},
                        sizeof(struct Idx))},
                }));

    Tok_fromStr(&t, &Str_fromLtl("\"asd\""));
    REQUIRE(Tok_eq(&t, &(Tok){
                Ltl,
                .Ltl = Str_fromLtl("asd")
                }));

    Tok_fromStr(&t, &Str_fromLtl("A"));
    REQUIRE(Tok_eq(&t, &(Tok){
                Sym, 
                .Sym = HashIdx_new(&Str_fromLtl("A"),0)
                }));
}

void
tokenize()
{
    Vec toks = Vec(Tok);
    Str s = Str_fromLtl("A: -123.234234, $asd, \"asd\\\"asd\"");
    REQUIRE(lex_tokenize(&toks, &s) == Ok);
    REQUIRE(Vec_count(&toks) == 4);

    REQUIRE(Vec_at(&toks, 0, Tok)->tokType == Sym);
    REQUIRE(strcmp(Str_at(&Vec_at(&toks, 0, Tok)->Sym.sym, 0), "A") == 0);

    REQUIRE(Vec_at(&toks, 1, Tok)->tokType == Num);
    REQUIRE(Vec_at(&toks, 1, Tok)->Num == -123.234234);

    REQUIRE(Vec_at(&toks, 2, Tok)->tokType == Var);
    REQUIRE(strcmp(Str_at(&Vec_at(&toks, 2, Tok)->Var.sym, 0), "asd") == 0);

    REQUIRE(Vec_at(&toks, 3, Tok)->tokType == Ltl);
    REQUIRE(strcmp(Str_at(&Vec_at(&toks, 3, Tok)->Ltl, 0), "asd\"asd") == 0);
}

int
main()
{

    fromStr();
    tokenize();

    SUM_UP();
    return 0;
}
