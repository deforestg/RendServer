/*
 * TcpServer.cpp
 *
 *  Created on: Aug 14, 2014
 *      Author: gdeforest
 */

#include "include/TcpServer.h"

TcpServer::TcpServer(boost::asio::io_service& ioService) {
	this->gm = &GameManager::GetInstance();
	acceptor = new tcp::acceptor(ioService, tcp::endpoint(tcp::v4(), 18206));

	startAccept();
}

void TcpServer::startAccept()
{
	TcpConnection::pointer newConnection = TcpConnection::create(acceptor->get_io_service());

	acceptor->async_accept(
		newConnection->getSocket(),
		boost::bind(
			&TcpServer::handleAccept,
			this,
			newConnection,
			boost::asio::placeholders::error
		)
	);
}

void TcpServer::handleAccept(TcpConnection::pointer newConnection, const boost::system::error_code& error)
{
	if (error) {
		startAccept();
	}

	boost::array<char, 1024> buffer;
	boost::system::error_code readError;

	size_t len = newConnection->getSocket().read_some(boost::asio::buffer(buffer), readError);

	if (readError == boost::asio::error::eof) {
		cout << "connection terminated" << endl;
		startAccept();
		return; // Connection closed cleanly by peer.
	} else if (error || len < 1) {
		//throw boost::system::system_error(error); // Some other error. TODO: logging
		startAccept();
	}

	string ip = newConnection->getSocket().remote_endpoint().address().to_string();
	int sendSize = 0;
	char* px;
	ServerMessage msg;

	switch (buffer[0]) {
		case JOIN:
			msg = gm->AcceptJoin(ip);
			break;
		case RESPAWN:
			msg = gm->Respawn(ip, buffer[1]);
			break;
		case LEAVE:
			msg = gm->Leave(ip, buffer[1]);
			break;
	}

	px = reinterpret_cast<char*>(&msg);
	sendSize = sizeof(msg);

	newConnection->Start(px, sendSize);

	startAccept();
}
