#include "../Command.hpp"

int Command::_cmdWALLOPS(std::string &prefix, std::vector<std::string> &param)
{
    return 0;
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
    return 0;
}
int Command::_cmdUSERHOST(std::string &prefix, std::vector<std::string> &param)
{
    return 0;
}
int Command::_cmdVERSION(std::string &prefix, std::vector<std::string> &param)
{
    return 0;
}
int Command::_cmdINFO(std::string &prefix, std::vector<std::string> &param)
{
    return 0;
}
int Command::_cmdADMIN(std::string &prefix, std::vector<std::string> &param)
{
    return 0;
}
int Command::_cmdTIME(std::string &prefix, std::vector<std::string> &param)
{
    return 0;
}