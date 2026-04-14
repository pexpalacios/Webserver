# Webserver
*This project has been created as part of the 42 curriculum by penpalac, ajodar-c & acastrov*

## Description

Webserv is a 42 School project about writing an [HTTP (Hyper Text Transfer Protocol)](https://www.ietf.org/rfc/rfc1945.txt) server in C++. The goal is to serve a full static website from a local server in your local machine (not external networking required).

In this project you will learn some useful skills in this areas:
- Common Gateway Interface (CGI) interface specification
- UNIX Network programming
- TCP/IP protocols
- HTTP protocol
- HTML language
- Object-oriented programming
- NGINX open source software

As the subject let's you choose whichever content you prefer to display, we proudly introduce you to Amiwuevo!

<p align="center">
	  <img src="www/amiwuevo/images/gifs/base_idle.gif"/>
</p>

Amiwuevo will be your loyal companion during this evaluation. As it's existence will abruptly end when your finish this assessment, you will have to nurture him, customize his clothes and help it overcome the dread of a futile existence and evade the absurd... Have fun!

## Instructions

### Initial setup

Clone the repository in your local machine

```
git clone git@github.com:pexpalacios/Webserver.git
```

Go to the folder and run the make command

```
cd Webserver && make
```

Here, your can run Webserver with a .conf file of your choosing as an argument (it must comply with the same rules as a NGINX .conf file), or run it without arguments and use the default configuration (/config/default.conf file will be used)

Run with your desired .conf file

```
./webserver example_file.conf
```

Or run with default .conf file (/config/default.conf)

```
./webserver
```

If you come across some CGI issues while running the project default .conf file (most likely `can't find CGI_PATH`), you can install the necessary dependencies (mostly Python and PHP) using `make dependencies` (remember to run sudo permissions)

```
make dependencies
```

### Server Configuration

#### Setting a basic configuration

Ok, so now you have your server running! But what can you do with that? Well, let's try to configure a simple Hello World! html page.

For a simple server configuration, you will have to specify a server object and list at lest these directives:

- Listen: the port where this server will list on (ie.8080)
- Host: the ip direction where this server will list on. It must be a IPv4 compliant ip address (i.e 127.0.0.1).
- Server Name: a name of your choosing to refer to this server (it will be use for name-based virtual host logic inside your network)
- Root: the folder where you will store all the resources of your server, like html or image files
- Index: the default page that the server will show. It must include the same path as root, and point to an index.html file
- Client max body size: defines the maximum allowed size of the client request body
- Location /: controls how the request of this URI is processed, mainly it's allowed_methods (Webserver only admits `GET`, `POST` and `DELETE`).

So, a basic configuration will look like this

```
server {
	listen 8080;
	host 127.0.0.1;
	server_name hello_world;
	root ./www/hello_world;
	index ./www/hello_world/index.html;
	client_max_body_size 100Mb;
	location /
	{
		allow_methods GET POST DELETE;
	}
}
```

#### Setting an ip:port and server_name

Once the server is running, go to your preferred web browser and enter the ip:port address of your choosing as stablish in your .conf file

For example, this basic server will display a "Hello World!" HTML page at url http://127.0.0.1:8080/ (or http://localhost:8080). Neat!

```
server {
	listen 8080;
	host 127.0.0.1;
	server_name hello_world;
	root ./www/hello_world;
	index ./www/hello_world/index.html;
	client_max_body_size 100Mb;
	location /
	{
		allow_methods GET POST DELETE;
	}
}
```

You can also make a server listen to multiple ports, like 8080 and 8081. In this case, this configuration will will display the same web page if you call either http://127.0.0.1:8080 or http://127.0.0.1:8081

```
server {
	listen 8080 8081;
	host 127.0.0.1;
	server_name hello_world;
	root ./www/hello_world;
	index ./www/hello_world/index.html;
	client_max_body_size 100Mb;
	location /
	{
		allow_methods GET POST DELETE;
	}
}
```

You can also run multiple servers with different ip:port...

```
server {
	listen 8080;
	host 127.0.0.1;
	server_name hello_world;
	root ./www/hello_world;
	index ./www/hello_world/index.html;
	client_max_body_size 100Mb;
	location /
	{
		allow_methods GET POST DELETE;
	}
}

server {
	listen 8081;
	host 127.0.0.1;
	server_name hello_world_2;
	root ./www/hello_world_2;
	index ./www/hello_world_2/index.html;
	client_max_body_size 100Mb;
	location /
	{
		allow_methods GET POST DELETE;
	}
}
```

... or even the same ip:port! In this case, the server shown will be dictated by the Host header and the server_name, for example http://localhost:8080 and http://hello_world:8080 (so you'll need to configure your /etc/hosts file with sudo permissions).

```
# http://localhost:8080
server {
	listen 8080;
	host 127.0.0.1;
	server_name localhost;
	root ./www/localhost;
	index ./www/localhost/index.html;
	client_max_body_size 100Mb;
	location /
	{
		allow_methods GET POST DELETE;
	}
}

# http:://hello_world:8080
server {
	listen 8080;
	host 127.0.0.1;
	server_name hello_world;
	root ./www/hello_world;
	index ./www/hello_world/index.html;
	client_max_body_size 100Mb;
	location /
	{
		allow_methods GET POST DELETE;
	}
}
```

If multiple servers listen to the same ip:port and not Host header is specified (just a call to the ip:port, like http://127.0.0.1:8080), only the first server will be shown in the explorer


```
# With http://127.0.0.1:8080, only this first server will display in the browser
server {
	listen 8080;
	host 127.0.0.1;
	server_name hello_world;
	root ./www/hello_world;
	index ./www/hello_world/index.html;
	client_max_body_size 100Mb;
	location /
	{
		allow_methods GET POST DELETE;
	}
}

# This server will not be shown unless you request for http://localhost:8080!
server {
	listen 8080;
	host 127.0.0.1;
	server_name localhost;
	root ./www/localhost;
	index ./www/localhost/index.html;
	client_max_body_size 100Mb;
	location /
	{
		allow_methods GET POST DELETE;
	}
}


```

Also, if two servers have the same ip:port and server_name, only the first one will persist. The rest won't be a part of the configuration of this Webserv.

#### Locations
You are already familiar with the '/' location, which defines the behavior of the request URI (i.e http://hello_world:8080/). But serving just one static html file with plain text is too boring! With the location directive you can dictate others resources and folders that your server can make use of, like a location /images or a location /sound for media files, or a location /error for custom error pages. The sky is the limit!

A location directive can hold the following parameters:

- allow_methods: with HTTP methods are allowed in the request to an specific location
- autoindex: switch this option to off or on if if you want to access a directory listing to any request ending with the character \ 
- root: set the root directory for the request
- index: define which file will be used as index (must be an 'index.html' file)

There's an example location for images

```
	location /images {
		allow_methods GET POST DELETE;
		autoindex on;
		root ./www/hello_world;
		index ./www/hello_word/images/index.html;
		}
```

#### CGI

Did you know that this webserver is more than capable to just serve an static site? It also can do stuff! Thanks to the Common Gateway Interface (CGI) you can run basics scripts and generate dynamic content. You just need to specify which scripts are gonna run, which path holds the necessary files to run it, and our CGI implementation will care of the rest.

Only to extra fields are necessary to configure a /cgi-location from any other location:
- cgi_path: the path that holds the program you want to run (i.e /usr/bin/bash)
- cgi_ext: the extension your script will use (i.e .sh)

This is how a location /cgi-bin would be like:

```
	location /cgi-bin {
		root ./www/hello_world;
		autoindex off;
		cgi_path /usr/bin/python3 /usr/bin/php /usr/bin/bash;
		cgi_ext .py .php .sh;
	}
```

#### TL;DR

To run this Webserv you will need to set a .conf file following NGINX grammar with a minimum of a server block, which can hold multiple locations blocks and can run external scripts via CGI.

```
server {
	listen 8080;
	host 127.0.0.1;
	server_name hello_world;
	root ./www/hello_world;
	index ./www/hello_world/index.html;
	client_max_body_size 100Mb;

	location / {
		allow_methods GET POST DELETE;
		autoindex on;
		index ./www/hello_world/index.html;
	}

	location /images {
		root ./www/hello_world;
		autoindex on;
		}

	location /cgi-bin {
		root ./www/hello_world;
		cgi_path /usr/bin/python3 /usr/bin/php /usr/bin/bash;
		cgi_ext .py .php .sh;
	}
}
```

## Resources
### Documentation
#### HTTP
- [RFC 1945, Hypertext Transfer Protocol - HTTP/1.0](https://www.ietf.org/rfc/rfc1945.txt)
- [RFC 2616, Hypertext Transfer Protocol - HTTP/1.1](https://www.ietf.org/rfc/rfc2616.txt)
#### NGINX
- [Beginner’s Guide](https://nginx.org/en/docs/beginners_guide.html)
- [Setting up a server](https://docs.nginx.com/nginx/admin-guide/web-server/web-server/)
- [Creating a Configuration File](https://docs.nginx.com/nginx/admin-guide/basic-functionality/managing-configuration-files/)
- [Directives](https://nginx.org/en/docs/http/ngx_http_core_module.html#server)
- [Create NGINX Plus and NGINX Configuration Files](https://docs.nginx.com/nginx/admin-guide/basic-functionality/managing-configuration-files/)
- [How nginx processes a request](https://nginx.org/en/docs/http/request_processing.html) 
#### UNIX Network
- [UNIX Network Programming: Vol. 1: The Sockets Networking API](https://bibliobuscador.uah.es/discovery/fulldisplay?vid=34UAH_INST:34UAH_VU1&docid=alma991003948018704214&context=L)
### Articles
#### HTML
- [HTML: HyperText Markup Language - MDN Web Docs](https://developer.mozilla.org/en-US/docs/Web/HTML)
#### HTTP
- [HTTP: Hypertext Transfer Protocol - MDN Web Docs](https://developer.mozilla.org/en-US/docs/Web/HTTP)
#### TCP
- [TCP and UDP Ports Explained - Bleeping Computer](https://www.bleepingcomputer.com/tutorials/tcp-and-udp-ports-explained/)
#### CGI
- [Common Gateway Interface - Wikipedia](https://en.wikipedia.org/wiki/Common_Gateway_Interface)
### Guides
#### HTTP
- [HTTP guides - MDN Web Docs](https://developer.mozilla.org/en-US/docs/Web/HTTP/Guides)
#### UNIX Network
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/html/split-wide/)
- [Beej's Guide to Network Concepts](https://beej.us/guide/bgnet-1/html/split/index.html)
### AI Disclaimer
AI usage in this project:
- Development and review of C++ and JavaScrypt code
- Search for information and concepts related to UNIX Sockets, HTTP protocol, HTML tags and JavaScrypt functions

AI Tools used:
- GitHub Copilot
- OpenAI ChatGPT
- Google Gemini
