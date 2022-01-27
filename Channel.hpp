/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cshanda <cshanda@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/19 09:42:29 by cshanda           #+#    #+#             */
/*   Updated: 2022/01/19 09:42:29 by cshanda          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#pragma once
#include <ostream>
#include "User.hpp"
#include "utils.hpp"

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#define PRIVATE			0b000001
#define SECRET			0b000010
#define MODERATED		0b000100
#define INVITEONLY		0b001000
#define TOPICSET		0b010000
#define NOMSGOUT		0b100000

class Channel {
public:
	typedef  std::vector<const User *> type_user_array;
	Channel(const std::string &name, const User &creator,
			const std::string &pass = "");

	Channel(Channel const &other);

	Channel &operator=(const Channel &other);

	const std::string getName() const;

	std::string getTopic() const;

	unsigned char getFlags() const;

	void setTopic(const User &user, const std::string &topic);

	void setLimit(unsigned short limit);

	void setKey(const User &user, const std::string &key);

	bool isInvited(const User &user) const;

	bool isOperator(const User &User) const;

	bool isSpeaker(const User &Uuser) const;

	bool isEmpty() const;

	bool containsNickname(std::string nic) const;

	void sendMessage(const std::string &message, const User &from,
					 bool includeUser) const;

	void connect(const User &user, const std::string &key);

	void setFlag(unsigned char flag);

	void removeFlag(unsigned char flag);

	void invite(const User &user, const User &receiver);

	void addOperator(const User &user);

	void removeOperator(const User &user);

	void addSpeaker(const User &user);

	void removeSpeaker(const User &user);

	void addBanMask(const std::string &mask);

	void removeBanMask(const std::string &mask);

	void displayTopic(const User &user);

	void displayNames(const User &user)  ;

	void disconnect(const User &user);

	void removeInvited(const User &user);

	void displayChanInfo(const User &user);

	std::string getFlagsAsString() const;
	int sizeUsers() const;
	virtual ~Channel();
	Channel();
private:

	std::string _name;
	type_user_array _operators;
	type_user_array _speakers;
	std::string _pass;
	unsigned short _userLimit;
	std::vector<std::string> _banMasks;
	unsigned char _flags;
	type_user_array _users;
	std::string _topic;
	type_user_array _invitedUsers;

	bool isBanned(const std::string &mask, const std::string &prefix);

	void sendInfo(const User &user);

	Channel &copy(const Channel &other);
};

std::ostream &operator<<(std::ostream &out, Channel const &source);

#endif
