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
		for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
		{
			Client *client = *it;
			FD_SET(client->getFd(), &readfds);
			FD_SET(client->getFd(), &writefds);
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
				_clients.push_back(new Client(_client_fd));
				continue;
			}
			reading(readfds);
			writing(writefds);
		}
	}
}

void Server::reading(fd_set readfds)
{
	for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		Client *client = *it;
		if (FD_ISSET(client->getFd(), &readfds))
		{
			DEBUG("New message\n");
			DEBUG("Nb clients: " << _clients.size() << "\n");
			DEBUG("Nb channels: " << _channels.size() << "\n");

			char buffer[READ_SIZE + 1] = {0};
			int read_val = read(client->getFd(), buffer, READ_SIZE);
			if (read_val <= 0)
			{
				close(client->getFd());
				DEBUG("delete client\n")
				delete client;
				_clients.erase(it);
			}
			else
			{
				client->appendMessageReceived(buffer);
			}
			INFO(buffer);
			break;
		}
	}

}

void Server::writing(fd_set writefds)
{
	for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		Client *client = *it;
		if (FD_ISSET(client->getFd(), &writefds))
		{
			// DEBUG("Ready to write\n");
			if (!client->getMessageReceived().empty() && *(client->getMessageReceived().end() - 1) == '\n')
			{
				// parseCommands(*it);
				try
				{
					parseCommands(*client);
					if (!client->getMessageToSend().empty())
					{
						write(client->getFd(), client->getMessageToSend().c_str(), client->getMessageToSend().size());
						DEBUG("Message sent: \n" << client->getMessageToSend());
						client->clearMessageToSend();
					}
				}
				catch (std::exception &e) 
				{
					client->appendMessageToSend("ERROR :" + std::string(e.what()) + "\r\n");
					write(client->getFd(), client->getMessageToSend().c_str(), client->getMessageToSend().size());
					DEBUG("Message sent: \n" << client->getMessageToSend());
					closeConnection(it);
				}
				break;

			}
		}
	}
}

