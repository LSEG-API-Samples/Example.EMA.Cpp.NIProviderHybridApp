#pragma once
#ifndef __ema_client_h_
#define __ema_client_h_
#include "Ema.h"
#include <map>
#include <string>
using namespace thomsonreuters::ema::access;
struct ITEMHANDLES
{
	UInt64 ConsumerHandle;
	UInt64 NiPubHandle;
};
typedef std::map<std::string, ITEMHANDLES> ITEMLIST;

class NIProviderClient : public thomsonreuters::ema::access::OmmProviderClient
{
public:

	NIProviderClient();
	virtual ~NIProviderClient();
	void dumpOutput(const bool& flag);
	bool isConnectionUp() const;

protected:

	void onRefreshMsg(const thomsonreuters::ema::access::RefreshMsg&, const thomsonreuters::ema::access::OmmProviderEvent&);
	void onStatusMsg(const thomsonreuters::ema::access::StatusMsg&, const thomsonreuters::ema::access::OmmProviderEvent&);
	bool  bConnectionUp;
	bool bDumpOutput;
};

class ConsumerClient : public thomsonreuters::ema::access::OmmConsumerClient
{
public:
	ConsumerClient();
	virtual ~ConsumerClient();
	bool isConnectionUp() const;
	void setNIProvider(thomsonreuters::ema::access::OmmProvider& provider);
	void setNIProviderClient(NIProviderClient& providerClient);
	void setConsumer(thomsonreuters::ema::access::OmmConsumer& consumer);
	void setItemList(ITEMLIST& itemlist);
	void setNIPubServiceName(const EmaString& serviceName);
	void usePrefix(const bool& bUsePrefix,const std::string prefixString);
	void dumpOutput(const bool& flag);
	
protected:

	void onRefreshMsg(const thomsonreuters::ema::access::RefreshMsg&, const thomsonreuters::ema::access::OmmConsumerEvent&);

	void onUpdateMsg(const thomsonreuters::ema::access::UpdateMsg&, const thomsonreuters::ema::access::OmmConsumerEvent&);

	void onStatusMsg(const thomsonreuters::ema::access::StatusMsg&, const thomsonreuters::ema::access::OmmConsumerEvent&);


	thomsonreuters::ema::access::OmmProvider* niProvider;
	thomsonreuters::ema::access::OmmConsumer* pConsumer;
	thomsonreuters::ema::access::Int64 handle;
	NIProviderClient* niProviderClient;
	ITEMLIST* pItemList;
	bool  bConnectionUp;
	EmaString defaultNIPubServiceName;
	std::string prefixString;
	bool bDumpOutput;
	bool bUsePrefix;
};


#endif