#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <map>

// 20260219 Terto: Simulación del parseo
class Response
{
private:
    int statusCode;
    std::string statusMessage;
    std::map<std::string, std::string> headers;
    std::string body;

public:
    Response();
    ~Response();

    void setStatus(int code);
    void setHeader(const std::string& key, const std::string& value);
    void setBody(const std::string& b);

    std::string toString() const;

private:
    std::string getStatusMessage(int code) const;
};

#endif
