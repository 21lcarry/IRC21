#include <sstream>
#include "../Command.hpp"
#include "../Channel.hh"
#include <queue>
#include <set>

std::queue<std::string>	split(const std::string &s, char sep, bool include)
{
	std::queue<std::string>	ret;
	std::string::const_iterator	i = s.begin();
	while(i != s.end())
	{
		while (i != s.end() && *i == sep)
			++i;
		std::string::const_iterator	j = std::find(i, s.end(), sep);
		if (i != s.end())
		{
			if (include && j != s.end())
				ret.push(std::string(i, j + 1));
			else
				ret.push(std::string(i, j));
			i = j;
		}
	}
	return ret;
}

int Command::_cmdPRIVMSG_(std::string &prefix, std::vector<std::string> &param,
						  std::string command) {
	if (param.size() == 0)
		return (_errorSend(_user, ERR_NORECIPIENT, command));
	if (param.size() == 1)
		return (_errorSend(_user, ERR_NOTEXTTOSEND));

	std::queue<std::string> receivers = split(param[0], ',', false);
	std::set<std::string> uniqReceivers;
	UserInfo infoUser = _user.getInfo();
	if (command == "NOTICE" && (receivers.size() > 1 \
	|| receivers.front()[0] == '#' || receivers.front()[0] == '&'))
		return (_errorSend(_user, ERR_NOSUCHNICK, param[0]));

	while (receivers.size() > 0)
	{
		// checking if there contains dublicate receiver
		if (uniqReceivers.find(receivers.front()) != uniqReceivers.end())
			return (_errorSend(_user, ERR_TOOMANYTARGETS, receivers.front()));
		// if receiver is channel
		if (receivers.front()[0] == '#' || receivers.front()[0] == '&')
		{
			// checking if there such a channel
			if (!_server.containsChannel(receivers.front()))
				return (_errorSend(_user, ERR_NOSUCHNICK, receivers.front()));
			// check that the current user is in the channel
			if (!_server.getChannels()[receivers.front()]->containsNickname(_user.getInfo().nickname))
				return (_errorSend(_user, ERR_CANNOTSENDTOCHAN, receivers.front()));
		}
			// checking if there such a nickname
		else if (!_server.containsNickname(receivers.front()))
			return (_errorSend(_user, ERR_NOSUCHNICK, param[0]));
		uniqReceivers.insert(receivers.front());
		receivers.pop();
	}
	for (std::set<std::string>::iterator it = uniqReceivers.begin(); it != uniqReceivers.end(); ++it)
	{
		if ((*it)[0] == '#' || (*it)[0] == '&')
		{
			Channel *receiverChannel = _server.getChannels()[*it];
			// check that user can send message to channel (user is operator or speaker on moderated channel)
			if (receiverChannel->getFlags() & MODERATED && (!receiverChannel->isOperator(infoUser) && !receiverChannel->isSpeaker(infoUser)))
				_errorSend(_user, ERR_CANNOTSENDTOCHAN, *it);
			else
				receiverChannel->sendMessage(command + " " + *it + " :" + param[1] + "\n", _user, false);
		}
		else
		{
			if (command == "PRIVMSG" && (_server.getUserByName(*it)->getInfo().flags & AWAY))
				sendReply(infoUser.servername, _user, RPL_AWAY, *it, _server.getUserByName(*it)->getInfo().awayMessage);
			if (command != "NOTICE" || (_server.getUserByName(*it)->getInfo().flags & RECEIVENOTICE))
				_server.getUserByName(*it)->sendMessage(":" + _user.getPrefix() + " " + command + " " + *it + " :" + param[1] + "\n");
		}
	}
	return 0;
}
int Command::_cmdPRIVMSG(std::string &prefix, std::vector<std::string> &param)
{
	_cmdPRIVMSG_(prefix,param,"PRIVMSG");
	return 0;
}

