#include "MainHeader.h"
#include "HTTP.h"
#include "HTTPParser.h"
#define PRIVATE
#define PUBLIC

/* Responses */

PUBLIC string HTTP::GetResponse(string i_Request) {
	string curPath = "C:\\temp\\";
	string HTTPAppLayer;
	string source = HTTPParser::GetResource(i_Request);
	unordered_map<string, string> parameters = HTTPParser::GetParameters(i_Request);

	if (!strcmp(source.c_str(), "/site.html")) {
		if		(parameters.empty())						HTTPAppLayer = getHTTPAppLayer(curPath + "en_site.html");
		else if	(!strcmp(parameters["lang"].c_str(), "en")) HTTPAppLayer = getHTTPAppLayer(curPath + "en_site.html");
		else if (!strcmp(parameters["lang"].c_str(), "es")) HTTPAppLayer = getHTTPAppLayer(curPath + "es_site.html");
		else if (!strcmp(parameters["lang"].c_str(), "fr")) HTTPAppLayer = getHTTPAppLayer(curPath + "fr_site.html");
		else												HTTPAppLayer = getHTTPAppLayer(curPath + "site_404.html");
	}

	else {
		HTTPAppLayer = getHTTPAppLayer(curPath + "site_404.html");
	}

	return HTTPAppLayer;
}

PUBLIC string HTTP::PostResponse(string i_Request) {

	string curTimeStr = HTTPParser::GetTime();
	string HTTPAppLayer = 
		"HTTP/1.1 200 OK\r\n"
		"Server: WebServer\r\n"
		"Connection: Keep-Alive\r\n"
		"Date: " + curTimeStr + "\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: 35\r\n\r\n"
		"Post request successfully recieved!\n";

	return HTTPAppLayer;
}

PUBLIC string HTTP::HeadResponse(string i_Request) {

	string curPath = "C:\\temp\\";
	string HTTPAppLayer;
	string source = HTTPParser::GetResource(i_Request);
	unordered_map<string, string> parameters = HTTPParser::GetParameters(i_Request);

	if (!strcmp(source.c_str(), "/site.html")) {
		if		(parameters.empty())				   HTTPAppLayer = headHTTPAppLayer(curPath + "en_site.html");
		else if (!strcmp(parameters[0].c_str(), "en")) HTTPAppLayer = headHTTPAppLayer(curPath + "en_site.html");
		else if (!strcmp(parameters[0].c_str(), "es")) HTTPAppLayer = headHTTPAppLayer(curPath + "es_site.html");
		else if (!strcmp(parameters[0].c_str(), "fr")) HTTPAppLayer = headHTTPAppLayer(curPath + "fr_site.html");
		else										   HTTPAppLayer = headHTTPAppLayer(curPath + "site_404.html");
	}
	else {
		HTTPAppLayer = headHTTPAppLayer("C:\\temp\\site_404.html");
	}

	return HTTPAppLayer;
}

PUBLIC string HTTP::OptionsResponse(string i_Request) {

	string curTimeStr = HTTPParser::GetTime();
	string HTTPAppLayer = 
		"HTTP/1.1 200 OK\r\n"
		"Server: WebServer\r\n"
		"Connection: Keep-Alive\r\n"
		"Date: " + curTimeStr + "\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: 69\r\n"
		"Allow: OPTIONS, GET, HEAD, POST, PUT, DELETE, TRACE\r\n\r\n"
		"The allowed methods are: OPTIONS, GET, HEAD, POST, PUT, DELETE, TRACE\n";
	
	return HTTPAppLayer;
}

PUBLIC string HTTP::PutResponse(string i_Request) {

	string resource = HTTPParser::GetResource(i_Request);
	ePutType sendSubType = isFileExists(resource) ? ePutType::MODIFIED : ePutType::CREATED;
	string body = HTTPParser::GetBody(i_Request);
	string path = "C:\\temp\\" + resource;
	string curTimeStr = HTTPParser::GetTime();

	ofstream newFile;
	newFile.open(path);
	newFile << body;
	newFile.close();
	
	string HTTPAppLayer;
	
	switch (sendSubType) {
	case ePutType::CREATED:
		HTTPAppLayer = 
			"HTTP/1.1 201 Created\r\n"
			"Connection: Keep-Alive\r\n"
			"Server: WebServer\r\n"
			"Date: " + curTimeStr + "\r\n"
			"Content-Type: text/html\r\n"
			"Content-Length: 45\r\n\r\n"
			"The target resource was successfully created.\n";
		break;
	case ePutType::MODIFIED:
		HTTPAppLayer =
			"HTTP/1.1 204 No Content\r\n"
			"Connection: Keep-Alive\r\n"
			"Server: WebServer\r\n"
			"Date: " + curTimeStr + "\r\n"
			"Content-Type: text/html\r\n"
			"Content-Length: 0\r\n\r\n\n";
		break;
	}

	return HTTPAppLayer;
}

