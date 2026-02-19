#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <map>

// 20260219 Terto: Simulación del parseo
class Request
{
private:
    std::string method;
    std::string path;
    std::string version;
    std::map<std::string, std::string> headers;
    std::string body;

public:
    Request();
    ~Request();

    // Setters (usados por el parser)
    void setMethod(const std::string& m);
    void setPath(const std::string& p);
    void setVersion(const std::string& v);
    void setHeader(const std::string& key, const std::string& value);
    void setBody(const std::string& b);

    // Getters (usados por tu lógica)
    const std::string& getMethod() const;
    const std::string& getPath() const;
    const std::string& getVersion() const;
    const std::string& getBody() const;

    bool hasHeader(const std::string& key) const;
    std::string getHeader(const std::string& key) const;
};

#endif
