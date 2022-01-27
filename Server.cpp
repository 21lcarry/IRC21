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
    _clients.reserve(MAX_USERS);
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
        std::cout << "\n PING to client fd " << client.getFd() << "\n\n";
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
        if ((time(0) - client.getActivity()) < static_cast<time_t>(client.getDelay()))
        {
            std::cout << "SPAM - " << client.getFd() << " \n";
            ++_spam_flag;
            client.setIsSend(1);
            return recv(client.getFd(), buff, MAX_INPUT, MSG_TRUNC);
        }
    }
    return -2;
}

void Server::_disconnect(std::vector<User>::iterator &thisClient)
{
    type_channel_arr &chans = thisClient->loadChannels();
    int j = 0;
	std::cout << ">>>delet channel " << chans.size() << std::endl;

    for (type_channel_arr::const_iterator i = chans.begin(); i != chans.end(); ++i)
    {
        std::cout << std::to_string(j++) << ":" <<  thisClient->getFd() << " DISCONNECT\n";
        const Channel *chan = *i;
        const_cast<Channel*>(chan)->disconnect(*thisClient);
		std::cout << ">>delet channel " << chan->getName() << std::endl;
        if(const_cast<Channel*>(chan)->isEmpty())
		{
			std::cout << "<<delet channel " << chan->getName() << std::endl;
			_channels.erase(chan->getName());
		}
    }
    _clients.erase(thisClient);
	close(thisClient->getFd());
}