int Command::_cmdAWAY(std::string &prefix, std::vector<std::string> &param)
{

	if (param.size() == 0)
	{
		_user.removeFlag(AWAY);
		sendReply(_user.getInfo().servername, _user, RPL_UNAWAY);
	}
	else
	{
		_user.setFlag(AWAY);
		_user.setAwayMessage(param[0]);
		sendReply(_user.getInfo().servername, _user, RPL_NOWAWAY);
	}
	return 0;
}
int Command::_cmdNOTICE(std::string &prefix, std::vector<std::string> &param)
{
	_cmdPRIVMSG_(prefix,param, "NOTICE");
    return 0;
}
int Command::_cmdWHO(std::string &prefix, std::vector<std::string> &param)
{
	if (param.size() == 0)
		return (_errorSend(_user, ERR_NEEDMOREPARAMS, "WHO"));
	std::vector<User> clients = this->_server.getClient();
	UserInfo infoUser = _user.getInfo();
	for (size_t i = 0; i < clients.size(); ++i)
	{
		UserInfo clientInfo = clients[i].getInfo();
		if (isEqualToRegex(param[0], clientInfo.nickname) && !(clientInfo.flags & INVISIBLE))
		{
			std::string channelName = "*";
			std::string userStatus = "";
			const std::vector<const Channel *> userChannels = clientInfo.channels;

			for (int j = userChannels.size() - 1; j >= 0; --j)
			{
				if ((!(userChannels[j]->getFlags() & SECRET) && !(userChannels[j]->getFlags() & PRIVATE)) \
				|| (userChannels[j]->containsNickname(infoUser.nickname)))
				{
					channelName = userChannels[j]->getName();
					if (userChannels[j]->isOperator(clientInfo))
						userStatus = "@";
					else if (userChannels[j]->isSpeaker(clientInfo))
						userStatus = "+";
					break;
				}
			}

			if (param.size() == 1  || param[1] != "o" \
			|| (param[1] == "o" && (clientInfo.flags & IRCOPERATOR)))
				sendReply(infoUser.servername, _user, RPL_WHOREPLY, channelName, clientInfo.username, clientInfo.hostname, \
							clientInfo.servername, clientInfo.nickname, "H" + userStatus, "0", clientInfo.realname);
		}
	}
	return (sendReply(infoUser.servername, _user, RPL_ENDOFWHO, infoUser.nickname));
}

