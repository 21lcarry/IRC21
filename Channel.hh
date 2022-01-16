//
// Created by Администратор on 16.01.2022.
//

#ifndef IRC_CHANEL_HH
#define IRC_CHANEL_HH
#include <ostream>
#include "User.hpp"
#define PRIVATE		0b000001
#define SECRET		0b000010
#define MODERATED	0b000100
#define INVITEONLY	0b001000
#define TOPICSET	0b010000
#define NOMSGOUT	0b100000
// todo
class Channel {
public:
	std::vector<const User *>		users;
	Channel();

	Channel(Channel const &other);

	Channel &operator=(Channel  &other);
	 std::string	getName() const;
	 std::string	getTopic() const ;
	unsigned char	getFlags() const;
	bool containsNickname(std::string nic) const;
	bool isOperator(const UserInfo &infoUser) const;
	bool isSpeaker(const UserInfo &infUuser) const;
	void	sendMessage(const std::string &message, const User &from, bool includeUser) const;
	virtual ~Channel();
};

std::ostream &operator<<(std::ostream &out, Channel const &source);


#endif //IRC_CHANEL_HH
