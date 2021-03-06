#include "SocketBase.h"

namespace	NaiveChat
{

SocketBase::SocketBase():
		m_sid(-1), m_addr{0}
{
	if ((m_sid = socket(AF_INET, SOCK_STREAM, 0)) == -1) throw 0;
	int tmp;
	if (setsockopt(m_sid, SOL_SOCKET, SO_REUSEADDR, (char*)&tmp, 4) == -1) throw 0;
	timeval timeout{86400,0};
	if (setsockopt(m_sid, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeval)) == -1) throw 0;	
	if (setsockopt(m_sid, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeval)) == -1) throw 0;	
	tmp = 16777216;
	if (setsockopt(m_sid, SOL_SOCKET, SO_SNDBUF, (char*)&tmp, sizeof(int)) == -1) throw 0;	
	if (setsockopt(m_sid, SOL_SOCKET, SO_RCVBUF, (char*)&tmp, sizeof(int)) == -1) throw 0;	
}

SocketBase::~SocketBase()
{
	if (m_sid != -1) close(m_sid);
}

void	SocketBase::closeconn()
{
	if (m_sid != -1)
	{
		close(m_sid);
		m_sid = -1;
	}
}

bool	SocketBase::isvalid() const
{
	return m_sid != -1;
}

bool	SocketBase::senddata(void const* ptr, size_t size)
{
	mtx_w.lock();
	if (size > MAX_SIZE) return -1;
	bool ret = (send(m_sid, ptr, size, MSG_NOSIGNAL) != -1);
	mtx_w.unlock();
	return ret;
}

int	SocketBase::recvdata(void* ptr)
{
	int ret = recv(m_sid, ptr, MAX_SIZE, 0);
}

}	// end namespace NaiveChat
