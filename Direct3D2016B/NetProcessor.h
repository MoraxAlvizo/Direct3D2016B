#pragma once
#include "HSM\EventListener.h"
#include "HSM\EventBase.h"
#include "HSM\StateMachineManager.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <map>
#include <list>

using namespace std;

class CNetProcessor;

struct CLIENT
{
	SOCKET Client;
	string NickName;
	CNetProcessor* pOwner;
};

#define EVENT_NETWORK 0xaa55aa55
class CEventNetwork :
	public CEventBase
{
	friend CNetProcessor; // Socket fuente del evento, Un client 
protected:
	SOCKET		  Source;
public:
	unsigned long m_nDataSize; // Datagrama de aplicacion 
	unsigned char* m_Data[512];

};

class CNetProcessor :
	public CEventListener
{
protected:
	WSADATA m_wsa;
	SOCKET m_Listener;
	map<SOCKET, CLIENT> m_mapClients;
	SOCKET m_Server;
	list<CEventNetwork*> m_lstEventsToSend;
	list<CEventNetwork*> m_lstEventsRecived;
	CRITICAL_SECTION m_csLock; // Mutex
	CStateMachineManager* m_pOwner;

public:
	void Lock() { EnterCriticalSection(&m_csLock); }
	void Unlock() { LeaveCriticalSection(&m_csLock); }
	CNetProcessor(CStateMachineManager* pHSMOwner);
	bool InitNetwork(void);
	void DoNetworkTask(void);
	void Uninitialize(void);
	static DWORD WINAPI ServiceListener(CNetProcessor* pNP);
	static DWORD WINAPI ClientThread(CLIENT* pClient);
	virtual ~CNetProcessor();

	unsigned long OnEvent(CEventBase* pEvent);
	bool Connect(wchar_t* pszAddress);
};

