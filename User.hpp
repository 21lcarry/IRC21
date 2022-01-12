#ifndef USER_HPP
# define USER_HPP
#include "ircserv.hpp"
#include "UserHistory.hpp"

struct UserInfo
{
	std::string							nickname;
	std::string							username;
	std::string							hostname;
	std::string							servername;
	std::string							realname;
	time_t								registrationTime;
};

class User
{
private:
	User();
	int							_fd;
	UserInfo					_info;
	std::vector<UserHistory>	_history;
	int							_send;
	std::vector<std::string>	_request;

	bool						_authorized;
	std::string					_pwd;

//	std::string	_nick;
//	std::string	_host;
//	std::string	_name;
//	std::string	_server;
public:
	User(int sock_fd, std::string servername);
	User(User const &src);
	~User();
	User		&operator=(User const &rhs);

	/* geters */
	int getFd();
	const int &isSend();
	const std::vector<std::string> &getRequest();
	const bool &authorized();
	const UserInfo &getInfo();
	const std::string &getPwd();
	/* seters */
	void setIsSend(int f);
	/* other */
	void requestToVector(std::string request);
	void eraseRequest();


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
