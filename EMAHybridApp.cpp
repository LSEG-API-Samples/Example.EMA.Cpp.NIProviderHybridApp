#include "EMAHybridApp.h"
#include <iostream>
#include <stdlib.h>
#include <map>
#include <string>
#include <list>
#include <fstream>
#include <exception>

using namespace thomsonreuters::ema;
using namespace thomsonreuters::ema::access;
using namespace thomsonreuters::ema::rdm;
using namespace std;

int runtime = 600; //second
UInt64 defaultNIPubServiceID = 8500;
EmaString defaultNIPubServiceName = "API_ADH_NI_PUB";
UInt64 defaultNISourceDirectoryHandle = 10;
EmaString defaultConsumerServiceName = "API_ELEKTRON_EPD_RSSL";
EmaString defaultSubUserName = "username";
EmaString defaultPubUserName = "pubusername";
EmaString defaultPosition = "127.0.0.1/net";
EmaString defaultAppID = "256";
EmaString defaultInstanceID = "1";
UInt64 handleStart = 200000;
list<string> defaultitems;
list<int> defaultView;
bool bDumpOutput = false;
bool bUseView = false;
bool bUsePrefix = false;
std::string prefixString = "";

void displayUsage()
{
	std::cout << "EMA Consumer Non Interactive Provider Hybrid demo\n";
	std::cout << "The following options are supported\n\n";
	std::cout << " -s        Followed by the subscribing service name \n";
	std::cout << " -nip      Followed by the non-interactive publishing service name\n";
	std::cout << " -mp       Followed by a list of Market Price item names separated by space. Required by consumer and non-iteractive client publish\n\n";
	std::cout << " -itemfile Followed absolute file path to file contains item list\n";
	std::cout << "           Note: command line options can contains either -mp or -filename. If it contains both options it will use item list from last one\n\n";
	std::cout << " Optional options\n\n";
	std::cout << " -v        Followed by a list of FID id (integer) seperated by space. Turn on OMM view request options \n\n";
	std::cout << " -nipuser  Followed by Username for Non interactive provider, default is username\n";
	std::cout << " -u        Followed by Username for Consumer side, default is username.\n";
	std::cout << " -pos      Followed by Position, default is 127.0.0.1/net\n";
	std::cout << " -instance Followed by NI Provier Login instance ID, default is 1.\n";
	std::cout << " -svcid    Followed by Service ID for Non Interactive publishing service, default is 8500\n";
	std::cout << " -appid	 Followed by Application ID, default is 256\n";
	std::cout << " -d        Dump OMM Refresh and Update message to console output\n";
	std::cout << " -runtime  Followed by Application runtime period in second, default is 600 second\n\n";
	std::cout << " -prefix   Followed by prefix string. It's used to rename publish item name.\n";
	std::cout << "           For example, \n";
	std::cout << "              use -mp TRI.N -prefix TEST_ \n";
	std::cout << "           The demo application will re-publish data using item name TEST_TRI.N instead\n";
	std::cout << "           Consumer application has to subscribe data using the new item name\n\n";
	std::cout << "Usage:\n";
	std::cout << "\temahybridapp -s DIRECT_FEED -nip NIPUB -u username -mp TRI.N GOOG.O\n";
	std::cout << "\tor\n";
	std::cout << "\temahybridapp -s DIRECT_FEED -nip NIPUB -u username -itemfile ./itemlist.txt\n";
	std::cout << "\tcreates a service called NIPUB which derives all its items from DIRECT_FEED\n\n";
	std::cout << "\temahybridapp.exe -s DIRECT_FEED -nip NIPUB -u username -mp TRI.N -runtime 3600\n";
	std::cout << "\tFor running the application 3600 second and then exit\n\n";
	std::cout << "\temahybridapp -s DIRECT_FEED -nip NIPUB -u username -mp TRI.N -v 2 3 6 11 12 13 15 16 17 18 19 21 22 25 30 31\n";
	std::cout << "\tTo request item TRI.N with OMM view option, the application will publish only requested FID to ADH cache.\n\n";
	std::cout << "The Demo application works in pass through mode.\n";
	std::cout << "It will re-publish the data as is based on the data from item list user provided\n\n";
	std::cout << std::endl;
}

