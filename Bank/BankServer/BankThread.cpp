#include "BankThread.h"

#include "BankSession.h"

#include "../Public/Logging.h"
#include "../Public/StringUtil.h"
#include "../Public/Exception.h"

#include <string>
#include <vector>

BankThread::BankThread(std::auto_ptr<Socket>& socket) : socket_(socket)
{
}

BankThread::~BankThread()
{
}

void BankThread::Run()
{
	BankSession* bs = new BankSession(socket_);	// 所有权转移到了BankSession
	while (!bs->IsDead())
	{
		try
		{
			bs->Process();
		}
		catch (Exception& e)
		{
			LOG_INFO<<e.what();
			bs->Kill();
		}
	}
	delete bs;
}
