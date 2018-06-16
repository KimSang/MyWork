#include <boost/asio.hpp>
#include <iostream>

using namespace std;
using namespace boost;

int main(int argc, char * argv[])
{
	unsigned short port_num = 5555;
	const int BACKLOG_SIZE = 30;
	
	asio::ip::tcp::endpoint ep(asio::ip::address_v4::any(), port_num);

	asio::io_service ios;

	try
	{
		asio::ip::tcp::acceptor acceptor(ios, ep.protocol());
		acceptor.bind(ep);
		acceptor.listen(BACKLOG_SIZE);
		asio::ip::tcp::socket sock(ios);
		acceptor.accept(sock);

	}
	catch(system::system_error &e)
	{
		cout<< "Failed to bind the acceptor socket" << "Error Code : " << e.code() << "Message : " << e.what() << endl;

		return e.code().value();
	}

	return 0;
}
