#include "UserInfo.hpp"

UserInfo::UserInfo(std::string nick, std::string host, std::string name, std::string server)
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

UserInfo::UserInfo(UserInfo const &src)
{
	*this = src;
}

UserInfo::~UserInfo()
{
	std::cout << "QUIT message" << std::endl;
}

UserInfo	&UserInfo::operator=(UserInfo const &rhs)
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

std::string	UserInfo::getNick()
{
	return this->_nick;
}

std::string	UserInfo::getHost()
{
	return this->_host;
}

std::string	UserInfo::getName()
{
	return this->_name;
}

std::string	UserInfo::getServer()
{
	return this->_server;
}

void	UserInfo::setNick(std::string nick)
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

void	UserInfo::setHost(std::string host)
{
	this->_host = host;
}

void	UserInfo::setName(std::string name)
{
	this->_name = name;
}

void	UserInfo::setServer(std::string server)
{
	this->_server = server;
}
