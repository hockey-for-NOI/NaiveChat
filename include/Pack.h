#pragma once
#include "SocketBase.h"

namespace	NaiveChat
{

struct	Pack
{
	char	op;

	static	const	int	OP_UPDATE = 0;
	static	const	int	OP_LOGOUT = 1;
	static	const	int	OP_CPWD = 2;
	static	const	int	OP_REGIST = 3;
	static	const	int	OP_LOGIN = 4;
	static	const	int	OP_REPLY = 5;

	union
	{
		char	pad[SocketBase::MAX_SIZE-1];
		char	cpwd[300];
		struct
		{
			char	name[25];
			char	passwd[25];
		}	namesp1;
		bool	reply;
	};
};

}	// end namespace NaiveChat
