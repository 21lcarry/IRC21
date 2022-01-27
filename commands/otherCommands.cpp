#include "../Command.hpp"

int Command::_cmdWALLOPS(std::string &prefix, std::vector<std::string> &param)
{
	if (!(_user.getInfo().flags & IRCOPERATOR))
		return (utils::_errorSend(_user, ERR_NOPRIVILEGES));
	if (param.size() == 0)
		return (utils::_errorSend(_user, ERR_NEEDMOREPARAMS, "WALLOPS"));

	const std::vector<User> usersList = _server.getClients();
	for (size_t i = 0; i < usersList.size(); ++i)
	{
		if (usersList[i].getInfo().flags & IRCOPERATOR)
			usersList[i].sendMessage(":" + _user.getPrefix() + " " \
			+"WALLOPS" + " :" + param[0] + "\n");
	}
	return 1;
}
int Command::_cmdPING(std::string &prefix, std::vector<std::string> &param)
{
    if (time(0) - _user.getActivity() > static_cast<time_t>(MAX_INACTIVE))
	{
		_user.sendMessage(":" + _user.getInfo().servername + " PING :" + _user.getInfo().servername + "\n");
		_user.setFlag(PINGING);
	}
	if ((_user.getFlag() & PINGING) && time(0) - _user.getActivity() > static_cast<time_t>(MAX_RESPONSE))
		return -2;
    return 1;
}
int Command::_cmdISON(std::string &prefix, std::vector<std::string> &param)
{
	if (param.size() == 0)
		return (utils::_errorSend(_user, ERR_NEEDMOREPARAMS, "ISON"));
	std::string	nicknamesList;
	for (size_t i = 0; i < param.size(); ++i)
	{
		if (_server.containsNickname(param[i]))
		{
			if (nicknamesList.size() > 0)
				nicknamesList += " ";
			nicknamesList +=param[i];
		}
	}
	return (utils::sendReply(_user.getFd(),_user.getInfo().servername, _user.getInfo(), RPL_ISON, nicknamesList));
}
int Command::_cmdUSERHOST(std::string &prefix, std::vector<std::string> &param)
{
	if (param.size() == 0)
		return (utils::_errorSend(_user, ERR_NEEDMOREPARAMS,"USERHOST"));

	std::string	replyMessage;
	for (size_t i = 0; i < param.size() && i < 5; ++i)
	{
		if (_server.containsNickname(param[i]))
		{
			User *currentUser = _server.getUserByName(param[i]);
			if (replyMessage.size() > 0)
				replyMessage += " ";
			replyMessage += param[i];
			if (currentUser->getInfo().flags & IRCOPERATOR)
				replyMessage += "*";
			replyMessage += (currentUser->getInfo().flags & AWAY) ? "=-@" : "=+@";
			replyMessage += currentUser->getInfo().hostname;
		}
	}
	return (utils::sendReply(_user.getFd(),_user.getInfo().servername, _user.getInfo(),  RPL_USERHOST, replyMessage));
}
int Command::_cmdVERSION(std::string &prefix, std::vector<std::string> &param)
{
	if (param.size() > 0 && param[0] != _user.getInfo().servername)
		return (utils::_errorSend(_user, ERR_NOSUCHSERVER, param[0]));
	return (utils::sendReply(_user.getFd(),_user.getInfo().servername, _user.getInfo(), RPL_VERSION, VERSION, debuglvl, NAME_SERVER, comments));
}
int Command::_cmdINFO(std::string &prefix, std::vector<std::string> &param)
{
	if (param.size() > 0 && param[0] != _user.getInfo().servername)
		return (utils::_errorSend(_user, ERR_NOSUCHSERVER, param[0]));
	std::queue<std::string>	lines = utils::split(describe, '\n', false);
	for (;lines.size() > 0; lines.pop())
		utils::sendReply(_user.getFd(),_user.getInfo().servername, _user.getInfo(), RPL_INFO, lines.front());
	utils::sendReply(_user.getFd(),_user.getInfo().servername, _user.getInfo(), RPL_INFO, INFO_SERVER);
	return (utils::sendReply(_user.getFd(),_user.getInfo().servername, _user.getInfo(), RPL_ENDOFINFO));
}
int Command::_cmdADMIN(std::string &prefix, std::vector<std::string> &param)
{
	if (param.size() > 0 && param[0] != _user.getInfo().servername)
		return (utils::_errorSend(_user, ERR_NOSUCHSERVER, param[0]));
	utils::sendReply(_user.getFd(),_user.getInfo().servername, _user.getInfo(), RPL_ADMINME, _user.getInfo().servername);
	utils::sendReply(_user.getFd(),_user.getInfo().servername, _user.getInfo(), RPL_ADMINLOC1, adminName);
	utils::sendReply(_user.getFd(),_user.getInfo().servername, _user.getInfo(), RPL_ADMINLOC2, adminNickname);
	utils::sendReply(_user.getFd(),_user.getInfo().servername, _user.getInfo(), RPL_ADMINEMAIL, adminEmail);
	return 1;
}
int Command::_cmdTIME(std::string &prefix, std::vector<std::string> &param)
{
	if (param.size() > 0 && param[0] != _user.getInfo().servername)
		return (utils::_errorSend(_user, ERR_NOSUCHSERVER, param[0]));
	time_t tmp = time(0);
	return (utils::sendReply(_user.getFd(),_user.getInfo().servername, _user.getInfo(), RPL_TIME, _user.getInfo().servername, ctime(&tmp)));
}

int		Command::_cmdPONG(std::string &prefix, std::vector<std::string> &param)
{
	if (param.size() <= 0 || param[0].substr(1, param[0].size()) != _server.getServerName())
		return (utils::_errorSend(_user, ERR_NOSUCHSERVER, param.size() > 0 ? param[0] : ""));
	return 1;
}