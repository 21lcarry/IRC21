#include "../Command.hpp"
#include "../utils.hpp"

int Command::_cmdMODE(std::string &prefix, std::vector<std::string> &param)
{
	if (param.size() < 1)
		utils::_errorSend(_user, ERR_NEEDMOREPARAMS, "MODE");
	else
	{
		if (param[0][0] == '#')
		{
			if (!_server.containsChannel(param[0])){
std::cout <<"~~1\n";
				utils::_errorSend(_user, ERR_NOSUCHCHANNEL, param[0]);
			}
			else if (!_server.getChannels().at(param[0]).isOperator(_user)) {std::cout <<"~~2\n";
				utils::_errorSend(_user, ERR_CHANOPRIVSNEEDED, param[0]);
			}
			else if (!_server.getChannels().at(param[0]).containsNickname(_user.getInfo().nickname)){ std::cout <<"~~3\n";
				utils::_errorSend(_user, ERR_NOTONCHANNEL, param[0]);}
			else if (param.size() == 1) {std::cout <<"~~4\n";
				utils::sendReply(_user.getFd(), _user.getInfo().servername,
								 _user.getInfo(), RPL_CHANNELMODEIS, param[0],
								 _server.getChannels().at(
										 param[0]).getFlagsAsString());
			} else if (_server.handleChanFlags(param, _user, "MODE") != -1)
			{std::cout <<"~~5\n";
				std::string	flag = param[1];
				std::string	tmp = (flag[1] == 'o' || flag[1] == 'v') ? " " + param[2] : "";
				Channel &channels = _server.getChannels().at(param[0]);
				channels.sendMessage("MODE " + param[0] + " " + param[1]  + tmp + "\n", _user, true);
			}
		}
		else
		{
			if (param[0] != _user.getInfo().nickname)
				utils::_errorSend(_user, ERR_USERSDONTMATCH);
			else
			{
				if (param.size() == 1)
				{
					std::string	flags = "+";
					if (_user.getInfo().flags & INVISIBLE)
						flags += "i";
					if (_user.getInfo().flags & RECEIVENOTICE)
						flags += "s";
					if (_user.getInfo().flags & RECEIVEWALLOPS)
						flags += "w";
					if (_user.getInfo().flags  & IRCOPERATOR)
						flags += "o";
					utils::sendReply(_user.getFd(),_user.getInfo().servername, _user.getInfo(), RPL_UMODEIS, flags);
				}
				else if (_server.handleUserFlags(param[1], _user) != -1)
					_user.sendMessage(":" + _user.getPrefix() + " MODE " +param[0] + " " + param[1] + "\n");
			}
		}
	}
	return 0;
}
int Command::_cmdTOPIC(std::string &prefix, std::vector<std::string> &param)
{
	if (param.size() < 1)
		return(utils::_errorSend(_user, ERR_NEEDMOREPARAMS, "TOPIC"));
	else if (!_server.containsChannel(param[0]))
		return(utils::_errorSend(_user, ERR_NOTONCHANNEL, param[0]));
	else
	{
		Channel	&chan = _server.getChannels().at(param[0]);
		if (!chan.containsNickname(_user.getInfo().nickname))
			return(utils::_errorSend(_user, ERR_NOTONCHANNEL, param[0]));
		else if (param.size() < 2)
			chan.displayTopic(_user);
		else
		{
			std::string topic;

			for (std::vector<std::string>::iterator it = (param.begin() + 1); it != param.end(); ++it)
				topic += ((*it)[0] == ':') ? it->substr(1, it->size() - 1) : *it + " ";
			topic = topic.substr(0, topic.size() - 1);
			chan.setTopic(_user, topic);
		}
	}
	return 1;
}
int Command::_cmdJOIN(std::string &prefix, std::vector<std::string> &param)
{
	if (param.size() == 0)
		utils::_errorSend(_user, ERR_NEEDMOREPARAMS, "JOIN");
	else
	{
		std::queue<std::string>	chans = utils::split(param[0], ',', false);
		std::queue<std::string>	keys;
		if (param.size() > 1)
			keys = utils::split(param[1], ',', false);
		for (; chans.size() > 0; chans.pop())
		{
			std::string	key = keys.size() ? keys.front() : "";
			if (keys.size() > 0)
				keys.pop();
			if (!utils::isValidChannelName(chans.front()))
				utils::_errorSend(_user, ERR_NOSUCHCHANNEL, chans.front());
			else if (_user.getInfo().channels.size() >= MAX_CHANNEl)
				utils::_errorSend(_user, ERR_TOOMANYCHANNELS, chans.front());
			else if (_server.connectToChannel(_user, chans.front(), key) == 1)
				_user.addChannel(_server.getChannels().at(chans.front()));
		}
	}
	return 1;
}
int Command::_cmdINVITE(std::string &prefix, std::vector<std::string> &param)
{
	if (param.size() < 2)
		utils::_errorSend(_user, ERR_NEEDMOREPARAMS, "INVITE");
	else if (!_server.containsNickname(param[0]))
		utils::_errorSend(_user, ERR_NOSUCHNICK, param[0]);
	else if (!_user.isOnChannel(param[1]) || !_server.containsChannel(param[1]))
		utils::_errorSend(_user, ERR_NOTONCHANNEL, param[1]);
	else {
		User	receiver;
		for (size_t i = 0; i < _server.getClients().size(); ++i)
			if (_server.getClients()[i].getInfo().nickname == param[0])
				receiver = (_server.getClients()[i]);
		Channel	&chan = _server.getChannels().at(param[1]);
		if (chan.containsNickname(param[0]))
			utils::_errorSend(_user, ERR_USERONCHANNEL, param[0], "!!!IRCat!!!");
		else
			chan.invite(_user, receiver);
	}
	return 0;
}
int Command::_cmdKICK(std::string &prefix, std::vector<std::string> &param)
{
	if (param.size() < 2)
		utils::_errorSend(_user, ERR_NEEDMOREPARAMS, "KICK");
	else if (!_server.containsChannel(param[0]))
		utils::_errorSend(_user, ERR_NOSUCHCHANNEL,param[0]);
	else if (!_server.getChannels().at(param[0]).isOperator(_user))
		utils::_errorSend(_user, ERR_CHANOPRIVSNEEDED, param[0]);
	else if (!_server.getChannels().at(param[0]).containsNickname(_user.getInfo().nickname))
		utils::_errorSend(_user, ERR_NOTONCHANNEL, param[0]);
	else if (!_server.containsNickname(param[1]))
		utils::_errorSend(_user, ERR_NOSUCHNICK,param[1]);
	else if (!_server.getChannels().at(param[0]).containsNickname(param[1]))
		utils::_errorSend(_user, ERR_USERNOTINCHANNEL, param[1], param[0]);
	else
	{
		Channel	&chan = _server.getChannels().at(param[0]);
		std::string	message = "KICK " + chan.getName() + " " + param[1] + " :";
		if (param.size() > 2)
			message += param[2];
		else
			message += _user.getInfo().nickname;
		chan.sendMessage(message + "\n", _user, true);
		chan.disconnect(*(_server.getUserByName(param[1])));
		_server.getUserByName(param[1])->removeChannel(param[0]);
	}
	return 0;
}
int Command::_cmdPART(std::string &prefix, std::vector<std::string> &param)
{
	if (param.size() < 1)
		utils::_errorSend(_user, ERR_NEEDMOREPARAMS, "PART");
	else
	{
		std::queue<std::string>	chans = utils::split(param[0], ',', false);
		while (chans.size() > 0)
		{
			if (!_server.containsChannel(chans.front()))
				utils::_errorSend(_user, ERR_NOSUCHCHANNEL, chans.front());
			else if (!_user.isOnChannel(chans.front()))
				utils::_errorSend(_user, ERR_NOTONCHANNEL, chans.front());
			else
			{
				_server.getChannels().at(chans.front()).sendMessage("PART " + chans.front() + "\n", _user, true);
				_server.getChannels().at(chans.front()).disconnect(_user);
				_user.removeChannel(chans.front());
			}
			if (_server.getChannels().at(chans.front()).sizeUsers()==0){
			std::map<std::string, Channel> & ch =_server.getChannels();
				ch.erase(chans.front());
			}
			chans.pop();
		}
	}
	return 0;
}
int Command::_cmdNAMES(std::string &prefix, std::vector<std::string> &param)
{
	if (param.size() == 0)
	{
		std::vector<std::string>	usersWithoutChannel;
		for (size_t i = 0; i < _server.getClients().size(); i++)
			usersWithoutChannel.push_back(_server.getClients()[i].getInfo().nickname);
		if (_server.getChannels().size() > 0)
		{
			std::map<std::string, Channel>::iterator	beg = _server.getChannels().begin();
			std::map<std::string, Channel >::iterator	end = _server.getChannels().end();
			for (; beg != end; ++beg)
			{
				Channel &ch= beg->second;
				unsigned char flag = ch.getFlags();
				if (!(flag & SECRET) && !(flag & PRIVATE))
				{

					ch.displayNames(_user);
					for (size_t i = 0; i < usersWithoutChannel.size(); i++)
						if (ch.containsNickname(usersWithoutChannel[i]))
							usersWithoutChannel.erase(usersWithoutChannel.begin() + i--);
				}
			}
		}
		std::string	names;
		for (size_t i = 0; i < usersWithoutChannel.size(); i++)
		{
			names += usersWithoutChannel[i];
			if (i != (usersWithoutChannel.size() - 1))
				names += " ";
		}
		utils::sendReply(_user.getFd(),_user.getInfo().servername, _user.getInfo(), RPL_NAMREPLY, "* *", names);
		utils::sendReply(_user.getFd(),_user.getInfo().servername, _user.getInfo(), RPL_ENDOFNAMES, "*");
	}
	else
	{
		std::queue<std::string>	chansToDisplay;
		chansToDisplay = utils::split(param[0], ',', false);
		while (chansToDisplay.size() > 0)
		{
			try
			{
				Channel	&tmp = _server.getChannels().at(chansToDisplay.front());
				if (!(tmp.getFlags() & SECRET) && !(tmp.getFlags() & PRIVATE))
				{
					tmp.displayNames(_user);
					utils::sendReply(_user.getFd(),_user.getInfo().servername, _user.getInfo(), RPL_ENDOFNAMES, tmp.getName());
				}
			}
			catch(const std::exception& e)
			{}
			chansToDisplay.pop();
		}
	}
	return 0;
}
int Command::_cmdLIST(std::string &prefix, std::vector<std::string> &param)
{
	if (param.size() > 1 && param[1] != _user.getInfo().servername)
		return (utils::_errorSend(_user, ERR_NOSUCHSERVER, param[1]));
	std::queue<std::string>	chans;
	std::vector<std::string>	chansToDisplay;
	if (param.size() > 0)
	{
		chans = utils::split(param[0], ',', false);
		while (chans.size() > 0)
		{
			if (_server.containsChannel(chans.front()))
				chansToDisplay.push_back(chans.front());
			chans.pop();
		}
	}
	else
	{
		std::map<std::string, Channel >::const_iterator	beg = _server.getChannels().begin();
		std::map<std::string, Channel >::const_iterator	end = _server.getChannels().end();
		for (; beg != end; ++beg)
			chansToDisplay.push_back((*beg).first);
	}
	utils::sendReply(_user.getFd(),_user.getInfo().servername, _user.getInfo(), RPL_LISTSTART);
	for (size_t i = 0; i < chansToDisplay.size(); ++i)
		_server.getChannels().at(chansToDisplay[i]).displayChanInfo(_user);
	utils::sendReply(_user.getFd(),_user.getInfo().servername, _user.getInfo(), RPL_LISTEND);
	return 0;
}