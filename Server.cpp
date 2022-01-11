#include "Server.hpp"

Server::Server(const std::string &pwd, int &port) : _ip(0), _port(port), _pass(pwd), _opt(1)
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
	setsockopt(newSocket, SOL_SOCKET, SO_NOSIGPIPE, &_opt, (socklen_t)sizeof(_opt));
	int flag = fcntl(newSocket, F_GETFL);
	fcntl(newSocket, F_SETFL, flag |  O_NONBLOCK);  //subject
	_clients.push_back(User(newSocket));
	std::cout << "New connection with socket fd" << newSocket << "\n";
}

void Server::run()
{
    fd_set readSet, writeSet;
	int sdTmp, rc, maxSd;

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
            sdTmp = thisClient->getFd();
            if (FD_ISSET(sdTmp, &readSet))
            {
                rc = _reciveRequest(*thisClient);
                if (rc < 0)
                {
                    FD_CLR(sdTmp, &readSet);
                    _clients.erase(thisClient);
                    std::cout << "ircserv: read [" << thisClient->getFd() << "] socket error" << std::endl;
                    break ;
                }
                if (rc == 0)
                {
                    FD_CLR(sdTmp, &readSet);
                    _clients.erase(thisClient);
                    std::cout << "ircserv: connection on socket [" << thisClient->getFd() << "] closed by client" << std::endl;
                    break ;
                }
            }
            if (FD_ISSET(sdTmp, &writeSet) && thisClient->isSend() == 0)
            {
                rc = _sendResponse(*thisClient);
                if (rc < 0)
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
    client.requestToVector(buff);

    /*********************************/
    std::vector<std::string> test = client.getRequest();
    std::cout << "New request vector: " << std::endl;
    for (std::vector<std::string>::iterator i = test.begin(); i != test.end() - 1; ++i)
        std::cout << "\"" << *i << "\", ";
    std::cout << "\"" << *(test.end() - 1) << "\"" << std::endl;
    /*********************************/
    client.setIsSend(5);
    return 1;
}

int Server::_sendResponse(User &client)
{
    std::string test_response = "Just an empty response";
    int rc = send(client.getFd(), test_response.c_str(), std::strlen(test_response.c_str()), SO_NOSIGPIPE);

    if (rc < 0)
    {
        std::cerr << "ircserv: send() failed[" << client.getFd() << "]: " <<  std::strerror(errno) << std::endl;
        client.setIsSend(1);
        return -1;
    }
    client.setIsSend(1);
    return 0;
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