#include "Server.hpp"
/* todo ????????????????????????????????????????? */
#define MAX_INPUT 510

Server::Server(const std::string &pwd, int &port) : _ip(0), _port(port), _pass(pwd), _opt(1), _serverName("IRCserv"), _spam_flag(0)
{
    _serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverFd < 0)
    {
        std::cerr << "ircserv: socket() failed: " <<  std::strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &_opt, (socklen_t)sizeof(_opt));
    _addrLen = sizeof(_addr);
    bzero(&_addr, _addrLen);
    _addr.sin_family = AF_INET;
    _addr.sin_addr.s_addr = INADDR_ANY;
    _addr.sin_port = htons(port);
    if (bind(_serverFd, (struct sockaddr *)&_addr, sizeof(_addr)) < 0 )
    {
        std::cerr << "ircserv: bind() failed: " <<  std::strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    fcntl(_serverFd, F_SETFL, O_NONBLOCK);
    std::cout << "server started on 0.0.0.0:" << _port << std::endl;
}

void Server::newConnection()
{
    int newSocket = accept(_serverFd, (struct sockaddr *)&_addr, (socklen_t *)&_addrLen);

	if (newSocket < 0)
        std::cerr << "ircserv: accept() failed: " <<  std::strerror(errno) << std::endl;

	setsockopt(newSocket, SOL_SOCKET, IRC_NOSIGNAL, &_opt, (socklen_t)sizeof(_opt));
	int flag = fcntl(newSocket, F_GETFL);
	fcntl(newSocket, F_SETFL, flag |  O_NONBLOCK);  //subject
	_clients.push_back(User(newSocket, _serverName));
	std::cout << "New connection with socket fd" << newSocket << "\n";
}

bool Server::_checkConnect(User &client)
{
    if (client.authorized() && time(0) - client.getActivity() > static_cast<time_t>(MAX_INACTIVE) && !(client.getFlag() & PINGING))
	{
		client.sendMessage(":" + client.getInfo().servername + " PING :" + client.getInfo().servername + "\n");
		client.setFlag(PINGING);
	}
	if ((client.getFlag() & PINGING) && time(0) - client.getActivity() > static_cast<time_t>(MAX_RESPONSE))
        return false;
    return true;
}

int Server::_checkActivity(User &client)
{
    char buff[MAX_INPUT];
    if (_spam_flag == 3)
    {
        client.incrementDelay();
        _spam_flag = 0;
    }
    if ((time(0) - client.getActivity()) < static_cast<time_t>(client.getDelay()))
    {
        std::cout << "SPAM - " << client.getFd() << " \n";
        ++_spam_flag;
        client.setIsSend(1);
        return recv(client.getFd(), buff, MAX_INPUT, MSG_TRUNC);
    }
    return -2;
}

