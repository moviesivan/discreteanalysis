#ifndef TINTEGER_H
#define TINTEGER_H

#include <cmath>
#include <cstring>
#include <cstdio>
#include "vector.h"

class TInteger
{
public:
	TInteger();
	TInteger(int num);
	TInteger(const char* str);
	
	void Print() const;
	
	TInteger operator + (const TInteger& in) const;
	TInteger operator - (const TInteger& in) const;
	TInteger operator * (const TInteger& in) const;
	TInteger operator / (const TInteger& in) const;
	TInteger operator ^ (const TInteger& in) const;
	
	bool operator < (const TInteger& in) const;
	bool operator > (const TInteger& in) const;
	bool operator == (const TInteger& in) const;

private:
	NDS::TVector<short> mNums;
	void Fix();
	TInteger Power(const TInteger& in) const;
};

#endif // TINTEGER_H
