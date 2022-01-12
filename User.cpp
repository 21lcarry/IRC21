#include "User.hpp"

User::User(int sock_fd, std::string servername) : _fd(sock_fd), _authorized(false) {
	_info.servername = servername;
	_send = -1;
	std::cout << "new user with " << _fd << " socket" << std::endl;
}
/*
User::User(std::string nick, std::string host, std::string name, std::string server)
{
	setNick(nick);
	setHost(host);
	setName(name);
	setServer(server);
}
*/

/*
User::User(std::string nick, std::string host, std::string name, std::string server)
{
	UserInfo add(nick, host, name, server);

	this->_info = add;
	this->_history.push_back(UserHistory(add));
}*/

User::User(User const &src)
{
	*this = src;
}

User::~User()
{
	std::cout << "QUIT message" << std::endl;
}

User		&User::operator=(User const &rhs)
{
	if (this != &rhs)
	{
		_fd = rhs._fd;
		_info = rhs._info;
		_send = rhs._send;
		_request = rhs._request;
	}
	return (*this);
}
void User::eraseRequest()
{
	if (_request.size() > 0)
	{
		std::vector<std::string> empty;
		_request.clear();
		_request.swap(empty);
	}
}


void User::requestToVector(std::string request)
{
	size_t start, end;
	std::string token;

    start = request.find_first_not_of(" \n\t", 0);
	while (1) 
    {
		end = request.find_first_of(" \n\t", start);
		if (end == std::string::npos)
			break;
		token = request.substr(start, end - start);
		_request.push_back(token);
        start = request.find_first_not_of(" \n\t", end);
		if (start == std::string::npos)
			break ;
	}
}

void User::setPassword(std::string &pwd)
{
	_pwd = pwd;
}

const std::vector<std::string> &User::getRequest()
{
	return _request;
}

int User::getFd()
{
	return _fd;
}

const int &User::isSend()
{
	return _send;
}

void User::setIsSend(int f)
{
	_send = f;
}

const bool &User::authorized()
{
	return _authorized;
}

const UserInfo &User::getInfo()
{
	return _info;
}

const std::string &User::getPwd()
{
	return _pwd;
}


/*
std::string	User::getNick()
{
	return this->_nick;
}

std::string	User::getHost()
{
	return this->_host;
}

std::string	User::getName()
{
	return this->_name;
}

std::string	User::getServer()
{
	return this->_server;
}

void	User::setNick(std::string nick)
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

void	User::setHost(std::string host)
{
	this->_host = host;
}

void	User::setName(std::string name)
{
	this->_name = name;
}

void	User::setServer(std::string server)
{
	this->_server = server;
}
*/