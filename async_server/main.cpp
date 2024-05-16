#include"client.h"
#include "server.h"


using namespace boost::asio;

int main()
{
	io_context context;

	ip::tcp::endpoint ep(ip::tcp::v4(), 1313);

	server MainServer(context,ep);

	MainServer.start_accept();

	context.run();

}
