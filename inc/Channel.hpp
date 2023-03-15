/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mriant <mriant@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/12 21:18:20 by bregneau          #+#    #+#             */
/*   Updated: 2023/03/15 17:00:09 by mriant           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

private:
	Channel();

private:
	std::string _name;
	std::vector<Client *> _clients;
	std::vector<Client *> _operators;
};

#endif /* CHANNEL_HPP */
