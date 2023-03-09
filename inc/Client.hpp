/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bregneau <bregneau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/01 19:17:07 by bregneau          #+#    #+#             */
/*   Updated: 2023/03/06 18:48:37 by bregneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "ft_irc.hpp"

class Client
{
public:
	Client(int fd);
	Client(const Client &other);
	Client &operator=(const Client &rhs);
	~Client();
	int getFd() const;
	void append_message_buffer(const std::string &str);
	void append_message_buffer(const char *str);
	void clear_message_buffer();

private:
	Client();

// Attributes
private:
	int _fd;
	std::string _message_buffer;
};

#endif /* CLIENT_HPP */
