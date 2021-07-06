#include "WebServer.h"
#define LISTEN_PORT 80
#define LISTEN_QUEUE_SIZE 5
#define MAX_SOCKETS 60
#define TIMEOUT_SEC 120

void main()
{
	try {
		WebServer server(MAX_SOCKETS, LISTEN_PORT, LISTEN_QUEUE_SIZE, TIMEOUT_SEC);
		cout << "server is running at localhost:" << LISTEN_PORT << endl;
		server.Run();
	}

	catch (const char* msg) {
		cout << msg << endl;
	}
}