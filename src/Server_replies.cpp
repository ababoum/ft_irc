#include "Server.hpp"

void Server::reply(int code, Client &client, const std::vector<std::string> &args)
{
	std::string message;
	std::string nicks = "";

	switch (code)
	{
	case RPL_WELCOME:
		message = "001 " + client.getNickname() + " :Welcome to " + _name + " " + client.getNickname() + "!" + client.getUsername() + "@" + client.getServername() + "\r\n";
		break;
	case ERR_UNKNOWNCOMMAND:
		message = "421 " + client.getNickname() + " " + args[0] + " :Unknown command\r\n";
		break;
	case ERR_NONICKNAMEGIVEN:
		message = "431 " + client.getNickname() + " :No nickname given\r\n";
		break;
	case ERR_ERRONEUSNICKNAME:
		message = "432 " + client.getNickname() + " " + args[1] + " :Erroneus nickname\r\n";
		break;
	case ERR_NICKNAMEINUSE:
		message = "433 " + client.getNickname() + " " + args[1] + " :Nickname is already in use.\r\n";
		break;
	case ERR_NOTREGISTERED:
		message = "451 " + client.getNickname() + " :You have not registered\r\n";
		break;
	case ERR_NEEDMOREPARAMS:
		message = "461 " + client.getNickname() + " " + args[0] + " :Not enough parameters\r\n";
		break;
	case ERR_ALREADYREGISTRED:
		message = "462 " + client.getNickname() + " :You may not reregister\r\n";
		break;
	case ERR_PASSWDMISMATCH:
		message = "464 " + client.getNickname() + " :Password incorrect\r\n";
		break;
	case RPL_ENDOFWHOIS:
		for (size_t i = 1; i < args.size(); i++)
		{
			nicks += args[i];
			if (i != args.size() - 1)
				nicks += " ";
		}
		message = "318 " + client.getNickname() + " " + nicks + " :End of /WHOIS list\r\n";
		break;

	default:
		DEBUG("reply: unknown code: " << code << "\n");
		break;
	}
	client.appendMessageToSend(message);
}

void Server::reply(int code, Client &client, const Channel &channel)
{
	std::string message;

	switch (code)
	{
	case RPL_TOPIC:
		message = "332 " + client.getNickname() + " " + channel.getName() + " :" + channel.getTopic() + "\r\n";
		break;
	case RPL_TOPICWHOTIME:
		message = "333 " + client.getNickname() + " " + channel.getName() + " " + channel.getTopicSetBy()->getNickname() + " " + ft_itoa(channel.getTopicSetAt()) + "\r\n";
		break;
	case RPL_NAMREPLY:
		message = "353 " + client.getNickname() + " =" + channel.getName() + " :";
		for (size_t j = 0; j < channel.getClients().size(); j++)
		{
			const Client *tmp = channel.getClients()[j];
			if (j != 0)
				message += " ";
			message += tmp->getNickname();
		}
		message += "\r\n";
		break;
	case RPL_ENDOFNAMES:
		message = "366 " + client.getNickname() + " " + channel.getName() + " :End of /NAMES list\r\n";
		break;
	case ERR_NOTONCHANNEL:
		message = "442 " + client.getNickname() + " " + channel.getName() + " :You're not on that channel\r\n";
		break;

	default:
		DEBUG("reply: unknown code: " << code << "\n");
		break;
	}
	client.appendMessageToSend(message);
}

void Server::who_reply(int code, Client &client, Channel *channel, const Client &target)
{
	std::string message;
	std::string channel_name = channel == NULL ? "*" : channel->getName();

	switch (code)
	{
	case RPL_WHOREPLY:
		message = "352 " + client.getNickname() + " " + channel_name +
				  " " + target.getUsername() + " " + target.getServername() +
				  " " + target.getNickname() + " H :0 " + target.getRealname() +
				  "\r\n";
		break;
	case RPL_WHOISUSER:
		message = "311 " + client.getNickname() + " " + target.getNickname() +
				  " " + target.getUsername() + " " + target.getServername() +
				  " * :" + target.getRealname() + "\r\n";
		break;
	case RPL_WHOISCHANNELS:
		message = "319 " + client.getNickname() + " " + target.getNickname() + " :";
		for (size_t i = 0; i < target.getJoinedChannels().size(); i++)
		{
			if (i != 0)
				message += " ";
			message += target.getJoinedChannels()[i]->getName();
		}
		message += "\r\n";
		break;

	default:
		DEBUG("reply: unknown code: " << code << "\n");
		break;
	}
	client.appendMessageToSend(message);
}

void Server::reply(int code, Client &client, const std::string &mask)
{
	std::string message;

	switch (code)
	{
	case RPL_ENDOFWHO:
		message = "315 " + client.getNickname() + " " +
				  mask + " :End of WHO list\r\n";
		break;
	case ERR_NOSUCHCHANNEL:
		message = "403 " + client.getNickname() + " " + mask + " :No such channel\r\n";
		break;
	case ERR_NOSUCHNICK:
		message = "401 " + client.getNickname() + " " + mask + " :No such nick/channel\r\n";
		break;

	default:
		DEBUG("reply: unknown code: " << code << "\n");
		break;
	}
	client.appendMessageToSend(message);
}