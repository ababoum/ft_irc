/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bregneau <bregneau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/01 19:16:44 by bregneau          #+#    #+#             */
/*   Updated: 2023/03/12 19:52:24 by bregneau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(int fd)
	: _fd(fd)
{
}

Client::Client(const Client &other)
	: _fd(other.getFd())
{
}

Client::~Client()
{
	// The client fd needs to be closed by hand when the client disconnects
}

Client &Client::operator=(const Client &rhs)
{
	_fd = rhs.getFd();
	return *this;
}

void Client::appendMessageReceived(const std::string &str)
{
	_message_received.append(str);
}

void Client::appendMessageReceived(const char *str)
{
	_message_received.append(str);
}

void Client::appendMessageToSend(const std::string &str)
{
	_message_to_send.append(str);
}

void Client::appendMessageToSend(const char *str)
{
	_message_to_send.append(str);
}

void Client::clearMessageReceived()
{
	_message_received.clear();
}

void Client::clearMessageToSend()
{
	_message_to_send.clear();
}

void Client::setNickName(const std::string &nick_name)
{
	_nick_name = nick_name;
}

void Client::setUserName(const std::string &user_name)
{
	_user_name = user_name;
}

void Client::setHostName(const std::string &host_name)
{
	_host_name = host_name;
}

void Client::setServerName(const std::string &server_name)
{
	_server_name = server_name;
}

void Client::setRealName(const std::string &real_name)
{
	_real_name = real_name;
}

const std::string &	Client::getMessageReceived() const
{
	return _message_received;
}

const std::string &	Client::getMessageToSend() const
{
	return _message_to_send;
}

const std::string &	Client::getNickName() const
{
	return _nick_name;
}

const std::string &	Client::getUserName() const
{
	return _user_name;
}

const std::string &	Client::getHostName() const
{
	return _host_name;
}

const std::string &	Client::getServerName() const
{
	return _server_name;
}

const std::string &	Client::getRealName() const
{
	return _real_name;
}

int Client::getFd() const
{
	return _fd;
}
