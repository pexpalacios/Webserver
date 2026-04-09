# Webserver
*This project has been created as part of the 42 curriculum by penpalac, ajodar-c & acastrov*

## Description

Webserv is a 42 School project about writing an [HTTP (Hyper Text Transfer Protocol)](https://www.ietf.org/rfc/rfc1945.txt) server in C++. The goal is to serve a full static website from a local server in your local machine (not external networking required).

In this project you will learn some useful skills in this areas:
- Common Gateway Interface (CGI) interface specification
- UNIX Network programming
- TCP/ IP protocols
- HTTP protocol
- HTML language
- Object-oriented programming
- NGINX open source software

As the subject let's you choose wichever content you prefer to display, we proudly introduce you to Amiwuevo!

<p align="center">
	  <img src="www/amiwuevo/images/gifs/base_idle.gif"/>
</p>

Amiwuevo is your field companion during this evaluation. As it's existence will abrutly end when your finish this assessment, you will have to nurture him, customize his clothes and help it overcome the dread of a futile existence and evade the absurd... Have fun!

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

Here, your can run Webserver with a .conf file or your choosing as an argument (it must comply with the same rules as a NGINX .conf file), or run it without arguments (/config/default.conf file will be used)

Run with your desired .conf file

```
./webserver example_file.conf
```

Or run with default .conf file (/config/default.conf)

```
./webserver
```

If you come across some CGI issues while running the project default .conf file (most likely `can't find CGI_PATH`), you can install the neccesary dependencies (mostly Python and PHP) using `make dependencies` (remember to run sudo permissions)

```
make dependencies
```

### Server Configuration

#### Setting a basic configuration

Ok, so now you have your server running! But what can you do with that? Well, let's try to configure a simple Hello World! html page.

For a simple server configuration, you will have to specify a server object and list at lest these directives:

- Listen: the port where this server will list on
- Host: the ip direction where this server will list on. It must be a IPv4 compliant ip address.
- Server Name: a name of your choosing to refer to this server
- Root: the folder where you will store all the resources of your server, like html or image files
- Index: the default page that the server will show. It must include the same path as root, and point to an index.html file
- Client max body size: defines the maximun allowed size of the client request body

So, a basic configuration will look like this

```
server {
	listen 8080;
	host 127.0.0.1;
	server_name hello_world;
	root ./www/hello_world;
	index ./www/hello_world/index.html;
	client_max_body_size 100Mb;
}
```

#### Setting an ip:port and server_name

Once the server is running, go to your prefered web browser and enter the ip:port address of your choosing, as stablished in your .conf file

For example, this basic server will display a "Hello World!" HTML page at url http://127.0.0.1:8080/ (or http://localhost:8080). Neat!

```
server {
	listen 8080;
	host 127.0.0.1;
	server_name hello_world;
	root ./www/hello_world;
	index ./www/hello_world/index.html;
	client_max_body_size 100Mb;
}
```

You can also make a server listen to multiple ports, like 8080 and 8081, for example. It will display a web page if you call either http://127.0.0.1:8080 or http://127.0.0.1:8081

```
server {
	listen 8080 8081;
	host 127.0.0.1;
	server_name hello_world;
	root ./www/hello_world;
	index ./www/hello_world/index.html;
	client_max_body_size 100Mb;
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
}

server {
	listen 8081;
	host 127.0.0.1;
	server_name hello_world_2;
	root ./www/hello_world_2;
	index ./www/hello_world_2/index.html;
	client_max_body_size 100Mb;
}
```

... or even the same ip:port! In this case, the server will be dictated by the Host header and the server_name, for example http://localhost:8080 and http://hello_world:8080 (so you'll need to configure your /etc/hosts file).

```
# http://localhost:8080
server {
	listen 8080;
	host 127.0.0.1;
	server_name locahost;
	root ./www/localhost;
	index ./www/localhost/index.html;
	client_max_body_size 100Mb;
}

# http:://hello_world:8080
server {
	listen 8080;
	host 127.0.0.1;
	server_name hello_world;
	root ./www/hello_world;
	index ./www/hello_world/index.html;
	client_max_body_size 100Mb;
}
```

If multiple servers listen to the same ip:port and not Host header is specified (just a call to the ip:port, like http://127.0.0.1:8080), only the first server will be shown in the explorer


```
# With http://127.0.0.1:8080, only this first server will display in the browser
server {
	listen 8080;
	host 127.0.0.1;
	server_name locahost;
	root ./www/localhost;
	index ./www/localhost/index.html;
	client_max_body_size 100Mb;
}

# This server will not be shown!
server {
	listen 8080;
	host 127.0.0.1;
	server_name hello_world;
	root ./www/hello_world;
	index ./www/hello_world/index.html;
	client_max_body_size 100Mb;
}
```

Also, if two servers have the same ip:port and server_name, only the first one will persist. The rest won't be a part of the server.

#### Locations

#### CGI

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
