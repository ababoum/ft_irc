#ifndef SERVER_HPP
#define SERVER_HPP

#include "ft_irc.hpp"

class Server
{
public:
	Server();
	Server(int port, std::string password);
	Server(const Server &other);
	~Server();

	Server &operator=(const Server &rhs);

	int getSocketFd(void) const;
	int getPort(void) const;
	std::string getPassword(void) const;

private:
	int _socket_fd;
	int _port;
	std::string _password;

	void launch(void);
};

#endif /* SERVER_HPP */