void Server::run()
{
    fd_set readSet, writeSet;
	int sdTmp, rc, maxSd, flag;
    struct timeval timeout;

    if (listen(_serverFd, 32) < 0)
	{
		std::cerr << "ircserv: listen() failed: " <<  std::strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
	std::cout << "Server listen port " << _port << "\n";
    while(true)
    {
        timeout.tv_sec = 2;
        timeout.tv_usec = 0;
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
        rc = select(maxSd + 1, &readSet, &writeSet, NULL, &timeout);
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
            sdTmp = thisClient->getFd();
            if (!(_checkConnect(*thisClient)))
            {
                std::cerr << "ircserv: client is disconnected due to inactivity. client: " << sdTmp << std::endl;
                FD_CLR(sdTmp, &writeSet);
                FD_CLR(sdTmp, &readSet);
                _disconnect(thisClient);
                break ;
            }
            if (FD_ISSET(sdTmp, &readSet))
            {
                if ((flag = _checkActivity(*thisClient)) <= 0 && flag != -2)
                {
                    FD_CLR(sdTmp, &readSet);
                    FD_CLR(sdTmp, &writeSet);
                    _disconnect(thisClient);
       //             close(thisClient->getFd());
        //            _clients.erase(thisClient);
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
                    FD_CLR(sdTmp, &writeSet);
                    _disconnect(thisClient);
   //                 _clients.erase(thisClient);
                    std::cerr << "ircserv: read [" << thisClient->getFd() << "] socket error" << std::endl;
                    break ;
                }
                if (rc == 0)
                {
                    FD_CLR(sdTmp, &readSet);
                    FD_CLR(sdTmp, &writeSet);
                    _disconnect(thisClient);
         //           _clients.erase(thisClient);
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
                    _disconnect(thisClient);
     //               close(thisClient->getFd());
       //             _clients.erase(thisClient);
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
    std::string request(buff);

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
    if (request.length() > 512)
		request = request.substr(0, 510) + "\r\n";
	while (request.find("\r\n") != std::string::npos)
		request.replace(request.find("\r\n"), 2, "\n");
	if (request.size() > 1)
    {
        std::queue<std::string> tmp = utils::split(request, '\n', true);
		client.setRawRequests(tmp);
    }
    client.removeFlag(PINGING);
    client.setActivity(time(0));
    client.setIsSend(0);
    return 1;
}

int Server::_sendResponse(User &client)
{
    int rc = -1;
    while (!client.hasNoQueue())
    {
        client.eraseRequest();
        client.requestToVector(client.getNextRequest());
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
        Command request(*this, client);
        rc = request.handleRequest();
        if (rc == -1)
        {
            std::cerr << "ircserv: send() failed[" << client.getFd() << "]: " <<  std::strerror(errno) << std::endl;
            return -1;
        }
    }
    client.setIsSend(1);
    return rc;
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

void Server::addUserHistoru(const UserInfo &info) {
	_history.addHistoryByUser(info);
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

std::map<std::string, Channel > &Server::getChannels() {
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

int Server::connectToChannel(const User &user, const std::string &name,
							 const std::string &key) {
	try
	{
		Channel	&tmp = _channels.at(name);
		tmp.connect(user, key);
		return (1);
	}
	catch(const std::exception& e)
	{
		_channels[name] =  Channel(name, user, key);
	}
	return (1);
}

const std::map<std::string, std::string> &Server::getOperators() const
{
    return _operators;
}


int		Server::handleUserFlags(std::string &flag, User &user)
{
	if (flag == "+i")
		user.setFlag(INVISIBLE);
	else if (flag == "-i")
		user.removeFlag(INVISIBLE);
	else if (flag == "+s")
		user.setFlag(RECEIVENOTICE);
	else if (flag == "-s")
		user.removeFlag(RECEIVENOTICE);
	else if (flag == "+w")
		user.setFlag(RECEIVEWALLOPS);
	else if (flag == "-w")
		user.removeFlag(RECEIVEWALLOPS);
	else if (flag == "+o")
	{}
	else if (flag == "-o")
		user.removeFlag(IRCOPERATOR);
	else
	{
		utils::_errorSend(user, ERR_UMODEUNKNOWNFLAG);
		return -1;
	}
	return 0;
}

int		Server::handleChanFlags(std::vector<std::string> &param, User &user, const std::string &commands)
{
	std::string	chanName = param[0];
	std::string	flag = param[1];
	if (flag == "+o")
	{
		if (param.size() < 3)
			return  utils::_errorSend(user, ERR_NEEDMOREPARAMS, commands);
		else if (!containsNickname(param[2]))
			return utils::_errorSend(user, ERR_NOSUCHNICK, param[2]);
		else
			_channels[chanName].addOperator(*(getUserByName(param[2])));
	}
	else if (flag == "-o")
	{
		if (param.size() < 3)
			return  utils::_errorSend(user, ERR_NEEDMOREPARAMS, commands);
		else if (!containsNickname(param[2]))
			return utils::_errorSend(user, ERR_NOSUCHNICK, param[2]);
		else
			_channels[chanName].removeOperator(*(getUserByName(param[2])));
	}
	else if (flag == "+p")
		_channels[chanName].setFlag(PRIVATE);
	else if (flag == "-p")
		_channels[chanName].removeFlag(PRIVATE);
	else if (flag == "+s")
		_channels[chanName].setFlag(SECRET);
	else if (flag == "-s")
		_channels[chanName].removeFlag(SECRET);
	else if (flag == "+i")
		_channels[chanName].setFlag(INVITEONLY);
	else if (flag == "-i")
		_channels[chanName].removeFlag(INVITEONLY);
	else if (flag == "+t")
		_channels[chanName].setFlag(TOPICSET);
	else if (flag == "-t")
		_channels[chanName].removeFlag(TOPICSET);
	else if (flag == "+n")
	{}
	else if (flag == "-n")
	{}
	else if (flag == "+m")
		_channels[chanName].setFlag(MODERATED);
	else if (flag == "-m")
		_channels[chanName].removeFlag(MODERATED);
	else if (flag == "+l")
	{
		if (param.size() < 3)
			return  utils::_errorSend(user, ERR_NEEDMOREPARAMS, commands);
		else
			_channels[chanName].setLimit(atoi(param[2].c_str()));
	}
	else if (flag == "-l")
	{
		if (param.size() < 3)
			return  utils::_errorSend(user, ERR_NEEDMOREPARAMS, commands);
		else
			_channels[chanName].setLimit(0);
	}
	else if (flag == "+b")
	{
		if (param.size() < 3)
			return  utils::_errorSend(user, ERR_NEEDMOREPARAMS, commands);
		else
			_channels[chanName].addBanMask(param[2]);
	}
	else if (flag == "-b")
	{
		if (param.size() < 3)
			return utils::_errorSend(user, ERR_NEEDMOREPARAMS, commands);
		else
			_channels[chanName].removeBanMask(param[2]);
	}
	else if (flag == "+v")
	{
		if (param.size() < 3)
			return  utils::_errorSend(user, ERR_NEEDMOREPARAMS, commands);
		else if (!containsNickname(param[2]))
			return utils::_errorSend(user, ERR_NOSUCHNICK, param[2]);
		else
			_channels[chanName].addSpeaker(*(getUserByName(param[2])));
	}
	else if (flag == "-v")
	{
		if (param.size() < 3)
			return  utils::_errorSend(user, ERR_NEEDMOREPARAMS, commands);
		else if (!containsNickname(param[2]))
			return utils::_errorSend(user, ERR_NOSUCHNICK, param[2]);
		else
			_channels[chanName].removeSpeaker(*(getUserByName(param[2])));
	}
	else if (flag == "+k")
	{
		if (param.size() < 3)
			return  utils::_errorSend(user, ERR_NEEDMOREPARAMS, commands);
		else
			_channels[chanName].setKey(user, param[2]);
	}
	else if (flag == "-k")
	{
		if (param.size() < 3)
			return  utils::_errorSend(user, ERR_NEEDMOREPARAMS, commands);
		else
			_channels[chanName].setKey(user, "");
	}
	else
		return utils::_errorSend(user, ERR_UNKNOWNMODE, flag);
	return 0;
}