#pragma once

#include "stdafx.h"

namespace bsc
{
	template<typename ENUM_TYPE>
	constexpr std::underlying_type_t<ENUM_TYPE> enum_as_value(const ENUM_TYPE value)
	{
		return static_cast<std::underlying_type_t<ENUM_TYPE>>(value);
	}

	template<typename ENUM_TYPE, ENUM_TYPE count>
	class bit_enum
	{
		using bit_data = std::bitset<enum_as_value(count)>;
		bit_data m_data;

	public:

		bit_enum() : m_data() {}
		bit_enum(const bit_data& data) : m_data(data) {}

		const bit_data& data() const { return m_data; }
		constexpr auto size() const { return enum_as_value(count); }

		constexpr bool operator[](const ENUM_TYPE i) const { return m_data[enum_as_value(i)]; }

		auto operator[](const ENUM_TYPE i) { return m_data[enum_as_value(i)]; }

		bool test(const ENUM_TYPE i) { return m_data.test(enum_as_value(i)); }

		// TODO finish exposing the rest of std::bitset functions

	};
}