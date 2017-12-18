#include "ServerSocket.h"

namespace	NaiveChat
{

bool	ServerSocket::bindport(int port)
{
	if (m_sid == -1) return 0;
	m_addr.sin_family = AF_INET;
	m_addr.sin_addr.s_addr = INADDR_ANY;
	m_addr.sin_port = htons(port);
	return bind(m_sid, (sockaddr*)&m_addr, sizeof(sockaddr)) != -1;
}

bool	ServerSocket::listento() const
{
	if (m_sid == -1) return 0;
	return listen(m_sid, MAX_USER) != -1;
}

bool	ServerSocket::acceptconn(ServerSocket& new_s) const
{
	int tmp = sizeof(sockaddr);
	return (new_s.m_sid = accept(m_sid, (sockaddr*)&m_addr, (socklen_t*)&tmp)) != -1;
}

}	// end namespace NaiveChat
