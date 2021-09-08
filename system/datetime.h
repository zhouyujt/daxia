/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 漓江里的大虾.
* All rights reserved.
*
* \file datetime.h
* \author 漓江里的大虾
* \date 七月 2021
*
* 时间相关
*
*/
#ifndef __DAXIA_SYSTEM_DATETIME_H
#define __DAXIA_SYSTEM_DATETIME_H

#include <ctime>
#include <chrono>
#include <ostream>
#include "../string.hpp"

namespace daxia
{
	namespace system
	{
		class DateTime;
		class DateTimeSpan
		{
			friend DateTime;
		public:
			DateTimeSpan();
			DateTimeSpan(int day, int hour, int minute, int second, int millisecond);
			DateTimeSpan(const DateTimeSpan& dt);
			~DateTimeSpan();
			size_t Count() const;
		private:
			std::chrono::system_clock::duration duration_;
		};

		class DateTime
		{
		public:
			DateTime();
			DateTime(std::time_t time);
			DateTime(const char* time);
			DateTime(const DateTime& dt);
			DateTime(const std::chrono::system_clock::time_point& tp);
			~DateTime();
		public:
			static DateTime Now();
		public:
			daxia::string ToString(const char* format = nullptr) const;
		public:
			DateTime operator+(const DateTimeSpan& span) const;
			DateTime& operator+=(const DateTimeSpan& span);
			DateTime operator-(const DateTimeSpan& span) const;
			DateTimeSpan operator-(const DateTime& dt) const;
			DateTime& operator-=(const DateTimeSpan& span);
			friend std::ostream& operator<<(std::ostream& os, const DateTime& dt)
			{
				os << dt.ToString();
				return os;
			}
		private:
			std::chrono::system_clock::time_point tp_;
		};
	}
}

#endif // !__DAXIA_SYSTEM_DATETIME_H