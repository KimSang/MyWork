#include <boost/asio.hpp>
#include <iostream>

using namespace boost;
using namespace std;

int main(int argc, char * argv[])
{
	string str_ip = "127.0.0.1";
	unsigned short port = 5555;

	try
	{
		asio::ip::tcp::endpoint ep(asio::ip::address::from_string(str_ip), port);
		asio::io_service ios;

		asio::ip::tcp::socket sock(ios, ep.protocol());

		sock.connect(ep);

	}
	catch(system::system_error & e)
	{
		cout<<"Error Code : " << e.code() << "Message : " << e.what() << endl;
		return e.code().value();
	}

	return 0;
}
