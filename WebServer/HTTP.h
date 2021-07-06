#pragma once

class HTTP {

private:
	static string headHTTPAppLayer(string i_Filename);
	static string getHTTPAppLayer(string i_Filename);
	static bool isFileExists(string i_Resource);

	enum class ePutType { CREATED, MODIFIED };
	enum class eDeleteType { SUCCESS, FAILED };

public:
	enum class eHTTP { _GET, _POST, _HEAD, _OPTIONS, _PUT, _DELETE, _TRACE, _ERROR };

	static string GetResponse(string i_Request);
	static string HeadResponse(string i_Request);
	static string PostResponse(string i_Request);
	static string OptionsResponse(string i_Request);
	static string PutResponse(string i_Request);
	static string DeleteResponse(string i_Request);
	static string TraceResponse(string i_Request);
	static string ErrorResponse(string i_Request);
	static string TimeoutResponse();
};
