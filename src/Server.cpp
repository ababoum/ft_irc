/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mriant <mriant@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/01 19:17:37 by bregneau          #+#    #+#             */
/*   Updated: 2023/03/17 14:44:05 by mriant           ###   ########.fr       */
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
                        it->appendMessageReceived(buffer);
                    }
                    INFO(buffer);
                    break;
                }
            }
			for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
			{
				if (FD_ISSET(it->getFd(), &writefds))
				{
					DEBUG("Ready to write\n");
					if (*(it->getMessageReceived().end() - 1) == '\n')
					{
						parseCommands(*it);
						write(it->getFd(), it->getMessageToSend().c_str(), it->getMessageToSend().size());
						DEBUG("Message sent: " << it->getMessageToSend());
						it->clearMessageToSend();
					}
				}
			}
        }
    }
}


void Server::parseCommands(Client &client)
{
	std::string command_name[] = {"NICK", "USER", "JOIN", "PING"};
	void (Server::*f[])(Client &client, const std::vector<std::string>& args) = {&Server::nick, &Server::user, &Server::join, &Server::ping};

	std::vector<std::string> lines = split(client.getMessageReceived(), "\r\n");
	for (std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); ++it)
	{
		// std::cout << "line: " << *it << std::endl;
		std::vector<std::string> command = split(*it, ' ');

		if (command[0][0] == '@')
			command.erase(command.begin());
		if (command[0][0] == ':')
			command.erase(command.begin());
		for (int i = 0; i < 4; i++)
		{
			if (command[0] == command_name[i])
				(this->*f[i])(client, command);
		}
	}

	
	client.clearMessageReceived();
}

void Server::nick(Client &client, const std::vector<std::string>& args)
{
	std::string old_nick(client.getNickname());

	std::cout << "nick function called" << std::endl;
	//tester si le nick name n'est pas deja pris
	if (args.size() < 2 )
	{
		//431 ERR_NONICKNAMEGIVEN
		return;
	}
	if (args[1].size() > 9 || args[1].size() < 2)
	{
		//432 ERR_ERRONEUSNICKNAME
		return;
	}
	for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->getNickname() == args[1])
		{
			//433 ERR_NICKNAMEINUSE;
			client.appendMessageToSend(":ircserv 433 " + client.getNickname() + " " + args[1] + " :Nickname is already in use.\n");
			return;
		}
	}
	client.setNickname(args[1]);
	if (client.isAuthentified())
		client.appendMessageToSend(":" + old_nick + " NICK :" + client.getNickname() + "\n");
	else if (client.getUsername() != "")
	{
		client.setAuthentified();
		// 001 RPL_WELCOME
		client.appendMessageToSend(":ircserv 001 " + client.getNickname() + " :Welcome to ircserv " + client.getNickname() + "!\r\n");
	}

}

void Server::user(Client &client, const std::vector<std::string>& args)
{
	std::cout << "user function called" << std::endl;
	if (args.size() < 5)
		return;
	client.setUsername(args[1]);
	client.setHostname(args[2]);
	client.setServername(args[3]);
	client.setRealname(args[4]);
	

	if (!client.isAuthentified() && client.getNickname() != "")
	{
		client.setAuthentified();
		// 001 RPL_WELCOME
		client.appendMessageToSend(":ircserv 001 " + client.getNickname() + " :Welcome to ircserv " + client.getNickname() + "!\n");
		return ;
	}
}

void Server::join(Client &client, const std::vector<std::string>& args)
{
	std::cout << "join function called" << std::endl;
	if (args.size() == 1)
	{
		//461 ERR_NEEDMOREPARAMS
	}
	if (args[1] == "0")
	{
		// Send PART message to all chans the client is connected to
		return ;
	}
	std::vector<std::string> channels = split(args[1], ",");
	for (size_t i = 0; i < channels.size(); i++)
	{
		// Check name >> 476 ERR_BADCHANMASK + return
		// Check if Chan exists
		// If not create chan
		// else Add client to chan and chan to client
		// JOIN Message
		client.appendMessageToSend(":" + client.getNickname() + " JOIN " + args[1] + "\n");
		// 332 RPL_TOPIC
		// 333 RPL_TOPICWHOTIME
		// Un 353 RPL_NAMREPLY par users sur le channel
		// 366 RPL_ENDOFNAMES
	}
}

void Server::ping(Client &client, const std::vector<std::string>& args)
{
	std::cout << "ping function called" << std::endl;
	client.appendMessageToSend(":ircserv PONG " + client.getNickname() + " " + args[1] + "\n");
}