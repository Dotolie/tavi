extern double __ieee754_log10(double x);
extern double __ieee754_pow(double x, double y);
extern double __floor(double x);
extern int    __isnan(double x);
extern double __ieee754_sqrt( double x );

double jumpto__ieee754_sqrt( double x )
{
	return __ieee754_sqrt( x );
}

double jumpto__fabs( double x )
{
	return __fabs(x);
}

double pow( double x, double y )
{
	return __ieee754_pow( x, y );	
}

int isnan( double value )
{
	return __isnan( value );
}

double log( double x )
{
	return __ieee754_log( x );
}

double log10( double x )
{
	return __ieee754_log10( x );
}

double exp( double x )
{
	return __ieee754_exp( x );
}

double floor( double x )
{
	return __floor( x );
}

