/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bregneau <bregneau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/01 19:17:37 by bregneau          #+#    #+#             */
/*   Updated: 2023/03/12 16:12:39 by bregneau         ###   ########.fr       */
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
    INFO("Socket created\n");
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
    INFO("Binding socket...\n");
    if (listen(_socket_fd, MAX_CLIENTS) < 0)
    {
        perror("In listen");
        return;
    }
    INFO("Listening on socket...\n");
    while (1)
    {
        DEBUG("Waiting for new connection\n");
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);
        FD_SET(_socket_fd, &readfds);

        // add all clients to the set of fd to read/write
        for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
        {
            FD_SET(it->getFd(), &readfds);
            FD_SET(it->getFd(), &writefds);
            DEBUG("Client fd " << it->getFd() << " added to select\n");
        }
        int ret = select(FD_SETSIZE, &readfds, 0, NULL, NULL);

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
                DEBUG("New connection\n");
                int _client_fd = accept(_socket_fd, (struct sockaddr *)&tmp, &addr_len);
                if (_client_fd < 0)
                {
                    perror("In accept");
                    return;
                }
                _clients.push_back(Client(_client_fd));
                continue;
            }
            for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
            {
                if (FD_ISSET(it->getFd(), &readfds))
                {
                    DEBUG("New message\n");
                    char buffer[READ_SIZE + 1] = {0};
                    int read_val = read(it->getFd(), buffer, READ_SIZE);
                    if (read_val <= 0)
                    {
                        close(it->getFd());
                        _clients.erase(it);
                    }
                    else
                    {
                        it->append_message_buffer(buffer);
                    }
                    DEBUG(buffer);
                    break;
                }
            }
        }
    }
}


void Server::execute_command(Client &client)
{
	std::string command_name[] = {"NICK", "USER", "JOIN"};
	void (Server::*f[])(Client &client, const std::vector<std::string>& args) = {&Server::nick, &Server::user, &Server::join};

	// std::string command = _message_buffer.substr(0, _message_buffer.find(" "));

	std::vector<std::string> command = split(client.getMessageBuffer(), ' ');

	for (int i = 0; i < 3; i++)
	{
		if (command[0] == command_name[i])
			(this->*f[i])(client, command);
	}
	
	client.clear_message_buffer();
}

void Server::nick(Client &client, const std::vector<std::string>& args)
{
	std::cout << "nick function called" << std::endl;

	(void)client;
	(void)args;
	//tester si le nick name n'est pas deja pris
	// _nick_name = args[1];
}


void Server::user(Client &client, const std::vector<std::string>& args)
{
	std::cout << "user function called" << std::endl;
	(void)client;
	(void)args;
}

void Server::join(Client &client, const std::vector<std::string>& args)
{
	std::cout << "join function called" << std::endl;
	(void)client;
	(void)args;
}
