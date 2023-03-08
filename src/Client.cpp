/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bregneau <bregneau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/01 19:16:44 by bregneau          #+#    #+#             */
/*   Updated: 2023/03/03 15:05:48 by bregneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(int fd)
	: _fd(fd)
{
}

Client::Client(const Client &other)
	: _fd(other.getFd())
{
}

Client::~Client()
{
	close(_fd);
}

Client &Client::operator=(const Client &rhs)
{
	(void)rhs;
	return *this;
}

int Client::getFd() const
{
	return _fd;
}