bool checkCommandLine(int argc, char* argv[])
{
	try{
		//Read parameters from command line argument and verify if it contains valid parameters 
		int requiredParameterCount = 0;
		if (argc < 2 || STRICMP(argv[1], "-?") == 0)
		{
			displayUsage();
			return false;
		}


		for (int i = 1; i < argc; i++)
		{
			if ((STRICMP(argv[i], "-s") != 0) && (STRICMP(argv[i], "-nip") != 0) && (STRICMP(argv[i], "-nipuser") != 0)
				&& (STRICMP(argv[i], "-u") != 0) && (STRICMP(argv[i], "-pos") != 0) && (STRICMP(argv[i], "-appid") != 0)
				&& (STRICMP(argv[i], "-mp") != 0) && (STRICMP(argv[i], "-svcid") != 0) && (STRICMP(argv[i], "-d") != 0)
				&& (STRICMP(argv[i], "-runtime") != 0) && (STRICMP(argv[i], "-prefix") != 0) && (STRICMP(argv[i], "-instance") != 0)
				&& (STRICMP(argv[i], "-v") != 0) && (STRICMP(argv[i], "-itemfile") != 0))
			{
				displayUsage();
				return false;
			}

			if (STRICMP(argv[i], "-s") == 0)
			{
				defaultConsumerServiceName = argv[++i];
				requiredParameterCount++;
			}
			else if (STRICMP(argv[i], "-nip") == 0)
			{
				defaultNIPubServiceName = argv[++i];
				requiredParameterCount++;
			}
			else if (STRICMP(argv[i], "-instance") == 0)
			{
				defaultInstanceID = argv[++i];
			}
			if (STRICMP(argv[i], "-prefix") == 0)
			{
				prefixString = argv[++i];
				bUsePrefix = true;
			}
			else if (STRICMP(argv[i], "-runtime") == 0)
				runtime = atoi(argv[++i]);
			else if (STRICMP(argv[i], "-u") == 0)
				defaultSubUserName = argv[++i];
			else if (STRICMP(argv[i], "-nipuser") == 0)
				defaultPubUserName = argv[++i];
			else if (STRICMP(argv[i], "-pos") == 0)
				defaultPosition = argv[++i];
			else if (STRICMP(argv[i], "-id") == 0)
				defaultAppID = argv[++i];
			else if (STRICMP(argv[i], "-itemfile") == 0)
			{
				defaultitems.clear();
				string ricName;
				ifstream ricFile(argv[++i]);
				if (ricFile.is_open())
				{
					while (getline(ricFile, ricName))
					{
						if (!ricName.empty())
							defaultitems.push_back(ricName);
					}
					ricFile.close();
					if (defaultitems.size() > 0)
						requiredParameterCount++;
				}
				else
				{
					cout << "Unable to open file " << argv[i] << endl;
					return false;
				}

			}
			else if (STRICMP(argv[i], "-d") == 0)
				bDumpOutput = true;
			else if (STRICMP(argv[i], "-svcid") == 0)
				defaultNIPubServiceID = atoi(argv[++i]);
			else if (STRICMP(argv[i], "-v") == 0)
			{
				defaultView.clear();
				i++;
				while ((i < argc))
				{
					defaultView.push_back(stoi(argv[i]));
					if ((i + 1) < argc && argv[i + 1][0] != '-')
						i++;
					else
						break;
				}
				if (defaultView.size() > 0)
					bUseView = true;
			}
			else if (STRICMP(argv[i], "-mp") == 0)
			{
				defaultitems.clear();
				i++;
				while ((i < argc))
				{
					defaultitems.push_back(argv[i]);
					if ((i + 1) < argc && argv[i + 1][0] != '-')
						i++;
					else
						break;
				}
				if (defaultitems.size() > 0)
					requiredParameterCount++;
			}
		}

		// Check if command line argument contains required options 
		if (defaultitems.empty() || requiredParameterCount < 3)
		{
			displayUsage();
			return false;
		}
	}
	catch (exception& ex)
	{
		std::cout << "Command line Error exception occurred " << ex.what() << std::endl;
	}
	return true;
}


