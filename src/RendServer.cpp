//============================================================================
// Name        : RendServer.cpp
// Author      : Gabriel de Forest
// Version     :
// Copyright   : No copyright
// Description : Rend Server main file
//============================================================================

#include "include/TcpServer.h"
#include "include/UdpServer.h"

int main() {
    boost::asio::io_service ioService;
    TcpServer server1(ioService);
    UdpServer server(ioService);
    ioService.run();
	return 0;
}
