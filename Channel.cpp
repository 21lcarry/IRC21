/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cshanda <cshanda@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/19 09:52:15 by cshanda           #+#    #+#             */
/*   Updated: 2022/01/19 09:52:15 by cshanda          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "Channel.hpp"


Channel &Channel::operator=(const Channel &other) {
	return copy(other);
}

Channel::Channel(const Channel &other) {
	copy(other);
}

const std::string Channel::getName() const {
	return _name;
}

std::string Channel::getTopic() const {
	return _topic;
}

unsigned char Channel::getFlags() const {
	return _flags;
}

bool Channel::containsNickname(std::string nic) const {
	std::vector<const User *>::const_iterator it = _users.begin();
	std::vector<const User *>::const_iterator end = _users.end();
	while (it != end) {
		if ((*it)->getInfo().nickname == nic)
			return (true);
		it++;
	}
	return (false);
}

bool Channel::isOperator(const User &user) const {
	for (size_t i = 0; i < _operators.size(); i++)
		if (_operators[i]->getPrefix() == user.getPrefix())
			return true;
	return false;
}

bool Channel::isSpeaker(const User &user) const {
	for (size_t i = 0; i < _speakers.size(); i++)
		if (_speakers[i]->getPrefix() == user.getPrefix())
			return true;
	return false;
}

Channel::~Channel() {

}

void Channel::sendMessage(const std::string &message, const User &from,
						  bool includeUser) const {
	std::string msg;
	msg += ":" + from.getPrefix() + " " + message;
	std::vector<const User *>::const_iterator begin = _users.begin();
	std::vector<const User *>::const_iterator end = _users.end();
	for (; begin != end; ++begin) {
		if (includeUser || *begin != &from)
			(*begin)->sendMessage(msg);
	}
}

std::string Channel::getFlagsAsString() const {
	std::string ret;
	if (_flags & INVITEONLY)
		ret += "i";
	if (_flags & NOMSGOUT)
		ret += "n";
	if (_flags & PRIVATE)
		ret += "p";
	if (_flags & SECRET)
		ret += "s";
	if (_flags & TOPICSET)
		ret += "t";
	return ret;
}

void Channel::displayChanInfo(const User &user) {
	std::string chanName = "";
	std::string info = "";
	UserInfo userinfo = user.getInfo();
	if ((_flags & SECRET) && !containsNickname(userinfo.nickname))
		return;
	if ((_flags & PRIVATE) && !containsNickname(userinfo.nickname))
		chanName = "*";
	else {
		chanName = _name;
		info = "[+" + getFlagsAsString() + "] " + _topic;
	}
	utils::sendReply(user.getFd(), userinfo.servername, userinfo, RPL_LIST\
, chanName, std::to_string(_users.size()), info);
}

void Channel::removeInvited(const User &user) {
	if (isInvited(user)) {
		size_t i;
		for (i = 0; i < _invitedUsers.size(); i++)
			if (_invitedUsers[i] == &user)
				break;
		_invitedUsers.erase(_invitedUsers.begin() + i);
	}
}

bool Channel::isInvited(const User &user) const {

	for (size_t i = 0; i < _invitedUsers.size(); i++)
	{
		if (_invitedUsers[i]->getPrefix() == user.getPrefix())
			return true;
	}
	return false;
}

void Channel::disconnect(const User &user) {
	std::vector<const User *>::iterator it = _users.begin();
	std::vector<const User *>::iterator end = _users.end();
	while (it != end) {
 		if (*it == &user)
		{
			_users.erase(it);
			removeOperator(user);
			removeSpeaker(user);
		}
		++it;
	}
}

void Channel::displayNames(const User &user) {
	std::string names;
	std::vector<const User *>::const_iterator it = _users.begin();
	std::vector<const User *>::const_iterator end = _users.end();
	while (it != end) {
		const UserInfo userinfo = (*it)->getInfo();
		if (isOperator(**it))
			names += "@";
		else if (isSpeaker(**it))
			names += "+";
		names += userinfo.nickname;
		++it;
		if (it != end)
			names += " ";
	}
	utils::sendReply(user.getFd(), user.getInfo().servername, user.getInfo(),
					 RPL_NAMREPLY, "= " + _name, names);
}

void Channel::displayTopic(const User &user) {
	UserInfo userinfo = user.getInfo();
	if (_topic.size() > 0)
		utils::sendReply(user.getFd(), userinfo.servername, userinfo, RPL_TOPIC,
						 _name, _topic);
	else
		utils::sendReply(user.getFd(), userinfo.servername, userinfo,
						 RPL_NOTOPIC, _name);
}

void Channel::removeBanMask(const std::string &mask) {
	size_t i = 0;
	while (i < _banMasks.size()) {
		if (_banMasks[i] == mask)
			break;
		i++;
	}
	_banMasks.erase(_banMasks.begin() + i);
}

void Channel::addBanMask(const std::string &mask) {
	_banMasks.push_back(mask);
}

void Channel::removeSpeaker(const User &user) {
	if (isSpeaker(user)) {
		size_t i = 0;
		while (i < _speakers.size()) {
			if (_speakers[i] == &user)
				break;
			i++;
		}
		_speakers.erase(_speakers.begin() + i);
	}
}

void Channel::addSpeaker(const User &user) {
	if (!isSpeaker(user))
		_speakers.push_back(&user);
}

void Channel::removeOperator(const User &user) {
	if (isOperator(user)) {
		size_t i;
		for (i = 0; i < _operators.size(); i++)
			if (_operators[i] == &user)
				break;
		_operators.erase(_operators.begin() + i);
		if (_operators.size() == 0 && _users.size() > 0) {
			_operators.push_back(_users[0]);
			sendMessage("MODE " + this->_name + " +o " +
						_users[0]->getInfo().nickname + "\n", user, true);
		}
	}
}

void Channel::addOperator(const User &user) {
	if (!isOperator(user))
		_operators.push_back(&user);
}

void Channel::invite(const User &user, const User &receiver) {
	if (_flags & INVITEONLY && !isOperator(user))
		utils::_errorSend(user, ERR_CHANOPRIVSNEEDED, _name);
	else {
		_invitedUsers.push_back(&receiver);
		UserInfo receiverInfo = receiver.getInfo();
		UserInfo userinfo = user.getInfo();
		receiver.sendMessage(
				":" + user.getPrefix() + " INVITE " + receiverInfo.nickname +
				" :" + _name + "\n");
		utils::sendReply(user.getFd(), userinfo.servername, userinfo,
						 RPL_INVITING, _name, receiverInfo.nickname);
		if (receiverInfo.flags & AWAY)
			utils::sendReply(user.getFd(), userinfo.servername, userinfo,
							 RPL_AWAY, receiverInfo.nickname,
							 receiverInfo.awayMessage);
	}
}

void Channel::removeFlag(unsigned char flag) {
	_flags &= ~flag;
}

void Channel::setFlag(unsigned char flag) {
	_flags |= flag;
}

void Channel::connect(const User &user, const std::string &key) {
	if ((_flags & PRIVATE) && key != _pass)
		utils::_errorSend(user, ERR_BADCHANNELKEY, _name);
	else if (_userLimit != 0 && _users.size() >= _userLimit)
		utils::_errorSend(user, ERR_CHANNELISFULL, _name);
	else if ((_flags & INVITEONLY) && !isInvited(user))
		utils::_errorSend(user, ERR_INVITEONLYCHAN, _name);
	else {

		for (size_t i = 0; i < _banMasks.size(); i++) {
			if (isBanned(_banMasks[i], user.getPrefix())) {
				utils::_errorSend(user, ERR_BANNEDFROMCHAN, _name);
				return;
			}
		}
		std::vector<const User *>::iterator it = _users.begin();
		std::vector<const User *>::iterator end = _users.end();
		while (it != end) {
			if ((*it)->getPrefix() == user.getPrefix())
				return;
			it++;
		}
		_users.push_back(&user);
		removeInvited(user);
		sendInfo(user);
	}
}

bool Channel::isEmpty() const {
	if (_users.size() == 0)
		return true;
	return false;
}

void Channel::setKey(const User &user, const std::string &key) {
	if (_pass.size() > 0 && key.size() > 0)
		utils::_errorSend(user, ERR_KEYSET, _name);
	else
		this->_pass = key;
}

void Channel::setLimit(unsigned short limit) {
	_userLimit = limit;
}

void Channel::setTopic(const User &user, const std::string &topic) {
	if ((_flags & TOPICSET) && !isOperator(user))
		utils::_errorSend(user, ERR_CHANOPRIVSNEEDED, _name);
	else {
		this->_topic = topic;
		sendMessage("TOPIC " + _name + " :" + this->_topic + "\n", user, true);
	}
}

bool Channel::isBanned(const std::string &mask, const std::string &prefix) {
	return (utils::isEqualToRegex(mask, prefix));
}

void Channel::sendInfo(const User &user) {
	sendMessage("JOIN :" + _name + "\n", user, true);
	displayTopic(user);
	displayNames(user);
	utils::sendReply(user.getFd(), user.getInfo().servername, user.getInfo(),
					 RPL_ENDOFNAMES, _name);
}

Channel::Channel(const std::string &name, const User &creator,
				 const std::string &pass) :
		_name(name), _pass(pass), _userLimit(0), _flags(NOMSGOUT) {

	_users.push_back(&creator);
	_operators.push_back(&creator);
	sendInfo(creator);
}

Channel &Channel::copy(const Channel &other) {
	_users.clear();
	_operators.clear();
	_banMasks.clear();
	_invitedUsers.clear();
	_speakers.clear();

	_name = other._name;
	_users.insert(_users.begin(), other._users.begin(), other._users.end());
	_flags = other._flags;
	_operators.insert(_operators.begin(), other._operators.begin(),
					  other._operators.end());
	_banMasks.insert(_banMasks.begin(), other._banMasks.begin(),
					 other._banMasks.end());
	_invitedUsers.insert(_invitedUsers.begin(), other._invitedUsers.begin(),
						 other._invitedUsers.end());
	_speakers.insert(_speakers.begin(), other._speakers.begin(),
					 other._speakers.end());
	_topic = other._topic;
	_pass = other._pass;
	_userLimit = other._userLimit;
	return *this;
}

std::ostream &operator<<(std::ostream &out, Channel const &source) {
	out << "Channel name:  " << source.getName() << " topic: "
		<< source.getTopic() << " flag: " << source.getFlags();
	return out;
}