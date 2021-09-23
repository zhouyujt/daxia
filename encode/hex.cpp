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

		daxia::string Hex::Marshal(const void* data, size_t size)
		{
			daxia::string result;
			char* pr = result.GetBuffer(size * 2);
			const char* pt = table_.GetString();
			for (size_t i = 0; i < size; ++i)
			{
				pr[i * 2] = pt[*(reinterpret_cast<const unsigned char*>(data)+i) >> 4];
				pr[i * 2 + 1] = pt[*(reinterpret_cast<const unsigned char*>(data)+i) & 0x0f];
			}
			
			return result;
		}

		daxia::buffer Hex::Unmarshal(const daxia::string& str)
		{
			daxia::string temp = str;
			temp.MakeLower();
			const char* pch = temp.GetString();

			daxia::buffer result;
			char* pr = result.GetBuffer(temp.GetLength() / 2 + temp.GetLength() % 2);
			size_t len = 0;
			for (size_t i = 0; i < temp.GetLength(); ++i)
			{
				size_t pos = table_.Find(pch[i]);
				if (pos == -1)
				{
					result.Empty();
					break;
				}

				if (i % 2 == 0)
				{
					++len;
					pr[len - 1] = pos << 4;
				}
				else
				{
					pr[len - 1] |= pos;
				}
			}

			return result;
		}
	}
}
