#include "User.h"

#include <chrono>

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
	chat_target = nullptr;
	mtx.unlock();
}

std::string	User::getname()
{
	mtx.lock();
	auto ret = name;
	mtx.unlock();
	return ret;
}

void	User::chat(std::shared_ptr<User> u)
{
	auto tar = u ? u->getname() : "";
	mtx.lock();
	chat_target = u;
	if (u) std::thread([this, tar](){
		while (chat_target && chat_target->getname() == tar)
		{
			mtx.lock();
			std::list<Pack> tmp;
			tmp.swap(chatbuf[tar]);
			for (auto &i: tmp) status->sendobj(i); 
			chatseq.remove(tar);
			tmp.clear();
			mtx.unlock();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}).detach();
	mtx.unlock();
}

void	User::recvchatpack(std::string const& src, Pack const& p)
{
	mtx.lock();
	chatbuf[src].push_back(p);
	chatseq.push_back(src);
	mtx.unlock();
}

void	User::recvfileinfo(std::string const& src, Pack const& p)
{
	mtx.lock();
	filebuf[src].emplace_back(p, std::vector<Pack>());
	mtx.unlock();
}

void	User::recvfilepack(std::string const& src, Pack const& p)
{
	std::shared_ptr<Pack> o;
	mtx.lock();
	filebuf[src].back().second.push_back(p);
	if (!p.filedata.hasnext)
	{
		fileseq.push_back(src);
		o = std::make_shared<Pack>(filebuf[src].back().first);
	}
	mtx.unlock();
	if (o)
	{
		Pack oo = *o;
		oo.op = Pack::OP_FILEINFOC;
		recvchatpack(src, oo);
	}
}

bool	User::recvmsg(Pack &p)
{
	std::shared_ptr<std::string> tmp;
	mtx.lock();
	if (!chatseq.empty())
	{
		tmp = std::make_shared<std::string>(std::move(chatseq.front()));
		chatseq.pop_front();
	}
	if (tmp)
	{
		p = chatbuf[*tmp].front();
		chatbuf[*tmp].pop_front();
	};
	mtx.unlock();
	return (bool)tmp;
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

void	User::forward(Pack const& p)
{
	mtx.lock();
	auto tsrc = name;
	auto tmp = chat_target;
	mtx.unlock();
	chat_target->recvchatpack(tsrc, p);
}

void	User::forwardfileinfo(Pack const& p)
{
	mtx.lock();
	auto tsrc = name;
	auto tmp = chat_target;
	mtx.unlock();
	chat_target->recvfileinfo(tsrc, p);
}

void	User::forwardfilepack(Pack const& p)
{
	mtx.lock();
	auto tsrc = name;
	auto tmp = chat_target;
	mtx.unlock();
	chat_target->recvfilepack(tsrc, p);
}

std::pair<Pack, std::vector <Pack> > User::recvfile()
{
	std::pair<Pack, std::vector<Pack> > ret;
	std::string tar = "";
	mtx.lock();
	auto tmp = chat_target;
	mtx.unlock();
	if (tmp) tar = tmp->getname();
	else
	{
		mtx.lock();
		if (!fileseq.empty()) tar = fileseq.front();
		mtx.unlock();
	}
	mtx.lock();
	if (filebuf.count(tar) && !filebuf[tar].empty())
		ret = filebuf[tar].front();
	mtx.unlock();
	return ret;
}

void	User::recvfin()
{
	std::string tar = "";
	mtx.lock();
	auto tmp = chat_target;
	mtx.unlock();
	if (tmp) tar = tmp->getname();
	else
	{
		mtx.lock();
		if (!fileseq.empty()) tar = fileseq.front();
		mtx.unlock();
	}
	mtx.lock();
	if (filebuf.count(tar) && !filebuf[tar].empty())
		filebuf[tar].pop_front();
	for (auto itr1 = fileseq.begin(); itr1 != fileseq.end(); itr1++)
		if (*itr1 == tar) {fileseq.erase(itr1); break;}
	mtx.unlock();
}

}	// end namespace NaiveChat
