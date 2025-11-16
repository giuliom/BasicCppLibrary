#pragma once

#include <bitset>
#include <utility>

namespace bsc
{
	template<typename ENUM_TYPE, ENUM_TYPE ENUM_COUNT>
		requires std::is_enum_v<ENUM_TYPE>
	class bit_enum
	{
		using bit_data = std::bitset<std::to_underlying(ENUM_COUNT)>;
		bit_data m_data;

	public:

		bit_enum() = default;
		explicit bit_enum(const bit_data& data) : m_data(data) {}

		const bit_data& data() const { return m_data; }
		static constexpr std::size_t size() { return std::to_underlying(ENUM_COUNT); }
		constexpr bool operator[](const ENUM_TYPE i) const { return m_data[std::to_underlying(i)]; }
		constexpr auto operator[](const ENUM_TYPE i) { return m_data[std::to_underlying(i)]; }

		constexpr bool test(const ENUM_TYPE i) const { return m_data.test(std::to_underlying(i)); }
		constexpr void set(const ENUM_TYPE i, bool value = true) { m_data.set(std::to_underlying(i), value); }
    	constexpr void reset(const ENUM_TYPE i) { m_data.reset(std::to_underlying(i)); }
    	constexpr void flip(const ENUM_TYPE i) { m_data.flip(std::to_underlying(i)); }
    
    	constexpr bool all() const noexcept { return m_data.all(); }
    	constexpr bool any() const noexcept { return m_data.any(); }
    	constexpr bool none() const noexcept { return m_data.none(); }
    	constexpr std::size_t count() const noexcept { return m_data.count(); }
	};
}