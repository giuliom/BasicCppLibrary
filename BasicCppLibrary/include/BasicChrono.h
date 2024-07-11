#pragma once

#include <chrono>

namespace bsc
{
	namespace chrono
	{
		template<typename OutputType, typename DurationMeasure, typename DurationType>
		OutputType convert_chrono_duration(const DurationType& duration)
		{
			return std::chrono::duration_cast<std::chrono::duration<OutputType, DurationMeasure::period>>(duration).count();
		}
	}
}

//using milliseconds = duration<long long, milli>;