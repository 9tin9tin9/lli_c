#include "../src/include/mem.h"
#include "test.h"

START_TEST();

void
pmem_at()
{
    Mem m = Mem_new();
    Value v10 = Value(Long, 10);
    Vec_push(&m.pmem, v10);
    Value d;
    Error r = Mem_pmem_at(&m, 1, &d);
    REQUIRE(r == Ok);
    REQUIRE(Value_eq(&d, &v10));
    r = Mem_pmem_at(&m, 2, &d);
    REQUIRE(r == Error_InvalidMemAccess);
    REQUIRE(Value_eq(&d, &v10));
}

void
pmem_set()
{
    Mem m = Mem_new();
    Value v10 = Value(Long, 10);
    Error r = Mem_pmem_set(&m, 1, &v10);
    REQUIRE(r == Error_InvalidMemAccess);
    Vec_push(&m.pmem, Value(Long, 0));
    r = Mem_pmem_set(&m, 1, &v10);
    REQUIRE(Value_eq(Vec_at(&m.pmem, 1, Value), &v10));
}

void
nmem_at()
{
    Mem m = Mem_new();
    Value v10 = Value(Long, 10);
    Vec_push(&m.nmem, v10);
    Value d;
    Error r = Mem_nmem_at(&m, 1, &d);
    REQUIRE(r == Ok);
    REQUIRE(Value_eq(&d, &v10));
    r = Mem_nmem_at(&m, 2, &d);
    REQUIRE(r == Error_InvalidMemAccess);
    REQUIRE(Value_eq(&d, &v10));
}

void
nmem_set()
{
    Mem m = Mem_new();
    Value v10 = Value(Long, 10);
    Error r = Mem_nmem_set(&m, 1, &v10);
    REQUIRE(r == Error_InvalidMemAccess);
    Vec_push(&m.nmem, Value(Long, 0));
    r = Mem_nmem_set(&m, 1, &v10);
    REQUIRE(Value_eq(Vec_at(&m.nmem, 0, Value), &v10));
}

void
readLtl()
{
    Mem m = Mem_new();
    Vec s = Vec_from(Value,
            Value(Long, 'a'),
            Value(Long, 's'),
            Value(Long, 'd'),
            Value(Long, 0));
    Mem_nmem_alloc(&m, &s);
    Str r = Str();
    Mem_readLtl(&m, -1, &r);
    printf("String: %s\n", Str_at(&r, 0));
    REQUIRE(strcmp(Str_at(&r, 0), "asd") == 0);
}

int
main()
{

    pmem_at();
    pmem_set();

    nmem_at();
    nmem_set();

    readLtl();

    SUM_UP();
    return 0;
}
