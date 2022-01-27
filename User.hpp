#ifndef USER_HPP
# define USER_HPP
#include "ircserv.hpp"
#include "UserHistory.hpp"

#define REGISTERED		0b00000001
#define INVISIBLE		0b00000010
#define RECEIVENOTICE	0b00000100
#define RECEIVEWALLOPS	0b00001000
#define IRCOPERATOR		0b00010000
#define AWAY			0b00100000
#define PINGING			0b01000000
#define BREAKCONNECTION	0b10000000
#ifdef __APPLE__
#define IRC_NOSIGNAL SO_NOSIGPIPE
#else
#define IRC_NOSIGNAL MSG_NOSIGNAL
#endif
class Channel;
typedef  std::vector<const Channel *> type_channel_arr;
struct UserInfo
{
	UserInfo();

	std::string							nickname;
	std::string							username;
	std::string							hostname;
	std::string							servername;
	std::string							realname;
	std::string 						awayMessage;
	std::string							quitMessage;
	unsigned char						flags;
	type_channel_arr					channels;
	time_t								registrationTime;
};

class User
{
private:

	int							_fd;
	UserInfo					_info;
//	std::vector<UserHistory>	_history;
	int							_send;
	std::vector<std::string>	_request;
	std::queue<std::string>		_raw_requests;

	bool						_authorized;
	std::string					_pwd;
	time_t						_last_activity;
    unsigned int        		_msg_delay;
	int							_queue;

//	std::string	_nick;
//	std::string	_host;
//	std::string	_name;
//	std::string	_server;
public:
	User();
	User(int sock_fd, std::string servername);
	User(User const &src);
	~User();
	User		&operator=(User const &rhs);

	/* geters */
	int getFd() const;
	const int &isSend();
	const std::vector<std::string> &getRequest();
	const bool &authorized();
	const UserInfo &getInfo() const;
	const std::string &getPwd();
	const time_t &getActivity() const;
	const unsigned char &getFlag() const;
	unsigned int &getDelay();
	std::string getNextRequest();
	const bool hasNoQueue() const;
	/* seters */
	void setIsSend(int f);
	void setPassword(std::string &pwd);
	void setInfo(std::string field, std::string &val);
	void setAuthorized(bool f);
	void	sendMessage(const std::string &msg) const;
	void setActivity(time_t time);
	void incrementDelay();
	void setRawRequests(std::queue<std::string> &val);
	/* other */
	void requestToVector(std::string request);
	void eraseRequest();
	void	setFlag(unsigned char flag);
	void	removeFlag(unsigned char flag);
	void	setAwayMessage(const std::string &msg);
	std::string getPrefix() const;
	void pingRequest(std::string cmd);
	void	addChannel(const Channel &channel);
	bool	isOnChannel(const std::string &name) const;
	void	removeChannel(const std::string &name);
	type_channel_arr &loadChannels();
};

#endif
