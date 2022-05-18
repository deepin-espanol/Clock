#ifndef HELPERS_H
#define HELPERS_H

#include <cstdarg>
#include <functional>
#include <vector>

/*
 * Miscellaneous functions used for many things.
 * - Bits.
 * - Flags.
 * - Overloading.
 */

#define CHECK_BIT(var, pos) ((var) & (1 << (pos)))

inline unsigned int &setBit(unsigned int &n, const unsigned long p)
{
	n |= 1 << p;
	return n;
}

inline unsigned int &toggleBit(unsigned int &n, const unsigned long p)
{
	n ^= 1 << p;
	return n;
}

inline unsigned int &clearBit(unsigned int &n, const unsigned long p)
{
	n &= ~(1 << p);
	return n;
}

inline unsigned int &changeBit(unsigned int &n, const unsigned long p, const bool v)
{
	const int mask = 1 << p;
	n = ((n & ~mask) | (((unsigned long)v) << p));
	return n;
}

inline unsigned int toggleInt(unsigned int source, const unsigned int other)
{
	unsigned long i = 0;
	while (i < sizeof(int)) {
		if (CHECK_BIT(other, i)) {
			toggleBit(source, i);
		}
		i++;
	}
	return source;
}

inline unsigned int mergeInt(unsigned int source, const unsigned int other)
{
	unsigned long i = 0;
	while (i < sizeof(int)) {
		if (CHECK_BIT(other, i)) {
			setBit(source, i);
		}
		i++;
	}
	return source;
}

inline unsigned int orInt(unsigned int source, const unsigned int other)
{
	unsigned int v = 0;
	unsigned long i = 0;
	while (i < sizeof(int)) {
		if (CHECK_BIT(other, i) || CHECK_BIT(source, i)) {
			setBit(v, i);
		}
		i++;
	}
	return v;
}

inline unsigned int xorInt(unsigned int source, const unsigned int other)
{
	unsigned long i = 0;
	while (i < sizeof(int)) {
		if (CHECK_BIT(other, i) != CHECK_BIT(source, i)) {
			setBit(source, i);
		}
		i++;
	}
	return source;
}

/**
 * Call functions depending on the bitset (flag)
 * @param flag Object containing all the bits.
 * @param handlers Number of handlers enable (from 1th bit to handlers-th one).
 * @param func... All the handlers, called in their param order depending on the flag.
 */
inline void flex_switch(unsigned int flag = 0, unsigned int handlers = 0, std::vector<std::function<void()>> funcs = {})
{
	unsigned long i = 0;
	while (i < sizeof(flag) && i < handlers) {
		if (CHECK_BIT(flag, i)) {
			funcs[i]();
		}
		i++;
	}
}

template<typename... Args> struct SELECT {
	template<typename C, typename R>
	static constexpr auto OVERLOAD_OF( R (C::*pmf)(Args...) ) -> decltype(pmf) {
		return pmf;
	}
};

#endif // HELPERS_H
