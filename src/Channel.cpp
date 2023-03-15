#include "Channel.hpp"

Channel::Channel(const std::string &name, Client *client)
	: _name(name)
{
	addClient(client);
	addOperator(client);
}

Channel::Channel(const Channel &other)
	: _name(other.getName())
{
	*this = other;
}

Channel::~Channel()
{
}

Channel &Channel::operator=(const Channel &rhs)
{
	_name = rhs.getName();
	return *this;
}

void Channel::addClient(Client *client)
{
	_clients.push_back(client);
}

void Channel::addOperator(Client *client)
{
	_operators.push_back(client);
}

void Channel::removeClient(Client *client)
{
	for (std::vector<Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (*it == client)
		{
			_clients.erase(it);
			break;
		}
	}
	removeOp(client);
}

void Channel::removeClient(const std::string &nickname)
{
	for (std::vector<Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if ((*it)->getNickname() == nickname)
		{
			_clients.erase(it);
			break;
		}
	}
	removeOp(nickname);
}

void Channel::removeClient(const int fd)
{
	for (std::vector<Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if ((*it)->getFd() == fd)
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
			_clients.erase(it);
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
			_clients.erase(it);
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
			_clients.erase(it);
			break;
		}
	}
}

std::string Channel::getName() const
{
	return _name;
}
