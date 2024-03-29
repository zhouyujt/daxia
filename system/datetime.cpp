#ifdef _WIN32
#include <windows.h>
#endif
#include <iomanip>
#include <sstream>
#include <iomanip>
#include "datetime.h"

namespace daxia
{
	namespace system
	{
		DateTimeSpan::DateTimeSpan()
		{

		}

		DateTimeSpan::DateTimeSpan(int day, int hour, int minute, int second, int millisecond)
		{
			std::chrono::milliseconds milli((long long)millisecond + (long long)second * 1000 + (long long)minute * 1000 * 60 + (long long)hour * 1000 * 60 * 60 + (long long)day * 1000 * 60 * 60 * 24);
			duration_ = std::chrono::duration_cast<std::chrono::system_clock::duration>(milli);
		}

		DateTimeSpan::DateTimeSpan(const DateTimeSpan& dt)
		{
			duration_ = dt.duration_;
		}

		DateTimeSpan::~DateTimeSpan()
		{

		}

		size_t DateTimeSpan::Microseconds() const
		{
			return (std::chrono::duration_cast<std::chrono::microseconds>(duration_)).count();
		}

		size_t DateTimeSpan::Milliseconds() const
		{
			return (std::chrono::duration_cast<std::chrono::milliseconds>(duration_)).count();
		}

		DateTime::DateTime()
		{

		}

		DateTime::DateTime(std::time_t time)
		{
			tp_ = std::chrono::system_clock::from_time_t(time);
		}

		DateTime::DateTime(const char* time)
		{
			if (time)
			{
				std::tm tm;
				std::stringstream ss(time);
				ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
				if (ss.fail())
				{
					ss.clear();
					ss >> std::get_time(&tm, "%a, %d %b %Y %H:%M:%S GMT");
				}

				if (!ss.fail())
				{
					tp_ = std::chrono::system_clock::from_time_t(std::mktime(&tm));
				}
			}
		}

		DateTime::DateTime(const DateTime& dt)
		{
			tp_ = dt.tp_;
		}

		DateTime::DateTime(const std::chrono::system_clock::time_point& tp)
		{
			tp_ = tp;
		}

#ifdef _WIN32
		DateTime::DateTime(const _FILETIME& ft)
		{
			ULARGE_INTEGER ui;
			ui.LowPart = ft.dwLowDateTime;
			ui.HighPart = ft.dwHighDateTime;

			std::time_t t = ((LONGLONG)(ui.QuadPart - 116444736000000000) / 10000000);
			tp_ = std::chrono::system_clock::from_time_t(t);
		}
#endif

		DateTime::~DateTime()
		{

		}

		DateTime DateTime::Now()
		{
			DateTime dt;
			dt.tp_ = std::chrono::system_clock::now();
			return dt;
		}

		daxia::string DateTime::ToString(const char* format) const
		{
			std::time_t t = std::chrono::system_clock::to_time_t(tp_);
			if (t == -1) return daxia::string();
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

		DateTime DateTime::operator+(const DateTimeSpan& span) const
		{
			return DateTime(tp_ + span.duration_);
		}

		DateTime& DateTime::operator+=(const DateTimeSpan& span)
		{
			tp_ += span.duration_;
			return *this;
		}

		DateTime DateTime::operator-(const DateTimeSpan& span) const
		{
			return DateTime(tp_ - span.duration_);
		}

		daxia::system::DateTimeSpan DateTime::operator-(const DateTime& dt) const
		{
			DateTimeSpan span;
			span.duration_ = tp_ - dt.tp_;

			return span;
		}

		DateTime& DateTime::operator-=(const DateTimeSpan& span)
		{
			tp_ -= span.duration_;
			return *this;
		}

		bool DateTime::operator>(const DateTime& dt) const
		{
			return tp_ > dt.tp_;
		}

		bool DateTime::operator<(const DateTime& dt) const
		{
			return tp_ < dt.tp_;
		}

		bool DateTime::operator==(const DateTime& dt) const
		{
			return tp_ == dt.tp_;
		}

		bool DateTime::operator!=(const DateTime& dt) const
		{
			return tp_ != dt.tp_;
		}
	}
}