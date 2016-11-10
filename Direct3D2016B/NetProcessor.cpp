#include "stdafx.h"
#include "NetProcessor.h"


CNetProcessor::CNetProcessor(CStateMachineManager* pHSMOwner)
{
	InitializeCriticalSection(&m_csLock);
	m_pOwner = pHSMOwner;
}

bool CNetProcessor::InitNetwork(void)
{
	WSAStartup(MAKEWORD(2, 2), &m_wsa);
	DWORD dwThreadID;
	SOCKADDR_IN Service;
	m_Listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	InetPton(AF_INET, L"0.0.0.0", &Service.sin_addr);
	Service.sin_port = htons(6112);
	Service.sin_family = AF_INET;
	if (SOCKET_ERROR == bind(m_Listener, (SOCKADDR*)&Service, sizeof(SOCKADDR_IN)))
	{
		closesocket(m_Listener);
		m_Listener = 0;
		return false;
	}
		
	listen(m_Listener, SOMAXCONN);

	CreateThread(NULL, 4096, (LPTHREAD_START_ROUTINE)CNetProcessor::ServiceListener, this, 0, &dwThreadID);
	return true;
}

void CNetProcessor::DoNetworkTask(void)
{
	Lock();
	for (auto s:m_lstEventsRecived)
	{
		auto d = new CEventNetwork();
		*d = *s;
		m_lstEventsToSend.push_back(d);
	}

	while(!m_lstEventsToSend.empty())
	{
		CEventNetwork* pEvent = m_lstEventsToSend.front();
		
		for (auto c : m_mapClients)
		{
			if (pEvent->Source != c.first)
			{
				send(c.second.Client, (char*)pEvent->m_nDataSize, sizeof(unsigned long), 0);
				send(c.second.Client, (char*)pEvent->m_Data, pEvent->m_nDataSize, 0);
			}
		}
		m_lstEventsToSend.pop_front();
		delete pEvent;
	}

	for (auto e : m_lstEventsRecived)
	{
		m_pOwner->PostEvent(e);
	}
	m_lstEventsRecived.clear();
	Unlock();
}

void CNetProcessor::Uninitialize(void)
{
	DeleteCriticalSection(&m_csLock);
}

DWORD CNetProcessor::ServiceListener(CNetProcessor * pNP)
{
	while (1)
	{
		DWORD dwThreadID;
		SOCKADDR_IN ClientAddress;
		int nClientAddrSize = sizeof(SOCKADDR_IN);
		SOCKET Client = accept(pNP->m_Listener, (SOCKADDR*)&ClientAddress, &nClientAddrSize);
		CLIENT &NewClient = *new CLIENT;
		NewClient.Client = Client;
		NewClient.NickName = "QuePedo";
		NewClient.pOwner = pNP;
		printf("Esta creando hilos a lo pendejo \n");
		pNP->Lock();
		pNP->m_mapClients.insert_or_assign(Client, NewClient);
		pNP->Unlock();
		CreateThread(NULL, 4096, (LPTHREAD_START_ROUTINE) CNetProcessor::ClientThread,&NewClient,0,&dwThreadID);
	}
	return 0;
}

DWORD CNetProcessor::ClientThread(CLIENT * pClient)
{
	while (1)
	{
		CEventNetwork* pEventNetwork = new CEventNetwork;
		pEventNetwork->m_ulEventType = EVENT_NETWORK;
		if (SOCKET_ERROR == recv(pClient->Client, (char*)pEventNetwork->m_nDataSize, sizeof(unsigned long), 0))
		{
			delete pEventNetwork;
			break;
		}

		if (SOCKET_ERROR == recv(pClient->Client, (char*)pEventNetwork->m_Data,pEventNetwork->m_nDataSize,0))
		{
			delete pEventNetwork;
			break;
		}

		pClient->pOwner->Lock();
		pEventNetwork->Source = pClient->Client;
		pClient->pOwner->m_lstEventsRecived.push_back(pEventNetwork);
		pClient->pOwner->Unlock();

	}

	// Quitarnos de la lista
	pClient->pOwner->Lock();
	pClient->pOwner->m_mapClients.erase(pClient->Client);
	pClient->pOwner->Unlock();

	closesocket(pClient->Client);
	delete pClient;


	return 0;
}


CNetProcessor::~CNetProcessor()
{
}

unsigned long CNetProcessor::OnEvent(CEventBase * pEvent)
{
	((CEventNetwork*)pEvent)->Source = 0;
	Lock();
	m_lstEventsToSend.push_back((CEventNetwork*)pEvent);
	Unlock();
	return 0;
}

bool CNetProcessor::Connect(wchar_t * pszAddress)
{
	SOCKET Server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKADDR_IN ServerAddress;
	InetPton(AF_INET, pszAddress, &ServerAddress.sin_addr);

	ServerAddress.sin_family = AF_INET;
	ServerAddress.sin_port = htons(6112);
	if(SOCKET_ERROR == connect(Server, (SOCKADDR*)&ServerAddress, sizeof(ServerAddress))) 
	{
		closesocket(Server);
		return false;
	}
	
	CLIENT Client;
	Client.NickName = "HolaMundo";
	Client.pOwner = this;
	Client.Client = Server;
	Lock();
		m_mapClients.insert_or_assign(Server, Client);
	Unlock();
}
