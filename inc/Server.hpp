#ifndef SERVER_HPP
#define SERVER_HPP

#include "ft_irc.hpp"
class Client;

class Server
{
public:
	Server(int port, std::string password);
	~Server();

	Server &operator=(const Server &rhs);

	int getSocketFd(void) const;
	int getPort(void) const;
	std::string getPassword(void) const;

private:
	Server();
	Server(const Server &other);
	int _socket_fd;
	int _port;
	std::string _password;
	std::vector<Client> _clients;

	void launch(void);
};

#endif /* SERVER_HPP */
