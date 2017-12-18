#pragma once

#include "SocketBase.h"

namespace	NaiveChat
{

class	ClientSocket: public SocketBase
{
public:
	ClientSocket() = default;
	~ClientSocket() = default;

	bool	conn(char const* host, int port);
};

}	// end namespace NaiveChat
