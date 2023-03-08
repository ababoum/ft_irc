/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bregneau <bregneau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/01 18:23:07 by bregneau          #+#    #+#             */
/*   Updated: 2023/03/07 13:08:10 by bregneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"

static void	ft_exit(int sig_code)
{
	throw sig_code;
}

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cout << "Bad number of arguments :" << std::endl;
		std::cout << argv[0] << " <port> <password>" << std::endl;
		return (1);
	}
	signal(SIGINT, ft_exit);
	try
	{
		int port = ft_parse_port(argv[1]);
		ft_check_password(argv[2]);
		Server test_server(port, argv[2]);
	}
	catch(const std::exception& e)
	{
		std::cerr << "Error\n" << e.what() << std::endl;
	}
	catch (int sig_code) {
		RUNTIME_MSG("\nQuitting ircserv... Thanks!\n");
		return (sig_code);
	}
	
	
}