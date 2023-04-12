#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "ft_irc.hpp"

class Channel;

class Client
{
public:
	Client(int fd);
	Client(const Client &other);
	Client &operator=(const Client &rhs);
	~Client();

	static bool	isNicknameValid(const std::string &nickname);
	
	void	appendMessageReceived(const std::string &str);
	void	appendMessageReceived(const char *str);
	void	appendMessageToSend(const std::string &str);
	void	appendMessageToSend(const char *str);
	void	clearMessageReceived();
	void	clearMessageToSend();

	void	setPass(bool pass);
	void	setAuthentified();
	void	setFatalError();
	void	setNickname(const std::string &nick_name);
	void	setUsername(const std::string &user_name);
	void	setHostname(const std::string &host_name);
	void	setServername(const std::string &server_name);
	void	setRealname(const std::string &real_name);
	void	addChan(Channel *channel);
	void 	removeChan(std::string name);

	bool				isPassOk() const;
	bool				isAuthentified() const;
	bool				isFatalError() const;
	const std::string &	getMessageReceived() const;
	const std::string &	getMessageToSend() const;
	const std::string &	getNickname() const;
	const std::string &	getUsername() const;
	const std::string &	getHostname() const;
	const std::string &	getServername() const;
	const std::string &	getRealname() const;
	const std::string getSource() const;
	const std::vector<Channel *> &getJoinedChannels() const;
	int getFd() const;

private:
	Client();

	// Attributes
private:
	int			_fd;
	bool 		_pass;
	bool		_is_authentified;
	bool		_fatal_error;
	std::string _message_received;
	std::string _message_to_send;
	std::string _nickname;
	std::string _username;
	std::string _hostname;
	std::string _servername;
	std::string _realname;
	std::vector<Channel *> _joined_channels;
	std::string _mode;
};

#endif /* CLIENT_HPP */
