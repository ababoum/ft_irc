#include "Client.hpp"

Client::Client(int fd)
	: _fd(fd), _pass(false), _is_authentified(false), _fatal_error(false)
	, _message_received(""), _message_to_send("")
	, _nickname(""), _username(""), _hostname(""), _servername(""), _realname("")
{
	_modes.i = false;
	_modes.o = false;
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
		_pass = rhs.isPassOk();
		_is_authentified = rhs.isAuthentified();
		_fatal_error = rhs.isFatalError();
		_message_received = rhs.getMessageReceived();
		_message_to_send = rhs.getMessageToSend();
		_nickname = rhs.getNickname();
		_username = rhs.getUsername();
		_hostname = rhs.getHostname();
		_servername = rhs.getServername();
		_realname = rhs.getRealname();
		_joined_channels = rhs.getJoinedChannels();
		_modes.i = rhs.isInvisible();
		_modes.o = rhs.isOperator();
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

void	Client::setFatalError()
{
	_fatal_error = true;
}

bool	Client::isNicknameValid(const std::string &nickname)
{
	// Nicknames are non-empty strings with the following restrictions:

    // They MUST NOT contain any of the following characters: space (' ', 0x20), comma (',', 0x2C), asterisk ('*', 0x2A), question mark ('?', 0x3F), exclamation mark ('!', 0x21), at sign ('@', 0x40).
    // They MUST NOT start with any of the following characters: dollar ('$', 0x24), colon (':', 0x3A).
    // They MUST NOT start with a character listed as a channel type prefix.
    // They SHOULD NOT contain any dot character ('.', 0x2E).

	bool res = true;
	if (nickname.empty())
		res = false;
	else if (nickname.find(' ') != std::string::npos ||
			nickname.find(',') != std::string::npos || 
			nickname.find('*') != std::string::npos || 
			nickname.find('?') != std::string::npos || 
			nickname.find('!') != std::string::npos || 
			nickname.find('@') != std::string::npos)
		res = false;
	else if (nickname[0] == '$' || nickname[0] == ':')
		res = false;
	// Channel prefixes
	else if (nickname[0] == '#' || nickname[0] == '&')
		res = false;
	else if (nickname.find('.') != std::string::npos)
		res = false;
	else if (nickname == "bot")
		res = false;

	return res;
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
	if (std::find(_joined_channels.begin(), _joined_channels.end(), channel) == _joined_channels.end())
		_joined_channels.push_back(channel);
}

void Client::removeChan(std::string name)
{
	for (std::vector<Channel *>::iterator it = _joined_channels.begin();
			it != _joined_channels.end(); it++)
	{
		if ((*it)->getName() == name)
		{
			_joined_channels.erase(it);
			break ;
		}
	}
}


bool Client::isPassOk() const
{
	return _pass;
}

bool Client::isAuthentified() const
{
	return _is_authentified;
}

bool Client::isFatalError() const
{
	return _fatal_error;
}

bool Client::isOperator() const
{
	return _modes.o;
}

bool Client::isInvisible() const
{
	return _modes.i;
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

const std::string Client::getSource() const
{
	return std::string(_nickname + "!" + _username + "@" + _servername);
}

const std::vector<Channel *> &	Client::getJoinedChannels() const
{
	return _joined_channels;
}

int Client::getFd() const
{
	return _fd;
}

bool Client::addMode(char c)
{
	switch (c)
	{
		case 'i':
			if (_modes.i)
				return false;
			_modes.i = true;
			return true;
		case 'o':
			if (_modes.o)
				return false;
			_modes.o = true;
			return true;
		default:
			return false;
	}
}

bool Client::removeMode(char c)
{
	switch (c)
	{
		case 'i':
			if (!_modes.i)
				return false;
			_modes.i = false;
			return true;
		case 'o':
			if (!_modes.o)
				return false;
			_modes.o = false;
			return true;
		default:
			return false;
	}
}
