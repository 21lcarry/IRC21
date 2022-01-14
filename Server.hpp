#ifndef SERVER_HPP
# define SERVER_HPP
# include "ircserv.hpp"
# include "User.hpp"
# include "Command.hpp"

class Server
{
    public:
        Server(const std::string &pwd, int &port);
        ~Server();
        Server(const Server &o);
        Server &operator=(const Server &o);

        void newConnection();
        void run();

    private:
        Server();
        std::string         _pass;
        unsigned int        _ip;
        int                 _port;
        int                 _opt;
        int                 _addrLen;
        struct sockaddr_in  _addr;
        int                 _serverFd;
        std::vector<User>   _clients;
        std::string         _serverName;
        //std::vector<UserHistory>	_history;
    	UserHistory 	_history;
        int _reciveRequest(User &client);
        int _sendResponse(User &client);
};

#endif