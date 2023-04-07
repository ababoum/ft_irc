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
	Channel	*searchChan(std::string name);
	Channel	*addChan(std::string name, Client *client);
	Client *searchClient(std::string nickname);
	void removeClientFromChannel(Client *client, Channel *channel);

	void launch(void);
	void routine(struct sockaddr_in &addr);
	bool reading(fd_set readfds);
	void writing(fd_set writefds);
	void closeConnection(std::vector<Client*>::iterator & it);


	void reply(int code, Client &client, const std::vector<std::string> &args = std::vector<std::string>());
	void reply(int code, Client &client, const Channel &channel);
	void reply(int code, Client &client, Channel *channel, const Client &target);
	void reply(int code, Client &client, const std::string &mask);
	void reply(int code, Client &client, Channel* channel, const std::string &mask);

	void parseCommands(Client &client);

	void authentificate(Client &client);

	void pass(Client &client, const std::vector<std::string> &args);
	void nick(Client &client, const std::vector<std::string> &args);
	void user(Client &client, const std::vector<std::string> &args);
	void join(Client &client, const std::vector<std::string> &args);
	void part(Client &client, const std::vector<std::string> &args);
	void kick(Client &client, const std::vector<std::string> &args);
	void invite(Client &client, const std::vector<std::string> &args);
	void ping(Client &client, const std::vector<std::string> &args);
	void who(Client &client, const std::vector<std::string> &args);
	void whois(Client &client, const std::vector<std::string> &args);
	void privmsg(Client &client, const std::vector<std::string> &args);
	void quit(Client &client, const std::vector<std::string> &args);
	void topic(Client &client, const std::vector<std::string> &args);

private:
	Server();
	Server(const Server &other);

	// Attributes
private:
	std::string				_name;
	int 					_socket_fd;
	int 					_port;
	std::string				_password;
	std::vector<Client*>	_clients;
	std::vector<Channel*>	_channels;
};

#endif /* SERVER_HPP */
