#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <string.h>
#include <time.h>
#include <fstream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <ctime>
using namespace std;
#pragma comment(lib, "Ws2_32.lib")

/* MACROS */
#define BUFFERSIZE 1000000