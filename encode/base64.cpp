#include <string>
#include <sstream>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include "base64.h"

namespace daxia
{
	namespace encode
	{
		daxia::string Base64::Marshal(const char* data, size_t size)
		{
			using namespace boost::archive::iterators;

			typedef base64_from_binary<transform_width<std::string::const_iterator, 6, 8>> Base64EncodeIter;

			std::stringstream  result;
			std::string str(data, size);
			std::copy(Base64EncodeIter(str.begin()), Base64EncodeIter(str.end()), std::ostream_iterator<char>(result));

			size_t Num = (3 - size % 3) % 3;
			for (size_t i = 0; i < Num; i++)
			{
				result.put('=');
			}

			return result.str();
		}

		daxia::string Base64::Unmarshal(const char* str)
		{
			using namespace boost::archive::iterators;
			typedef transform_width<binary_from_base64<std::string::const_iterator>, 8, 6> Base64DecodeIter;

			std::stringstream result;
			std::string temp(str);
			try
			{
				copy(Base64DecodeIter(temp.begin()), Base64DecodeIter(temp.end()), std::ostream_iterator<char>(result));
			}
			catch (...)
			{
			}
			
			return result.str();
		}

		daxia::string Base64::Unmarshal(const std::string& str)
		{
			using namespace boost::archive::iterators;
			typedef transform_width<binary_from_base64<std::string::const_iterator>, 8, 6> Base64DecodeIter;

			std::stringstream result;
			try
			{
				copy(Base64DecodeIter(str.begin()), Base64DecodeIter(str.end()), std::ostream_iterator<char>(result));
			}
			catch (...)
			{
			}

			return result.str();
		}
	}// namespace encode
}// namespace daxia
