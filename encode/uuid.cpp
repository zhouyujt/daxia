#include <memory.h>
#ifdef _WIN32
#pragma warning(disable:4996)
#endif
#include <boost/uuid/uuid_generators.hpp>
#ifdef _WIN32
#pragma warning(default:4996)
#endif
#include <boost/uuid/uuid_io.hpp>
#include "../encode/hex.h"
#include "uuid.h"

namespace daxia
{
	namespace encode
	{
		Uuid::Uuid()
		{
			*dynamic_cast<boost::uuids::uuid*>(this) = boost::uuids::random_generator()();
		}


		Uuid::~Uuid()
		{
		}

		daxia::string Uuid::ToString() const
		{
			return boost::uuids::to_string(*this);
		}

		Uuid& Uuid::FromString(const daxia::string& str)
		{
			daxia::buffer buff = daxia::encode::Hex::Unmarshal(str);
			if (buff.GetLength() == sizeof(data))
			{
				memcpy(data, buff.GetBuffer(), sizeof(data));
			}

			return *this;
		}

	}
}
