#include <string.h>
#include "url.h"
#include "hex.h"

namespace daxia
{
	namespace encode
	{
		
		Url::Url()
		{

		}

		Url::~Url()
		{

		}

		daxia::string Url::Marshal(const void* data, size_t size)
		{
			daxia::string result;
			for (size_t i = 0; i < size; ++i)
			{
				const char& ch = static_cast<const char*>(data)[i];
				if (isAlpha(ch) || strchr("=!~*'()", ch))
				{
					result += ch;
				}
				else if (ch == ' ')
				{
					result += '+';
				}
				else
				{
					result += '%';
					result += Hex::Marshal(&ch, sizeof(ch));
				}
			}

			return result;
		}

		daxia::string Url::Marshal(const daxia::string& str)
		{
			return Marshal(str.GetString(), str.GetLength());
		}

		daxia::buffer Url::Unmarshal(const void* data, size_t size)
		{
			daxia::buffer result;
			for (size_t i = 0; i < size; ++i)
			{
				const char& ch = static_cast<const char*>(data)[i];
				if (ch == '%')
				{
					if (i + 2 < size)
					{
						result += Hex::Unmarshal(daxia::string(static_cast<const char*>(data)+ i + 1, 2));
						i += 2;
					}
				}
				else
				{
					result += ch;
				}
			}

			return result;
		}

		daxia::buffer Url::Unmarshal(const daxia::string& str)
		{
			return Unmarshal(str.GetString(), str.GetLength());
		}

		inline bool Url::isAlpha(char ch)
		{
			if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9'))
			{
				return true;
			}

			return false;
		}

	}
}
