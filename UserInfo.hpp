#ifndef USERINFO_HPP
# define USERINFO_HPP

#include "ircserv.hpp"

class UserInfo
{
private:
	int _fd;

	std::string	_nick;
	std::string	_host;
	std::string	_name;
	std::string	_server;

public:
	UserInfo() {}
	UserInfo(std::string nick, std::string host, std::string name, std::string server);
	UserInfo(UserInfo const &src);
	~UserInfo();
	UserInfo	&operator=(UserInfo const &rhs);


	std::string	getNick();
	std::string	getHost();
	std::string	getName();
	std::string	getServer();
	void		setNick(std::string nick);
	void		setHost(std::string host);
	void		setName(std::string name);
	void		setServer(std::string server);
};

#endif
