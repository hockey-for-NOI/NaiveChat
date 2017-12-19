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

}	// end namespace NaiveChat
