#include "UserHistory.hpp"

std::vector<const UserInfo *>
UserHistory::getHistoryByUser(const std::string &nickname) const  {
	std::vector<const  UserInfo *> filteredHistory;
	for (size_t i = 0; i < _historyList.size(); ++i)
	{
		if (_historyList[i].nickname == nickname)
			filteredHistory.push_back(&(_historyList[i]));
	}
	return filteredHistory;
}

UserHistory::UserHistory() {

}

UserHistory::~UserHistory() {

}

void UserHistory::addHistoryByUser(const UserInfo &info) {
	_historyList.push_back(info);
}
