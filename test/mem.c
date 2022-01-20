#include "../src/include/mem.h"
#include "test.h"

START_TEST();

void
mem_at()
{
    Mem m = Mem_new(0);
    Value v10 = Value(Long, 10);
    Vec_push(&m.mem, v10);
    Value d;
    Error r = Mem_mem_at(&m, 2, &d);
    REQUIRE(r == Ok);
    REQUIRE(Value_eq(&d, &v10));
    r = Mem_mem_at(&m, 3, &d);
    REQUIRE(r == Error_InvalidMemAccess);
    REQUIRE(Value_eq(&d, &v10));
}

void
mem_set()
{
    Mem m = Mem_new(0);
    Value v10 = Value(Long, 10);
    Error r = Mem_mem_set(&m, 2, &v10);
    REQUIRE(r == Error_InvalidMemAccess);
    Vec_push(&m.mem, Value(Long, 0));
    r = Mem_mem_set(&m, 2, &v10);
    REQUIRE(Value_eq(Vec_at(&m.mem, 2, Value), &v10));
}

void
readLtl()
{
    Mem m = Mem_new(0);
    Vec s = Vec_from(Value,
            Value(Long, 'a'),
            Value(Long, 's'),
            Value(Long, 'd'),
            Value(Long, 0));
    Mem_mem_push(&m, 4);
    for (int i = 0; i < 4; i++){
        Mem_mem_set(&m, i+2, Vec_at(&s, i, Value));
    }
    Str r = Str();
    Mem_readLtl(&m, -2, &r);
    printf("String: %s\n", Str_at(&r, 0));
    REQUIRE(strcmp(Str_at(&r, 0), "asd") == 0);
}

int
main()
{

    mem_at();
    mem_set();

    readLtl();

    SUM_UP();
    return 0;
}
