#include"client.h"

 void client::accept_clients(io_context& context,std::vector<boost::shared_ptr<client>>& clients, const boost::system::error_code& ec)
{

	 if (ec) { std::cout << ec.what() << std::endl; return; }

	ip::tcp::acceptor acceptor(context, ip::tcp::endpoint(ip::tcp::v4(), 1313));

	boost::shared_ptr<client> new_(new client(context,clients));

	acceptor.async_accept(new_->sock_, boost::bind(&client::on_connect,new_->shared_from_this(), boost::ref(context), clients, _1));
	

}

 void client::on_connect(io_context& context, std::vector<boost::shared_ptr<client>>& clients, const boost::system::error_code& ec)
 {
	 ErrorHandle(ec);

	 clients.push_back(boost::shared_ptr<client>(this));

	 this->start();

	 accept_clients(context, clients, ec);
 }

 ip::tcp::socket& client::get_sock()
 {
	 return sock_;
 }


 void client::start()
 {
	 if (sock_.is_open())
	 {
		 started_ = true;

		 do_read();
	 }
	 else
	 {
		 std::cout << "Erorr, no connection" << std::endl;
	 }
 }

 void client::stop()
 {
	 sock_.close();

	 started_ = false;

	 boost::shared_ptr<client> this_cl = shared_from_this();

	 auto it = find(clients_.begin(), clients_.end(), this_cl);

	 for (auto client : clients_)
	 {
		 client->clients_list_changed = true;
	 }
	 
	 if (it != clients_.end())
	 {
		 clients_.erase(it);
	 }



 }

 void client::do_read()
 {
	 async_read_until(sock_, read_buf, '\n', bind(&client::on_read, shared_from_this(), _1, _2));

	 post_check_ping();
 }

 void client::on_read(const boost::system::error_code& ec, size_t bytes)
 {
	 ErrorHandle(ec);

	 std::istream in(&read_buf);

	 std::string msg;

	 std::getline(in, msg);

	 std::cout << "Message from client " << username << ":::" << msg << std::endl;

	 if (msg.find("ping") == 0) { on_ping(); }
	 
	 else if (msg.find("ask_clients") == 0) { on_clients(); }

	 else if (msg.find("login") == 0) { on_login(msg); }

 }

 void client::do_write(const std::string& msg)
 {
	 std::copy(msg.begin(), msg.end(), write_buf);

	 sock_.async_write_some(buffer(write_buf,msg.size()), bind(&client::on_write, shared_from_this(), _1, _2));
 }

 void client::on_write(const boost::system::error_code& ec, size_t bytes)
 {
	 ErrorHandle(ec);

	 do_read();
 }


 void client::on_ping()
 {
	 do_write(clients_list_changed ? "ping clients_list_changed\n" : "ping ok\n");

	 clients_list_changed = false;
 }

 void client::on_clients()
 {
	 std::string answer = "clients ";

	 for (auto client : clients_)
	 {
		 answer += client->username;
	 }
	 answer += '\n';

	 do_write(answer);

	 clients_list_changed = false;
 }

 void client::on_login(std::string &msg)
 {
	 std::istringstream in(msg);

	 in >> username >> username;

	 last_ping = boost::posix_time::microsec_clock::local_time();

	 do_write("login_ok\n");

	 for (auto client : clients_)
	 {
		 client->clients_list_changed = true;
	 }
 }

 void client::post_check_ping()
 {
	 timer_.expires_from_now(boost::posix_time::millisec(5000));

	 timer_.async_wait(boost::bind(&client::on_check_ping, shared_from_this()));
 }

 void client::on_check_ping()
 {
	 boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();

	 auto resolution = (now - last_ping).total_milliseconds();

	 std::cout<<"Time from last ping " << resolution << std::endl;

	 if ( resolution> 5000)
	 {
		 std::cout << "Client " + username << " was stopped" << std::endl;

		 stop();

	 }

	 last_ping = boost::posix_time::microsec_clock::local_time();
 }

 inline void client::ErrorHandle(const boost::system::error_code& ec)
 {
	 if (!ec)
	 {
		 return;
	 }
	 else if (ec.value() == 10054)
	 {
		 std::cout << "Connections reset by client "<<username << std::endl;

		 stop();
	 }
	 else
	 {
		 std::cout << "Unrnown error" << std::endl;

		 std::cout << ec.what() << std::endl;
	 }

 }



