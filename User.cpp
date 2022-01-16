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
	{ // todo
		UserInfo inf = rhs.getInfo();
		_fd = rhs._fd;
		_info.flags = inf.flags;
		_info.servername = inf.servername;
		_info.nickname = inf.nickname;
		_info.username = inf.username;
		_info.hostname = inf.hostname;
		_info.awayMessage = inf.awayMessage;
		_info.registrationTime = inf.registrationTime;
		_info.realname = inf.realname;
		//_info.channels = inf.channels;
		_send = rhs._send;
		_request = rhs._request;
		_authorized = rhs._authorized;
		_pwd = rhs._pwd;
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

void User::setInfo(std::string field, std::string &val)
{
	if (field == "nickname")
		_info.nickname = val;
	else if (field == "username")
		_info.username = val;
	else if (field == "hostname")
		_info.hostname = val;
	else if (field == "servername")
		_info.servername = val;
	else if (field == "realname")
		_info.realname = val;
}

void User::setAuthorized(bool f)
{
	_authorized = f;
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

const UserInfo &User::getInfo() const
{
	return _info;
}

const std::string &User::getPwd()
{
	return _pwd;
}

void User::setFlag(unsigned char flag) {
	_info.flags |= flag;
	// todo
	//if (flag == BREAKCONNECTION && quitMessage.size() == 0)
	//	quitMessage = "Client exited";

}

void User::removeFlag(unsigned char flag) {
	_info.flags &= ~flag;
}

void User::setAwayMessage(const std::string &msg) {
	_info.awayMessage = msg;
}

void User::sendMessage(const std::string &msg) const {
	if (msg.size() > 0)
		send(_fd, msg.c_str(), msg.size(), IRC_NOSIGNAL); // todo возможно не _fd
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
std::string User::getPrefix() const {
		return std::string(_info.nickname + "!" + _info.username + "@" + _info.hostname);
}

UserInfo::UserInfo() {

}
