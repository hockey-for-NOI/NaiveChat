#include "Debug.h"
#include <iostream>

namespace	NaiveChat
{

void	debug(std::shared_ptr<SocketBase> soc)
{
	char	buf[SocketBase::MAX_SIZE];
	while (true)
	{
		auto	status = soc->recvdata(buf);
		if (status > 0) std::cout << buf << std::endl;
		else if (status < 0) break;
	}
}

}	// end namespace NaiveChat
