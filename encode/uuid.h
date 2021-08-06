/*!
* Licensed under the MIT License.See License for details.
* Copyright (c) 2021 �콭��Ĵ�Ϻ.
* All rights reserved.
*
* \file uuid.h
* \author �콭��Ĵ�Ϻ
* \date ���� 2021
*
* ����UUID
*
*/
#ifndef __DAXIA_ENCODE_UUID_H
#define  __DAXIA_ENCODE_UUID_H
#include <boost/uuid/uuid.hpp>
#include "../string.hpp"
namespace daxia
{
	namespace encode
	{
		class Uuid : public boost::uuids::uuid
		{
		public:
			Uuid();
			~Uuid();
		public:
			daxia::string ToString() const;
			Uuid& FromString(const daxia::string& str);
		};
	}
}

#endif	// !__DAXIA_ENCODE_UUID_H

