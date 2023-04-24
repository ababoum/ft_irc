#include "Server.hpp"

typedef std::vector<std::pair<std::string, Client *> > ClientList;

Server::Server()
	: _socket_fd(0), _port(0), _password("")
{
}

Server::Server(int port, std::string password)
	: _name(SERVER_NAME), _socket_fd(-1), _port(port), _password(password)
{
	launch();
}

Server::Server(const Server &other)
{
	*this = other;
}

Server::~Server()
{
	RUNTIME_MSG("Server destructor called\n");

	if (_socket_fd >= 0)
		close(_socket_fd);

	// Destroy clients
	for (std::vector<Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		close((*it)->getFd());
		DEBUG("Client fd " << (*it)->getFd() << " destroyed\n");
		delete *it;
	}

	// Destroy channels
	for (std::vector<Channel *>::iterator it = _channels.begin(); it != _channels.end(); ++it)
	{
		delete *it;
	}
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
	memset(&_addr, 0, sizeof(_addr));
	_addr.sin_family = AF_INET;
	_addr.sin_port = htons(_port);
	_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(_socket_fd, (struct sockaddr *)&_addr, sizeof(_addr)) < 0)
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
}

void Server::routine(void)
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
		for (std::vector<Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
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
				addr_len = sizeof(_addr);
				int _client_fd = accept(_socket_fd, (struct sockaddr *)&_addr, &addr_len);
				if (_client_fd < 0)
				{
					perror("In accept");
					return;
				}
				_clients.push_back(new Client(_client_fd));
				continue;
			}
			if (reading(readfds))
				writing(writefds);
		}
	}
}

bool Server::reading(fd_set readfds)
{
	for (std::vector<Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		Client *client = *it;
		if (FD_ISSET(client->getFd(), &readfds))
		{
			DEBUG("New message\n");
			// DEBUG("Nb clients: " << _clients.size() << "\n");
			// DEBUG("Nb channels: " << _channels.size() << "\n");

			char buffer[READ_SIZE + 1] = {0};
			int read_val = read(client->getFd(), buffer, READ_SIZE);
			if (read_val <= 0)
			{
				INFO("Client disconnected\n");
				closeConnection(it);
				return false;
			}
			else
				client->appendMessageReceived(buffer);
			INFO(buffer);
			if (!client->getMessageReceived().empty() && *(client->getMessageReceived().end() - 1) == '\n')
			{
				try
				{
					parseCommands(*client);
				}
				catch (std::exception &e)
				{
					client->appendMessageToSend("ERROR :" + std::string(e.what()) + "\r\n");
					client->setFatalError();
				}
			}
			return false;
		}
	}
	return true;
}

void Server::writing(fd_set writefds)
{
	for (std::vector<Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		Client *client = *it;
		if (FD_ISSET(client->getFd(), &writefds))
		{
			if (!client->getMessageToSend().empty())
			{
				write(client->getFd(),
					  client->getMessageToSend().c_str(),
					  client->getMessageToSend().size());
				DEBUG("Message sent: \n"
					  << client->getMessageToSend());
				client->clearMessageToSend();
				break;
			}
			if (client->isFatalError())
			{
				closeConnection(it);
				break;
			}
		}
	}
}

void Server::closeConnection(std::vector<Client *>::iterator &client_it)
{
	Client *client = *client_it;
	std::vector<Channel *> joined_channels = client->getJoinedChannels();
	for (std::vector<Channel *>::iterator it = joined_channels.begin(); it != joined_channels.end(); ++it)
	{
		client->removeChan((*it)->getName());
		removeClientFromChannel(client, *it);
	}
	close(client->getFd());
	_clients.erase(client_it);
	delete client;
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
								  "PART",
								  "KICK",
								  "INVITE",
								  "PING",
								  "WHO",
								  "WHOIS",
								  "PRIVMSG",
								  "NOTICE",
								  "MODE",
								  "QUIT",
								  "OPER",
								  "TOPIC",
								  "NAMES",
								  "LIST",
								  "KILL"};
	size_t nb_commands = sizeof(command_name) / sizeof(command_name[0]);
	void (Server::*f[])(Client & client, const std::vector<std::string> &args) = {
		&Server::pass,
		&Server::nick,
		&Server::user,
		&Server::join,
		&Server::part,
		&Server::kick,
		&Server::invite,
		&Server::ping,
		&Server::who,
		&Server::whois,
		&Server::privmsg,
		&Server::notice,
		&Server::mode,
		&Server::quit,
		&Server::oper,
		&Server::topic,
		&Server::names,
		&Server::list,
		&Server::kill};

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
				if (args[0] != "NICK" && args[0] != "USER" && args[0] != "PASS" && args[0] != "QUIT" && !client.isAuthentified())
				{
					reply(ERR_NOTREGISTERED, client, args);
				}
				else
					(this->*f[i])(client, args);
				break;
			}
			else if (i == nb_commands - 1 && client.isAuthentified())
			{
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
	reply(RPL_WELCOME, client);
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

Client *Server::searchClient(std::string nickname)
{
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i]->getNickname() == nickname)
			return _clients[i];
	}
	return (NULL);
}

void Server::removeClientFromChannel(Client *client, Channel *channel)
{
	// Elect a new channel operator if the client was the only one
	if (channel->isOperator(client) && channel->getNbOperators() == 1)
	{
		ClientList clients = channel->getClients();
		for (ClientList::iterator it = clients.begin(); it != clients.end(); ++it)
		{
			if (it->second != client)
			{
				std::vector<std::string> args;
				args.push_back("MODE");
				args.push_back(channel->getName());
				args.push_back("+o");
				args.push_back(it->second->getNickname());
				mode(*client, args);
				break;
			}
		}
	}

	// Remove the client from the channel
	channel->removeClient(client);

	// Delete the channel if it's empty after the client left
	if (channel->getClients().empty())
	{
		_channels.erase(std::find(_channels.begin(), _channels.end(), channel));
		delete channel;
	}
}