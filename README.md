# Asynchronous Web Server With C++
Asynchronous single-thread web server in C++
that supports HTTP requests from scratch!

The server implemented by using three classes:
WebServer, basic HTTP parser and HTTP.

It supports the following requests:

##### GET, POST, HEAD, OPTIONS, PUT, DELETE, TRACE

with parameter 'lang' to get "hello world" pages in different languages.

-------------------------------------
### PROPERTIES OF THE SERVER

In order to change the properties of the server you can edit the macros in program.cpp:

listen socket port

listen socket queue size

max sockets the server can serve

timeout interval for closing old connections

--------------------------------------

### TESTING

place the html files from 'testing' in c:/temp
**or** change the variable 'curPath' in HTTP.cpp to the right path.

use the browser and test the following requests:

http://localhost

http://localhost/site.html?lang=en

http://localhost/site.html?lang=es

http://localhost/site.html?lang=fr


