/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_irc.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bregneau <bregneau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/01 18:27:56 by bregneau          #+#    #+#             */
/*   Updated: 2023/03/07 13:03:12 by bregneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_IRC_HPP
#define FT_IRC_HPP

#include <string>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <cstdio>
#include <arpa/inet.h>
#include <sys/select.h>
#include <vector>
#include <signal.h>
#include <poll.h>

#include "Server.hpp"
#include "Client.hpp"

#define MAX_CLIENTS 1000
#define READ_BUFFER 30000

# define RED_TXT "\e[31m"
# define GREEN_TXT "\e[32m"
# define YELLOW_TXT "\e[33m"
# define BLUE_TXT "\e[34m"
# define MAG_TXT "\e[35m"

# define BOLD_TXT "\e[1m"
# define ITALIC_TXT "\e[2m"
# define UL_TXT "\e[4m"

# define RESET_TXT "\e[0m"

# define DEBUG(x)		std::cerr << YELLOW_TXT << x << RESET_TXT;
# define FATAL_ERR(x)	std::cerr << RED_TXT << x << RESET_TXT;
# define INFO(x)		std::cerr << ITALIC_TXT << x << RESET_TXT;
# define RUNTIME_MSG(x)	std::cerr << GREEN_TXT << x << RESET_TXT;

bool	ft_is_number(const std::string &str);
int		ft_parse_port(const std::string &str);
void	ft_check_password(const std::string &str);


#endif /* FT_IRC_HPP */
