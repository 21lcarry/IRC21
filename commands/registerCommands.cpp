#include "../Command.hpp"
#include "../utils.hpp"
int Command::_cmdPASS(std::string &prefix, std::vector<std::string> &param)
{
    if (param.size() == 0)
		return (utils::_errorSend(_user, ERR_NEEDMOREPARAMS, "PASS"));
	else if (_user.authorized() == true)
		return (utils::_errorSend(_user, ERR_ALREADYREGISTRED));
	else
	{
		if (param[0][0] == ':' && param[0] != _server.getPass())
			param[0] = param[0].substr(1, param[0].size() - 1);
		_user.setPassword(param[0]);
	}
	return 1;
}

bool Command::_validateNick(std::string &nick)
{
	std::string	special = "-[]\\`^{}";

	if (nick.length() > 9)
		return (false);
	for (size_t i = 0; i < nick.size(); ++i)
	{
		if ((nick[i] >= 'a' && nick[i] <= 'z') || (nick[i] >= 'A' && nick[i] <= 'Z')
		|| (nick[i] >= '0' && nick[i] <= '9') || (special.find(nick[i]) != std::string::npos))
			continue ;
		else
			return (false);
	}
	return (true);
}

bool Command::_nickInUse(std::string &nick)
{
	for (std::vector<User>::const_iterator i = _server.getClients().begin(); \
		i != _server.getClients().end(); ++i )
	{
		if (nick == i->getInfo().nickname)
			return false;
	}
	return true;
}

int Command::_authorization()
{
	int ret = 1;
	UserInfo infoUser = _user.getInfo();
	if (_user.getInfo().nickname.size() > 0 && infoUser.username.size() > 0)
	{
		if (_server.getPass().size() == 0 || _user.getPwd() == _server.getPass())
		{
			if (!(_user.authorized()))
			{
				_user.setAuthorized(true);

				_server.addUserHistoru(infoUser);//  todo возможно стоит  запихнуть в _user.setAuthorized
				UserInfo userinfo = _user.getInfo();
				std::string test = "*MOTD reply should be here*\n";
				utils::sendReply(_user.getFd(), userinfo.servername, userinfo, RPL_MOTDSTART, userinfo.servername);
				utils::sendReply(_user.getFd(),userinfo.servername, userinfo, RPL_MOTD, test);
				utils::sendReply(_user.getFd(), userinfo.servername, userinfo, RPL_ENDOFMOTD);
			}
		}
		else
			ret = -2;
	}
	return ret;
}

void Command::_notifyUsers(const std::string &msg)
{
	 std::vector<const Channel *> chans = _user.getInfo().channels;
	for (size_t i = 0; i < _server.getClients().size(); ++i)
	{
		for (size_t j = 0; j < chans.size(); j++)
		{
			if (chans.at(j)->containsNickname(_server.getClients()[i].getInfo().nickname))
			{
				_server.getClients()[i].sendMessage(msg);
				break ;
			}
		}
	}
}

int Command::_cmdNICK(std::string &prefix, std::vector<std::string> &param)
{
    if (param.size() == 0)
			return (utils::_errorSend(_user, ERR_NEEDMOREPARAMS, "NICK"));
	else if (!_validateNick(param[0]) || param[0] == _user.getInfo().nickname)
		return (utils::_errorSend(_user, ERR_ERRONEUSNICKNAME, param[0]));
	else if (!_nickInUse(param[0]))
		return (utils::_errorSend(_user, ERR_NICKNAMEINUSE, param[0]));
	else
	{
		if (_user.authorized())
		{
			_notifyUsers(":" + _user.getPrefix() + " NICK" + " " + param[0] + "\n");
			_server.addUserHistoru(_user.getInfo());
		}
		_user.setInfo("nickname", param[0]);
	}
	return (_authorization());
}

int Command::_cmdUSER(std::string &prefix, std::vector<std::string> &param)
{
	if (param.size() < 4){
		return (utils::_errorSend(_user, ERR_NEEDMOREPARAMS, "USER"));
	}
	else if (_user.authorized()){
		return (utils::_errorSend(_user, ERR_ALREADYREGISTRED));
	}
	else
	{
		std::string realname;
		for (std::vector<std::string>::iterator i = (param.begin() + 3); i != (param.end() - 1); ++i)
			realname += *i + " ";
		realname += *(param.end() - 1);
		_user.setInfo("username", param[0]);
		_user.setInfo("realname", realname);
	}
	return (_authorization());
}

int Command::_cmdOPER(std::string &prefix, std::vector<std::string> &param)
{	
	if (param.size() < 2)
		return utils::_errorSend(_user, ERR_NEEDMOREPARAMS, "OPER");
	else if (_server.getOperators().size() == 0)
		return utils::_errorSend(_user, ERR_NOOPERHOST);
	else
	{
		std::string pwd = _server.getOperators().at(param[0]);
		if (param[1] == pwd)
		{
			_user.setFlag(IRCOPERATOR);
			return utils::sendReply(_user.getFd(), _user.getInfo().servername, _user.getInfo(), RPL_YOUREOPER);
		}
		return utils::_errorSend(_user, ERR_PASSWDMISMATCH);
	}
}
int Command::_cmdQUIT(std::string &prefix, std::vector<std::string> &param)
{
	if (param.size() > 0)
		_user.setInfo("quitMessage", param[0]);
	_server.addUserHistoru(_user.getInfo());
	return (-2);
}
