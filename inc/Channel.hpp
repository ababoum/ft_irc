#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "ft_irc.hpp"

class Channel
{
public:
	Channel(const std::string &name, Client *client);
	Channel(const Channel &other);
	~Channel();
	Channel &operator=(const Channel &rhs);

	void	addClient(Client *client);
	void	removeClient(Client *client);
	void	removeClient(const std::string &nickname);
	void	removeClient(const int fd);
	void	addOperator(Client *client);
	void	removeOp(Client *client);
	void	removeOp(const std::string &nickname);
	void	removeOp(const int fd);

	std::string	getName() const;
	std::string	getTopic() const;
	Client *getTopicSetBy() const;
	time_t getTopicSetAt() const;

private:
	Channel();

private:
	std::string 			_name;
	std::vector<Client *>	_clients;
	std::vector<Client *>	_operators;
	std::string				_topic;
	Client *				_topic_set_by;
	time_t					_topic_set_at;
};

#endif /* CHANNEL_HPP */
