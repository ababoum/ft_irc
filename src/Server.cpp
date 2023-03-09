/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mriant <mriant@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/01 19:17:37 by bregneau          #+#    #+#             */
/*   Updated: 2023/03/06 17:02:41 by mriant           ###   ########.fr       */
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
	socklen_t addr_len;
    fd_set readfds;
    fd_set writefds;
    int opt = 1;
    
	_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket_fd < 0)
	{
		perror("cannot create socket");
		return;
	}
	std::cout << "Socket created" << std::endl;
    if (setsockopt(_socket_fd, SOL_SOCKET, SO_REUSEADDR, 
                    &opt, sizeof(opt))) 
     { 
        perror("setsockopt"); 
        return; 
     }
	tmp.sin_family = AF_INET;
	tmp.sin_port = htons(_port);
	tmp.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(_socket_fd, (struct sockaddr *)&tmp, sizeof(tmp)) < 0)
	{
		perror("bind failed");
		return;
	}
	std::cout << "Socket binded" << std::endl;
	if (listen(_socket_fd, MAX_CLIENTS) < 0)
	{
		perror("In listen");
		return;
	}
	std::cout << "Socket is listening" << std::endl;
	while (1)
	{
		std::cout << "Waiting for new connection" << std::endl;
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);
        FD_SET(_socket_fd, &readfds);

        // add all clients to the set of fd to read/write
        for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
        {
            FD_SET(it->getFd(), &readfds);
            FD_SET(it->getFd(), &writefds);
            std::cout << "Client fd added to select: " << it->getFd() << std::endl;
        }
        int ret = select(FD_SETSIZE, &readfds, &writefds, NULL, NULL);

        if (ret < 0)
        {
            perror("In select");
            return;
        }
        else if (ret == 0)
        {
            std::cout << "Timeout" << std::endl;
            continue;
        }
        else
        {
            if (FD_ISSET(_socket_fd, &readfds))
            {
                std::cout << "New connection" << std::endl;
                int _client_fd = accept(_socket_fd, (struct sockaddr *)&tmp, &addr_len);
                if (_client_fd < 0)
                {
                	perror("In accept");
                	return ;
                }
                _clients.push_back(Client(_client_fd));
                continue;
            }
            for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
            {
                if (FD_ISSET(it->getFd(), &readfds))
                {
                    std::cout << "New message" << std::endl;
                    char buffer[30000] = {0};
                    if (read(it->getFd(), buffer, MAX_CLIENTS) < 0)
                    {
                        close(it->getFd());
                    }
                    std::cout << buffer << std::endl;
                    continue;
                }
            }
        }
	}
}