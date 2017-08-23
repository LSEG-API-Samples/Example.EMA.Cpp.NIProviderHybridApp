#include "ClientManager.h"
#include <iostream>
using namespace thomsonreuters::ema::domain::login;
using namespace thomsonreuters::ema::access;
using namespace thomsonreuters::ema::rdm;
using namespace std;
ConsumerManager::ConsumerManager(const EmaString& username, const EmaString& position, const EmaString& appID, const bool& dumpOutput) :
running(false),
ommConsumer(0)
{
	try{
		consumerClient1 = new ConsumerClient();
		consumerClient1->dumpOutput(dumpOutput);
		ommConsumer = new OmmConsumer(OmmConsumerConfig()
			.operationModel(OmmConsumerConfig::UserDispatchEnum)
			.addAdminMsg(Login::LoginReq().name(username).applicationId(appID)
			.position(position).allowSuspectData(true).getMessage()), *consumerClient1, (void*)"ConsumerClient");

		consumerClient1->setConsumer(*ommConsumer);
	}
	catch (OmmInvalidUsageException& ex)
	{
		std::cout << ex.getText() << std::endl;
	}
}

ConsumerManager::~ConsumerManager()
{
	stop();

	if (ommConsumer)
	{
		delete ommConsumer;
		ommConsumer = 0;
	}
}
ConsumerClient& ConsumerManager::getConsumerClient()
{
	return *consumerClient1;
}
OmmConsumer & ConsumerManager::getOmmConsumer()
{
	return *ommConsumer;
}

void ConsumerManager::run()
{
	while (running)
	{
		if (ommConsumer)
			ommConsumer->dispatch(1000);
	}
}

#ifdef WIN32
unsigned __stdcall ConsumerManager::ThreadFunc(void* pArguments)
{
	((ConsumerManager *)pArguments)->run();

	return 0;
}

#else
extern "C"
{
	void * ConsumerManager::ThreadFunc(void* pArguments)
	{
		((ConsumerManager *)pArguments)->run();
	}
}
#endif

void  ConsumerManager::start()
{
#ifdef WIN32
	_handle = (HANDLE)_beginthreadex(NULL, 0, ThreadFunc, this, 0, &_threadId);
	assert(_handle != 0);
	SetThreadPriority(_handle, THREAD_PRIORITY_NORMAL);
#else
	pthread_create(&_threadId, NULL, ThreadFunc, this);
	assert(_threadId != 0);
#endif

	running = true;
}

void  ConsumerManager::stop()
{
	if (running)
	{
		running = false;
#ifdef WIN32
		WaitForSingleObject(_handle, INFINITE);
		CloseHandle(_handle);
		_handle = 0;
		_threadId = 0;
#else
		pthread_join(_threadId, NULL);
		_threadId = 0;
#endif
	}

	running = false;
}

NIProviderManager::NIProviderManager(const EmaString& username, const EmaString& position, const EmaString& appID, const EmaString& instanceID, const bool& dumpOutput) :
running(false),
ommProvider(0)
{
	try{
		providerClient1 = new NIProviderClient();
		providerClient1->dumpOutput(dumpOutput);
		ommProvider = new OmmProvider(OmmNiProviderConfig().operationModel(OmmNiProviderConfig::UserDispatchEnum)
			.addAdminMsg(Login::LoginReq().name(username).applicationId(appID)
			.position(position).instanceId(instanceID).allowSuspectData(true)
			.providePermissionExpressions(true).providePermissionProfile(true)
			.role(1).getMessage()), *providerClient1, (void*)"NIProviderClilent");

	}
	catch (OmmInvalidUsageException& ex)
	{
		std::cout << ex.getText() << std::endl;
	}
}

NIProviderManager::~NIProviderManager()
{
	stop();

	if (ommProvider)
	{
		delete ommProvider;
		ommProvider = 0;
	}
}
NIProviderClient& NIProviderManager::getNIProviderCLient()
{
	return *providerClient1;
}
OmmProvider & NIProviderManager::getOmmNIProvider()
{
	return *ommProvider;
}

void NIProviderManager::run()
{
	while (running)
	{
		if (ommProvider)
			ommProvider->dispatch(1000);
	}
}

#ifdef WIN32
unsigned __stdcall NIProviderManager::ThreadFunc(void* pArguments)
{
	((NIProviderManager *)pArguments)->run();

	return 0;
}

#else
extern "C"
{
	void * NIProviderManager::ThreadFunc(void* pArguments)
	{
		((NIProviderManager *)pArguments)->run();
	}
}
#endif

void  NIProviderManager::start()
{
#ifdef WIN32
	_handle = (HANDLE)_beginthreadex(NULL, 0, ThreadFunc, this, 0, &_threadId);
	assert(_handle != 0);

	SetThreadPriority(_handle, THREAD_PRIORITY_NORMAL);
#else
	pthread_create(&_threadId, NULL, ThreadFunc, this);
	assert(_threadId != 0);
#endif

	running = true;
}

void NIProviderManager::stop()
{
	if (running)
	{
		running = false;
#ifdef WIN32
		WaitForSingleObject(_handle, INFINITE);
		CloseHandle(_handle);
		_handle = 0;
		_threadId = 0;
#else
		pthread_join(_threadId, NULL);
		_threadId = 0;
#endif
	}

	running = false;
}