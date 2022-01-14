#ifndef USERHISTORY_HPP
# define USERHISTORY_HPP

#include "ircserv.hpp"
#include "User.hpp"
struct UserInfo;
class User;
class UserHistory
{
/*private:
	int	_fd;

	std::string	_nick;
	std::string	_host;
	std::string	_name;
	std::string	_server;

public:
	UserHistory() {}
	UserHistory(std::string nick, std::string host, std::string name, std::string server);
	UserHistory(UserHistory const &src);
	~UserHistory();
	UserHistory	&operator=(UserHistory const &rhs);

	std::string	getNick();
	std::string	getHost();
	std::string	getName();
	std::string	getServer();
	void		setNick(std::string nick);
	void		setHost(std::string host);
	void		setName(std::string name);
	void		setServer(std::string server);*/
private:
	UserHistory(const UserHistory &copy);
	UserHistory &operator=(const UserHistory &copy);

	std::vector< UserInfo>				_historyList;

public:
	UserHistory();
	~UserHistory();

	void	addHistoryByUser(User user);
	 std::vector< const UserInfo *>
	 getHistoryByUser(const std::string &nickname) const;
};

#endif
