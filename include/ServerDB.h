#pragma once

#include "User.h"


namespace	NaiveChat
{

class	ServerDB
{
public:
	static	ServerDB&	get_instance();
	std::shared_ptr<User>	regist(std::string const& name, std::string const& sp1);
	std::shared_ptr<User>	login(std::string const& name, std::string const& sp1);
	std::vector < std::pair <std::string, bool> >	listuser();
	std::shared_ptr<User>	getuser(std::string name);
private:
	ServerDB();
	static	ServerDB*	m_instance;
	std::mutex	mtx;
	std::unordered_map <std::string, std::shared_ptr<User>> m_users;
};

}	// end namespace NaiveChat
