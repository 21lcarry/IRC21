#include <sstream>
#include "../Command.hpp"
#include "../Channel.hpp"
#include "../utils.hpp"
#include <queue>
#include <set>

int Command::_cmdPRIVMSG_(std::string &prefix, std::vector<std::string> &param,
						  std::string command) {
	if (param.size() == 0)
		return (utils::_errorSend(_user, ERR_NORECIPIENT, command));
	if (param.size() == 1)
		return (utils::_errorSend(_user, ERR_NOTEXTTOSEND));
	std::string msg;
	std::queue<std::string> receivers = utils::split(param[0], ',', false);
	std::set<std::string> uniqReceivers;
	UserInfo infoUser = _user.getInfo();

	for (std::vector<std::string>::iterator it = (param.begin() + 1); it != param.end(); ++it)
		msg += (((*it)[0] == ':') ? it->substr(1, it->size() - 1) : *it) + " ";
	msg = msg.substr(0, msg.size() - 1);
	if (command == "NOTICE" && (receivers.size() > 1 \
	|| receivers.front()[0] == '#' || receivers.front()[0] == '&'))
		return (utils::_errorSend(_user, ERR_NOSUCHNICK, param[0]));
	while (receivers.size() > 0)
	{
		if (uniqReceivers.find(receivers.front()) != uniqReceivers.end())
			return (utils::_errorSend(_user, ERR_TOOMANYTARGETS, receivers.front()));
		if (receivers.front()[0] == '#' || receivers.front()[0] == '&')
		{
			if (!_server.containsChannel(receivers.front()))
				return (utils::_errorSend(_user, ERR_NOSUCHNICK, receivers.front()));
			Channel & c = _server.getChannels()[receivers.front()];
			if (!c.containsNickname(_user.getInfo().nickname)){
				std::cout << "####2 " <<_user.getInfo().nickname << std::endl;
				return (utils::_errorSend(_user, ERR_CANNOTSENDTOCHAN, receivers.front()));}
		}
		else if (!_server.containsNickname(receivers.front()))
			return (utils::_errorSend(_user, ERR_NOSUCHNICK, param[0]));
		uniqReceivers.insert(receivers.front());
		receivers.pop();
	}
	for (std::set<std::string>::iterator it = uniqReceivers.begin(); it != uniqReceivers.end(); ++it)
	{
		if ((*it)[0] == '#' || (*it)[0] == '&')
		{
			Channel &receiverChannel = _server.getChannels()[*it];

			if (receiverChannel.getFlags() & MODERATED && (!receiverChannel.isOperator(_user) && !receiverChannel.isSpeaker(_user)))
				utils::_errorSend(_user, ERR_CANNOTSENDTOCHAN, *it);
			else
				receiverChannel.sendMessage(command + " " + *it + " :" + msg + "\n", _user, false);
		}
		else
		{
			if (command == "PRIVMSG" && (_server.getUserByName(*it)->getInfo().flags & AWAY))
				utils::sendReply(_user.getFd(), infoUser.servername, infoUser, RPL_AWAY, *it, _server.getUserByName(*it)->getInfo().awayMessage);
			if (command == "NOTICE" && !(_server.getUserByName(*it)->getInfo().flags & RECEIVENOTICE))
				_server.getUserByName(*it)->sendMessage(":" + _user.getPrefix() + " " + command + " " + *it + " :" + msg + "\n");
			else if (command != "NOTICE" || (_server.getUserByName(*it)->getInfo().flags & RECEIVENOTICE))
				_server.getUserByName(*it)->sendMessage(":" + _user.getPrefix() + " " + command + " " + *it + " :" + msg + "\n");
		}
	}
	return 1;
}
int Command::_cmdPRIVMSG(std::string &prefix, std::vector<std::string> &param)
{
	_cmdPRIVMSG_(prefix,param,"PRIVMSG");
	return 1;
}

int Command::_cmdAWAY(std::string &prefix, std::vector<std::string> &param)
{

	if (param.size() == 0)
	{
		_user.removeFlag(AWAY);
		utils::sendReply(_user.getFd(), _user.getInfo().servername, _user.getInfo(), RPL_UNAWAY);
	}
	else
	{
		_user.setFlag(AWAY);
		_user.setAwayMessage(param[0]);
		utils::sendReply(_user.getFd(),_user.getInfo().servername, _user.getInfo(), RPL_NOWAWAY);
	}
	return 1;
}
int Command::_cmdNOTICE(std::string &prefix, std::vector<std::string> &param)
{
	_cmdPRIVMSG_(prefix,param, "NOTICE");
    return 1;
}
int Command::_cmdWHO(std::string &prefix, std::vector<std::string> &param)
{
	if (param.size() == 0)
		return (utils::_errorSend(_user, ERR_NEEDMOREPARAMS, "WHO"));
	std::vector<User> clients = _server.getClients();
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
					if (userChannels[j]->isOperator(clients[i]))
						userStatus = "@";
					else if (userChannels[j]->isSpeaker(clients[i]))
						userStatus = "+";
					break;
				}
			}

			if (param.size() == 1  || param[1] != "o" \
			|| (param[1] == "o" && (clientInfo.flags & IRCOPERATOR)))
				utils::sendReply(_user.getFd(),infoUser.servername, _user.getInfo(), RPL_WHOREPLY, channelName, clientInfo.username, clientInfo.hostname, \
							clientInfo.servername, clientInfo.nickname, "H" + userStatus, "0", clientInfo.realname);
		}
	}
	return (utils::sendReply(_user.getFd(), infoUser.servername, _user.getInfo(), RPL_ENDOFWHO, infoUser.nickname));
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
		return (utils::_errorSend(_user, ERR_NONICKNAMEGIVEN));
	if (!this->_server.userIsConnecting(param[0]))
	{
		 std::vector<const UserInfo *> historyList = this->_server.getHistoryByUser(param[0]);
		if (historyList.size() == 0)
			utils::_errorSend(_user, ERR_WASNOSUCHNICK, param[0]);
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
				historyList[i]->servername, "IRC server based on rfc1459 standard");
			}
		}
	}
	return (utils::sendReply(_user.getFd(), userinfo.servername, userinfo, RPL_ENDOFWHOWAS, param[0]));
}
int Command::_cmdWHOIS(std::string &prefix, std::vector<std::string> &param) {
	if (param.size() == 0)
		return (utils::_errorSend(_user, ERR_NONICKNAMEGIVEN));
	UserInfo userinfo = _user.getInfo();
	bool suchNick = false;
	std::vector<User> clients = _server.getClients();
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
		utils::_errorSend(_user, ERR_NOSUCHNICK, param[0]);
	return (utils::sendReply(_user.getFd(), userinfo.servername, userinfo, RPL_ENDOFWHOIS, param[0]));
}