int main(int argc, char* argv[])
{
	try{
		if (!checkCommandLine(argc, argv))
			return -1;

		std::cout << "Hybrid application demo...\n\n";
		cout << "NIProvider server connecting to ADH..." << endl;
		NIProviderManager providerManager(defaultPubUserName, defaultPosition, defaultAppID,defaultInstanceID, bDumpOutput);
	
		providerManager.start();
		NIProviderClient* providerClient1 = &providerManager.getNIProviderCLient();
		if (!providerClient1->isConnectionUp())
		{
			cout << "NI Provider unable to login to ADH server" << std::endl;
			cout << "Stop Provider Manager and exit" << endl;
			providerManager.stop();
			return -1;
		}
		cout << "NIProvder ready to publish data..." << endl;
		
		cout << "Publish Directory "<<defaultNIPubServiceName<<" with service id "<<defaultNIPubServiceID<<" to ADH server..." << endl;
		try{
			// Submit Directory to ADH with initial OpenLimit 50000
			providerManager.getOmmNIProvider().submit(RefreshMsg().domainType(MMT_DIRECTORY).filter(SERVICE_INFO_FILTER | SERVICE_STATE_FILTER)
				.payload(Map()
				.addKeyUInt(defaultNIPubServiceID, MapEntry::AddEnum, FilterList()
				.add(SERVICE_INFO_ID, FilterEntry::SetEnum, ElementList()
					.addAscii(ENAME_NAME, defaultNIPubServiceName)
					.addAscii(ENAME_VENDOR, "Vender1")
					.addUInt(ENAME_IS_SOURCE, 1)
					.addArray(ENAME_CAPABILITIES, OmmArray().addUInt(MMT_DICTIONARY).addUInt(MMT_MARKET_PRICE).addUInt(MMT_MARKET_BY_PRICE).complete())
					.addArray(ENAME_DICTIONARYS_USED, OmmArray().addAscii("RWFFld").addAscii("RWFEnum").complete())
					.addArray(ENAME_DICTIONARYS_PROVIDED, OmmArray().addAscii("RWFFld").addAscii("RWFEnum").complete())
					.addArray(ENAME_QOS, OmmArray().addQos(OmmQos::Timeliness::RealTimeEnum, OmmQos::Rate::TickByTickEnum).complete())
					.addUInt(ENAME_SUPPS_OOB_SNAPSHOTS,0)
					.addUInt(ENAME_ACCEPTING_CONS_STATUS, 0)
					.addUInt(ENAME_SUPPS_QOS_RANGE, 0)
				.complete())
				.add(SERVICE_STATE_ID, FilterEntry::SetEnum, ElementList()
				.addUInt(ENAME_SVC_STATE, SERVICE_UP)
				.addUInt(ENAME_ACCEPTING_REQS,1)
				.addState(ENAME_STATUS,OmmState::OpenEnum,OmmState::OkEnum,0,"OK")
				.complete())
				.add(SERVICE_LOAD_ID, FilterEntry::SetEnum, ElementList()
				.addUInt(ENAME_LOAD_FACT, 1)
				.addUInt(ENAME_OPEN_LIMIT, 50000)
				.complete())
				.complete())
				.complete()).complete(), defaultNISourceDirectoryHandle);
		}
		catch (OmmInvalidUsageException& ex)
		{
			cout << "Error:" << ex.getText() << endl;
		}
		cout << "Consumer connecting to Provider server..." << endl;
		ConsumerManager consumerManager(defaultSubUserName, defaultPosition, defaultAppID, bDumpOutput);
		ConsumerClient* consumerClient = &consumerManager.getConsumerClient();
		consumerClient->setNIProvider(providerManager.getOmmNIProvider());
		consumerClient->setNIProviderClient(*providerClient1);
		consumerClient->setNIPubServiceName(defaultNIPubServiceName);
		if (bUsePrefix)
			consumerClient->usePrefix(true, prefixString);
		consumerManager.start();
		if (!consumerClient->isConnectionUp())
		{
			cout << "OmmConsumer unable to login to Provider server" << std::endl;
			cout << "Stop Provider and Consumer Manager and exit" << std::endl;
			return -1;
		}
		cout << "Consumer login accepted..." << std::endl;
		cout << "Start requesting data from Provider server..." << endl;

		//Add item to item list and generate handle for publisher side.
		ITEMLIST itemList;
		consumerClient->setItemList(itemList);
		ITEMLIST::iterator it;
		for (std::list<string>::iterator strit = defaultitems.begin(); strit != defaultitems.end(); strit++)
		{
			ITEMLIST::iterator it;
			it = itemList.find(*strit);
			if (it == itemList.end())
			{
				
				ITEMHANDLES tempHandle;
				cout << "Add " << (*strit).c_str() << " to item list" << endl;
				// Send Item request from Provider/ADS server
				if (!bUseView)
				{
					cout << "	Send Item Request for item " << (*strit).c_str() << endl;
					tempHandle.ConsumerHandle = consumerManager.getOmmConsumer().registerClient(ReqMsg().serviceName(defaultConsumerServiceName)
						.name((*strit).c_str()), *consumerClient, (void*)((*strit).c_str()));
				}
				else
				{
					// Send Item request with view
					cout << "	Send Item with View Request for item " << (*strit).c_str() << endl;
					OmmArray filter;
					for (std::list<int>::iterator intIt = defaultView.begin(); intIt != defaultView.end(); intIt++)
					{
						filter.addInt((int)*intIt);
					}
					filter.complete();
					ElementList viewElementList;
					viewElementList.addUInt(ENAME_VIEW_TYPE, 1).addArray(ENAME_VIEW_DATA, filter);
					viewElementList.complete();
					tempHandle.ConsumerHandle = consumerManager.getOmmConsumer().registerClient(ReqMsg().serviceName(defaultConsumerServiceName)
						.name((*strit).c_str()).payload(viewElementList), *consumerClient, (void*)((*strit).c_str()));
				}
				tempHandle.NiPubHandle = handleStart;
				itemList[*strit] = tempHandle;
				handleStart++;
			}
		}

		sleep(runtime * 1000);
		cout << "Runtime expired. Stop Consumer and Provider thread and exit." << endl;
		consumerManager.stop();
		providerManager.stop();
	}
	catch (exception& ex)
	{
		std::cout << "Exception occurred "<< ex.what() << std::endl;
	}
	return 0;
}