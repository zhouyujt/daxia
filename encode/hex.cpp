#include "hex.h"

namespace daxia
{
	namespace encode
	{
		daxia::string Hex::table_ = "0123456789abcdef";

		Hex::Hex()
		{

		}

		Hex::~Hex()
		{

		}

		daxia::string Hex::ToString(const void* data, size_t size)
		{
			daxia::string result;
			result.GetBuffer(size * 2);
			for (size_t i = 0; i < size; ++i)
			{
				result[i * 2] = table_[static_cast<unsigned char>((*reinterpret_cast<const unsigned char*>(data)+i)) >> 4];
				result[i * 2 + 1] = table_[static_cast<unsigned char>((*reinterpret_cast<const unsigned char*>(data)+i)) & 0x0f];
			}
			
			return result;
		}

		daxia::buffer Hex::FromString(const daxia::string& str)
		{
			daxia::string temp = str;
			temp.MakeLower();

			daxia::buffer result;
			for (size_t i = 0; i < temp.GetLength(); ++i)
			{
				const char& ch = temp[i];
				size_t pos = table_.Find(ch);
				if (pos == -1)
				{
					break;
				}

				if (i % 2 == 0)
				{
					result += (char)0x00;
					result[result.GetLength() - 1] = pos << 4;
				}
				else
				{
					result[result.GetLength() - 1] |= pos;
				}
			}

			return result;
		}
	}
}
