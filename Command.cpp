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

    if (!(_user.authorized()) && command != "QUIT" && command != "PASS" && command != "USER" && command != "NICK") {
		std::cout << "!!!1" << std::endl;
		return (utils::_errorSend(_user, ERR_NOTREGISTERED));
	}
	else
	{
        if ((commandPtr = _commandMap.find(command)) == _commandMap.end()) {
			std::cout << "!!!2" << std::endl;
			return utils::_errorSend(_user, ERR_UNKNOWNCOMMAND, command);
		}
        else {
			std::cout << "!!!3" << command << std::endl;
			return (this->*_commandMap[command])(prefix, param);
		}
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
    _commandMap["PING"] = &Command::_cmdPING;
    _commandMap["ISON"] = &Command::_cmdISON;
    _commandMap["USERHOST"] = &Command::_cmdUSERHOST;
    _commandMap["VERSION"] = &Command::_cmdVERSION;
    _commandMap["INFO"] = &Command::_cmdINFO;
    _commandMap["ADMIN"] = &Command::_cmdADMIN;
    _commandMap["TIME"] = &Command::_cmdTIME;
}

/*

int Command::_errorSend(User &user, int code, std::string param1, std::string param2)
{
	std::string	msg = ":" + user.getInfo().servername + " ";
	
    msg += std::to_string(code);
	msg += " " + user.getInfo().nickname;
	switch (code)
	{
	case ERR_NOSUCHNICK:
		msg += " " + param1 + " :No such nick/channel\n";
		break;
	case ERR_NOSUCHSERVER:
		msg += " " + param1 + " :No such server\n";
		break;
	case ERR_NOSUCHCHANNEL:
		msg += " " + param1 + " :No such channel\n";
		break;
	case ERR_CANNOTSENDTOCHAN:
		msg += " " + param1 + " :Cannot send to channel\n";
		break;
	case ERR_TOOMANYCHANNELS:
		msg += " " + param1 + " :You have joined too many channels\n";
		break;
	case ERR_WASNOSUCHNICK:
		msg += " " + param1 + " :There was no such nickname\n";
		break;
	case ERR_TOOMANYTARGETS:
		msg += " " + param1 + " :Duplicate recipients. No param1 delivered\n";
		break;
	case ERR_NOORIGIN:
		msg += " :No origin specified\n";
		break;
	case ERR_NORECIPIENT:
		msg += " :No recipient given (" + param1 + ")\n";
		break;
	case ERR_NOTEXTTOSEND:
		msg += " :No text to send\n";
		break;
	case ERR_NOTOPLEVEL:
		msg += " " + param1 + " :No toplevel domain specified\n";
		break;
	case ERR_WILDTOPLEVEL:
		msg += " " + param1 + " :Wildcard in toplevel domain\n";
		break;
	case ERR_UNKNOWNCOMMAND:
		msg += " " + param1 + " :Unknown command\n";
		break;
	case ERR_NOMOTD:
		msg += " :MOTD File is missing\n";
		break;
	case ERR_NOADMININFO:
		msg += " " + param1 + " :No administrative info available\n";
		break;
	case ERR_FILEERROR:
		msg += " :File error doing \n" + param1 + " on " + param2 + "\n";
		break;
	case ERR_NONICKNAMEGIVEN:
		msg += " :No nickname given\n";
		break;
	case ERR_ERRONEUSNICKNAME:
		msg += " " + param1 + " :Erroneus nickname\n";
		break;
	case ERR_NICKNAMEINUSE:
		msg += " " + param1 + " :Nickname is already in use\n";
		break;
	case ERR_NICKCOLLISION:
		msg += " " + param1 + " :Nickname collision KILL\n";
		break;
	case ERR_USERNOTINCHANNEL:
		msg += " " + param1 + " " + param2 + " :They aren't on that channel\n";
		break;
	case ERR_NOTONCHANNEL:
		msg += " " + param1 + " :You're not on that channel\n";
		break;
	case ERR_USERONCHANNEL:
		msg += " " + param1 + " " + param2 + " :is already on channel\n";
		break;
	case ERR_NOLOGIN:
		msg += " " + param1 + " :User not logged in\n";
		break;
	case ERR_SUMMONDISABLED:
		msg += " :SUMMON has been disabled\n";
		break;
	case ERR_USERSDISABLED:
		msg += " :USERS has been disabled\n";
		break;
	case ERR_NOTREGISTERED:
		msg += " :You have not registered\n";
		break;
	case ERR_NEEDMOREPARAMS:
		msg += " " + param1 + " :Not enough parameters\n";
		break;
	case ERR_ALREADYREGISTRED:
		msg += " :You may not reregister\n";
		break;
	case ERR_NOPERMFORHOST:
		msg += " :Your host isn't among the privileged\n";
		break;
	case ERR_PASSWDMISMATCH:
		msg += " :Password incorrect\n";
		break;
	case ERR_YOUREBANNEDCREEP:
		msg += " :You are banned from this server\n";
		break;
	case ERR_KEYSET:
		msg += " " + param1 + " :Channel key already set\n";
		break;
	case ERR_CHANNELISFULL:
		msg += " " + param1 + " :Cannot join channel (+l)\n";
		break;
	case ERR_UNKNOWNMODE:
		msg += " " + param1 + " :is unknown mode char to me\n";
		break;
	case ERR_INVITEONLYCHAN:
		msg += " " + param1 + " :Cannot join channel (+i)\n";
		break;
	case ERR_BANNEDFROMCHAN:
		msg += " " + param1 + " :Cannot join channel (+b)\n";
		break;
	case ERR_BADCHANNELKEY:
		msg += " " + param1 + " :Cannot join channel (+k)\n";
		break;
	case ERR_NOPRIVILEGES:
		msg += " :Permission Denied- You're not an IRC operator\n";
		break;
	case ERR_CHANOPRIVSNEEDED:
		msg += " " + param1 + " :You're not channel operator\n";
		break;
	case ERR_CANTKILLSERVER:
		msg += " :You cant kill a server!\n";
		break;
	case ERR_NOOPERHOST:
		msg += " :No O-lines for your host\n";
		break;
	case ERR_UMODEUNKNOWNFLAG:
		msg += " :Unknown MODE flag\n";
		break;
	case ERR_USERSDONTMATCH:
		msg += " :Cant change mode for other users\n";
		break;
	default:
		msg += "UNKNOWN ERROR\n";
		break;
	}
    return(send(user.getFd(), msg.c_str(), msg.size(), IRC_NOSIGNAL));
}*/
