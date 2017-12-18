#include "SocketBase.h"

namespace	NaiveChat
{

SocketBase::SocketBase():
		m_sid(-1), m_addr{0}
{}

SocketBase::~SocketBase()
{
	if (m_sid == -1) close(m_sid);
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
