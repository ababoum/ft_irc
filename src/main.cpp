/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mriant <mriant@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/01 18:23:07 by bregneau          #+#    #+#             */
/*   Updated: 2023/03/06 16:02:16 by mriant           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cout << "Bad number of arguments :" << std::endl;
		std::cout << argv[0] << " <port> <password>" << std::endl;
		return (1);
	}

	Server test_server(8198, "mdp");
}