#pragma once
#ifndef __clientmanager_h
#define __clientmanager_h
#include <assert.h>

#ifdef WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <process.h>
#include <windows.h>
#include <sys/timeb.h>
#include <time.h>
#else
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <strings.h>
#include <sys/time.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

#include "Ema.h"
#include "EMAClient.h"
using namespace thomsonreuters::ema::access;
using namespace thomsonreuters::ema::rdm;
using namespace std;

class ConsumerManager
{
public:

	ConsumerManager(const EmaString& username, const EmaString& position, const EmaString& appID, const bool& dumpOutput = false);
	virtual ~ConsumerManager();
	thomsonreuters::ema::access::OmmConsumer& getOmmConsumer();
	ConsumerClient& getConsumerClient();
	void start();
	void stop();
	void run();

protected:

	bool running;

	thomsonreuters::ema::access::OmmConsumer* ommConsumer;
	ConsumerClient* consumerClient1;

#ifdef WIN32
	static unsigned __stdcall ThreadFunc(void* pArguments);

	HANDLE					_handle;
	unsigned int			_threadId;
#else
	static void *ThreadFunc(void* pArguments);

	pthread_t				_threadId;
#endif
};

class NIProviderManager
{
public:

	NIProviderManager(const EmaString& username, const EmaString& position, const EmaString& appID,const EmaString& instanceID, const bool& dumpOutput = false);
	virtual ~NIProviderManager();
	thomsonreuters::ema::access::OmmProvider& getOmmNIProvider();
	NIProviderClient& getNIProviderCLient();
	void start();
	void stop();
	void run();

protected:

	bool running;

	thomsonreuters::ema::access::OmmProvider* ommProvider;
	NIProviderClient* providerClient1;
#ifdef WIN32
	static unsigned __stdcall ThreadFunc(void* pArguments);

	HANDLE					_handle;
	unsigned int			_threadId;
#else
	static void *ThreadFunc(void* pArguments);

	pthread_t				_threadId;
#endif
};


#endif 
