/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bregneau <bregneau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/12 21:26:17 by bregneau          #+#    #+#             */
/*   Updated: 2023/03/12 22:40:36 by bregneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel(const std::string &name)
	: _name(name)
{
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
}

std::string Channel::getName() const
{
	return _name;
}
