#pragma once

#include "SocketBase.h"

namespace	NaiveChat
{

class	ServerSocket: public SocketBase
{
public:
	static	const	int	MAX_USER = 16;

	ServerSocket() = default;
	~ServerSocket() = default;

	bool	bindport(int port);
	bool	listento() const;
	bool	acceptconn(ServerSocket&) const;
};

}	// end namespace NaiveChat
