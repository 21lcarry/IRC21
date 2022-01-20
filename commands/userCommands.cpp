#include <sstream>
#include "../Command.hpp"
#include "../Channel.hpp"
#include "../utils.hpp"
#include <queue>
#include <set>



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
	UserInfo userinfo = _user.getInfo();
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
				utils::sendReply(_user.getFd(), userinfo.servername, userinfo, RPL_WHOWASUSER, historyList[i]->nickname, \
				historyList[i]->username, historyList[i]->hostname, historyList[i]->realname);
				utils::sendReply(_user.getFd(), userinfo.servername, userinfo, RPL_WHOISSERVER, historyList[i]->nickname, \
				historyList[i]->servername, "IRC server based on TCP/IP protocol to rfc1459 standard");
			}
		}
	}
	return (utils::sendReply(_user.getFd(), userinfo.servername, userinfo, RPL_ENDOFWHOWAS, param[0]));
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
			utils::sendReply(_user.getFd(), userinfo.servername, userinfo, RPL_WHOISUSER, clientInfo.nickname, \
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
					if (userChannels[j]->isOperator(clients[i]))
						channelsList += "@";
					else if (userChannels[j]->isSpeaker(clients[i]))
						channelsList += "+";
					channelsList += userChannels[j]->getName();
				}
			}
			utils::sendReply(_user.getFd(), userinfo.servername, userinfo, RPL_WHOISCHANNELS, clientInfo.nickname, channelsList);
			utils::sendReply(_user.getFd(), userinfo.servername, userinfo, RPL_WHOISSERVER, clientInfo.nickname, clientInfo.servername, "IRC server based on TCP/IP protocol to rfc1459 standard");
			if (clientInfo.flags & AWAY)
				utils::sendReply(_user.getFd(), userinfo.servername, userinfo, RPL_AWAY, clientInfo.nickname,  clientInfo.awayMessage);
			if (clientInfo.flags & IRCOPERATOR)
				utils::sendReply(_user.getFd(), userinfo.servername, userinfo, RPL_WHOISOPERATOR, clientInfo.nickname);
			std::stringstream	onServer, regTime;
			onServer << (time(0) -  clientInfo.registrationTime);
			regTime << clientInfo.registrationTime;
			utils::sendReply(_user.getFd(), userinfo.servername, userinfo, RPL_WHOISIDLE, clientInfo.nickname, \
			onServer.str(), regTime.str());
			suchNick = true;
		}
	}
	if (!suchNick)
		_errorSend(_user, ERR_NOSUCHNICK, param[0]);
	return (utils::sendReply(_user.getFd(), userinfo.servername, userinfo, RPL_ENDOFWHOIS, param[0]));
}