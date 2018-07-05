#include "icmp.h"

Ping::Ping()
{

}
bool Ping::InitWSA()
{
	//初始化Socket库
	WSADATA WsaData;
	if(WSAStartup(MAKEWORD(2,2),&WsaData)!=NO_ERROR)
		return false;
	return true;
}
void Ping::CloseWSA()
{
	WSACleanup();
}
int Ping::ping(LPSTR Host)
{
	HOSTENT *Hostent;
	//初始化套接字
	this->Socket=socket(AF_INET,SOCK_RAW,IPPROTO_ICMP);
	//创建SOCK_RAW(原始套接字)需要管理员权限 否则会返回10013错误
	if(this->Socket==SOCKET_ERROR)
		return -1;
	//通过域名查找目标IP(或者直接返回IP)
	Hostent=gethostbyname(Host);
	if(Hostent==NULL)
		return -2;
	this->IP=inet_ntoa(*((in_addr*)Hostent->h_addr_list[0]));
	this->SockAddr.sin_addr.S_un.S_addr=*((unsigned long*)Hostent->h_addr);
	this->SockAddr.sin_family=AF_INET;
	this->SockAddr.sin_port=0;//IMCP协议不指定端口
	//发送ICMP包
	if(!this->sendEchoRequest())
		return -3;
	if(!this->waitEchoRequest())
		return -4;
	if(this->PingTime>10000)
		return -5;
	return this->PingTime;
}
bool Ping::sendEchoRequest()
{
	ECHOREQUEST EchoRequest;
	static unsigned int Id=0;
	static unsigned int Seq=0;
	int Result;
	EchoRequest.ICMPHeader.Type=ICMP_ECHO;
	EchoRequest.ICMPHeader.Code=0;
	EchoRequest.ICMPHeader.Checksum=0;
	EchoRequest.ICMPHeader.Identifier=Id++;
	EchoRequest.ICMPHeader.Sequence=Seq++;
	for(register unsigned int i=0;i<4;i++)
		EchoRequest.Payload[i]=i;
	EchoRequest.ICMPHeader.Checksum=
		this->In_CheckSum((unsigned short*)&EchoRequest);
	this->PingTime=GetTickCount();
	Result=sendto(this->Socket,(LPSTR)&EchoRequest,sizeof(ECHOREQUEST),0,
		(SOCKADDR*)&this->SockAddr,sizeof(SOCKADDR_IN));
	if(Result==SOCKET_ERROR)
		return false;
	return true;
}
bool Ping::waitEchoRequest()
{
	int Result;
	timeval Timeout;
	fd_set ReadFd;
	Timeout.tv_sec=3;
	Timeout.tv_usec=0;
	ReadFd.fd_array[0]=this->Socket;
	ReadFd.fd_count=1;
	Result=select(1,&ReadFd,NULL,NULL,&Timeout);
	if(Result<=0)
		return false;
	DWORD nowTime=GetTickCount();
	this->PingTime=nowTime-this->PingTime;
	return true;
}
unsigned short Ping::In_CheckSum(unsigned short *Data)
{
	unsigned int Count=sizeof(ECHOREQUEST);
	//unsigned short是16Bit变量
	register long Sum=0;
	while(Count>1)
	{
		Sum+=*Data++;
		Count-=2;
	}
	if(Count>0)
		Sum+=*(unsigned char*)Data;
	while(Sum>>16)
		Sum=(Sum&0xFFFF)+(Sum>>16);
	return (unsigned short)~Sum;
}
char* Ping::GetIP()
{
	return this->IP;
}