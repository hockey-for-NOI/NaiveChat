#include "ClientSocket.h"

namespace	NaiveChat
{

bool	ClientSocket::conn(char const* host, int port)
{
	if (m_sid == -1) return 0;
	m_addr.sin_family = AF_INET;
	m_addr.sin_port = htons(port);
	if (inet_pton(AF_INET, host, &m_addr.sin_addr) == -1) return 0;
	return connect(m_sid, (sockaddr*)&m_addr, sizeof(m_addr)) != -1;
}

}	// end namespace NaiveChat
