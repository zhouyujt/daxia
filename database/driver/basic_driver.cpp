#include "basic_driver.h"

namespace daxia
{
	namespace database
	{
		namespace driver
		{
			BasicDriver::BasicDriver(const daxia::string& host, unsigned short port, const daxia::string& db, const daxia::string& user, const daxia::string& psw, daxia::system::ThreadPool* tp)
				: host_(host)
				, port_(port)
				, db_(db)
				, user_(user)
				, psw_(psw)
				, tp_(tp)
			{
			}

			BasicDriver::~BasicDriver()
			{

			}
		}
	}
}