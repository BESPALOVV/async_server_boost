#pragma once
#ifndef SERVER_H
#define SERVER_H
#include "client.h"

using namespace boost::asio;

class server
{
public:

	server(io_context& context, ip::tcp::endpoint& ep) :context_(context),acceptor_(context,ep)
	{
		
	}
	
	

	void start_accept();

	


private:

	void on_accept(const boost::system::error_code& ec);

	io_context& context_;

	ip::tcp::acceptor acceptor_;

	std::vector<boost::shared_ptr<client>> clients_;

	



};

#endif // !SERVER_H
