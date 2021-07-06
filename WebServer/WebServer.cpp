#include "WebServer.h"
#include "HTTP.h"
#include "HTTPParser.h"
#define PRIVATE
#define PUBLIC
#define eHTTP HTTP::eHTTP /* using enum HTTP::eHTTP as eHTTP */

/* -------------- Private Methods -------------- */

/* ----- Settings Methods ------ */

PRIVATE bool WebServer::addSocket(SOCKET i_Id, eRecvStatus i_RecvState)
{
	for (int i = 0; i < m_MaxSockets; i++)
	{
		if (m_Sockets[i].RecvStatus == eRecvStatus::EMPTY)
		{
			m_Sockets[i].Id = i_Id;
			m_Sockets[i].RecvStatus = i_RecvState;
			m_Sockets[i].SendStatus = eSendStatus::IDLE;
			m_Sockets[i].StartTimeout = (unsigned int)GetTickCount();
			m_SocketsCount++;
			return true;
		}
	}
	return false;
}

PRIVATE void WebServer::removeSocket(int i_Index)
{
	m_Sockets[i_Index].Id = 0;
	m_Sockets[i_Index].RecvStatus = eRecvStatus::EMPTY;
	m_Sockets[i_Index].SendStatus = eSendStatus::EMPTY;
	m_Sockets[i_Index].Request.clear();
	m_Sockets[i_Index].SecondRequest.clear();
	m_Sockets[i_Index].StartTimeout = 0;
	m_SocketsCount--;
}

PRIVATE void WebServer::throwIfInvalidSocket(SOCKET i_Socket) {
	if (INVALID_SOCKET == i_Socket)
	{
		throw "Web Server: Error at throwIfInvalidSocket method after accept(): " + WSAGetLastError();
	}
}

PRIVATE void WebServer::makeNonBlocking(SOCKET& i_MsgSocket) {
	unsigned long flag = 1;
	if (ioctlsocket(i_MsgSocket, FIONBIO, &flag) != 0)
	{
		throw "Web Server: Error at makeNonBlocking method while calling ioctlsocket(): " + WSAGetLastError();
	}
}

PRIVATE int WebServer::getSocketIdx(SOCKET i_Socket) {
	for (int i = 0; i < m_MaxSockets; i++)
	{
		if (m_Sockets[i].Id == i_Socket)
		{
			return i;
		}
	}
	return -1;
}

PRIVATE void WebServer::addNewRecvSocket(SOCKET& i_Id, SOCKET& i_MsgSocket) {
	if (addSocket(i_MsgSocket, eRecvStatus::RECEIVE) == false)
	{
		closesocket(i_Id);
		throw "Web Server: Error at acceptConnection method while calling addSockets(): "
			"Server is busy and cant handle another client at the moment.";
	}
}

PRIVATE void WebServer::acceptConnection(int i_Index)
{
	try {
		SOCKET id = m_Sockets[i_Index].Id;
		struct sockaddr_in from;
		int fromLen = sizeof(from);
		SOCKET msgSocket = accept(id, (struct sockaddr*)&from, &fromLen);
		throwIfInvalidSocket(msgSocket);
		makeNonBlocking(msgSocket);
		addNewRecvSocket(id, msgSocket);
		int newSocketIdx = getSocketIdx(msgSocket);
		m_Sockets[newSocketIdx].ClientIP = inet_ntoa(from.sin_addr);
		m_Sockets[newSocketIdx].ClientPort = ntohs(from.sin_port);
		cout << "Web Server: Client " << m_Sockets[newSocketIdx].ClientIP << ":"
			 << m_Sockets[newSocketIdx].ClientPort << " is connected." << endl;
	}
	catch (const char* msg) {
		cout << msg << endl;
	}
}

PRIVATE void WebServer::startWSA() {
	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &m_WsaData))
	{
		throw "Web Server: Error at startWSA method while calling WSAStartup().";
	}
}

PRIVATE void WebServer::setListenSocket(const int i_ListenPort) {
	m_ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (INVALID_SOCKET == m_ListenSocket)
	{
		WSACleanup();
		throw "Web Server: Error at setListenSocket method while calling socket(): " + WSAGetLastError();
	}

	bindListenSocket(i_ListenPort);
}

