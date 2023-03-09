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
	// The client fd needs to be closed by hand when the client disconnects
}

Client &Client::operator=(const Client &rhs)
{
	_fd = rhs.getFd();
	return *this;
}

int Client::getFd() const
{
	return _fd;
}

void Client::append_message_buffer(const std::string &str)
{
	_message_buffer.append(str);
}

void Client::append_message_buffer(const char *str)
{
	_message_buffer.append(str);
}

void Client::clear_message_buffer()
{
	_message_buffer.clear();
}