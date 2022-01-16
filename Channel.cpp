//
// Created by Администратор on 16.01.2022.
//

#include "Channel.hh"

Channel &Channel::operator=( Channel &other) {
	// todo

	return *this;
}

Channel::Channel(const Channel &other) {
// todo
}

Channel::Channel() {

}

 std::string Channel::getName() const {
	// todo
	return "todo";
}

 std::string Channel::getTopic() const {
	// todo
	return "todo";
}

unsigned char Channel::getFlags() const {
	// todo
	return 0;
}

bool Channel::containsNickname(std::string nic) const {
	// todo
	return false;
}

bool Channel::isOperator(const UserInfo &infoUser) const {
	// todo
	return false;
}

bool Channel::isSpeaker(const UserInfo &infUuser) const {
	// todo
	return false;
}

Channel::~Channel() {
// todo
}

void Channel::sendMessage(const std::string &message, const User &from,
						  bool includeUser) const {
	std::string	msg;
	msg += ":" + from.getPrefix() + " " + message;
	std::vector<const User *>::const_iterator	begin = users.begin();
	std::vector<const User *>::const_iterator	end = users.end();
	for (; begin != end; ++begin)
	{
		if (includeUser || *begin != &from)
			(*begin)->sendMessage(msg);
	}
}
