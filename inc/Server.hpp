#ifndef SERVER_HPP
#define SERVER_HPP

#include "ft_irc.hpp"

class Client;
class Channel;

class Server
{
public:
	Server(int port, std::string password);
	~Server();

	Server &operator=(const Server &rhs);

private:
	int getSocketFd(void) const;
	int getPort(void) const;
	std::string getPassword(void) const;

	void launch(void);
	void routine(struct sockaddr_in &addr);

	void reply(int code, Client &client, const std::vector<std::string>& args = std::vector<std::string>());
	void reply(int code, Client &client, const Channel& channel);

	void parseCommands(Client &client);
	
	void authentificate(Client &client);
	
	void pass(Client &client, const std::vector<std::string>& args);
	void nick(Client &client, const std::vector<std::string>& args);
	void user(Client &client, const std::vector<std::string>& args);
	void join(Client &client, const std::vector<std::string>& args);
	void ping(Client &client, const std::vector<std::string>& args);
	int	 searchChan(std::string name, Client &client);

private:
	Server();
	Server(const Server &other);


// Attributes
private:
	std::string				_name;
	int 					_socket_fd;
	int 					_port;
	std::string				_password;
	std::vector<Client>		_clients;
	std::vector<Channel>	_channels;

};

#endif /* SERVER_HPP */
