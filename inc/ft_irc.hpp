#ifndef FT_IRC_HPP
#define FT_IRC_HPP

#include <string>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <cstdio>
#include <arpa/inet.h>
#include <sys/select.h>
#include <vector>
#include <map>
#include <set>
#include <ctime>
#include <sstream>
#include <cstring>
#include <algorithm>

#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

#define SERVER_NAME "ircserv"
#define MAX_CLIENTS 1000
#define READ_SIZE 4096

#define RED_TXT "\e[31m"
#define GREEN_TXT "\e[32m"
#define YELLOW_TXT "\e[33m"
#define BLUE_TXT "\e[34m"
#define MAG_TXT "\e[35m"

#define BOLD_TXT "\e[1m"
#define ITALIC_TXT "\e[2m"
#define UL_TXT "\e[4m"
#define RESET_TXT "\e[0m"

#define DEBUG(x) std::cerr << YELLOW_TXT << x << RESET_TXT;
#define FATAL_ERR(x) std::cerr << RED_TXT << x << RESET_TXT;
#define INFO(x) std::cerr << ITALIC_TXT << x << RESET_TXT;
#define RUNTIME_MSG(x) std::cerr << GREEN_TXT << x << RESET_TXT;

#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

#define BOT_NAME "bot"

enum reply_code
{
	RPL_WELCOME = 001,
	RPL_UMODEIS = 221,				// ToDo: implement reply
	RPL_WHOISUSER = 311,
	RPL_ENDOFWHO = 315,
	RPL_ENDOFWHOIS = 318,
	RPL_WHOISCHANNELS = 319,
	RPL_LISTSTART = 321,
	RPL_LIST = 322,
	RPL_LISTEND = 323,
	RPL_CHANNELMODEIS = 324,		// ToDo: implement reply
	RPL_NOTOPIC = 331,
	RPL_TOPIC = 332,
	RPL_TOPICWHOTIME = 333,
	RPL_INVITING = 341,
	RPL_WHOREPLY = 352,
	RPL_NAMREPLY = 353,
	RPL_ENDOFNAMES = 366,
	RPL_YOUREOPER = 381,
	ERR_NOSUCHNICK = 401,
	ERR_NOSUCHCHANNEL = 403,
	ERR_UNKNOWNCOMMAND = 421,
	ERR_NONICKNAMEGIVEN = 431,
	ERR_ERRONEUSNICKNAME = 432,
	ERR_NICKNAMEINUSE = 433,
	ERR_USERNOTINCHANNEL = 441,
	ERR_NOTONCHANNEL = 442,
	ERR_USERONCHANNEL = 443,
	ERR_NOTREGISTERED = 451,
	ERR_NEEDMOREPARAMS = 461,
	ERR_ALREADYREGISTRED = 462,
	ERR_PASSWDMISMATCH = 464,
	ERR_BADCHANMASK = 476,
	ERR_CHANOPRIVSNEEDED = 482,
	ERR_UMODEUNKNOWNFLAG = 501,		// ToDo: implement reply
	ERR_USERSDONTMATCH = 502		// ToDo: implement reply
};

std::vector<std::string>	split(const std::string &str, char delim);
std::vector<std::string>	split(const std::string &str, const char *set_delim);

bool		ft_is_number(const std::string &str);
int			ft_parse_port(const std::string &str);
void		ft_check_password(const std::string &str);
std::string	ft_itoa(int n);
std::string to_lower(const std::string &str);

#endif /* FT_IRC_HPP */
