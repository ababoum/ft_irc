#include "Server.hpp"

Server::Server()
	: _socket_fd(0), _port(0), _password("")
{
}

Server::Server(int port, std::string password)
	: _name(SERVER_NAME), _socket_fd(-1), _port(port), _password(password)
{
	// Check args
	launch();
}

Server::Server(const Server &other)
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
	struct sockaddr_in addr;
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
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(_socket_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
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
	routine(addr);
}

void Server::routine(struct sockaddr_in &addr)
{
	socklen_t addr_len;
	fd_set readfds;
	fd_set writefds;

	while (1)
	{
		// DEBUG("Waiting for new connection\n");
		FD_ZERO(&readfds);
		FD_ZERO(&writefds);
		FD_SET(_socket_fd, &readfds);

		// add all clients to the set of fd to read/write
		for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
		{
			FD_SET(it->getFd(), &readfds);
			FD_SET(it->getFd(), &writefds);
			// DEBUG("Client fd " << it->getFd() << " added to select\n");
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
				DEBUG("New connection\n");
				addr_len = sizeof(addr);
				int _client_fd = accept(_socket_fd, (struct sockaddr *)&addr, &addr_len);
				if (_client_fd < 0)
				{
					perror("In accept");
					return;
				}
				_clients.push_back(Client(_client_fd));
				continue;
			}
			reading(readfds);
			writing(writefds);
		}
	}
}

void Server::reading(fd_set readfds)
{

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

}

void Server::writing(fd_set writefds)
{
	for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (FD_ISSET(it->getFd(), &writefds))
		{
			// DEBUG("Ready to write\n");
			if (!it->getMessageReceived().empty() && *(it->getMessageReceived().end() - 1) == '\n')
			{
				parseCommands(*it);
				if (!it->getMessageToSend().empty())
				{
					write(it->getFd(), it->getMessageToSend().c_str(), it->getMessageToSend().size());
					DEBUG("Message sent: " << it->getMessageToSend());
				}
				it->clearMessageToSend();
			}
		}
	}
}

static void stripPrefix(std::string &line, char c)
{
	if (line[0] == c)
	{
		size_t pos = line.find_first_of(' ');
		pos = line.find_first_not_of(' ', pos);
		line.erase(0, pos);
	}
}

void Server::parseCommands(Client &client)
{
	std::string command_name[] = {"PASS",
								  "NICK",
								  "USER",
								  "JOIN",
								  "PING",
								  "WHO"};
	size_t nb_commands = sizeof(command_name) / sizeof(command_name[0]);
	void (Server::*f[])(Client & client, const std::vector<std::string> &args) = {
		&Server::pass,
		&Server::nick,
		&Server::user,
		&Server::join,
		&Server::ping,
		&Server::who};

	std::vector<std::string> lines = split(client.getMessageReceived(), "\r\n");
	for (std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); ++it)
	{
		std::string last_param;
		std::string line(*it);
		stripPrefix(line, '@');
		stripPrefix(line, ':');
		size_t pos = line.find_first_of(':');
		if (pos != std::string::npos)
		{
			last_param = line.substr(pos + 1, line.size() - pos);
			line = line.substr(0, pos);
		}
		std::vector<std::string> command = split(line, ' ');
		if (last_param.size() > 0)
			command.push_back(last_param);

		for (size_t i = 0; i < nb_commands; i++)
		{
			if (command[0] == command_name[i])
			{
				if (command[0] != "NICK" && command[0] != "USER" && command[0] != "PASS" && !client.isAuthentified())
				{
					// 451
					reply(ERR_NOTREGISTERED, client, command);
				}
				else
					(this->*f[i])(client, command);
				break;
			}
			else if (i == nb_commands - 1 && client.isAuthentified())
			{
				// 421
				reply(ERR_UNKNOWNCOMMAND, client, command);
			}
		}
	}
	client.clearMessageReceived();
}

void Server::authentificate(Client &client)
{
	if (!client.isPassOk())
	{
		// ERROR
		reply(ERR_PASSWDMISMATCH, client);
		client.appendMessageToSend("ERROR :Password is not correct\r\n");
		// close
		return;
	}
	client.setAuthentified();
	// 001 RPL_WELCOME
	reply(RPL_WELCOME, client);
}

void Server::pass(Client &client, const std::vector<std::string> &args)
{
	std::cout << "pass function called" << std::endl;
	if (!client.isAuthentified() && (client.getNickname() != "*" || !client.getUsername().empty()))
		return;
	if (args.size() < 2)
	{
		// 461 ERR_NEEDMOREPARAMS
		reply(ERR_NEEDMOREPARAMS, client, args);
		return;
	}
	if (client.isAuthentified())
	{
		// 462 ERR_ALREADYREGISTRED
		reply(ERR_ALREADYREGISTRED, client, args);
		return;
	}
	client.setPass(args[1] == _password);
}

