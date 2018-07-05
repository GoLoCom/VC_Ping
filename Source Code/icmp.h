#pragma once
#include <Windows.h>
#include <WinSock.h>
#pragma comment(lib,"ws2_32.lib")
#define ICMP_ECHOREPLY 0
#define ICMP_ECHO 8

#pragma pack(1)

typedef struct _ICMPHEADER{
	unsigned char Type;
	unsigned char Code;
	unsigned short Checksum;
	unsigned short Identifier;
	unsigned short Sequence;
}ICMPHEADER;

typedef struct _ECHOREQUEST{
	ICMPHEADER ICMPHeader;
	char Payload[4];
}ECHOREQUEST;

#pragma pack()

class Ping{
public:
	Ping();
	static bool InitWSA();
	static void CloseWSA();
	int ping(LPSTR);
	char* GetIP();
protected:
	bool sendEchoRequest();
	bool waitEchoRequest();
private:
	char* IP;
	DWORD PingTime;
	unsigned short In_CheckSum(unsigned short*);
	sockaddr_in SockAddr;
	SOCKET Socket;
	ECHOREQUEST EchoRequest;
};