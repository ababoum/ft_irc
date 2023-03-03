#ifndef SERVER_HPP
#define SERVER_HPP

class Server
{
public:
	Server();
	Server(const Server &other);
	~Server();

	Server &	operator=(const Server &rhs);

private:
	int _socket_ft;
	int _port
}

#endif /* SERVER_HPP */
