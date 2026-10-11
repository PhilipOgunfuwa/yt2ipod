#ifndef SERVER_H
#define SERVER_H
#include "httplib.h"
#include "iPod.h"
#include <nlohmann/json.hpp>
#include <optional>

class Server {
public:
    // No copy/move semantics for now
    Server(Server&) = delete;
    Server &operator=(Server&) = delete;
    Server(Server&&) = delete;
    Server &operator=(Server&&) = delete;

    Server(std::string addr, int port);
    Server();

    std::string address() const; // Get address of server
    int port() const; // Get port of server

    void listen(); // Should be called LAST

    static bool valid_ipv4(std::string_view addr);

private:
    const std::string default_address { "127.0.0.1" };
    const std::string landing_page_url { "/yt2ipod" };
    const std::string ipod_url { "/yt2ipod/api/ipod" };
    const int default_port { 8080 };

    httplib::Server __svr;
    std::optional<iPod> ipod;
    std::string _address;
    int _port;


    void setup_endpoints();
};

#endif