void Server::nick(Client &client, const std::vector<std::string> &args)
{
	std::cout << "nick function called" << std::endl;
	if (args.size() < 2)
	{
		// 431 ERR_NONICKNAMEGIVEN
		reply(ERR_NONICKNAMEGIVEN, client, args);
		return;
	}
	std::string nickname(args[1]);
	if (nickname.size() > 9 || nickname.size() < 2 || nickname.find_first_of(" ,*?!@.") != std::string::npos)
	{
		// 432 ERR_ERRONEUSNICKNAME
		reply(ERR_ERRONEUSNICKNAME, client, args);
		return;
	}
	if (nickname == client.getNickname())
		return;
	for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->getNickname() == nickname)
		{
			// 433 ERR_NICKNAMEINUSE;
			reply(ERR_NICKNAMEINUSE, client, args);
			return;
		}
	}
	if (client.isAuthentified())
		client.appendMessageToSend(":" + client.getNickname() + " NICK :" + nickname + "\n");
	else if (client.getUsername() != "")
	{
		authentificate(client);
	}
	client.setNickname(args[1]);
}

void Server::user(Client &client, const std::vector<std::string> &args)
{
	std::cout << "user function called" << std::endl;
	if (args.size() < 5)
	{
		// 461 ERR_NEEDMOREPARAMS
		reply(ERR_NEEDMOREPARAMS, client, args);
		return;
	}
	if (client.isAuthentified())
	{
		// 462 ERR_ALREADYREGISTRED
		reply(ERR_ALREADYREGISTRED, client, args);
		return;
	}
	client.setUsername(args[1]);
	client.setHostname(args[2]);
	client.setServername(args[3]);
	client.setRealname(args[4]);
	if (!client.isAuthentified() && client.getNickname() != "")
	{
		authentificate(client);
	}
}

int Server::searchChan(std::string name, Client &client)
{
	for (size_t i = 0; i < _channels.size(); i++)
	{
		if (_channels[i].getName() == name)
			return i;
	}
	Channel newChannel(name, &client);
	_channels.push_back(newChannel);
	return (_channels.size() - 1);
}

void Server::join(Client &client, const std::vector<std::string> &args)
{
	std::cout << "join function called" << std::endl;
	if (args.size() == 1)
	{
		reply(ERR_NEEDMOREPARAMS, client, args);
		return;
	}
	if (args[1] == "0")
	{
		// Send PART message to all chans the client is connected to
		return;
	}
	std::vector<std::string> channels = split(args[1], ",");
	for (size_t i = 0; i < channels.size(); i++)
	{
		// Check name >> 476 ERR_BADCHANMASK + return
		int chan_pos = searchChan(channels[i], client);
		Channel &channel = _channels[chan_pos];
		channel.addClient(&client);
		client.addChan(&channel);
		// JOIN Message
		std::string message = ":" + client.getNickname() + " JOIN " + channels[i] + "\n";
		client.appendMessageToSend(message);
		if (channel.getTopic().size() > 0)
		{
			reply(RPL_TOPIC, client, channel);
			reply(RPL_TOPICWHOTIME, client, channel);
		}
		reply(RPL_NAMREPLY, client, channel);
		reply(RPL_ENDOFNAMES, client, channel);
		// Broadcast join message to all other clients in the channel
		// The message is the same as the one sent to the main client
		channel.broadcast(message, &client);
	}
}

void Server::ping(Client &client, const std::vector<std::string> &args)
{
	std::cout << "ping function called" << std::endl;
	client.appendMessageToSend(":ircserv PONG " + client.getNickname() + " " + args[1] + "\n");
}

void Server::who(Client &client, const std::vector<std::string> &args)
{
	std::cout << "who function called" << std::endl;
	
	if (args.size() == 1)
	{
		reply(ERR_NEEDMOREPARAMS, client, args);
		return;
	}

	// We suppose that WHO commands can have multiple arguments
	for (size_t i = 1; i < args.size(); i++)
	{
		// Check if the argument is a channel name
		if (args[i][0] == '#')
		{
			// Check if the channel exists
			bool found = false;
			for (size_t j = 0; j < _channels.size(); j++)
			{
				if (_channels[j].getName() == args[i])
				{
					found = true;
					for (size_t k = 0; k < _channels[j].getClients().size(); k++)
					{
						who_reply(RPL_WHOREPLY, client, &_channels[j], *_channels[j].getClients()[k]);
					}
					reply_mask(RPL_ENDOFWHO, client, args[i]);
				}
			}
			if (!found)
			{
				// 403
				reply(ERR_NOSUCHCHANNEL, client, args);
			}
		}
		else
		{
			// Check if the user exists
			bool found = false;
			for (size_t j = 0; j < _clients.size(); j++)
			{
				if (_clients[j].getNickname() == args[i])
				{
					found = true;
					who_reply(RPL_WHOREPLY, client, NULL, _clients[j]);
					reply_mask(RPL_ENDOFWHO, client, args[i]);
				}
			}
			if (!found)
			{
				// 401
				reply(ERR_NOSUCHNICK, client, args);
			}
		}
	}
}