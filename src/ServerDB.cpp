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

std::vector <std::string>	ServerDB::listuser()
{
	mtx.lock();
	std::vector <std::string> result;
	for (auto const& i: m_users) result.push_back(i.first);
	mtx.unlock();
	return result;
}

}	// end namespace NaiveChat