int Command::sendReply(const std::string &from, const User &user, int rpl, \
				const std::string &arg1,const std::string &arg2, \
				const std::string &arg3,const std::string &arg4, \
				const std::string &arg5,const std::string &arg6, \
				const std::string &arg7,const std::string &arg8) const
{
	std::string	msg = ":" + from + " ";
	std::stringstream	ss;
	ss << rpl;
	msg += ss.str() + " " + user.getInfo().username + " ";
	switch (rpl)
	{
		case RPL_USERHOST:
			msg += ":" + arg1 + "\n";
			break;
		case RPL_ISON:
			msg += ":" + arg1 + "\n";
			break;
		case RPL_AWAY:
			msg += arg1 + " :" + arg2 + "\n";
			break;
		case RPL_UNAWAY:
			msg += ":You are no longer marked as being away\n";
			break;
		case RPL_NOWAWAY:
			msg += ":You have been marked as being away\n";
			break;
		case RPL_WHOISUSER:
			msg += arg1 + " " + arg2 + " " + arg3 + " * :" + arg4 + "\n";
			break;
		case RPL_WHOISSERVER:
			msg += arg1 + " " + arg2 + " :" + arg3 + "\n";
			break;
		case RPL_WHOISOPERATOR:
			msg += arg1 + " :is an IRC operator\n";
			break;
		case RPL_WHOISIDLE:
			msg += arg1 + " " + arg2 + " " + arg3 + " :seconds idle\n";
			break;
		case RPL_ENDOFWHOIS:
			msg += arg1 + " :End of /WHOIS list\n";
			break;
		case RPL_WHOISCHANNELS:
			msg += arg1 + " :" + arg2 + "\n";
			break;
		case RPL_WHOWASUSER:
			msg += arg1 + " " + arg2 + " " + arg3 + " * :" + arg4 + "\n";
			break;
		case RPL_ENDOFWHOWAS:
			msg += arg1 + " :End of WHOWAS\n";
			break;
		case RPL_LISTSTART:
			msg += "Channel :Users  Name\n";
			break;
		case RPL_LIST:
			msg += arg1 +  " " + arg2 + " :" + arg3 + "\n";
			break;
		case RPL_LISTEND:
			msg += ":End of /LIST\n";
			break;
		case RPL_CHANNELMODEIS:
			msg += arg1 + " +" + arg2 + "\n";
			break;
		case RPL_NOTOPIC:
			msg += arg1 + " :No topic is set\n";
			break;
		case RPL_TOPIC:
			msg += arg1 + " :" + arg2 + "\n";
			break;
		case RPL_INVITING:
			msg += arg1 + " " + arg2 + "\n";
			break;
		case RPL_SUMMONING:
			msg += arg1 + " :Summoning user to IRC\n";
			break;
		case RPL_VERSION:
			msg += arg1 + "." + arg2 + " " + arg3 + " :" + arg4 + "\n";
			break;
		case RPL_WHOREPLY:
			msg += arg1 + " " + arg2 + " " + arg3 + " " + arg4 + " ";
			msg += arg5 + " " + arg6 + " :" + arg7 + " " + arg8 + "\n";
			break;
		case RPL_ENDOFWHO:
			msg += arg1 + " :End of /WHO list\n";
			break;
		case RPL_NAMREPLY:
			msg += arg1 + " :" + arg2 + "\n";
			break;
		case RPL_ENDOFNAMES:
			msg += arg1 + " :End of /NAMES list\n";
			break;
		case RPL_LINKS:
			msg += arg1 + " " + arg2 + ": " + arg3 + " " + arg4 + "\n";
			break;
		case RPL_ENDOFLINKS:
			msg += arg1 + " :End of /LINKS list\n";
			break;
		case RPL_BANLIST:
			msg += arg1 + " " + arg2 + "\n";
			break;
		case RPL_ENDOFBANLIST:
			msg += arg1 + " :End of channel ban list\n";
			break;
		case RPL_INFO:
			msg += ":" + arg1 + "\n";
			break;
		case RPL_ENDOFINFO:
			msg += ":End of /INFO list\n";
			break;
		case RPL_MOTDSTART:
			msg += ":- " + arg1 + " Message of the day - \n";
			break;
		case RPL_MOTD:
			msg += ":- " + arg1 + "\n";
			break;
		case RPL_ENDOFMOTD:
			msg += ":End of /MOTD command\n";
			break;
		case RPL_YOUREOPER:
			msg += ":You are now an IRC operator\n";
			break;
		case RPL_REHASHING:
			msg += arg1 + " :Rehashing\n";
			break;
		case RPL_TIME:
			msg += arg1 + " :" + arg2; // ctime return string following '\n'
			break;
		case RPL_USERSSTART:
			msg += ":UserID   Terminal  Host\n";
			break;
		case RPL_USERS:
			msg += ":%-8s %-9s %-8s\n"; // Хз, что это UPD: Понял, строка длиной 8 символов
			break;
		case RPL_ENDOFUSERS:
			msg += ":End of users\n";
			break;
		case RPL_NOUSERS:
			msg += ":Nobody logged in\n";
			break;
		case RPL_TRACELINK:
			msg += "Link " + arg1 + " " + arg2 + " " + arg3 + "\n";
			break;
		case RPL_TRACECONNECTING:
			msg += "Try. " + arg1 + " " + arg2 + "\n";
			break;
		case RPL_TRACEHANDSHAKE:
			msg += "H.S. " + arg1 + " " + arg2 + "\n";
			break;
		case RPL_TRACEUNKNOWN:
			msg += "???? " + arg1 + " " + arg2 + "\n"; // ????
			break;
		case RPL_TRACEOPERATOR:
			msg += "Oper " + arg1 + " " + arg2 + "\n";
			break;
		case RPL_TRACEUSER:
			msg += "User " + arg1 + " " + arg2 + "\n";
			break;
		case RPL_TRACESERVER:
			msg += "Serv " + arg1 + " " + arg2 + "S " + arg3 + "C ";
			msg += arg4 + " " + arg5 + "@" + arg6 + "\n";
			break;
		case RPL_TRACENEWTYPE:
			msg += arg1 + " 0 " + arg2 + "\n";
			break;
		case RPL_TRACELOG:
			msg += "File " + arg1 + " " + arg2 + "\n";
			break;
		case RPL_STATSLINKINFO:
			msg += arg1 + " " + arg2 + " " + arg3 + " " + arg4 + " ";
			msg += arg5 + " " + arg6 + " " + arg7 + "\n";
			break;
		case RPL_STATSCOMMANDS:
			msg += arg1 + " " + arg2 + "\n";
			break;
		case RPL_STATSCLINE:
			msg += "C " + arg1 + " * " + arg2 + " " + arg3 + " " + arg4 + "\n";
			break;
		case RPL_STATSNLINE:
			msg += "N " + arg1 + " * " + arg2 + " " + arg3 + " " + arg4 + "\n";
			break;
		case RPL_STATSILINE:
			msg += "I " + arg1 + " * " + arg2 + " " + arg3 + " " + arg4 + "\n";
			break;
		case RPL_STATSKLINE:
			msg += "K " + arg1 + " * " + arg2 + " " + arg3 + " " + arg4 + "\n";
			break;
		case RPL_STATSYLINE:
			msg += "Y " + arg1 + " * " + arg2 + " " + arg3 + " " + arg4 + "\n";
			break;
		case RPL_ENDOFSTATS:
			msg += arg1 + " :End of /STATS report\n";
			break;
		case RPL_STATSLLINE:
			msg += "L " + arg1 + " * " + arg2 + " " + arg3 + "\n";
			break;
		case RPL_STATSUPTIME:
			msg += ":Server Up %d days %d:%02d:%02d\n"; // Поменять
			break;
		case RPL_STATSOLINE:
			msg += "O " + arg1 + " * " + arg2 + "\n";
			break;
		case RPL_STATSHLINE:
			msg += "H " + arg1 + " * " + arg2 + "\n";
			break;
		case RPL_UMODEIS:
			msg += arg1 + "\n";
			break;
		case RPL_LUSERCLIENT:
			msg += ":There are " + arg1 + " users and " + arg2;
			msg += " invisible on " + arg3 + " servers\n";
			break;
		case RPL_LUSEROP:
			msg += arg1 + " :operator(s) online\n";
			break;
		case RPL_LUSERUNKNOWN:
			msg += arg1 + " :unknown connection(s)\n";
			break;
		case RPL_LUSERCHANNELS:
			msg += arg1 + " :channels formed\n";
			break;
		case RPL_LUSERME:
			msg += ":I have " + arg1 + " clients and " + arg2 + " servers\n";
			break;
		case RPL_ADMINME:
			msg += arg1 + " :Administrative info\n";
			break;
		case RPL_ADMINLOC1:
			msg += ":Name     " + arg1 + "\n";
			break;
		case RPL_ADMINLOC2:
			msg += ":Nickname " + arg1 + "\n";
			break;
		case RPL_ADMINEMAIL:
			msg += ":E-Mail   " + arg1 + "\n";
			break;
		default:
			msg += "UNKNOWN REPLY\n";
			break;
	}
	send(_user.getFd(), msg.c_str(), msg.size(), IRC_NOSIGNAL);
	return 0;
}
bool	Command::isEqualToRegex(std::string mask, std::string subString)
{
	const char *rs=0, *rp;
	const char *s = subString.c_str();
	const char *p = mask.c_str();
	while (1)
	{
		if (*p == '*')
		{
			rs = s;
			rp = ++p;
		}
		else if (!*s)
		{
			return (!(*p));
		}
		else if (*s == *p)
		{
			++s;
			++p;
		}
		else if (rs)
		{
			s = ++rs;
			p = rp;
		}
		else
		{
			return (false);
		}
	}
}