PRIVATE void WebServer::bindListenSocket(const int i_ListenPort) {
	m_ServerService.sin_family = AF_INET;
	m_ServerService.sin_addr.s_addr = INADDR_ANY;
	m_ServerService.sin_port = htons(i_ListenPort);
	if (SOCKET_ERROR == bind(m_ListenSocket, (SOCKADDR*)&m_ServerService, sizeof(m_ServerService)))
	{
		closesocket(m_ListenSocket);
		WSACleanup();
		throw "Web Server: Error at bindListenSocket method while calling bind(): " + WSAGetLastError();
	}
}

PRIVATE void WebServer::startListening(const int i_ListenQueueSize) {
	if (SOCKET_ERROR == listen(m_ListenSocket, i_ListenQueueSize))
	{
		closesocket(m_ListenSocket);
		WSACleanup();
		throw "Web Server: Error at startListening method while calling listen(): " + WSAGetLastError();
	}
}


/* ----- Main Loop Methods ------ */

PRIVATE void WebServer::handleWaitRecv(fd_set& io_WaitRecv, int& io_Nfd) {

	for (int i = 0; i < m_MaxSockets && io_Nfd > 0; i++)
	{
		if (FD_ISSET(m_Sockets[i].Id, &io_WaitRecv))
		{
			io_Nfd--;
			switch (m_Sockets[i].RecvStatus)
			{
			case eRecvStatus::LISTEN:
				acceptConnection(i);
				break;

			case eRecvStatus::RECEIVE:
				receiveMessage(i);
				break;
			}
		}
	}
}

PRIVATE void WebServer::handleWaitSend(fd_set& io_WaitSend, int& io_Nfd) {

	for (int i = 0; i < m_MaxSockets && io_Nfd > 0; i++)
	{
		if (FD_ISSET(m_Sockets[i].Id, &io_WaitSend))
		{
			io_Nfd--;
			switch (m_Sockets[i].SendStatus)
			{
			case eSendStatus::SEND:
				sendMessage(i);
				break;
			}
		}
	}
}

PRIVATE void WebServer::initializeWaitSets(fd_set& io_WaitRecv, fd_set& io_WaitSend) {

	/* iterate m_Sockets buffer and insert the sockets in listen\recv state to waitRecv set*/
	FD_ZERO(&io_WaitRecv); /* initialize waitRecv set */
	for (int i = 0; i < m_MaxSockets; i++)
	{
		if (m_Sockets[i].RecvStatus != eRecvStatus::EMPTY)
			FD_SET(m_Sockets[i].Id, &io_WaitRecv);
	}

	/* iterate m_Sockets buffer and insert the sockets in send state to waitSend set*/
	FD_ZERO(&io_WaitSend); /* initialize waitSend set */
	for (int i = 0; i < m_MaxSockets; i++) 
	{
		if (m_Sockets[i].SendStatus == eSendStatus::SEND)
			FD_SET(m_Sockets[i].Id, &io_WaitSend);
	}
}

PRIVATE void WebServer::receiveMessage(int i_Index)
{
	SOCKET msgSocket = m_Sockets[i_Index].Id;

	int bytesRecv = recv(msgSocket, m_SocketBuffer, sizeof(m_SocketBuffer), 0);
	if (!validRequest(i_Index, bytesRecv)) return;

	else
	{
		m_SocketBuffer[bytesRecv] = '\0';
		setRequest(i_Index);
		m_Sockets[i_Index].SendStatus = eSendStatus::SEND;
	}
}

PRIVATE void WebServer::sendMessage(int i_Index)
{
	int bytesSent = 0;
	string response;
	string request = getRequest(i_Index);
	string getSource(request);
	eHTTP requestType = HTTPParser::GetRequestType(request);

	switch (requestType)
	{
	case eHTTP::_GET:
		response = HTTP::GetResponse(request);
		break;
	case eHTTP::_POST:
		cout << HTTPParser::GetBody(request) << endl;
		response = HTTP::PostResponse(request);
		break;
	case eHTTP::_HEAD:
		response = HTTP::HeadResponse(request);
		break;
	case eHTTP::_OPTIONS:
		response = HTTP::OptionsResponse(request);
		break;
	case eHTTP::_PUT:
		response = HTTP::PutResponse(request);
		break;
	case eHTTP::_DELETE:
		response = HTTP::DeleteResponse(request);
		break;
	case eHTTP::_TRACE:
		response = HTTP::TraceResponse(request);
		break;
	default:
		response = HTTP::ErrorResponse(request);
		break;
	}

	sendResponseInBuffer(response, i_Index);
	m_Sockets[i_Index].StartTimeout = (unsigned int)GetTickCount();
}

