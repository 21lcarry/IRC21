//
// Created by Администратор on 18.01.2022.
//
#include "utils.hpp"
namespace utils{

	int sendReply(const int fd, const std::string &from, const UserInfo &userInfo, int rpl, \
				const std::string &arg1,const std::string &arg2, \
				const std::string &arg3,const std::string &arg4, \
				const std::string &arg5,const std::string &arg6, \
				const std::string &arg7,const std::string &arg8)
	{
		std::string	msg = ":" + from + " ";
		msg += std::to_string(rpl) + " " + userInfo.nickname + " ";
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
				msg += ":%-8s %-9s %-8s\n";
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
				msg += "???? " + arg1 + " " + arg2 + "\n"; // todo ????
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
				msg += ":Server Up %d days %d:%02d:%02d\n"; // todo Поменять
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
		std::cout << "\n\nREPLY:\n" << msg << "\n\n";
		return(send(fd, msg.c_str(), msg.size(), IRC_NOSIGNAL));
	}
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
	int _errorSend(const User &user, int code, const std::string param1, const std::string param2)
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
		std::cout << "\n\nERROR:\n" << msg << "\n\n";
		return(send(user.getFd(), msg.c_str(), msg.size(), IRC_NOSIGNAL));
	}
	bool	isEqualToRegex(std::string mask, std::string subString)
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

	bool	isValidChannelName(const std::string &name)
	{
		if (name[0] != '#' && name[0] != '&')
			return false;
		for (size_t i = 1; i < name.size(); i++)
		{
			if (name[i] == ' ' || name[i] == 7 || name[i] == 0 \
			|| name[i] == 13 || name[i] == 10 || name[i] == ',')
				return false;
		}
		return true;
	}
}
#include "utils.hpp"
