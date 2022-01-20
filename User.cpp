#include "User.hpp"

User::User(int sock_fd, std::string servername) : _fd(sock_fd), _authorized(false), _msg_delay(1) {
	_info.servername = servername;
	_info.flags = 0;
	_last_activity = time(0);
	_send = -1;
	std::cout << "new user with " << _fd << " socket" << std::endl;
}

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
		_info.channels.clear();
		_info.channels.insert(_info.channels.begin(), inf.channels.begin(), inf.channels.end());
		_send = rhs._send;
		_request = rhs._request;
		_authorized = rhs._authorized;
		_pwd = rhs._pwd;
		_last_activity = rhs._last_activity;
		_msg_delay = rhs._msg_delay;
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
	_info.registrationTime = time(0);
}

const std::vector<std::string> &User::getRequest()
{
	return _request;
}

int User::getFd() const
{
	return _fd;
}

const unsigned char &User::getFlag() const
{
	return _info.flags;
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

void User::setActivity(time_t time)
{
	_last_activity = time;
}

void User::removeFlag(unsigned char flag) {
	_info.flags &= ~flag;
}

void User::setAwayMessage(const std::string &msg) {
	_info.awayMessage = msg;
}

void User::sendMessage(const std::string &msg) const {
	int bytes;
	if (msg.size() > 0)
		bytes = send(_fd, msg.c_str(), msg.size(), IRC_NOSIGNAL); // todo возможно не _fd
	if (bytes < 0)
		std::cerr << "ircserv: send() failed on sendMessage() [" << _fd << "]: " <<  std::strerror(errno) << std::endl;
}

void User::incrementDelay()
{
	++_msg_delay;
}

const time_t &User::getActivity() const
{
	return _last_activity;
}

unsigned int &User::getDelay()
{
	return _msg_delay;
}

std::string User::getPrefix() const {
		return std::string(_info.nickname + "!" + _info.username + "@" + _info.hostname);
}

UserInfo::UserInfo() {

}
