#pragma once

namespace bsc
{
	namespace math
	{
		template<typename T>
		T fibonacci(T n)
		{
			if (n < 2)
			{
				return n;
			}

			T a = 0;
			T b = 1;

			T f = 0;

			for (T i = 2; i <= n; ++i)
			{
				f = a + b;
				a = b;
				b = f;
			}

			return f;
		}
	}
}