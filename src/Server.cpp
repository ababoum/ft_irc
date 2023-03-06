/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mriant <mriant@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/01 19:17:37 by bregneau          #+#    #+#             */
/*   Updated: 2023/03/06 16:02:43 by mriant           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server()
	: _socket_fd(0), _port(0), _password("")
{
}

Server::Server(int port, std::string password)
	: _socket_fd(-1), _port(port), _password(password)
{
	// Check args
	launch();
}

Server::Server(const Server &other)
	: _socket_fd(other.getSocketFd()), _port(other.getPort()), _password(other.getPassword())
{
	*this = other;
}

Server::~Server()
{
	if (_socket_fd >= 0)
		close(_socket_fd);
}

Server &Server::operator=(const Server &rhs)
{
	_socket_fd = rhs.getSocketFd();
	_port = rhs.getPort();
	_password = rhs.getPassword();
	return *this;
}

int Server::getSocketFd() const
{
	return _socket_fd;
}

int Server::getPort() const
{
	return _port;
}

std::string Server::getPassword() const
{
	return _password;
}

void Server::launch()
{
	struct sockaddr_in tmp;

	_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket_fd < 0)
	{
		perror("cannot create socket");
		return;
	}
	std::cout << "Socket created" << std::endl;
	tmp.sin_family = AF_INET;
	tmp.sin_port = htons(_port);
	tmp.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(_socket_fd, (struct sockaddr *)&tmp, sizeof(tmp)) < 0)
	{
		perror("bind failed");
		return ;
	}
	std::cout << "Socket binded" << std::endl;
}