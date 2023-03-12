/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bregneau <bregneau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/12 21:18:20 by bregneau          #+#    #+#             */
/*   Updated: 2023/03/12 22:41:05 by bregneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
