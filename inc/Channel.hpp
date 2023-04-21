#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "ft_irc.hpp"

class Channel
{
public:

	struct Modes
	{
		bool i;
		bool t;
	};

	Channel(const std::string &name, Client *client);
	Channel(const Channel &other);
	~Channel();
	Channel &operator=(const Channel &rhs);

	static bool isChannelNameValid(const std::string &name);

	void	addClient(std::string mode, Client *client);
	void	removeClient(Client *client);
	void	removeClient(const std::string &nickname);
	void	removeClient(const int fd);
	void	addOperator(Client *client);
	void	removeOp(Client *client);
	void	removeOp(const std::string &nickname);
	void	removeOp(const int fd);

	void	fullBroadcast(const std::string &message);
	void	broadcast(const std::string &message, Client *clientToExclude);

	std::string getName() const;
	std::string getTopic() const;
	std::size_t getNbOperators() const;

	Client *getTopicSetBy() const;
	time_t getTopicSetAt() const;
	std::vector<std::pair<std::string, Client *> > const &getClients() const;
	Client *searchClient(const std::string &nickname) const;
	Client *searchClient(const int fd) const;
	bool isOperator(const Client *client) const;
	std::vector<Client *> const &getInviteList() const;
	bool isInvited(Client *client) const;

	void setTopic(const std::string &topic);
	void setTopicSetBy(Client *client);
	void setTopicSetAt(time_t timestamp);
	void addToInvite(Client *client);
	bool addMode(char c, std::string nickname);
	bool removeMode(char c, std::string nickname);
	bool isInviteOnly() const;
	bool isTopicProtected() const;

private:
	Channel();

private:
	std::string 			_name;
	Modes					_modes;
	std::vector<std::pair<std::string, Client *> >	_clients;
	std::vector<Client *>	_operators;
	std::vector<Client *>	_invite_list;
	std::string				_topic;
	Client *				_topic_set_by;
	time_t					_topic_set_at;
};

#endif /* CHANNEL_HPP */
