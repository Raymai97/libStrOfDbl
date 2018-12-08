/*
	StrOfDbl is a header-only library.
	By default, it defines a static function called StrOfDbl.
	By #define you may define
		the function name,
		the pointer to char type,
		the double type,
		the intmax_t type.
	As such, it may be included more than once.

	By default, 'intmax_t' is defined as 'long'. Since 'long' is 32-bit
	on many platforms, it is highly recommended to #define 'intmax_t' to
	better type like 'long long' or '__int64' to enhance StrOfDbl.

	To ensure the correctness of StrOfDbl, caller must make sure the value is
	conforming to the following restrictions, or undefined behavior may occur.
	* The integer part must be representable by 'intmax_t'.
	* Max precision = min(digit_of_INTMAX_MAX - 2, precision_of_double).

	For reference:
	> With 64-bit IEEE 754 'double' and 32-bit 'intmax_t',
	  max precision is 8 (bottlenecked by intmax_t)
	> With 64-bit IEEE 754 'double' and 64-bit 'intmax_t',
	  max precision is 15 (bottlenecked by double)

	On success:
		The function returns the number of characters that have been written.

	On error:
		The function returns negative value such as StrOfDbl_errBufBoom.
*/

#ifndef STROFDBL_FUNCNAME
#define STROFDBL_FUNCNAME  StrOfDbl
#endif

#ifndef STROFDBL_PSTRTYPE
#define STROFDBL_PSTRTYPE  char *
#endif

#ifndef STROFDBL_DBLTYPE
#define STROFDBL_DBLTYPE  double
#endif

#ifndef STROFDBL_INTMAXTYPE
#define STROFDBL_INTMAXTYPE  long
#endif

#ifndef STROFDBL__ERR_ENUM
#define STROFDBL__ERR_ENUM
enum {
	StrOfDbl_errBufBoom = -1,
	StrOfDbl_errBadArg = -2
};
#endif

#define STROFDBL__CONSUME(c) \
	if (i >= cchStr) { return StrOfDbl_errBufBoom; } \
	pszStr[i++] = (char)(c);

#define STROFDBL__REVERSE(pszStr, i, n) { \
	int tmp; rPStr p1 = &pszStr[i], p2 = &p1[(n) - 1]; \
	while (p1 < p2) { tmp = *p1; *p1 = *p2; *p2 = *(rPStr)&tmp; ++p1; --p2; } }

static int
STROFDBL_FUNCNAME(
	STROFDBL_PSTRTYPE const pszStr,
	int const cchStr,
	STROFDBL_DBLTYPE const dblVal,
	int const preci)
{
	typedef STROFDBL_PSTRTYPE rPStr;
	typedef STROFDBL_DBLTYPE rDbl;
	typedef STROFDBL_INTMAXTYPE rIntMax;
	rDbl dDbl = 0, dZeroes = 0;
	rIntMax dVal = 0;
	int i = 0, j = 0, bewareNegZero = 0;
	if (!pszStr || preci < 0) {
		return StrOfDbl_errBadArg;
	}

	/* while (-0.0f == 0.0f), they are not exactly same on some platform */
	/* where printf will print out negative sign for -0.0f. In order to  */
	/* check if the zero is negative zero, test by (1 / floatValue < 0) */
	/* will work, but this crashes platform where -0.0f is exactly same */
	/* as 0.0f (divide-by-zero is fatal error), hence the test below... */
	{
		typedef unsigned char rB;
		rDbl a = 0.0f, b = -0.0f;
		rB *p1 = (rB*)&a, *p2 = (rB*)&b;
		for (j = 0; j < (int)sizeof(rDbl); ++j) {
			if (p1[j] != p2[j]) {
				bewareNegZero = 1;
			}
		}
	}

	/* dZeroes = pow(10, preci) */
	/* dDbl = round(dblVal, preci) */
	dZeroes = 1;
	for (j = 0; j < preci; ++j) {
		dZeroes *= 10;
	}
	dDbl = dblVal * (dZeroes * 10);
	if ((rIntMax)dDbl % 10 >= 5) {
		dDbl += 10;
	}
	dDbl /= (dZeroes * 10);

	/* stringify Integer part */
	if (bewareNegZero ? (1 / dDbl < 0) : (dDbl < 0)) {
		STROFDBL__CONSUME('-');
	}
	dVal = (rIntMax)dDbl;
	if (dVal == 0) {
		STROFDBL__CONSUME('0'); j = 1;
	}
	else for (j = 0; dVal != 0; ++j) {
		int c = (dVal % 10); if (c < 0) { c = -c; }
		STROFDBL__CONSUME(c + '0');
		dVal /= 10;
	}
	STROFDBL__REVERSE(pszStr, i - j, j);

	/* stringify Fractional part */
	if (preci > 0) {
		STROFDBL__CONSUME('.');
		dVal = (rIntMax)((dDbl - (rIntMax)dDbl) * dZeroes);
		for (j = 0; j < preci; ++j) {
			int c = (dVal % 10); if (c < 0) { c = -c; }
			STROFDBL__CONSUME(c + '0');
			dVal /= 10;
		}
		STROFDBL__REVERSE(pszStr, i - j, j);
	}
	return i;
}

#undef STROFDBL__CONSUME
#undef STROFDBL__REVERSE
