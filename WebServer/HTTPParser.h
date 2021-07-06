#pragma once
#include "HTTP.h"
#include <unordered_map>

class HTTPParser {

private:
	static int findParametersBeginning(string i_Request);
	static void parseString(string& i_Set, string& i_Request, int& i_Idx, vector<char> i_Cond);


public:
	static string GetBody(string i_Request);
	static string GetTitle(string i_Request);
	static string GetResource(string i_Request);
	static unordered_map<string, string> GetParameters(string i_Request);
	static HTTP::eHTTP GetRequestType(string i_Request);
	static string GetTime();
	static string GetLastModified(string i_FileName);
};