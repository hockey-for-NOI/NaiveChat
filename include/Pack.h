#pragma once

namespace	NaiveChat
{

struct	Pack
{
	char	op;

	static	const	int	OP_UPDATE = 0;
	static	const	int	OP_LOGOUT = 1;
	static	const	int	OP_CPWD = 2;

	union
	{
		char	pad[ServerSocket::MAX_SIZE-1];
		char	cpwd[300];
	};
};

}	// end namespace NaiveChat