int Command::_cmdWHOWAS(std::string &prefix, std::vector<std::string> &param)
{
	if (param.size() == 0)
		return (_errorSend(_user, ERR_NONICKNAMEGIVEN, param[0]));

	else if (!this->_server.userIsConnecting(param[0]))
	{
		 std::vector<const UserInfo *> historyList = this->_server.getHistoryByUser(param[0]);
		if (historyList.size() == 0)
			_errorSend(_user, ERR_WASNOSUCHNICK, param[0]);
		else
		{
			UserInfo userinfo = _user.getInfo();
			int n = 0;
			if (param.size() > 1)
				n = atoi(param[1].c_str());
			n = (n == 0) ? historyList.size() : n;

			for (int i = 0; i < n && i < static_cast<int>(historyList.size()); ++i)
			{
				sendReply(userinfo.servername, _user, RPL_WHOWASUSER, historyList[i]->nickname, \
				historyList[i]->username, historyList[i]->hostname, historyList[i]->realname);
				sendReply(userinfo.servername, _user, RPL_WHOISSERVER, historyList[i]->nickname, \
				historyList[i]->servername, "IRC server based on TCP/IP protocol to rfc1459 standard");
			}
		}
	}
	return (sendReply(_user.getInfo().servername, _user, RPL_ENDOFWHOWAS, param[0]));
}
int Command::_cmdWHOIS(std::string &prefix, std::vector<std::string> &param) {
	if (param.size() == 0)
		return (_errorSend(_user, ERR_NONICKNAMEGIVEN));
	UserInfo userinfo = _user.getInfo();
	bool suchNick = false;
	std::vector<User> clients = this->_server.getClient();
	for (size_t i = 0; i < clients.size(); ++i)
	{
		UserInfo clientInfo = clients[i].getInfo();
		if (isEqualToRegex(param[0], clientInfo.nickname) && !(clientInfo.flags & IRCOPERATOR))
		{
			sendReply(userinfo.servername, _user, RPL_WHOISUSER, clientInfo.nickname, \
			clientInfo.username, clientInfo.hostname, clientInfo.realname);

			const type_channel_arr userChannels = clientInfo.channels;
			std::string	channelsList;
			for (size_t j = 0; j < userChannels.size(); ++j)
			{
				if ((!(userChannels[j]->getFlags() & SECRET) && !(userChannels[j]->getFlags() & PRIVATE)) \
				|| (userChannels[j]->containsNickname(userinfo.nickname)))
				{
					if (j != 0)
						channelsList += " ";
					if (userChannels[j]->isOperator(clientInfo))
						channelsList += "@";
					else if (userChannels[j]->isSpeaker(clientInfo))
						channelsList += "+";
					channelsList += userChannels[j]->getName();
				}
			}
			sendReply(userinfo.servername, _user, RPL_WHOISCHANNELS, clientInfo.nickname, channelsList);
			sendReply(userinfo.servername, _user, RPL_WHOISSERVER, clientInfo.nickname, clientInfo.servername, "IRC server based on TCP/IP protocol to rfc1459 standard");
			if (clientInfo.flags & AWAY)
				sendReply(userinfo.servername, _user, RPL_AWAY, clientInfo.nickname,  clientInfo.awayMessage);
			if (clientInfo.flags & IRCOPERATOR)
				sendReply(userinfo.servername, _user, RPL_WHOISOPERATOR, clientInfo.nickname);
			std::stringstream	onServer, regTime;
			onServer << (time(0) -  clientInfo.registrationTime);
			regTime << clientInfo.registrationTime;
			sendReply(userinfo.servername, _user, RPL_WHOISIDLE, clientInfo.nickname, \
			onServer.str(), regTime.str());
			suchNick = true;
		}
	}
	if (!suchNick)
		_errorSend(_user, ERR_NOSUCHNICK, param[0]);
	return (sendReply(userinfo.servername, _user, RPL_ENDOFWHOIS, param[0]));
}