#include "Command.hpp"

int Command::_cmdPASS(std::string &prefix, std::vector<std::string> &param)
{
    if (param.size() == 0)
		return (_errorSend(_user, ERR_NEEDMOREPARAMS, "PASS"));
	else if (!(_user.authorized()))
		return (_errorSend(_user, ERR_ALREADYREGISTRED));
	else
		_user.setPassword(param[0]);
	return 1;
}

int Command::_cmdNICK(std::string &prefix, std::vector<std::string> &param)
{
    if (param.size() == 0)
		return (_errorSend(_user, ERR_NEEDMOREPARAMS, "NICK"));
}