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

	void	setAuthentified();
	void	setNickname(const std::string &nick_name);
	void	setUsername(const std::string &user_name);
	void	setHostname(const std::string &host_name);
	void	setServername(const std::string &server_name);
	void	setRealname(const std::string &real_name);

	bool				isAuthentified() const;
	const std::string &	getMessageReceived() const;
	const std::string &	getMessageToSend() const;
	const std::string &	getNickname() const;
	const std::string &	getUsername() const;
	const std::string &	getHostname() const;
	const std::string &	getServername() const;
	const std::string &	getRealname() const;
	int getFd() const;

private:
	Client();

	// Attributes
private:
	int _fd;
	bool _is_authentified;
	std::string _message_received;
	std::string _message_to_send;
	std::string _nickname;
	std::string _username;
	std::string _hostname;
	std::string _servername;
	std::string _realname;
};

#endif /* CLIENT_HPP */
