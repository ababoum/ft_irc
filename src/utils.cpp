/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bregneau <bregneau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/06 18:44:06 by bregneau          #+#    #+#             */
/*   Updated: 2023/03/07 13:02:31 by bregneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_irc.hpp"

bool ft_is_number(const std::string &str)
{
	return !str.empty() &&
		   (str.find_first_not_of("0123456789") == std::string::npos);
}

int ft_parse_port(const std::string &str)
{
	int	port;

	if (!ft_is_number(str))
		throw std::invalid_argument("Port number is not valid");
	port = std::atoi(str.c_str());
	if (port > 65535)
		throw std::invalid_argument("Bad/illegal port format (max value: 65535)");
	return (port);
}

void	ft_check_password(const std::string &str)
{
	if (str.empty())
		throw std::invalid_argument("Empty password");
	for (int i = 0; str[i]; i++)
	{
		if (!std::isprint(str[i]))
			throw std::invalid_argument("Only printables characters are allowed");
	}
}
