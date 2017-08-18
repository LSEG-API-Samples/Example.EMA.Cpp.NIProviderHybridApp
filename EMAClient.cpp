#include "EMAClient.h"
#include <iostream>
using namespace thomsonreuters::ema::access;
using namespace thomsonreuters::ema::rdm;
using namespace std;

NIProviderClient::NIProviderClient() :
bConnectionUp(false),
bDumpOutput(false)
{
}

NIProviderClient ::~NIProviderClient()
{
}

bool NIProviderClient::isConnectionUp() const
{
	return bConnectionUp;
}
void NIProviderClient::dumpOutput(const bool& flag)
{
	this->bDumpOutput = flag;
}
void NIProviderClient::onRefreshMsg(const RefreshMsg& refreshMsg, const OmmProviderEvent& ommEvent)
{
	if (refreshMsg.getDomainType() == MMT_LOGIN)
	{
		cout << endl << "NIProvier OnRefreshMsg Handle: " << ommEvent.getHandle() << " Closure: " << ommEvent.getClosure() << endl;
		if (bDumpOutput)
			cout << refreshMsg << endl;

		if (refreshMsg.getState().getStreamState() == OmmState::OpenEnum)
		{
			if (refreshMsg.getState().getDataState() == OmmState::OkEnum)
				bConnectionUp = true;
			else
				bConnectionUp = false;
		}
		else
			bConnectionUp = false;
	}
}

void NIProviderClient::onStatusMsg(const StatusMsg& statusMsg, const OmmProviderEvent& ommEvent)
{
	cout << endl << "NIProviderClient OnStatus Handle: " << ommEvent.getHandle() << " Closure: " << ommEvent.getClosure() << endl;
	if (bDumpOutput)
		cout << statusMsg << endl;

	if (statusMsg.getDomainType() == MMT_LOGIN)
	{
		if (statusMsg.hasState())
		{
			if (statusMsg.getState().getStreamState() == OmmState::OpenEnum)
			{
				if (statusMsg.getState().getDataState() == OmmState::OkEnum)
					bConnectionUp = true;
				else
					bConnectionUp = false;
			}
			else
				bConnectionUp = false;
		}
		
	}
}



ConsumerClient::ConsumerClient():
niProvider(0),
pConsumer(0),
handle(0),
pItemList(0),
bDumpOutput(false),
bUsePrefix(false),
prefixString(""),
bConnectionUp(false)
{

}
ConsumerClient::~ConsumerClient()
{
}
bool ConsumerClient::isConnectionUp() const
{
	return bConnectionUp;
}
void ConsumerClient::setConsumer(thomsonreuters::ema::access::OmmConsumer& consumer)
{
	pConsumer = &consumer;
}
void ConsumerClient::setNIProvider(OmmProvider& provider)
{
	niProvider = &provider;
}
void ConsumerClient::setNIProviderClient(NIProviderClient& providerClient)
{
	this->niProviderClient = &providerClient;
}
void ConsumerClient::setItemList(ITEMLIST& itemlist)
{
	this->pItemList = &itemlist;
}
void ConsumerClient::usePrefix(const bool& bUsePrefix, const std::string prefixString)
{
	this->bUsePrefix = bUsePrefix;
	this->prefixString = prefixString;
}
void ConsumerClient::dumpOutput(const bool& flag)
{
	this->bDumpOutput = flag;
}
void ConsumerClient::setNIPubServiceName(const EmaString& serviceName)
{
	defaultNIPubServiceName = serviceName;
}
void ConsumerClient::onRefreshMsg(const RefreshMsg& refreshMsg, const OmmConsumerEvent& ommEvent)
{
	try {

		if (refreshMsg.getDomainType() == MMT_LOGIN)
		{
			cout << endl << "Login OnRefreshMsg Handle: " << ommEvent.getHandle() << " Closure: " << (const char *)(ommEvent.getClosure()) << endl;
			if (bDumpOutput)
				cout << refreshMsg << endl;

			if (refreshMsg.getState().getStreamState() == OmmState::OpenEnum)
			{
				if (refreshMsg.getState().getDataState() == OmmState::OkEnum)
					bConnectionUp = true;
				else
					bConnectionUp = false;
			}
			else
				bConnectionUp = false;
		
		}
		if (refreshMsg.getDomainType() == MMT_MARKET_PRICE)
		{
			cout << endl << "Market Price OnRefreshMsg Handle: " << ommEvent.getHandle() << " ItemName " << refreshMsg.getName().c_str() << endl;
			if (bDumpOutput)
				cout << refreshMsg << endl;
			
			if (niProviderClient->isConnectionUp())
			{ 
				ITEMLIST::iterator it;
				it = pItemList->find(refreshMsg.getName().c_str());
				if (it != pItemList->end())
				{
					string pubItemName = "";
					if (bUsePrefix)
						pubItemName = prefixString;
					pubItemName.append((*it).first);
					UInt64 pubHandle = (*it).second.NiPubHandle;
					RefreshMsg refresh;
					cout << "Publish Refresh message for Item "<<pubItemName.c_str()<<" to Non Interactive provider service..." << endl;
					niProvider->submit(refresh.clear().serviceName(defaultNIPubServiceName).name(pubItemName.c_str()).domainType(MMT_MARKET_PRICE)
						.state(OmmState::OpenEnum, OmmState::OkEnum, OmmState::NoneEnum, "UnSolicited Refresh Completed").complete(true)
						.payload(refreshMsg.getPayload().getFieldList()), pubHandle);
				}
			}

		}
	}
	catch (OmmException& ex)
	{
		cout<< ex <<std::endl;
	}

}

