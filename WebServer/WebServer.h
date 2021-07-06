#pragma once
#include "MainHeader.h"

class WebServer {

private:

	enum class eSendStatus { EMPTY, IDLE, SEND };
	enum class eRecvStatus { EMPTY, LISTEN, RECEIVE };

	struct SocketState
	{
		SOCKET Id;
		eRecvStatus	RecvStatus = eRecvStatus::EMPTY;
		eSendStatus	SendStatus = eSendStatus::EMPTY;
		string Request;
		string SecondRequest;
		string ClientIP;
		u_short ClientPort;
		unsigned int StartTimeout;
	};

	SocketState* m_Sockets;
	WSAData m_WsaData;
	SOCKET m_ListenSocket;
	sockaddr_in m_ServerService;
	int m_SocketsCount;
	int m_MaxSockets;
	int m_TimeoutInterval;
	char m_SocketBuffer[BUFFERSIZE];

	/* ----- Settings Methods ------ */
	bool addSocket(SOCKET i_Id, eRecvStatus i_RecvState);
	void removeSocket(int i_Index);
	void throwIfInvalidSocket(SOCKET i_Socket);
	void makeNonBlocking(SOCKET& i_MsgSocket);
	int getSocketIdx(SOCKET i_Socket);
	void addNewRecvSocket(SOCKET& i_Id, SOCKET& i_MsgSocket);
	void acceptConnection(int i_Index);
	void startWSA();
	void setListenSocket(const int i_ListenPort);
	void bindListenSocket(const int i_ListenPort);
	void startListening(const int i_ListenQueueSize);

	/* ----- Main Loop Methods ------ */
	bool validRequest(int i_Index, int i_BytesRecv);
	void setRequest(int i_Index);
	void receiveMessage(int i_Index);
	void sendResponseInBuffer(string i_Response, int i_Index);
	string getRequest(int i_Index);
	void sendMessage(int i_Index);
	void initializeWaitSets(fd_set& io_WaitRecv, fd_set& io_WaitSend);
	void handleWaitRecv(fd_set& io_WaitRecv, int& io_Nfd);
	void handleWaitSend(fd_set& io_WaitSend, int& io_Nfd);
	bool isActiveSocket(const int i_Idx);
	void checkForTimeout();

public:

	WebServer(const int i_MaxSockets, const int i_ListenPort, const int i_ListenQueueSize, const int i_TimeoutInterval);
	void Run();
};