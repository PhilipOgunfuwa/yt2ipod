#include "include/Server.h"

using json = nlohmann::json;

Server::Server(std::string addr, int port) 
    : _address { addr }
    , _port { port }
{ setup_endpoints(); }

Server::Server() 
    : _address { default_address }
    , _port { default_port }
{ setup_endpoints(); }

std::string Server::address() const { return _address; }
int Server::port() const { return _port; }

void Server::listen() {
    bool status { __svr.listen(default_address, 8080) };

    if (status) std::cout << "Dandy";
    else std::cout << "Not Dandy";
}


bool Server::valid_ipv4(std::string_view addr){
    return false;
}

void Server::setup_endpoints() {

    __svr.Get(landing_page_url, [](const httplib::Request &req, httplib::Response &res) {
        res.set_content("Hello, World!", "text/plain");
    });

    // Set up iPod that server has hopefully
    __svr.Post(ipod_url, [&](const httplib::Request &req, httplib::Response &res) {
        
        try {

            json input { json::parse(req.body) };

            // don't overwrite ipod if server already has one
            if (ipod.has_value()) {
                res.status = 400;
            }

            else {
                std::string value { input["ipod_mount_point"] };
                ipod.emplace(value.c_str());

                if (ipod.value().ok()) res.status = 200; // ipod is ok
                else res.status = 400; // Error w/ ipod
            }
        }

        catch(std::exception& e) {
            res.status = 400;
            res.set_content("{\"error\":\"invalid json\"}", "application/json");
        }
    });


}