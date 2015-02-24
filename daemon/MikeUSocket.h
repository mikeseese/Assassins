#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>

#ifndef INCLUDES
#define INCLUDES
#include "includes.h"
#endif

#ifndef FUNCTIONS
#define FUNCTIONS
#include "functions.h"
#endif

class MikeUSocket
{
	// Construction
	public:
		MikeUSocket(bool debug = false);	// standard constructor
		int mySocket;
		sockaddr_in connection_info;
		string ipAddress;
		int port;
		int Start(bool tcp = true);
		void Close();
		int Listen(int port);
		int Connect(string ipAddress, int portNumber, bool isHostName = false);
		int Accept(int kListen);
		int Accept(MikeUSocket kListen);
		int Accept();
		int Receive(char *buf, int len, int flags = 0);
		int Send(const char *data, int len, int flags = 0);
		int Send(const char *data);
		int Send(string data);
		int Send(int data);
		int Send(int data, int len);
		int GetSocket();
		void SetSocket(int kSocket);
		string GetIP();
		int GetPort();
	
	private:
		struct sockaddr_in socketInfo;
		bool display;
};

typedef MikeUSocket MUSocket;

MikeUSocket::MikeUSocket(bool debug)
{
	//socketInfo = {0};
	display = debug;
}

int MikeUSocket::Start(bool tcp)
{
	//Start winsock
	int kSocket = NULL;

	if(tcp)
		kSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	else
		kSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(kSocket < 0)
	{
		if(display)
			cout << "Socket init failed";
		return 0;
	}
	if(display)
		cout << "Socket initialized\n";
	SetSocket(kSocket);
	return 1;
}

void MikeUSocket::Close()
{
	int kSocket = GetSocket();
	close(kSocket);
	SetSocket(kSocket);
}

int MikeUSocket::Listen(int portNumber)
{
	int kSocket = GetSocket();
	sockaddr_in sIN;
	sIN.sin_port = htons(portNumber); //Port number
	sIN.sin_addr.s_addr = INADDR_ANY;
	sIN.sin_family = AF_INET;
	if(bind(kSocket,(sockaddr*)&sIN, sizeof(sIN)) < 0)
	{
		cout << "Failed to bind" << endl;
		Close();
		return 0;
	}
	if(display)
		cout << "Bind successful!\n";
	if(display)
		cout << "Configuring listen socket...";
	listen(kSocket, SOMAXCONN);
	if(display)
		cout << "Socket configured and listening!" << endl;
	SetSocket(kSocket);
	return 1;
}

int MikeUSocket::Accept()
{
	return Accept(GetSocket());
}

int MikeUSocket::Accept(MUSocket kListen)
{
	return Accept(kListen.GetSocket());
}

int MikeUSocket::Accept(int kListen)
{
	int kSocket = NULL;
  socklen_t size = sizeof(connection_info);
  kSocket = accept(kListen, (sockaddr *) &connection_info, &size);
  if(kSocket < 0)
  {
		cout << "Invalid Socket." << std::endl;
		return 0;
	}
	SetSocket(kSocket);
  return 1;
}

int MikeUSocket::Connect(string ipAddress, int portNumber, bool isHostName)
{
	int kSocket = GetSocket();
	int err = -1;
	sockaddr_in sIN;
	//addrinfo aiHints;
	struct addrinfo *aiList = NULL;

	sIN.sin_port = htons(portNumber); //Port number
	if(isHostName)
	{
		struct hostent *he;
		he = gethostbyname(ipAddress.c_str());
		if(he == NULL)
    {
			return 0;
    }
		char * ip = inet_ntoa (*((struct in_addr *) he->h_addr_list[0]));
		sIN.sin_addr.s_addr = inet_addr(ip);
	}
	else
		sIN.sin_addr.s_addr = inet_addr(ipAddress.c_str());
	sIN.sin_family = AF_INET;
	if(display)
		cout << "Waiting for connection...";
	while(err < 0)
	{ 
		//Delay to prevent 100% cpu usage
		err = connect(kSocket,(sockaddr*)&sIN, sizeof(sIN));
		sleep(1); //sleep for 1 seconds
	}
	if(display)
		cout << "Connection established." << endl;
	SetSocket(kSocket);
	return 1;
}

int MikeUSocket::Receive(char *buf, int len, int flags)
{
	int kSocket = GetSocket();
	return recv(kSocket, buf, len, flags);
}

int MikeUSocket::Send(int data, int len)
{
	return Send(itos(data).c_str(), len);
}

int MikeUSocket::Send(int data)
{
	return Send(itos(data));
}

int MikeUSocket::Send(string data)
{
	return Send(data.c_str(), data.length());
}

int MikeUSocket::Send(const char *data)
{
	string s = data;
	return Send(s.c_str(), s.length());
}

int MikeUSocket::Send(const char *data, int len, int flags)
{
	int kSocket = GetSocket();
	return send(kSocket, data, len, flags);
}

int MikeUSocket::GetSocket()
{
	return mySocket;
}

void MikeUSocket::SetSocket(int kSocket)
{
	mySocket = kSocket;
}

string MikeUSocket::GetIP()
{
	if(ipAddress == "")
	{
		ipAddress = inet_ntoa(connection_info.sin_addr);
	}

	return ipAddress;
}

int MikeUSocket::GetPort()
{
	if(port == NULL)
	{
		port = ntohs(connection_info.sin_port);
	}

	return port;
}
