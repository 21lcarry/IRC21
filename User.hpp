#ifndef USER_HPP
# define USER_HPP
#include "ircserv.hpp"
#include "UserInfo.hpp"
#include "UserHistory.hpp"

class User
{
private:
	User();
	int							_fd;
	UserInfo					_info;
	std::vector<UserHistory>	_history;
	int							_send;
	std::vector<std::string>	_request;

//	std::string	_nick;
//	std::string	_host;
//	std::string	_name;
//	std::string	_server;
public:
	User(int sock_fd);
	User(User const &src);
	~User();
	User		&operator=(User const &rhs);

	/* geters */
	int getFd();
	const int &isSend();
	const std::vector<std::string> &getRequest();
	/* seters */
	void setIsSend(int f);
	/* other */
	void requestToVector(std::string request);


//	User(std::string nick, std::string host, std::string name, std::string server);
//	User(_info(nick, host, name, server), _history(nick, host, name, server));
/*	std::string	getNick();
	std::string	getHost();
	std::string	getName();
	std::string	getServer();
	void		setNick(std::string nick);
	void		setHost(std::string host);
	void		setName(std::string name);
	void		setServer(std::string server);*/
};

#endif