PUBLIC string HTTP::DeleteResponse(string i_Request) {

	string resource = HTTPParser::GetResource(i_Request);
	string curTimeStr = HTTPParser::GetTime();

	ofstream newFile;
	eDeleteType sendSubType = isFileExists(resource) ? eDeleteType::SUCCESS : eDeleteType::FAILED;
	string path = "C:\\temp\\" + resource;
	if (sendSubType == eDeleteType::SUCCESS) {
		remove(path.c_str());
	}

	string HTTPAppLayer;

	switch (sendSubType) {
	case eDeleteType::SUCCESS:
		HTTPAppLayer = 
			"HTTP/1.1 200 OK\r\n"
			"Server: WebServer\r\n"
			"Connection: Keep-Alive\r\n"
			"Date: " + curTimeStr + "\r\n"
			"Content-Type: text/html\r\n"
			"Content-Length: 45\r\n\r\n"
			"The target resource was successfully deleted.\n";
		break;
	case eDeleteType::FAILED:
		HTTPAppLayer = 
			"HTTP/1.1 400 Bad Request\r\n"
			"Server: WebServer\r\n"
			"Connection: Keep-Alive\r\n"
			"Date: " + curTimeStr + "\r\n"
			"Content-Type: text/html\r\n"
			"Content-Length: 57\r\n\r\n"
			"The target resource wasnt found and therfore not deleted.\n";
		break;
	}

	return HTTPAppLayer;
}

PUBLIC string HTTP::TraceResponse(string i_Request){
	string curTimeStr = HTTPParser::GetTime();
	string HTTPAppLayer = 
		"HTTP/1.1 200 OK\r\n"
		"Server: WebServer\r\n"
		"Connection: Keep-Alive\r\n"
		"Date: " + curTimeStr + "\r\n"
		"Content-Type: message/http\r\n"
		"Content-Length: " + to_string(i_Request.length() + 1) + "\r\n\r\n"
		+ i_Request + "\r\n";
	return HTTPAppLayer;
}

PUBLIC string HTTP::ErrorResponse(string i_Request) {
	string curTimeStr = HTTPParser::GetTime();
	string HTTPAppLayer = "HTTP/1.1 405 Method Not Allowed\r\n"
		"Server: WebServer\r\n"
		"Connection: Keep-Alive\r\n"
		"Date: " + curTimeStr + "\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: 88\r\n"
		"Allow: OPTIONS, GET, HEAD, POST, PUT, DELETE, TRACE\r\n\r\n"
		"Method Not Allowed\n"
		"The allowed methods are: OPTIONS, GET, HEAD, POST, PUT, DELETE, TRACE\n";
	
	return HTTPAppLayer;
}

PUBLIC string HTTP::TimeoutResponse() {
	string curTimeStr = HTTPParser::GetTime();
	string HTTPAppLayer =
		"HTTP/1.1 408 Request Timeout\r\n"
		"Server: WebServer\r\n"
		"Date: " + curTimeStr + "\r\n"
		"Connection: close\r\n\r\n\n";
	return HTTPAppLayer;
}


PRIVATE string HTTP::headHTTPAppLayer(string i_Filename) {

	string HTTPAppLayer, htmlContent, htmlContentSize, headerContent;
	string curTimeStr = HTTPParser::GetTime();
	string lastModified = HTTPParser::GetLastModified(i_Filename);

	getline(ifstream(i_Filename), htmlContent, '\0');
	htmlContent.erase(0, 3);
	htmlContentSize = to_string(strlen(htmlContent.c_str()) - 1);
	if (!strcmp(i_Filename.c_str(), "C:\\temp\\site_404.html")) {
		headerContent = "HTTP/1.1 404 Not Found\r\n";
	}
	else {
		headerContent = "HTTP/1.1 200 OK\r\n";
	}

	headerContent +=
		"Server: WebServer\r\n"
		"Connection: Keep-Alive\r\n"
		"Date: " + curTimeStr + "\r\n"
		"Last-Modified: " + lastModified + "\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: " + htmlContentSize + "\r\n\r\n\n";

	return headerContent;
}

PRIVATE string HTTP::getHTTPAppLayer(string i_Filename) {
	string HTTPAppLayer, headerContent, htmlContent, htmlContentSize;
	string curTimeStr = HTTPParser::GetTime();
	string lastModified = HTTPParser::GetLastModified(i_Filename);

	getline(ifstream(i_Filename), htmlContent, '\0');
	htmlContent.erase(0, 3); // undisired characters
	htmlContentSize = to_string(strlen(htmlContent.c_str()) - 1);
	if (!strcmp(i_Filename.c_str(), "C:\\temp\\site_404.html")) {
		headerContent = "HTTP/1.1 404 Not Found\r\n";
	}
	else {
		headerContent = "HTTP/1.1 200 OK\r\n";
	}

	headerContent +=
		"Server: WebServer\r\n"
		"Connection: Keep-Alive\r\n"
		"Date: " + curTimeStr + "\r\n"
		"Last-Modified: " + lastModified + "\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: " + htmlContentSize + "\r\n\r\n";

	return headerContent + htmlContent;
}

PRIVATE bool HTTP::isFileExists(string i_Resource) {

	bool fileExist;
	string path = "C:\\temp\\" + i_Resource;
	ifstream file(path.c_str());
	if (file.is_open()) {
		fileExist = true;
		file.close();
	}
	else {
		fileExist = false;
	}

	return fileExist;
}
