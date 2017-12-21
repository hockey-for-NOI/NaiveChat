#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <mutex>

namespace	NaiveChat
{

class	SocketBase
{
public:
	static	const	int	MAX_SIZE = 1000;

	bool	senddata(void const* ptr, size_t size);
	int	recvdata(void* ptr);

	template<typename T>
	inline	bool	sendobj(T const& obj, size_t size = sizeof(T)) {senddata(&obj, size);}
	template<typename T>
	inline	size_t	recvobj(T& obj) {return recvdata(&obj);}

	void	closeconn();
	bool	isvalid() const;

protected:
	SocketBase();
	virtual	~SocketBase();
	int	m_sid;
	sockaddr_in	m_addr;
	std::mutex	mtx_w;
};

}	// end namespace NaiveChat
