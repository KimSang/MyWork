#include <boost/asio.hpp>
#include <iostream>

using namespace std;
using namespace boost;

int main(int argc, char * argv[])
{
	string host = "www.google.com";
	string port_num = "80";

	asio::io_service ios;
	boost::system::error_code ec;

	asio::ip::tcp::resolver::query resolver_query(host, port_num, asio::ip::tcp::resolver::query::numeric_service);

	asio::ip::tcp::resolver resolver(ios);

	asio::ip::tcp::resolver::iterator it = resolver.resolve(resolver_query, ec);

	if( ec!= 0)
	{
		cout << "Failed to resolve a DNS name. " << "Error Code : " << ec.value() << "Message : " << ec.message() << endl;

		return ec.value();
	}

	asio::ip::tcp::resolver::iterator it_end;

	for(; it != it_end; ++it)
	{
		asio::ip::tcp::endpoint ep = it->endpoint();
		cout<<ep.address()<<endl;
	}

	return 0;
}
