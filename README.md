# Web-Server-C++
Asynchronous single-thread web server in C++
that supports HTTP requests from scratch!
The server implemented by using three classes:
WebServer, basic HTTP parser and HTTP.
it support the following requests:
GET, POST, HEAD, OPTIONS, PUT, DELETE, TRACE
with parameter 'lang' to get "hello world" pages in different languages.

Testing:
place the html files from 'testing' in c:/temp
or change curPath variable in HTTP.cpp to the right path.
use the browser and test the following requests:
http://localhost
http://localhost/site.html?lang=en
http://localhost/site.html?lang=es
http://localhost/site.html?lang=fr