void ConsumerClient::onUpdateMsg(const UpdateMsg& updateMsg, const OmmConsumerEvent& ommEvent)
{
	try{

		if (updateMsg.getDomainType() == MMT_MARKET_PRICE)
		{
			cout << endl << "Market Price OnUpdateMsg Handle: " << ommEvent.getHandle() << " ItemName: " << updateMsg.getName().c_str() << endl;
			if (bDumpOutput)
				cout << updateMsg << endl;
			
			if (niProviderClient->isConnectionUp())
			{
				ITEMLIST::iterator it;
				it = pItemList->find(updateMsg.getName().c_str());
				if (it != pItemList->end())
				{
					string pubItemName = "";
					if (bUsePrefix)
						pubItemName = prefixString;
					pubItemName.append((*it).first);
					UInt64 pubHandle = (*it).second.NiPubHandle;
					UpdateMsg update;
					cout << "Publish Update message for Item " << pubItemName.c_str() << " to Non Interactive provider service..." << endl;
					niProvider->submit(update.clear().serviceName(defaultNIPubServiceName).name(pubItemName.c_str())
						.payload(updateMsg.getPayload().getFieldList()), pubHandle);

				}
			}
		}
	}
	catch (OmmException& ex)
	{
		cout << ex << std::endl;
	}
}

void ConsumerClient::onStatusMsg(const StatusMsg& statusMsg, const OmmConsumerEvent& ommEvent)
{
	
	
	if (statusMsg.getDomainType() == MMT_LOGIN)
	{	
		cout << endl << "Login OnStatusMsg Handle: " << ommEvent.getHandle() << " Closure: " << (char *)ommEvent.getClosure() << endl;
		cout << statusMsg << endl;

		if (statusMsg.hasState())
		{
			if (statusMsg.getState().getStreamState() == OmmState::OpenEnum)
			{
				if (statusMsg.getState().getDataState() == OmmState::OkEnum)
					bConnectionUp = true;
				else
					bConnectionUp = false;
			}
			else
			bConnectionUp = false;
		}
	}
	if (statusMsg.getDomainType() == MMT_MARKET_PRICE)
	{
		cout << endl << "Market Price OnStatusMsg Handle: " << ommEvent.getHandle() << " Closure: " << (char *)ommEvent.getClosure() << endl;
		cout << statusMsg << endl;
	}
}
