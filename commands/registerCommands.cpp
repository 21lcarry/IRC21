#include "../Command.hpp"
#include "../utils.hpp"
int Command::_cmdPASS(std::string &prefix, std::vector<std::string> &param)
{
    if (param.size() == 0)
		return (_errorSend(_user, ERR_NEEDMOREPARAMS, "PASS"));
	else if (_user.authorized() == true)
		return (_errorSend(_user, ERR_ALREADYREGISTRED));
	else
		_user.setPassword(param[0]);
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
				ret = send(_user.getFd(), test.c_str(), test.size(), IRC_NOSIGNAL);
			}
		}
		else
			ret = -2;
	}
	return ret;
}

int Command::_cmdNICK(std::string &prefix, std::vector<std::string> &param)
{
    if (param.size() == 0)
			return (_errorSend(_user, ERR_NEEDMOREPARAMS, "NICK"));
	else if (!_validateNick(param[0]) || param[0] == _user.getInfo().nickname)
		return (_errorSend(_user, ERR_ERRONEUSNICKNAME, param[0]));
	else if (!_nickInUse(param[0]))
		return (_errorSend(_user, ERR_NICKNAMEINUSE, param[0]));
	else
	{
//		if (_user.authorized())
//		{
		// добавление в историю и оповещение о смене ника
//		}
		_user.setInfo("nickname", param[0]);
	}
	return (_authorization());
}

int Command::_cmdUSER(std::string &prefix, std::vector<std::string> &param)
{
	if (param.size() < 4){
		return (_errorSend(_user, ERR_NEEDMOREPARAMS, "USER"));
	}
	else if (_user.authorized()){
		return (_errorSend(_user, ERR_ALREADYREGISTRED));
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
	return 0;
}
int Command::_cmdQUIT(std::string &prefix, std::vector<std::string> &param)
{
	return 0;
}

