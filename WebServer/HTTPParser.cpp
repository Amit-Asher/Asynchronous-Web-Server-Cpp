#include "MainHeader.h"
#include "HTTPParser.h"
#include "HTTP.h"
#include <algorithm>
#define PRIVATE
#define PUBLIC
#define eHTTP HTTP::eHTTP /* using enum HTTP::eHTTP as eHTTP */

/* HTTP Parser */

PRIVATE int HTTPParser::findParametersBeginning(string i_Request) {
	int paramBeginning = 0;
	while (i_Request[paramBeginning++] != ' ') {}
	while (i_Request[paramBeginning++] != '?') {
		if (i_Request[paramBeginning] == ' ') {
			return -1;
		}
	}
	return paramBeginning;
}

PRIVATE void HTTPParser::parseString(string& i_Set, string& i_Request, int& i_Idx, vector<char> i_Cond) {
	char curChar;

	i_Set.clear();
	while (!count(i_Cond.begin(), i_Cond.end(), i_Request[i_Idx])) {
		curChar = i_Request[i_Idx++];
		i_Set.push_back(curChar);
	}
}

PUBLIC string HTTPParser::GetBody(string i_Request) {

	int i = 0;
	/* search the ending string: \r\n\r\n which divides between app headers and app body */
	while (!(i_Request[i]	  == '\r' && i_Request[i + 1] == '\n' &&
			 i_Request[i + 2] == '\r' && i_Request[i + 3] == '\n')) {
		i++;
	}

	string body = i_Request;
	body.erase(0, i + 4);
	return body;
}

PUBLIC string HTTPParser::GetTitle(string i_Request) {
	string title = "";
	char curChar;

	int i = 0;
	/* search the end of the first row \r */
	while (i_Request[i] != '\r') {
		curChar = i_Request[i++];
		title.push_back(curChar);
	}

	return title;
}

PUBLIC string HTTPParser::GetResource(string i_Request) {
	string resource = "";
	char curChar;

	int i = 0;
	/* search the word between first and second spaces in the first row */
	while (i_Request[i++] != ' ') {}
	while (i_Request[i] != ' ' && i_Request[i] != '?') {
		curChar = i_Request[i++];
		resource.push_back(curChar);
	}

	return resource;
}

PUBLIC unordered_map<string, string> HTTPParser::GetParameters(string i_Request) {

	unordered_map<string, string> parameters;
	string keyParameter;
	string value;
	vector<char> parseUntil = { ' ', ' ' };

	char curChar;

	int parametersBeginning = findParametersBeginning(i_Request);
	if (parametersBeginning != -1) {

		bool thereAreMoreParamters = true;
		int i = parametersBeginning;
		while (thereAreMoreParamters) {

			parseUntil[0] = '=';
			parseString(keyParameter, i_Request, i, parseUntil);
			if (i_Request[i++] == ' ') {
				thereAreMoreParamters = false;
			}
			else {
				parseUntil[0] = '&';
				parseString(value, i_Request, i, parseUntil);
				if (i_Request[i++] == ' ') {
					thereAreMoreParamters = false;
				}

				parameters.insert(pair<string, string>(keyParameter, value));
			}
		}
	}

	return parameters;
}

PUBLIC eHTTP HTTPParser::GetRequestType(string i_Request) {
	string requestType = "";
	eHTTP requestTypeToReturn;
	char curChar;

	int i = 0;
	while (i_Request[i] != ' ') {
		curChar = i_Request[i++];
		requestType.push_back(curChar);
	}

	if	(!strcmp(requestType.c_str(), "GET"))		requestTypeToReturn = eHTTP::_GET;
	else if (!strcmp(requestType.c_str(), "POST"))		requestTypeToReturn = eHTTP::_POST;
	else if (!strcmp(requestType.c_str(), "HEAD")) 		requestTypeToReturn = eHTTP::_HEAD;
	else if (!strcmp(requestType.c_str(), "OPTIONS")) 	requestTypeToReturn = eHTTP::_OPTIONS;
	else if (!strcmp(requestType.c_str(), "PUT")) 		requestTypeToReturn = eHTTP::_PUT;
	else if (!strcmp(requestType.c_str(), "DELETE")) 	requestTypeToReturn = eHTTP::_DELETE;
	else if (!strcmp(requestType.c_str(), "TRACE")) 	requestTypeToReturn = eHTTP::_TRACE;
	else 							requestTypeToReturn = eHTTP::_ERROR;

	return requestTypeToReturn;
}

PUBLIC string HTTPParser::GetTime() {
	time_t timer = time(nullptr);
	struct tm  tstruct;
	tstruct = *localtime(&timer);
	char curTime[100];
	strftime(curTime, sizeof(curTime), "%a, %d %b %Y %X GMT", &tstruct);
	string curTimeStr = curTime;
	return curTimeStr;
}

PUBLIC string HTTPParser::GetLastModified(string i_FileName) {
	struct stat result;
	stat(i_FileName.c_str(), &result);
	struct tm  tstruct;
	tstruct = *localtime(&result.st_mtime);
	char lastModified[100];
	strftime(lastModified, sizeof(lastModified), "%a, %d %b %Y %X GMT", &tstruct);
	string lastModifiedStr = lastModified;
	return lastModifiedStr;
}
