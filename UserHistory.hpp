#ifndef USERHISTORY_HPP
# define USERHISTORY_HPP
# include "ircserv.hpp"
# include "User.hpp"

struct UserInfo;
class User;

class UserHistory
{
	private:
		UserHistory(const UserHistory &copy);
		UserHistory &operator=(const UserHistory &copy);

		std::vector< UserInfo>				_historyList;

	public:
		UserHistory();
		~UserHistory();

		void	addHistoryByUser(const UserInfo &info);
		std::vector< const UserInfo *>
		getHistoryByUser(const std::string &nickname) const;
};

#endif
