#pragma once

namespace bsc
{
	namespace chrono
	{
		template<typename DurationMeasure, typename DurationType, typename OutputType>
		void convert_chrono_duration(const DurationType& duration, OutputType& output)
		{
			output = std::chrono::duration_cast<DurationMeasure>(duration).count();
		}
	}
}