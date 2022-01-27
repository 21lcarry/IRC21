#include "User.hpp"
#include "Channel.hpp"

User::User(int sock_fd, std::string servername) : _fd(sock_fd), _authorized(false), _msg_delay(1), _queue(0) {
	_info.servername = servername;
	_info.flags = 0;
	_info.hostname = "127.0.0.1";
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
	std::cout << "QUIT message: " << (void*)this << std::endl;
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

    start = request.find_first_not_of(" \t", 0);
	while (1) 
    {
		end = request.find_first_of(" \t", start);
		if (end == std::string::npos)
			break;
		token = request.substr(start, end - start);
		_request.push_back(token);
        start = request.find_first_not_of(" \t", end);
		if (start == std::string::npos)
			break ;
	}
	token = request.substr(start, end - start);
	if (token[token.size() - 1] == '\n')
		token = token.substr(0, token.size() - 1);
	_request.push_back(token);
}

const bool User::hasNoQueue() const
{
	return _raw_requests.empty();
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
	else if (field == "quitMessage")
		_info.quitMessage = val;
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

void User::setRawRequests(std::queue<std::string> &val)
{
	_queue += val.size();

	int i = val.size();
	while (i > 0)
	{
		_raw_requests.push(val.front());
		val.pop();
		--i;
	}
}

std::string User::getNextRequest()
{
	std::string ret = _raw_requests.front();

	_raw_requests.pop();
	return ret;
}

void User::removeFlag(unsigned char flag) {
	_info.flags &= ~flag;
}

void User::setAwayMessage(const std::string &msg) {
	_info.awayMessage = msg;
}

void User::sendMessage(const std::string &msg) const {
	int bytes;
	if (msg.size() > 0){


		bytes = send(_fd, msg.c_str(), msg.size(), IRC_NOSIGNAL); // todo возможно не _fd
		std::cout <<"{" <<bytes <<"}" <<std::endl;
		 }
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
//	std::string prefix = _info.nickname + "!" + _info.username + "@" + _info.hostname;

	return std::string(_info.nickname + "!" + _info.username + "@" + _info.hostname);
}

void User::addChannel(const Channel &channel) {
	_info.channels.push_back(&channel);
	std::cout << "size user channels " <<_info.channels.size() <<std::endl;
}

User::User() {

}

bool User::isOnChannel(const std::string &name) const {
	for (size_t i = 0; i < _info.channels.size(); i++){
		const Channel *ch = (const Channel *) _info.channels.at(i); //todo
		if (ch->getName() == name)
			return true;
	}
	return false;
}

void User::removeChannel(const std::string &name) {
	std::vector<const Channel *>::iterator	begin = _info.channels.begin();
	std::vector<const Channel *>::iterator	end = _info.channels.end();
	for (; begin != end; ++begin)
		if ((*begin)->getName() == name)
			break ;
	_info.channels.erase(begin);
}

type_channel_arr &User::loadChannels() {
	return _info.channels;
}

UserInfo::UserInfo() {

}
