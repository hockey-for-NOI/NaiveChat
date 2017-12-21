#include "ServerDB.h"

namespace	NaiveChat
{


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
