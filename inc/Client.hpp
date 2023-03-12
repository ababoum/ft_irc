/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bregneau <bregneau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/01 19:17:07 by bregneau          #+#    #+#             */
/*   Updated: 2023/03/12 19:51:36 by bregneau         ###   ########.fr       */
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

	void	appendMessageReceived(const std::string &str);
	void	appendMessageReceived(const char *str);
	void	appendMessageToSend(const std::string &str);
	void	appendMessageToSend(const char *str);
	void	clearMessageReceived();
	void	clearMessageToSend();

	void	setNickName(const std::string &nick_name);
	void	setUserName(const std::string &user_name);
	void	setHostName(const std::string &host_name);
	void	setServerName(const std::string &server_name);
	void	setRealName(const std::string &real_name);

	const std::string &	getMessageReceived() const;
	const std::string &	getMessageToSend() const;
	const std::string &	getNickName() const;
	const std::string &	getUserName() const;
	const std::string &	getHostName() const;
	const std::string &	getServerName() const;
	const std::string &	getRealName() const;
	int getFd() const;

private:
	Client();

	// Attributes
private:
	int _fd;
	bool _is_authentified;
	std::string _message_received;
	std::string _message_to_send;
	std::string _nick_name;
	std::string _user_name;
	std::string _host_name;
	std::string _server_name;
	std::string _real_name;
};

#endif /* CLIENT_HPP */
