#include "UserHistory.hpp"

UserHistory::UserHistory(std::string nick, std::string host, std::string name, std::string server)
{
/*
	_nick = nick;
	_host = host;
	_name = name;
	_server = server;
*/
	setNick(nick);
	setHost(host);
	setName(name);
	setServer(server);
}

UserHistory::UserHistory(UserHistory const &src)
{
	*this = src;
}

UserHistory::~UserHistory()
{
//	std::cout << "QUIT message" << std::endl;
}

UserHistory		&UserHistory::operator=(UserHistory const &rhs)
{
	if (this != &rhs)
	{
		_nick = rhs._nick;
		_host = rhs._host;
		_name = rhs._name;
		_server = rhs._server;
	}
	return (*this);
}

std::string	UserHistory::getNick()
{
	return this->_nick;
}

std::string	UserHistory::getHost()
{
	return this->_host;
}

std::string	UserHistory::getName()
{
	return this->_name;
}

std::string	UserHistory::getServer()
{
	return this->_server;
}

void	UserHistory::setNick(std::string nick)
{
	int	rtrn = 0;

	if (nick.length() <= 9)
	{
		for (int i = 0; i < nick.length(); ++i)
			if ((nick[i] >= 'a' && nick[i] <= 'z') || (nick[i] >= 'A' && nick[i] <= 'Z') ||\
			(nick[i] >= '0' && nick[i] <= '9') || nick[i] == '-' || nick[i] == '^' ||\
			nick[i] == '[' || nick[i] == ']' || nick[i] == '{' || nick[i] == '}' ||\
			nick[i] == '\\')
				rtrn = 1;
			else
			{
				rtrn = 0;
				break ;
			}
	}
	else
		rtrn = 0;
	if (rtrn == 0)
		std::cerr << "Invalid NickName of User" << std::endl;
	else
		this->_nick = nick;
}

void	UserHistory::setHost(std::string host)
{
	this->_host = host;
}

void	UserHistory::setName(std::string name)
{
	this->_name = name;
}

void	UserHistory::setServer(std::string server)
{
	this->_server = server;
}
