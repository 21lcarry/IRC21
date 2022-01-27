#include "Command.hpp"
#ifdef __APPLE__
#define IRC_NOSIGNAL SO_NOSIGPIPE
#else
#define IRC_NOSIGNAL MSG_NOSIGNAL
#endif
Command::Command(Server &server, User &user) : _server(server), _user(user) {
    _initCommandMap();
}

int Command::handleRequest()
{
    std::vector<std::string> request = _user.getRequest(), param;
    std::vector<std::string>::iterator it = request.begin();
    std::string prefix;
    std::string command;

    if (it != request.end() && (*it)[0] == ':')
        prefix = *it++;
    if (it != request.end())
        command = *it++;
    for (;it != request.end(); ++it)
        param.push_back(*it);
    return (_executeCommand(prefix, command, param));
}

int Command::_executeCommand(std::string &prefix, std::string &command, std::vector<std::string> &param)
{
    int ret = 0;
    std::map<std::string, int (Command::*)(std::string &, std::vector<std::string> &)>::iterator commandPtr;

	std::cout << (void*)&_user << std::endl;
    if (!(_user.authorized()) && command != "QUIT" && command != "PASS" && command != "USER" && command != "NICK") {
		return (utils::_errorSend(_user, ERR_NOTREGISTERED));
	}
	else
	{
        if ((commandPtr = _commandMap.find(command)) == _commandMap.end())
			return utils::_errorSend(_user, ERR_UNKNOWNCOMMAND, command);
        else 
			return (this->*_commandMap[command])(prefix, param);
	}
}

void Command::_initCommandMap()
{
    /*register*/
    _commandMap["PASS"] = &Command::_cmdPASS;
    _commandMap["NICK"] = &Command::_cmdNICK;
    _commandMap["USER"] = &Command::_cmdUSER;
    _commandMap["OPER"] = &Command::_cmdOPER;
    _commandMap["QUIT"] = &Command::_cmdQUIT;
    /*users*/
    _commandMap["PRIVMSG"] =  &Command::_cmdPRIVMSG;
    _commandMap["AWAY"] = &Command::_cmdAWAY;
    _commandMap["NOTICE"] = &Command::_cmdNOTICE;
    _commandMap["WHO"] = &Command::_cmdWHO;
    _commandMap["WHOIS"] = &Command::_cmdWHOIS;
    _commandMap["WHOWAS"] = &Command::_cmdWHOWAS;
    /*channel*/
    _commandMap["MODE"] = &Command::_cmdMODE;
    _commandMap["TOPIC"] = &Command::_cmdTOPIC;
    _commandMap["JOIN"] = &Command::_cmdJOIN;
    _commandMap["INVITE"] = &Command::_cmdINVITE;
    _commandMap["KICK"] = &Command::_cmdKICK;
    _commandMap["PART"] = &Command::_cmdPART;
    _commandMap["NAMES"] = &Command::_cmdNAMES;
    _commandMap["LIST"] = &Command::_cmdLIST;
    /*other*/
    _commandMap["WALLOPS"] = &Command::_cmdWALLOPS;
	_commandMap["PONG"] = &Command::_cmdPONG;
    _commandMap["PING"] = &Command::_cmdPING;
    _commandMap["ISON"] = &Command::_cmdISON;
    _commandMap["USERHOST"] = &Command::_cmdUSERHOST;
    _commandMap["VERSION"] = &Command::_cmdVERSION;
    _commandMap["INFO"] = &Command::_cmdINFO;
    _commandMap["ADMIN"] = &Command::_cmdADMIN;
    _commandMap["TIME"] = &Command::_cmdTIME;
}
