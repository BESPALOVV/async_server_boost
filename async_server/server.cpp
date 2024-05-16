#include"server.h"

using namespace boost::asio;



void server::start_accept()
{
	clients_.emplace_back(new client(context_, clients_));

	acceptor_.async_accept(clients_.back()->get_sock(),bind(&server::on_accept,this,_1));
	
}

void server::on_accept(const boost::system::error_code& ec)
{
	if (ec) { std::cout << ec.what() << std::endl; return; }

	clients_.back()->start();

	start_accept();

}
