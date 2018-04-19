#include "ing_integer.h"

TInteger::TInteger()
{
	mNums.PushBack(0);
}

TInteger::TInteger(int num)
{
	if (!num)
	{
		mNums.PushBack(0);
	}
	else
	{
		while (num)
		{
			mNums.PushBack(num % 10000);
			num /= 10000;
		}
	}
}

TInteger::TInteger(const char* str)
{
	int len = strlen(str);

	for (int i = len; i > 0; i -= 4)
	{
		short num = 0;

		for (int j = (i - 4 > 0 ? i - 4 : 0); j < i; ++j)
		{
			num = num * 10 + str[j] - '0';
		}

		mNums.PushBack(num);
	}

	Fix();
}

void TInteger::Print() const
{
	int size = mNums.Size();

	printf("%d", mNums[size - 1]);

	for (int i = size - 2; i >= 0; --i)
	{
		printf("%04d", mNums[i]);
	}

	printf("\n");
}

TInteger TInteger::operator + (const TInteger& in) const
{
	short carry = 0;
	int maxSize = mNums.Size() > in.mNums.Size() ? mNums.Size() : in.mNums.Size();
	TInteger res;

	res.mNums.PopBack();

	for (int i = 0; i < maxSize; ++i)
	{
		short x = i < mNums.Size() ? mNums[i] : 0;
		short y = i < in.mNums.Size() ? in.mNums[i] : 0;
		short tmp = x + y + carry;

		carry = tmp / 10000;
		res.mNums.PushBack(tmp % 10000);
	}

	if (carry)
	{
		res.mNums.PushBack(carry);
	}

	return res;
}

TInteger TInteger::operator - (const TInteger& in) const
{
	short carry = 0;
	TInteger res;

	res.mNums.PopBack();

	for (int i = 0; i < mNums.Size(); ++i)
	{
		short y = i < in.mNums.Size() ? in.mNums[i] : 0;
		short tmp = mNums[i] - y + carry;

		carry = tmp < 0 ? -1 : 0;
		res.mNums.PushBack(tmp < 0 ? tmp + 10000 : tmp);
	}

	res.Fix();

	return res;
}

TInteger TInteger::operator * (const TInteger& in) const
{
	int m = mNums.Size();
	int n = in.mNums.Size();
	int maxSize = m > n ? m : n;
	TInteger res;

	res.mNums.Resize(maxSize * 2, 0);

	for (int j = 0; j < n; ++j)
	{
		if (in.mNums[j] != 0)
		{
			short carry = 0;

			for (int i = 0; i < m; ++i)
			{
				int tmp = mNums[i] * in.mNums[j] + res.mNums[i + j] + carry;

				carry = tmp / 10000;
				res.mNums[i + j] = tmp % 10000;
			}

			res.mNums[j + m] = carry;
		}
	}

	res.Fix();

	return res;
}

TInteger TInteger::operator / (const TInteger& in) const
{
	int norm = 10000 / (in.mNums.Back() + 1);

	TInteger a = *this * norm;
	TInteger b = in * norm;
	TInteger q;
	TInteger r;

	q.mNums.Resize(a.mNums.Size());

	for (int i = a.mNums.Size() - 1; i >= 0; --i)
	{
		r = r * 10000;
		r = r + a.mNums[i];

		short s1 = r.mNums.Size() <= b.mNums.Size() ? 0 : r.mNums[b.mNums.Size()];
		short s2 = r.mNums.Size() <= b.mNums.Size() - 1 ? 0 : r.mNums[b.mNums.Size() - 1];
		short d = static_cast<short>((static_cast<int>(s1) * 10000 + s2) / b.mNums.Back());
		
		TInteger tmp = b * d;

		while (tmp > r)
		{
			tmp = tmp - b;
			--d;
		}

		r = r - tmp;
		q.mNums[i] = d;
	}

	q.Fix();

	return q;
}

TInteger TInteger::operator ^ (const TInteger& in) const
{
	return Power(in);
}

bool TInteger::operator < (const TInteger& in) const
{
	if (mNums.Size() != in.mNums.Size())
	{
		return mNums.Size() < in.mNums.Size();
	}

	for (int i = mNums.Size() - 1; i >= 0; --i)
	{
		if (mNums[i] != in.mNums[i])
		{
			return mNums[i] < in.mNums[i];
		}
	}

	return false;
}

bool TInteger::operator > (const TInteger& in) const
{
	return in < *this;
}

bool TInteger::operator == (const TInteger& in) const
{
	return !(*this < in) && !(in < *this);
}

TInteger TInteger::Power(const TInteger& b) const
{
	if (b == 0)
	{
		return 1;
	}

	if (b.mNums[0] & 1)
	{
		return *this * Power(b - 1);
	}
	
	TInteger c = Power(b / 2);

	return c * c;
}

void TInteger::Fix()
{
	while (mNums.Size() > 1 && mNums.Back() == 0)
	{
		mNums.PopBack();
	}
}