void Server::closeConnection(std::vector<Client*>::iterator client_it)
{
	Client *client = *client_it;
	std::vector<Channel *> joined_channels = client->getJoinedChannels();
	for (std::vector<Channel *>::iterator it = joined_channels.begin(); it != joined_channels.end(); ++it)
	{
		Channel *channel = *it;
		channel->removeClient(client);
		if (channel->getClients().empty())
		{
			delete channel;
			_channels.erase(std::find(_channels.begin(), _channels.end(), channel));
		}
	}
	close(client->getFd());
	delete client;
	_clients.erase(client_it);
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
								  "WHO",
								  "WHOIS",
								  "QUIT"};
	size_t nb_commands = sizeof(command_name) / sizeof(command_name[0]);
	void (Server::*f[])(Client & client, const std::vector<std::string> &args) = {
		&Server::pass,
		&Server::nick,
		&Server::user,
		&Server::join,
		&Server::ping,
		&Server::who,
		&Server::whois,
		&Server::quit};

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
		std::vector<std::string> args = split(line, ' ');
		if (last_param.size() > 0)
			args.push_back(last_param);

		std::transform(args[0].begin(), args[0].end(), args[0].begin(), ::toupper);
		for (size_t i = 0; i < nb_commands; i++)
		{
			if (args[0] == command_name[i])
			{
				if (args[0] != "NICK"
					&& args[0] != "USER"
					&& args[0] != "PASS"
					&& args[0] != "QUIT"
					&& !client.isAuthentified())
				{
					//451
					reply(ERR_NOTREGISTERED, client, args);
				}
				else
					(this->*f[i])(client, args);
				break ;
			}
			else if (i == nb_commands - 1 && client.isAuthentified())
			{
				//421
				reply(ERR_UNKNOWNCOMMAND, client, args);
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
		throw std::invalid_argument("Password is not correct");
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
	for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if ((*it)->getNickname() == nickname)
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

Channel *Server::searchChan(std::string name)
{
	for (size_t i = 0; i < _channels.size(); i++)
	{
		if (_channels[i]->getName() == name)
			return _channels[i];
	}
	return (NULL);
}

Channel *Server::addChan(std::string name, Client *client)
{
	Channel *newChannel = new Channel(name, client);
	_channels.push_back(newChannel);
	return newChannel;
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
		Channel *channel = searchChan(channels[i]);
		if (!channel)
			channel = addChan(channels[i], &client);
		channel->addClient(&client);
		client.addChan(channel);
		// JOIN Message
		std::string message = ":" + client.getNickname() + " JOIN " + channels[i] + "\n";
		// Broadcast join message to all other clients in the channel
		// The message is the same as the one sent to the main client
		channel->fullBroadcast(message);
		// client.appendMessageToSend(message);
		if (channel->getTopic().size() > 0)
		{
			reply(RPL_TOPIC, client, *channel);
			reply(RPL_TOPICWHOTIME, client, *channel);
		}
		reply(RPL_NAMREPLY, client, *channel);
		reply(RPL_ENDOFNAMES, client, *channel);
	}
}

void Server::part(Client &client, const std::vector<std::string> &args)
{
	std::cout << "part function called" << std::endl;
	if (args.size() < 2)
	{
		reply(ERR_NEEDMOREPARAMS, client, args);
		return ;
	}
	std::vector<std::string> channels = split(args[1], ",");
	for (size_t i = 0; i < channels.size(); i++)
	{
		Channel *channel = searchChan(channels[i]);
		if (!channel)
		{
			// 1 ERR_NOSUCHCHANNEL 403 for each channel that does not exist
			continue ;
		}
		Client *chan_client = channel->searchClient(client.getNickname());
		if (!chan_client)
		{
			// 1 ERR_NOTONCHANNEL 442 for each channel the client is not on
			continue ;
		}
	// 1 PART message for each channel the client is leaving
	// BROADCAST TO OTHER CLIENTS IN THE CHANNEL
	}

}

void Server::ping(Client &client, const std::vector<std::string> &args)
{
	std::cout << "ping function called" << std::endl;
	client.appendMessageToSend(":ircserv PONG " + client.getNickname() + " " + args[1] + "\n");
}

// Warning: what happens if we query about both a channel and a nickname?
void Server::who(Client &client, const std::vector<std::string> &args)
{
	std::cout << "who function called" << std::endl;
	
	if (args.size() == 1)
	{
		reply(ERR_NEEDMOREPARAMS, client, args);
		return;
	}

	// WHO command can have multiple arguments
	for (size_t i = 1; i < args.size(); i++)
	{
		// Check if the argument is a channel name
		if (args[i][0] == '#')
		{
			// Check if the channel exists
			Channel *found = NULL;
			for (size_t j = 0; j < _channels.size(); j++)
			{
				if (_channels[j]->getName() == args[i])
				{
					found = _channels[j];
					for (size_t k = 0; k < found->getClients().size(); k++)
					{
						who_reply(RPL_WHOREPLY, client, found, *found->getClients()[k]);
					}
					reply(RPL_ENDOFWHO, client, args[i]);
				}
			}
			if (!found)
			{
				// 403
				reply(ERR_NOSUCHCHANNEL, client, args[i]);
			}
		}
		else
		{
			// Check if the user exists
			bool found = false;
			for (size_t j = 0; j < _clients.size(); j++)
			{
				if (_clients[j]->getNickname() == args[i])
				{
					found = true;
					who_reply(RPL_WHOREPLY, client, NULL, *_clients[j]);
					reply(RPL_ENDOFWHO, client, args[i]);
				}
			}
			if (!found)
			{
				// 401
				reply(ERR_NOSUCHNICK, client, args[i]);
			}
		}
	}
}

void Server::whois(Client &client, const std::vector<std::string> &args)
{
	std::cout << "who function called" << std::endl;
	
	if (args.size() == 1)
	{
		reply(ERR_NONICKNAMEGIVEN, client, args);
		return;
	}

	// WHOIS command can have multiple arguments
	for (size_t i = 1; i < args.size(); i++)
	{
		// Check if the nickname exists
		bool found = false;
		for (size_t j = 0; j < _clients.size(); j++)
		{
			if (_clients[j]->getNickname() == args[i])
			{
				found = true;
				who_reply(RPL_WHOISUSER, client, NULL, *_clients[j]);
				who_reply(RPL_WHOISCHANNELS, client, NULL, *_clients[j]);
				// We can add more if necessary...
			}
		}
		if (!found)
		{
			// 401
			reply(ERR_NOSUCHNICK, client, args[i]);
		}
	}
	reply(RPL_ENDOFWHOIS, client, args);
}

void Server::quit(Client &client, const std::vector<std::string> &args)
{
	(void)client;
	std::cout << "quit function called" << std::endl;
	if (args.size() == 1)
		throw std::runtime_error("Quit");
	else
		throw std::runtime_error("Quit :" + args[1]);
}
