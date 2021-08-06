#include <memory.h>
#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif
#include <boost/uuid/uuid_generators.hpp>
#ifdef _MSC_VER
#pragma warning(default:4996)
#endif
#include <boost/uuid/uuid_io.hpp>
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
			return *this;
		}

	}
}
