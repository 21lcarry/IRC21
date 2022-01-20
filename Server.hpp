#ifndef SERVER_HPP
# define SERVER_HPP
# include "ircserv.hpp"
# include "User.hpp"
# include "Command.hpp"
#ifdef __APPLE__
#define IRC_NOSIGNAL SO_NOSIGPIPE
#else
#define IRC_NOSIGNAL MSG_NOSIGNAL
#endif
#define MAX_INACTIVE 60U
#define MAX_RESPONSE 120U

class Server
{
    public:
        Server(const std::string &pwd, int &port);
        ~Server();
        Server(const Server &o);
        Server &operator=(const Server &o);

        void newConnection();
        void run();

        const std::vector<User> &getClients();
        const std::string &getPass();
		bool userIsConnecting(std::string &nickname) const;
		std::vector<User> getClient();
		UserHistory *getHistory();
		void addUserHistoru(UserInfo &info);
		std::vector< const UserInfo *>
		getHistoryByUser(const std::string &nickname) const;
		bool	containsNickname(const std::string &nickname) const;
		std::map<std::string, Channel *>	getChannels();
	bool	containsChannel(const std::string &name) const;
	User	*getUserByName(const std::string &name);
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
        id_t                _spam_flag;
		std::map<std::string, Channel *>		_channels;
        //std::vector<UserHistory>	_history;
    	UserHistory 	_history;


	    bool _checkConnect(User &client);
        int _checkActivity(User &client);
        int _reciveRequest(User &client);
        int _sendResponse(User &client);

};

#endif