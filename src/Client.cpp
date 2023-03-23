#include "Client.hpp"

Client::Client(int fd)
	: _fd(fd), _is_authentified(false)
	, _message_received(""), _message_to_send("")
	, _nickname(""), _username(""), _hostname(""), _servername(""), _realname("")
{
}

Client::Client(const Client &other)
{
	*this = other;
}

Client::~Client()
{
	// The client fd needs to be closed by hand when the client disconnects
}

Client &Client::operator=(const Client &rhs)
{
	if (this != &rhs)
	{
		_fd = rhs.getFd();
		_is_authentified = rhs.isAuthentified();
		_message_received = rhs.getMessageReceived();
		_message_to_send = rhs.getMessageToSend();
		_nickname = rhs.getNickname();
		_username = rhs.getUsername();
		_hostname = rhs.getHostname();
		_servername = rhs.getServername();
		_realname = rhs.getRealname();
	}
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

void Client::setPass(bool pass)
{
	_pass = pass;
}

void Client::setAuthentified()
{
	_is_authentified = true;
}

void Client::setNickname(const std::string &nick_name)
{
	_nickname = nick_name;
}

void Client::setUsername(const std::string &user_name)
{
	_username = user_name;
}

void Client::setHostname(const std::string &host_name)
{
	_hostname = host_name;
}

void Client::setServername(const std::string &server_name)
{
	_servername = server_name;
}

void Client::setRealname(const std::string &real_name)
{
	_realname = real_name;
}

void Client::addChan(Channel *channel)
{
	_joined_channels.push_back(channel);
}

bool Client::isPassOk() const
{
	return _pass;
}

bool Client::isAuthentified() const
{
	return _is_authentified;
}

const std::string &	Client::getMessageReceived() const
{
	return _message_received;
}

const std::string &	Client::getMessageToSend() const
{
	return _message_to_send;
}

const std::string &	Client::getNickname() const
{
	return _nickname;
}

const std::string &	Client::getUsername() const
{
	return _username;
}

const std::string &	Client::getHostname() const
{
	return _hostname;
}

const std::string &	Client::getServername() const
{
	return _servername;
}

const std::string &	Client::getRealname() const
{
	return _realname;
}

int Client::getFd() const
{
	return _fd;
}
