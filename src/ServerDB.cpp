#include "ServerDB.h"

namespace	NaiveChat
{

std::string	User::salted(std::string const& raw)
{
	std::string encrypted = "#";
	int	base = 9029;
	std::vector <int> tmp;
	for (char ch: raw) tmp.push_back(base = ((int(ch) + (base << 2)) ^ base));
	for (int _=0; _<3; _++) for (int &x: tmp) x = base = ((x + (base << 2)) ^ base);
	for (int &x: tmp) encrypted += std::to_string(x) + "#";
	return encrypted;
}

bool	User::login(std::string const& salted_passwd1)
{
	mtx.lock();
	bool flag = (salted_passwd2 == salted(salted_passwd1));
	mtx.unlock();
	return flag;
}

void	User::cpwd(std::string const& salted_passwd1)
{
	mtx.lock();
	salted_passwd2 = salted(salted_passwd1);
	mtx.unlock();
}

bool	User::isonline()
{
	mtx.lock();
	bool flag = status && status->isvalid();
	mtx.unlock();
	return flag;
}

void	User::setsoc(std::shared_ptr <ServerSocket> soc)
{
	mtx.lock();
	status = soc;
	mtx.unlock();
}

void	User::closeconn()
{
	mtx.lock();
	if (status) status->closeconn();
	status = nullptr;
	mtx.unlock();
}

bool	User::add(std::string const& name, std::shared_ptr<User> u)
{
	mtx.lock();
	bool	flag = true;
	if (friends.count(name)) flag = false;
	else friends[name] = u;
	mtx.unlock();
	return flag;
}

std::vector < std::pair <std::string, bool> >	User::listfriend()
{
	mtx.lock();
	std::vector < std::pair <std::string, std::shared_ptr<User> > > tmp(friends.begin(), friends.end());
	mtx.unlock();
	std::vector < std::pair <std::string, bool> > res;
	for (auto &i: tmp) res.emplace_back(i.first, i.second->isonline());
	return res;
}

ServerDB&	ServerDB::get_instance()
{
	if (!m_instance) m_instance = new ServerDB();
	return *m_instance;
}

ServerDB::ServerDB()
{
}

ServerDB*	ServerDB::m_instance = nullptr;

std::shared_ptr <User> ServerDB::regist(std::string const& name, std::string const& sp1)
{
	mtx.lock();

	if (m_users.count(name)) {mtx.unlock(); return nullptr;}
	auto t = m_users[name] = std::make_shared<User>(name, sp1);

	mtx.unlock();
	return t;
}

std::shared_ptr <User> ServerDB::login(std::string const& name, std::string const& sp1)
{
	mtx.lock();

	if (!m_users.count(name)) {mtx.unlock(); return nullptr;}
	if (!m_users[name]->login(sp1)) {mtx.unlock(); return nullptr;}

	auto	t = m_users[name];

	mtx.unlock();
	return t;
}

std::vector < std::pair <std::string, bool> >	ServerDB::listuser()
{
	mtx.lock();
	std::vector < std::pair <std::string, bool> > result;
	for (auto const& i: m_users) result.emplace_back(i.first, i.second->isonline());
	mtx.unlock();
	return result;
}

std::shared_ptr<User>	ServerDB::getuser(std::string name)
{
	mtx.lock();
	std::shared_ptr<User> res = nullptr;
	if (m_users.count(name)) res = m_users[name];
	mtx.unlock();
	return res;
}

}	// end namespace NaiveChat
