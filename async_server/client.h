#pragma once
#ifndef ASYNC_SERVER_H
#define ASYNC_SERVER_H

#include"boost/asio.hpp"
#include"boost/bind.hpp"
#include"boost/enable_shared_from_this.hpp"
#include"boost/noncopyable.hpp"
#include<iostream>

using namespace boost::asio;

class client :public boost::enable_shared_from_this<client>, boost::noncopyable
{
public:

		client(io_context& context, std::vector<boost::shared_ptr<client>>& clients) :sock_(context), clients_(clients),timer_(context){}

	    static void accept_clients(io_context& context,std::vector<boost::shared_ptr<client>>& clients, const boost::system::error_code& ec);

		void start();

		void stop();

		ip::tcp::socket& get_sock();
		
		
private:

	void on_connect(io_context& context, std::vector<boost::shared_ptr<client>>& clients, const boost::system::error_code& ec);

	void do_read();

	void do_write(const std::string& msg);

	void on_read(const boost::system::error_code& ec, size_t bytes);

	void on_write(const boost::system::error_code& ec,size_t bytes);

	void on_ping();

	void on_clients();

	void on_login(std::string& msg);

	void on_check_ping();

	void post_check_ping();


	
	ip::tcp::socket sock_;

	std::vector<boost::shared_ptr<client>> clients_;
	
	std::string username;

	bool started_ = false;

	bool clients_list_changed;

	streambuf read_buf;

	char write_buf[1024];

	deadline_timer timer_;

	boost::posix_time::ptime last_ping;


		
};


#endif // !ASYNC_SERVER_H

