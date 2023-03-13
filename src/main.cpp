/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bregneau <bregneau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/01 18:23:07 by bregneau          #+#    #+#             */
/*   Updated: 2023/03/13 13:55:06 by bregneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cout << "Bad number of arguments :" << std::endl;
		std::cout << argv[0] << " <port> <password>" << std::endl;
		return (1);
	}

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
	
	
}
