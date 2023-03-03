/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bregneau <bregneau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/01 19:16:44 by bregneau          #+#    #+#             */
/*   Updated: 2023/03/01 19:39:45 by bregneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"


Client::Client(int fd)
: _fd(fd)
{}

Client::Client(const Client &other)
: _fd(other.getFd())
{
	*this = other;
}

Client::~Client()
{
	
}

Client &	Client::operator=(const Client &rhs)
{
	return *this
}

int	Client::getFd() const
{
	return _fd;
}