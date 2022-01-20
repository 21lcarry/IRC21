//
// Created by Администратор on 18.01.2022.
//

#ifndef IRC_UTILS_HPP
#define IRC_UTILS_HPP
#include <queue>
#include <ostream>
#include <string>
#include "User.hpp"
namespace utils {
	int sendReply(const int fd, const std::string &from, const UserInfo &userInfo, int rpl, \
				const std::string &arg1 = "",const std::string &arg2 = "", \
				const std::string &arg3 = "",const std::string &arg4 = "", \
				const std::string &arg5 = "",const std::string &arg6 = "", \
				const std::string &arg7 = "",const std::string &arg8 = "") ;
	std::queue<std::string>	split(const std::string &s, char sep, bool include);
	int _errorSend(const User &user, int code, const std::string param1 = "", const std::string param2 = ""); // todo dublicate!!!
	bool						isEqualToRegex(std::string mask, std::string subString);
};

#endif //IRC_UTILS_HPP
