#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "ft_irc.hpp"

class Channel
{
public:
	Channel(const std::string &name);
	Channel(const Channel &other);
	~Channel();
	Channel &operator=(const Channel &rhs);

	void	addClient(Client *client);
	void	removeClient(Client *client);
	void	removeClient(const std::string &nickname);
	void	removeClient(const int fd);

	std::string	getName() const;

private:
	Channel();

private:
	std::string _name;
	std::vector<Client *> _clients;
	
	
};

#endif /* CHANNEL_HPP */
