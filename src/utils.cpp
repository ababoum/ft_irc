#include "ft_irc.hpp"

// CPP spliter
std::vector<std::string> split(const std::string &str, char delim)
{
	std::size_t i = 0;
	std::vector<std::string> list;

	std::size_t pos = str.find(delim);

	while (pos != std::string::npos)
	{
		if (pos == i)
		{
			i = ++pos;
		}
		else
		{
			list.push_back(str.substr(i, pos - i));
			i = ++pos;
		}
		pos = str.find(delim, pos);
	}

	if (i != str.length())
		list.push_back(str.substr(i, str.length()));

	return list;
}

std::vector<std::string> split(const std::string &str, const char *set_delim)
{
	std::size_t i = 0;
	std::vector<std::string> list;

	std::size_t pos = str.find_first_of(set_delim);

	while (pos != std::string::npos)
	{
		if (pos == i)
		{
			i = ++pos;
		}
		else
		{
			list.push_back(str.substr(i, pos - i));
			i = ++pos;
		}
		pos = str.find_first_of(set_delim, pos);
	}

	if (i != str.length())
		list.push_back(str.substr(i, str.length()));

	return list;
}

bool ft_is_number(const std::string &str)
{
	return !str.empty() &&
		   (str.find_first_not_of("0123456789") == std::string::npos);
}

int ft_parse_port(const std::string &str)
{
	int port;

	if (!ft_is_number(str))
		throw std::invalid_argument("Port number is not valid");
	port = std::atoi(str.c_str());
	if (port > 65535)
		throw std::invalid_argument("Bad/illegal port format (max value: 65535)");
	return (port);
}

void ft_check_password(const std::string &str)
{
	if (str.empty())
		throw std::invalid_argument("Empty password");
	for (int i = 0; str[i]; i++)
	{
		if (!std::isprint(str[i]))
			throw std::invalid_argument("Only printables characters are allowed");
	}
}
