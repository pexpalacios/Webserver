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

![Amiwuevo](www/amiwuevo/images/gifs/base_idle.gif)

Amiwuevo is your field companion during this exercise evaluation. As it's existence will abrutly end when your finish this assessment, you will have to nurture him, customize his clothes and help it overcome the dread of a futile existence and evade the absurd... Have fun!

## Instructions

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

Once the server is running, go to your prefered web browser and enter the ip:port address of your choosing, as stablished in your .conf file

For example, 

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