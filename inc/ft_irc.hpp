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

#include "Server.hpp"
#include "Client.hpp"

#define MAX_CLIENTS 1000

bool	ft_is_number(const std::string &str);
int		ft_parse_port(const std::string &str);
void	ft_check_password(const std::string &str);


#endif /* FT_IRC_HPP */
