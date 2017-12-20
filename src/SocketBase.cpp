#include "SocketBase.h"

namespace	NaiveChat
{

SocketBase::SocketBase():
		m_sid(-1), m_addr{0}
{
	if ((m_sid = socket(AF_INET, SOCK_STREAM, 0)) == -1) throw 0;
	int tmp;
	if (setsockopt(m_sid, SOL_SOCKET, SO_REUSEADDR, (char*)&tmp, 4) == -1) throw 0;
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

bool	SocketBase::senddata(void const* ptr, size_t size) const
{
	if (size > MAX_SIZE) return -1;
	return send(m_sid, ptr, size, MSG_NOSIGNAL) != -1;
}

int	SocketBase::recvdata(void* ptr) const
{
	return recv(m_sid, ptr, MAX_SIZE, 0);
}

}	// end namespace NaiveChat
