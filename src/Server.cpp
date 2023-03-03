/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bregneau <bregneau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/01 19:17:37 by bregneau          #+#    #+#             */
/*   Updated: 2023/03/01 19:18:09 by bregneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server()
    : _socket_fd(0), _port(0), _password("")
{
}

Server::Server(int socket_fd, int port, std::string password)
    : _socket_fd(socket_fd), _port(port), _password(password)
{
}

Server::Server(const Server &other)
    : _socket_fd(other.getSocketFd()), _port(other.getPort()), _password(other.getPassword())
{
    *this = other;
}

Server::~Server()
{
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