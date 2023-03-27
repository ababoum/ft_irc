#include "ft_irc.hpp"

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cout << "Wrong number of arguments:" << std::endl;
		std::cout << argv[0] << " <port> <password>" << std::endl;
		return (1);
	}

	try
	{
		int port = ft_parse_port(argv[1]);
		ft_check_password(argv[2]);
		Server test_server(port, argv[2]);
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error\n"
				  << e.what() << std::endl;
	}
}