void Server::run()
{
    fd_set readSet, writeSet;
	int sdTmp, rc, maxSd, flag;

    if (listen(_serverFd, 32) < 0)
	{
		std::cerr << "ircserv: listen() failed: " <<  std::strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
	std::cout << "Server listen port " << _port << "\n";
    while(true)
    {
        maxSd = _serverFd;
        FD_ZERO(&readSet);
        FD_ZERO(&writeSet);
        FD_SET(_serverFd, &readSet);
        for (std::vector<User>::iterator thisClient = _clients.begin(); thisClient != _clients.end(); ++thisClient)
        {
            sdTmp = thisClient->getFd();
            FD_SET(sdTmp, &readSet);
            FD_SET(sdTmp, &writeSet);
            maxSd = (sdTmp > maxSd) ? sdTmp : maxSd;
        }
        rc = select(maxSd + 1, &readSet, &writeSet, NULL, NULL);
        if (rc < 0)
        {
            std::cerr << "ircserv: select() failed: " <<  std::strerror(errno) << std::endl;
            exit(EXIT_FAILURE); 
        }
        if (rc == 0)
            continue ; //
        if (FD_ISSET(_serverFd, &readSet))
            this->newConnection();
        for (std::vector<User>::iterator thisClient = _clients.begin(); thisClient != _clients.end(); ++thisClient)
        {
            if (!(_checkConnect(*thisClient)))
            {
                std::cerr << "ircserv: client is disconnected due to inactivity. client: " << sdTmp << std::endl;
                FD_CLR(sdTmp, &writeSet);
                FD_CLR(sdTmp, &readSet);
                close(sdTmp);
                _clients.erase(thisClient);
                break ;
            }
            sdTmp = thisClient->getFd();
            if (FD_ISSET(sdTmp, &readSet))
            {
                if ((flag = _checkActivity(*thisClient)) <= 0 && flag != -2)
                {
                    FD_CLR(sdTmp, &readSet);
                    close(thisClient->getFd());
                    _clients.erase(thisClient);
                    std::cerr << "ircserv: connection on socket [" << thisClient->getFd() << "] closed by client" << std::endl;
                    break ;
                }
                else if ((flag > 0))
                    continue ;
                else
                    rc = _reciveRequest(*thisClient);
                if (rc < 0)
                {
                    FD_CLR(sdTmp, &readSet);
                    _clients.erase(thisClient);
                    std::cerr << "ircserv: read [" << thisClient->getFd() << "] socket error" << std::endl;
                    break ;
                }
                if (rc == 0)
                {
                    FD_CLR(sdTmp, &readSet);
                    _clients.erase(thisClient);
                    std::cerr << "ircserv: connection on socket [" << thisClient->getFd() << "] closed by client" << std::endl;
                    break ;
                }
            }
            if (FD_ISSET(sdTmp, &writeSet) && thisClient->isSend() == 0)
            {
                rc = _sendResponse(*thisClient);
                if (rc == -2)
                {
                    std::cerr << "ircserv: Wrong authorization attempt. client " << thisClient->getFd() << std::endl;
                    FD_CLR(sdTmp, &writeSet);
                    FD_CLR(sdTmp, &readSet);
                    close(thisClient->getFd());
                    _clients.erase(thisClient);
                    break;
                }
                else if (rc < 0)
                {
                    FD_CLR(sdTmp, &writeSet);
                    break ;
                }
            }
        }
    }

}

int Server::_reciveRequest(User &client)
{
    char buff[MAX_INPUT + 1];
    int fd = client.getFd();
    bzero(buff, MAX_INPUT);
    int rc = recv(fd, buff, MAX_INPUT, 0);
    if (rc < 0)
    {
        std::cerr << "ircserv: recv() failed: " <<  std::strerror(errno) << std::endl;
        return -1;
    }
    if (rc == 0)
    {
        std::cout << "User on [" << fd << "] closed connection" << std::endl;
        close(fd);
        return 0;
    }
    client.eraseRequest();
    client.requestToVector(buff);
    client.setActivity(time(0));
    /*********************************/
    std::vector<std::string> test = client.getRequest();
    if (test.size() > 0)
    {
        std::cout << "New request vector: " << std::endl;
        for (std::vector<std::string>::iterator i = test.begin(); i != test.end() - 1; ++i)
            std::cout << "\"" << *i << "\", ";
        std::cout << "\"" << *(test.end() - 1) << "\"" << std::endl;
    }
    /*********************************/
    client.setIsSend(0);
    return 1;
}

int Server::_sendResponse(User &client)
{
    Command request(*this, client);
    int rc = request.handleRequest();
    if (rc == -1)
    {
        std::cerr << "ircserv: send() failed[" << client.getFd() << "]: " <<  std::strerror(errno) << std::endl;
//       client.setIsSend(1);
        return -1;
    }
    client.setIsSend(1);
    return rc;
    /*
    std::string test_response = "Just an empty response\n";
    int rc = send(client.getFd(), test_response.c_str(), std::strlen(test_response.c_str()), SO_NOSIGPIPE);

*/

}

Server::~Server() {}

Server::Server(const Server &o)
{
    if (this != &o)
    {
        _pass   = o._pass;
        _ip     = o._ip;
        _port   = o._port;
        _opt    = o._opt;
        _addrLen = o._addrLen;
        _addr   = o._addr;
        _serverFd = o._serverFd;
        _clients = o._clients;
    }
}

Server &Server::operator=(const Server&o)
{
    if (this != &o)
    {
        _pass   = o._pass;
        _ip     = o._ip;
        _port   = o._port;
        _opt    = o._opt;
        _addrLen = o._addrLen;
        _addr   = o._addr;
        _serverFd = o._serverFd;
        _clients = o._clients;
    }
    return *this;
}

const std::vector<User> &Server::getClients()
{
    return _clients;
}

const std::string &Server::getPass()
{
    return _pass;
}

 UserHistory *Server::getHistory()  {
	return &_history;
}

bool Server::userIsConnecting(std::string &nickname) const{
	size_t	usersCount = _clients.size();
	for (size_t i = 0; i < usersCount; i++)
	{
		if (_clients[i].getInfo().nickname == nickname)
			return (true);
	}
	return (false);
}

std::vector<const UserInfo *>
Server::getHistoryByUser(const std::string &nickname) const {
	return _history.getHistoryByUser(nickname);
}

void Server::addUserHistoru(UserInfo &info) {
	_history.addHistoryByUser(info);
}

std::vector<User> Server::getClient() {
	return _clients;
}
bool	Server::containsNickname(const std::string &nickname) const
{
	size_t	usersCount = _clients.size();
	for (size_t i = 0; i < usersCount; i++)
	{
		if (_clients[i].getInfo().nickname== nickname)
			return (true);
	}
	return (false);
}

std::map<std::string, Channel *> Server::getChannels() {
	return _channels;
}

bool Server::containsChannel(const std::string &name) const {
	try
	{
		_channels.at(name);
		return true;
	}
	catch(const std::exception& e)
	{}
	return false;
}

User *Server::getUserByName(const std::string &name) {
	User	*ret;
	size_t	usersCount = _clients.size();
	for (size_t i = 0; i < usersCount; i++)
		if (_clients[i].getInfo().nickname== name)
			ret = &_clients[i];
	return ret;
}
