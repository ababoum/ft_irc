#include "ft_irc.hpp"

static void ft_exit(int sig_code)
{
	throw sig_code;
}

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
		signal(SIGINT, ft_exit);

		int port = ft_parse_port(argv[1]);
		ft_check_password(argv[2]);
		Server main_server(port, argv[2]);
		main_server.routine();
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error\n"
				  << e.what() << std::endl;
	}
	catch (int sig_code)
	{
		RUNTIME_MSG("\nQuitting IRC server... Thanks!\n");
		return sig_code;
	}
}