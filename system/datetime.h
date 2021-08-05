/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 �콭��Ĵ�Ϻ.
* All rights reserved.
*
* \file datetime.h
* \author �콭��Ĵ�Ϻ
* \date ���� 2021
*
* ʱ�����
*
*/
#ifndef __DAXIA_SYSTEM_DATETIME_H
#define __DAXIA_SYSTEM_DATETIME_H

#include <ctime>
#include <chrono>
#include "../string.hpp"

namespace daxia
{
	namespace system
	{
		class DateTime
		{
		public:
			DateTime();
			DateTime(std::time_t time);
			DateTime(const char* time);
			DateTime(const DateTime& dt);
			~DateTime();
		public:
			static DateTime Now();
		public:
			daxia::string ToString(const char* format = nullptr) const;
		private:
			std::chrono::system_clock::time_point tp_;
		};

		class DateTimeSpan
		{
		public:
			DateTimeSpan();
			DateTimeSpan(const DateTimeSpan& dt);
			~DateTimeSpan();
		private:
			std::chrono::system_clock::duration duration_;
		};
	}
}

#endif // !__DAXIA_SYSTEM_DATETIME_H