#include "Channel.hpp"

//==============================================================================
// Constructors and destructor
//==============================================================================

Channel::Channel(const std::string &name, Client *client)
	: _name(name)
{
	addClient("@", client);
	addOperator(client);
	_modes.i = false;
	_modes.t = false;
}

Channel::Channel(const Channel &other)
{
	*this = other;
}

Channel::~Channel()
{
}

//==============================================================================
// Operators
//==============================================================================

Channel &Channel::operator=(const Channel &rhs)
{
	_name = rhs.getName();
	_topic = rhs.getTopic();
	_topic_set_by = rhs.getTopicSetBy();
	_topic_set_at = rhs.getTopicSetAt();
	_clients = rhs._clients;
	_operators = rhs._operators;
	_invite_list = rhs._invite_list;
	_modes = rhs._modes;
	return *this;
}

//==============================================================================
// Getters and Setters
//==============================================================================

std::string Channel::getName() const
{
	return _name;
}

std::string Channel::getTopic() const
{
	return _topic;
}

Client *Channel::getTopicSetBy() const
{
	return _topic_set_by;
}

time_t Channel::getTopicSetAt() const
{
	return _topic_set_at;
}

const std::vector<std::pair<std::string, Client *> > &Channel::getClients() const
{
	return _clients;
}

Client *Channel::searchClient(const std::string &nickname) const
{
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i].second->getNickname() == nickname)
			return _clients[i].second;
	}
	return NULL;
}

std::vector<Client *> const &Channel::getInviteList() const
{
	return _invite_list;
}

void Channel::addToInvite(Client *client)
{
	if (std::find(_invite_list.begin(), _invite_list.end(), client) == _invite_list.end())
		_invite_list.push_back(client);
}

bool Channel::addMode(char c, std::string nickname)
{
	Client *client_target = searchClient(nickname);
	switch (c)
	{
		case 'i':
			if (_modes.i)
				return false;
			_modes.i = true;
			return true;
		case 't':
			if (_modes.t)
				return false;
			_modes.t = true;
			return true;
		case 'o':
			if (!client_target)
			{
				FATAL_ERR("You must check the client exists in channel before changing his mode !\n");
				return false;
			}
			if (isOperator(client_target))
				return false;
			addOperator(client_target);
			return true;
		default:
			return false;
	}
}

bool Channel::removeMode(char c, std::string nickname)
{
	Client *client_target = searchClient(nickname);
	switch (c)
	{
		case 'i':
			if (!_modes.i)
				return false;
			_modes.i = false;
			return true;
		case 't':
			if (!_modes.t)
				return false;
			_modes.t = false;
			return true;
		case 'o':
			if (!client_target)
			{
				FATAL_ERR("You must check the client exists in channel before changing his mode !\n");
				return false;
			}
			if (!isOperator(client_target))
				return false;
			removeOp(client_target);
			return true;
		default:
			return false;
	}
}

void Channel::setTopic(const std::string &topic)
{
	_topic = topic;
}

void Channel::setTopicSetBy(Client *client)
{
	_topic_set_by = client;
}

void Channel::setTopicSetAt(time_t timestamp)
{
	_topic_set_at = timestamp;
}

bool Channel::isInviteOnly() const
{
	return _modes.i;
}

bool Channel::isTopicProtected() const
{
	return _modes.t;
}


//==============================================================================
// Functions
//==============================================================================

bool Channel::isChannelNameValid(const std::string &name)
{
	bool res = true;

	if (name.empty())
		return false;
	else if ((name[0] != '#' && name[0] != '&' ) ||
			  name.find(' ') != std::string::npos ||
			  name.find(',') != std::string::npos ||
			  name.find(0x07) != std::string::npos)
		return false;

	return res;
}

void Channel::addClient(std::string mode, Client *client)
{
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i].second->getNickname() == client->getNickname())
			return;
	}
	_clients.push_back(std::make_pair(mode, client));
}

void Channel::addOperator(Client *client)
{
	for (size_t i = 0; i < _operators.size(); i++)
	{
		if (_operators[i]->getNickname() == client->getNickname())
			return;
	}
	_operators.push_back(client);
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i].second == client)
		{
			_clients[i].first = "@";
			break;
		}
	}
}

void Channel::removeClient(Client *client)
{
	for (std::vector<std::pair<std::string, Client *> >::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->second == client)
		{
			_clients.erase(it);
			break;
		}
	}
	removeOp(client);
}

void Channel::removeClient(const std::string &nickname)
{
	for (std::vector<std::pair<std::string, Client *> >::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->second->getNickname() == nickname)
		{
			_clients.erase(it);
			break;
		}
	}
	removeOp(nickname);
}

void Channel::removeClient(const int fd)
{
	for (std::vector<std::pair<std::string, Client *> >::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->second->getFd() == fd)
		{
			_clients.erase(it);
			break;
		}
	}
	removeOp(fd);
}

void Channel::removeOp(Client *client)
{
	for (std::vector<Client *>::iterator it = _operators.begin(); it != _operators.end(); ++it)
	{
		if (*it == client)
		{
			_operators.erase(it);
			break;
		}
	}
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i].second == client)
		{
			_clients[i].first = "";
			break;
		}
	}
}

void Channel::removeOp(const std::string &nickname)
{
	for (std::vector<Client *>::iterator it = _operators.begin(); it != _operators.end(); ++it)
	{
		if ((*it)->getNickname() == nickname)
		{
			_operators.erase(it);
			break;
		}
	}
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i].second->getNickname() == nickname)
		{
			_clients[i].first = "";
			break;
		}
	}
}

void Channel::removeOp(const int fd)
{
	for (std::vector<Client *>::iterator it = _operators.begin(); it != _operators.end(); ++it)
	{
		if ((*it)->getFd() == fd)
		{
			_operators.erase(it);
			break;
		}
	}
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i].second->getFd() == fd)
		{
			_clients[i].first = "";
			break;
		}
	}
}

bool Channel::isOperator(Client *client) const
{
	for (size_t i = 0; i < _operators.size(); i++)
	{
		if (_operators[i]->getNickname() == client->getNickname())
			return true;
	}
	return false;
}

// *
// * Broadcast a message to all clients in the channel.
// *
// * @param message The message to broadcast.
// *
void Channel::fullBroadcast(const std::string &message)
{
	for (size_t i = 0; i < _clients.size(); i++)
	{
		_clients[i].second->appendMessageToSend(message);
	}
}

// *
// * Broadcast a message to all clients in the channel except the one specified
// * in the second argument.
// *
// * @param message The message to broadcast.
// * @param clientToExclude The client to exclude from the broadcast.
// *
void Channel::broadcast(const std::string &message, Client *clientToExclude)
{
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i].second != clientToExclude)
			_clients[i].second->appendMessageToSend(message);
	}
}
