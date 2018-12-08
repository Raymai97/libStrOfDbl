#include <raymai/StrOfDbl.h>
#include <limits.h>

#if defined(LLONG_MAX) && !defined(TEST_NO_LLONG)
typedef long long SMAX;
#define SMAX_MAX  LLONG_MAX
#define SMAX_MIN  LLONG_MIN

#elif defined(_WIN32) && !defined(TEST_NO_I64)
typedef __int64 SMAX;
#define SMAX_MAX  _I64_MAX
#define SMAX_MIN  _I64_MIN

#else
typedef long SMAX;
#define SMAX_MAX  LONG_MAX
#define SMAX_MIN  LONG_MIN

#endif

#undef STROFDBL_FUNCNAME
#define STROFDBL_FUNCNAME  StrOfDbl_SMax
#undef STROFDBL_INTMAXTYPE
#define STROFDBL_INTMAXTYPE  SMAX
#include <raymai/StrOfDbl.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
	MaxBuf = 99
};

#define PROLOG(CharType) \
	int ret; double val; CharType szPri[MaxBuf], szBuf[MaxBuf];

#define TEST2(Func, cch, preci) \
	sprintf(szPri, "%.*f", preci, val); \
	ret = Func(szBuf, (cch), val, (preci))

#define TEST(Func, preci)  TEST2(Func, MaxBuf - 1, preci)

#define ASSERT_OK \
	assert(ret >= 0), szBuf[ret] = 0; \
	printf("[%s] [%s]  Same as asserted \n", szPri, szBuf); \
	assert(strcmp(szBuf, szPri) == 0)

#define ASSERT_OK_MAYBE_DIFF \
	assert(ret >= 0), szBuf[ret] = 0; \
	printf("[%s] [%s]  %s\n", szPri, szBuf, \
		strcmp(szBuf, szPri) ? "Diff" : "Same")

static void TestBasic(void)
{
	PROLOG(char);
	val = 0.0;
	TEST(StrOfDbl, 0), ASSERT_OK;
	TEST(StrOfDbl, 1), ASSERT_OK;
	TEST(StrOfDbl, 2), ASSERT_OK;

	val = -0.0;
	TEST(StrOfDbl, 0), ASSERT_OK;

	val = 1.234;
	TEST(StrOfDbl, 2), ASSERT_OK;
	val = 1.235;
	TEST(StrOfDbl, 2), ASSERT_OK;

	val = 1235.8876;
	TEST(StrOfDbl, 0); ASSERT_OK;
	TEST(StrOfDbl, 1); ASSERT_OK;
	TEST(StrOfDbl, 2); ASSERT_OK;
	TEST(StrOfDbl, 3); ASSERT_OK;
	TEST(StrOfDbl, 4); ASSERT_OK;

	val = 3.141592653500015;
	TEST(StrOfDbl, 6), ASSERT_OK;
	TEST(StrOfDbl, 8), ASSERT_OK;
}

static void TestEdgeCases(void)
{
	PROLOG(char);
	/* 32-bit integer should be totally fine */
	val = (double)((long)0x7fffffff);
	TEST(StrOfDbl, 0), ASSERT_OK;
	TEST(StrOfDbl, 6), ASSERT_OK;
	val = (double)((long)0x80000000);
	TEST(StrOfDbl, 0), ASSERT_OK;
	TEST(StrOfDbl, 6), ASSERT_OK;
	/* If SMAX is bigger than 32-bit, there is no guarantee that */
	/* it will be 100% same as printf. If it is, that's luck. */
	val = (double)SMAX_MAX;
	TEST(StrOfDbl_SMax, 0), ASSERT_OK_MAYBE_DIFF;
	TEST(StrOfDbl_SMax, 6), ASSERT_OK_MAYBE_DIFF;
	val = (double)SMAX_MIN;
	TEST(StrOfDbl_SMax, 0), ASSERT_OK_MAYBE_DIFF;
	TEST(StrOfDbl_SMax, 6), ASSERT_OK_MAYBE_DIFF;
}

int main(void)
{
	TestBasic();
	TestEdgeCases();
	return 0;
}
