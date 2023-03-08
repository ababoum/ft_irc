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
    // fd_set all_fds;
    // fd_set readfds;
    // fd_set writefds;
    int opt = 1;
    std::vector<struct pollfd> fds;
    
	_socket_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
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

    // FD_ZERO(&all_fds);
    // FD_SET(_socket_fd, &all_fds);
    struct pollfd tmp_fd = {.fd = _socket_fd, .events = POLLIN};
    fds.push_back(tmp_fd);

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
        // readfds = all_fds;
        // writefds = all_fds;

        // int ret = select(FD_SETSIZE, &readfds, &writefds, NULL, NULL);
        nfds_t nfds = fds.size();
        int ret = poll(fds.data(), nfds, -1);
        if (ret < 0)
        {
            // perror("In select");
            perror("In poll");
            return;
        }
        else if (ret == 0)
        {
            std::cout << "Timeout" << std::endl;
            return;
        }
        else
        {
            // if (FD_ISSET(_socket_fd, &readfds))
            if (fds[0].revents & POLLIN)
            {
                std::cout << "New connection" << std::endl;
                struct sockaddr_in clientaddr;
                socklen_t len = sizeof(clientaddr);

                int _client_fd = accept(_socket_fd, (struct sockaddr *)&clientaddr, &len);
                if (_client_fd < 0)
                {
                	perror("In accept");
                	return ;
                }
                std::cout << "New client connected: " << _client_fd << std::endl;
                _clients.push_back(Client(_client_fd));
                // FD_SET(_client_fd, &all_fds);
                // FD_SET(_client_fd, &readfds);
                struct pollfd tmp = {.fd = _client_fd, .events = POLLIN};
                fds.push_back(tmp);
                continue;
            }
            // for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
            for (std::vector<struct pollfd>::iterator it = fds.begin() + 1; it != fds.end(); ++it)
            {
                if (it->fd == 3)
                    continue;
                if (it->revents & POLLIN)
                {
                    std::cout << "New message" << std::endl;
                    char buffer[READ_BUFFER + 1] = {0};
                    std::cout << "Reading from " << it->fd << std::endl;
                    if (read(it->fd, buffer, READ_BUFFER) <= 0)
                    {
                        perror("In read");
                        // FD_CLR(it->getFd(), &all_fds);
                        Client::clearFd(it->fd, _clients);
                        fds.erase(it);
                        break;
                    }
                    std::cout << buffer << std::endl;
                    break;
                }
                else if (it->revents == POLLERR || it->revents == POLLNVAL)
                {
                    std::cout << "Client disconnected" << std::endl;
                    // FD_CLR(it->getFd(), &all_fds);
                    Client::clearFd(it->fd, _clients);
                    fds.erase(it);
                    break;
                }
            }
        }
	}
}