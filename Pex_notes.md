### General notes
- Added a directory in /www for the images and cgis that'll be used in the web. 
- May need to do two different pages? Not sure.
- ^ Subject says server should listen in different ports for different content, so at least two pages are needed. 
But that could also be the menu page and then the game or something.
- Need to look up how to connect servers. WIll probably be a link in an HTML towards the execution of the other server, but I 
don't know if that would need any change in server functions or it'll just work
- ^ New plan involves making Listen() a vector<int> so it can take several ports at the same time, hence, the other server is optional

### Changes in execution
- Need two changes in server execution to connect well with the parsing:
```
void Server::configureErrorPages(const std::string& root, std::string errorPage)
```
This one needs to be changed so that it takes only a string. It will be called in main() in a loop, giving it all the error pages available
```
for (std::vector<std::string>::iterator it = str.begin(); it != str.end(); ++it)
	server.configureErrorPages(conf[0].getRoot(), *it);
```
In it, it should get the number and full Url of the page to send it to ``` setErrorPage() ```
```
	//Assuming all error pages are html
	int start = 0;
	for (size_t i = 0; i < errorPage.length(); i++)
		if (std::isdigit(errorPage[i]))
		{
			start = i;
			break;
		}
	int errorNumber;
	errorNumber = std::atoi(errorPage.substr(start, errorPage.length() - 5).c_str());

	std::string fullUrl;
	if (!root.empty() && root[root.length() - 1] == '/')
		fullUrl = root + errorPage;
	else
		fullUrl = root + "/" + errorPage;

	setErrorPage(errorNumber, fullUrl);
	std::cout << " Error page: " << errorNumber << " configured." << std::endl;
```


- Second change would be in:
```
void Server::handleClientConnection(int clientSock, Server& server)
```
Which needs a way to execute files instead of just reading them. The server should have access to the locations to be able to read what extensions are executable (.py, .sh, .php). Inside the server configuration, a localizations configuration should be added, and then sent in main().


### CGIs
Subject:
```
	Execution of CGI, based on file extension (for example .php). Here are some specific remarks regarding CGIs:
	∗ Do you wonder what a CGI is?
	∗ Have a careful look at the environment variables involved in the web server-CGI communication. The full 
	request and arguments provided by the client must be available to the CGI.
	∗ Just remember that, for chunked requests, your server needs to un-chunk them, the CGI will expect EOF as the end of the body.
	∗ The same applies to the output of the CGI. If no content_length is returned from the CGI, EOF will mark the end of the returned data.
	∗ The CGI should be run in the correct directory for relative path file access.
	∗ Your server should support at least one CGI (php-CGI, Python, and so forth).
```

CGIs are just bash or python scripts (in our case anyway). They must be executed by the server and can practically do whatever we want them to.
The server will need a way to handle CGIs: read, execute them and clean them.

Gave them a little try and the gist of it is:
- HTML has a link to a CGI
- CGI must be executable, that's why python, bash or php are used (if it was C it would need to be compiled)
- The server must read the file and decide if it's executable or just readable //changed function Server::handleClientConnection for this
- I need to add the executable extensions into parsing so that the server can locate them easily

