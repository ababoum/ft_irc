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

void stripPrefix(std::string &line, char c)
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
	std::string command_name[] = {"NICK", "USER", "JOIN", "PING"};
	size_t nb_commands = sizeof(command_name) / sizeof(command_name[0]);
	void (Server::*f[])(Client &client, const std::vector<std::string>& args) = {&Server::nick, &Server::user, &Server::join, &Server::ping};

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

		for (std::vector<std::string>::iterator it = command.begin(); it != command.end(); ++it)
			std::cout << *it << std::endl;
		for (size_t i = 0; i < nb_commands; i++)
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
		client.appendMessageToSend(":ircserv 431 " + client.getNickname() + " :No nickname given\n");
		return;
	}
	std::string nickname(args[1]);
	if (nickname.size() > 9 || nickname.size() < 2 || nickname.find_first_of(" ,*?!@."))
	{
		//432 ERR_ERRONEUSNICKNAME
		client.appendMessageToSend(":ircserv 432 " + client.getNickname() + " " + nickname + " :Erroneus nickname\n");
		return;
	}
	for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->getNickname() == nickname)
		{
			//433 ERR_NICKNAMEINUSE;
			client.appendMessageToSend(":ircserv 433 " + client.getNickname() + " " + nickname + " :Nickname is already in use.\n");
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
	(void)client;
	(void)args;
}

void Server::ping(Client &client, const std::vector<std::string>& args)
{
	std::cout << "ping function called" << std::endl;
	client.appendMessageToSend(":ircserv PONG " + client.getNickname() + " " + args[1] + "\n");
}