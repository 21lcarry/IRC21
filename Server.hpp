#ifndef SERVER_HPP
# define SERVER_HPP
# include "ircserv.hpp"
# include "User.hpp"
# include "Command.hpp"
# include "Channel.hpp"
#ifdef __APPLE__
#define IRC_NOSIGNAL SO_NOSIGPIPE
#else
#define IRC_NOSIGNAL MSG_NOSIGNAL
#endif
#define MAX_INACTIVE 60U
#define MAX_RESPONSE 120U
#define MAX_CHANNEl 10
#define VERSION "1.0.0" //todo
#define debuglvl "1" // todo
#define NAME_SERVER "ircServer"
#define comments "" //todo
#define describe "" //todo
#define INFO_SERVER "" //todo
#define adminName "" //todo
#define adminNickname "" //todo
#define adminEmail "" //todo
#define MAX_USERS 256
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
		//std::vector<User> getClient(); //todo dublicate
		UserHistory *getHistory();
		void addUserHistoru(const UserInfo &info);
		std::vector< const UserInfo *>
		getHistoryByUser(const std::string &nickname) const;
		bool	containsNickname(const std::string &nickname) const;
		std::map<std::string, Channel *>	getChannels();
        bool	containsChannel(const std::string &name) const;
        User	*getUserByName(const std::string &name);
        int		connectToChannel(const User &user, const std::string &name, const std::string &key);
        int		handleChanFlags(std::vector<std::string> &param, User &user, const std::string &commands);
        int     handleUserFlags(std::string &flag, User &user);
        const std::map<std::string, std::string> &getOperators() const;
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
        std::map<std::string, std::string> _operators;
	    std::map<std::string, Channel *>		_channels;
        //std::vector<UserHistory>	_history;
    	UserHistory 	_history;


	    bool _checkConnect(User &client);
        int _checkActivity(User &client);
        int _reciveRequest(User &client);
        int _sendResponse(User &client);
        void _disconnect(std::vector<User>::iterator &thisClient);

};

#endif