#include "../src/include/mem.h"
#include "test.h"

START_TEST();

void
pmem_at()
{
    Mem m = Mem_new();
    Vec_push(&m.pmem, 10.0);
    double d;
    Error r = Mem_pmem_at(&m, 1, &d);
    REQUIRE(r == Ok);
    REQUIRE(d == 10.0);
    r = Mem_pmem_at(&m, 2, &d);
    REQUIRE(r == Error_InvalidMemAccess);
    REQUIRE(d == 10.0);
}

void
pmem_set()
{
    Mem m = Mem_new();
    Error r = Mem_pmem_set(&m, 1, 10.0);
    REQUIRE(r == Error_InvalidMemAccess);
    Vec_push(&m.pmem, 0.0);
    r = Mem_pmem_set(&m, 1, 10.0);
    REQUIRE(*Vec_at(&m.pmem, 1, double) == 10.0);
}

void
nmem_at()
{
    Mem m = Mem_new();
    Vec_push(&m.nmem, 10.0);
    double d;
    Error r = Mem_nmem_at(&m, 1, &d);
    REQUIRE(r == Ok);
    REQUIRE(d == 10.0);
    r = Mem_nmem_at(&m, 2, &d);
    REQUIRE(r == Error_InvalidMemAccess);
    REQUIRE(d == 10.0);
}

void
nmem_set()
{
    Mem m = Mem_new();
    Error r = Mem_nmem_set(&m, 1, 10.0);
    REQUIRE(r == Error_InvalidMemAccess);
    Vec_push(&m.nmem, 0.0);
    r = Mem_nmem_set(&m, 1, 10.0);
    REQUIRE(*Vec_at(&m.nmem, 0, double) == 10.0);
}

void
readLtl()
{
    Mem m = Mem_new();
    Vec s = Vec_from(double, 'a', 's', 'd', 0);
    Mem_nmem_alloc(&m, &s);
    Mem_nmem_push(&m, 0);
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
