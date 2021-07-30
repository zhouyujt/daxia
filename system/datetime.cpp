#include <iomanip>
#include <sstream>
#include "datetime.h"

namespace daxia
{
	namespace system
	{
		DateTime::DateTime()
		{

		}

		DateTime::DateTime(std::time_t time)
		{
			tp_ = std::chrono::system_clock::from_time_t(time);
		}

		DateTime::DateTime(const char* time)
		{
			std::tm tm;
			std::stringstream ss(time);
			ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
			tp_ = std::chrono::system_clock::from_time_t(std::mktime(&tm));
		}

		DateTime::DateTime(const DateTime& dt)
		{
			tp_ = dt.tp_;
		}

		DateTime::~DateTime()
		{

		}

		DateTime DateTime::Now()
		{
			DateTime dt;
			dt.tp_ = std::chrono::system_clock::now();
			return dt;
		}

		daxia::string DateTime::ToString(const char* format)
		{
			std::time_t t = std::chrono::system_clock::to_time_t(tp_);
			std::tm tm = *std::localtime(&t);

			if (format == nullptr)
			{
				format = "%Y-%m-%d %H:%M:%S";
			}

			daxia::string str;
			strftime(str.GetBuffer(64), 64, format, &tm);
			str.ReleaseBuffer();

			return str;
		}
	}
}