PRIVATE void WebServer::sendResponseInBuffer(string i_Response, int i_Index) {

	int bytesSent = 0;
	SOCKET msgSocket = m_Sockets[i_Index].Id;
	strcpy(m_SocketBuffer, i_Response.c_str());

	m_SocketBuffer[i_Response.length() - 1] = 0;
	bytesSent = send(msgSocket, m_SocketBuffer, (int)strlen(m_SocketBuffer), 0);
	if (SOCKET_ERROR == bytesSent)
	{
		cout << "Web Server: Error at send(): " << WSAGetLastError() << endl;
		return;
	}
}

PRIVATE bool WebServer::validRequest(int i_Index, int i_BytesRecv) {

	SOCKET msgSocket = m_Sockets[i_Index].Id;

	if (SOCKET_ERROR == i_BytesRecv)
	{
		cout << "Web Server: Error at recv(): " << WSAGetLastError() << endl;
		closesocket(msgSocket);
		removeSocket(i_Index);
		return false;
	}

	if (i_BytesRecv == 0)
	{
		/* client close the connection */
		closesocket(msgSocket);
		removeSocket(i_Index);
		return false;
	}

	return true;
}

PRIVATE void WebServer::setRequest(int i_Index) {

	if (m_Sockets[i_Index].SendStatus == eSendStatus::SEND) {
		m_Sockets[i_Index].SecondRequest = m_SocketBuffer;
	}
	else { /* m_Sockets[index].send == IDLE */
		m_Sockets[i_Index].Request = m_SocketBuffer;
	}
}

PRIVATE string WebServer::getRequest(int i_Index) {
	
	string request = m_Sockets[i_Index].Request;

	if (m_Sockets[i_Index].SecondRequest.empty()) {
		m_Sockets[i_Index].Request.clear();
		m_Sockets[i_Index].SendStatus = eSendStatus::IDLE;
	}

	else /* msg2 not empty */ {
		m_Sockets[i_Index].Request = m_Sockets[i_Index].SecondRequest;
		m_Sockets[i_Index].SecondRequest.clear();
		m_Sockets[i_Index].SendStatus = eSendStatus::SEND;
	}
	return request;
}

PRIVATE bool WebServer::isActiveSocket(const int i_Idx) {
	return m_Sockets[i_Idx].RecvStatus != eRecvStatus::EMPTY || m_Sockets[i_Idx].SendStatus != eSendStatus::EMPTY;
}

PRIVATE void WebServer::checkForTimeout() {

	unsigned int endTimeout;
	string response;

	for (int i = 1; i < m_MaxSockets; i++)
	{
		if (isActiveSocket(i))
		{
			endTimeout = (unsigned int)GetTickCount();
			if (endTimeout - m_Sockets[i].StartTimeout > (m_TimeoutInterval * 1000)) {
				cout << "Web Server: Client " << m_Sockets[i].ClientIP << ":" << m_Sockets[i].ClientPort << " disconnected." << endl;
				response = HTTP::TimeoutResponse();
				sendResponseInBuffer(response, i);
				closesocket(m_Sockets[i].Id);
				removeSocket(i);
			}
			endTimeout = 0;
		}
	}
}


/* -------------- Public Methods -------------- */

PUBLIC WebServer::WebServer(const int i_MaxSockets, const int i_ListenPort, const int i_ListenQueueSize, const int i_TimeoutInterval) {
	m_MaxSockets = i_MaxSockets;
	m_Sockets = new SocketState[m_MaxSockets]{ 0 };
	m_SocketsCount = 0;
	m_TimeoutInterval = i_TimeoutInterval;
	startWSA();
	setListenSocket(i_ListenPort);
	startListening(i_ListenQueueSize);
	addSocket(m_ListenSocket, eRecvStatus::LISTEN);
}

PUBLIC void WebServer::Run() {

	const timeval* timeoutInterval = new timeval { m_TimeoutInterval , 0 };

	while (true)
	{
		fd_set waitRecv;
		fd_set waitSend;
		initializeWaitSets(waitRecv, waitSend);
		
		int nfd = select(0, &waitRecv, &waitSend, NULL, timeoutInterval);

		if (nfd == SOCKET_ERROR)
		{
			WSACleanup();
			throw "Web Server: Error at select(): " + WSAGetLastError();
		}

		checkForTimeout();
		handleWaitRecv(waitRecv, nfd);
		handleWaitSend(waitSend, nfd);
	}

	cout << "Web Server: Closing Connection.\n";
	closesocket(m_ListenSocket);
	WSACleanup();
}
