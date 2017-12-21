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
	static	const	int	OP_SEARCH = 6;
	static	const	int	OP_SEARCHRES = 7;
	static	const	int	OP_ADD = 8;

	union
	{
		char	pad[SocketBase::MAX_SIZE-1];
		char	cpwd[300];
		struct
		{
			char	name[25];
			char	passwd[25];
		}	namesp1;
		char	reply;
		struct
		{
			char	name[20][25];
			char	len, hasnext;
		}	searchres;
		struct
		{
			char	name[25];
		}	addfriend;
	};
};

}	// end namespace NaiveChat
