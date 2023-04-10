#include "Server.hpp"

void Server::reply(int code, Client &client, const std::vector<std::string> &args)
{
	std::string nickname(client.getNickname());
	if (nickname.empty())
		nickname = "*";
	std::string message;
	std::string nicks = "";
	std::string prefix = SSTR(code) + " " + nickname + " ";

	switch (code)
	{
	case RPL_WELCOME:
		message = "001 " + nickname + " :Welcome to " + _name + " " + nickname + "!" + client.getUsername() + "@" + client.getServername() + "\r\n";
		break;
	case ERR_UNKNOWNCOMMAND:
		message = prefix + args[0] + " :Unknown command\r\n";
		break;
	case ERR_NONICKNAMEGIVEN:
		message = "431 " + nickname + " :No nickname given\r\n";
		break;
	case ERR_ERRONEUSNICKNAME:
		message = prefix + args[1] + " :Erroneus nickname\r\n";
		break;
	case ERR_NICKNAMEINUSE:
		message = prefix + args[1] + " :Nickname is already in use.\r\n";
		break;
	case ERR_NOTREGISTERED:
		message = prefix + ":You have not registered\r\n";
		break;
	case ERR_NEEDMOREPARAMS:
		message = prefix + args[0] + " :Not enough parameters\r\n";
		break;
	case ERR_ALREADYREGISTRED:
		message = prefix + ":You may not reregister\r\n";
		break;
	case ERR_PASSWDMISMATCH:
		message = prefix + ":Password incorrect\r\n";
		break;
	case RPL_ENDOFWHOIS:
		for (size_t i = 1; i < args.size(); i++)
		{
			nicks += args[i];
			if (i != args.size() - 1)
				nicks += " ";
		}
		message = prefix + nicks + " :End of /WHOIS list\r\n";
		break;

	default:
		DEBUG("reply: unknown code: " << code << "\n");
		break;
	}
	client.appendMessageToSend(message);
}

void Server::reply(int code, Client &client, const Channel &channel)
{
	std::string nickname(client.getNickname());
	std::string message;
	std::string prefix = SSTR(code) + " " + nickname + " ";

	switch (code)
	{
	case RPL_NOTOPIC:
		message = prefix + channel.getName() + " :No topic is set\r\n";
		break;
	case RPL_TOPIC:
		message = prefix + channel.getName() + " :" + channel.getTopic() + "\r\n";
		break;
	case RPL_TOPICWHOTIME:
		message = prefix + channel.getName() + " " + channel.getTopicSetBy()->getNickname() 
				  + "!" + channel.getTopicSetBy()->getUsername() 
				  + "@" + channel.getTopicSetBy()->getServername() 
				  + " " + ft_itoa(channel.getTopicSetAt()) + "\r\n";
		break;
	case RPL_NAMREPLY:
		message = prefix + "= " + channel.getName() + " :";
		for (size_t j = 0; j < channel.getClients().size(); j++)
		{
			const std::pair<std::string, Client *> tmp = channel.getClients()[j];
			// ADD blocage if (invisible mode and requesting client not on chan!!!!!!!!!!!!)
			if (j != 0)
				message += " ";
			message += tmp.first + tmp.second->getNickname();
		}
		message += "\r\n";
		break;
	case RPL_ENDOFNAMES:
		message = prefix + channel.getName() + " :End of /NAMES list\r\n";
		break;
	case ERR_NOTONCHANNEL:
		message = prefix + channel.getName() + " :You're not on that channel\r\n";
		break;
	case ERR_CHANOPRIVSNEEDED:
		message = prefix + channel.getName() + " :You're not channel operator\r\n";
		break;

	default:
		DEBUG("reply: unknown code: " << code << "\n");
		break;
	}
	client.appendMessageToSend(message);
}

void Server::reply(int code, Client &client, Channel *channel, const Client &target)
{
	std::string nickname(client.getNickname());
	std::string message;
	std::string channel_name = channel == NULL ? "*" : channel->getName();
	std::string prefix = SSTR(code) + " " + nickname + " ";

	switch (code)
	{
	case RPL_WHOREPLY:
		message = prefix + channel_name +
				  " " + target.getUsername() + " " + target.getServername() +
				  " " + target.getNickname() + " H :0 " + target.getRealname() +
				  "\r\n";
		break;
	case RPL_WHOISUSER:
		message = prefix + target.getNickname() +
				  " " + target.getUsername() + " " + target.getServername() +
				  " * :" + target.getRealname() + "\r\n";
		break;
	case RPL_WHOISCHANNELS:
		message = prefix + target.getNickname() + " :";
		for (size_t i = 0; i < target.getJoinedChannels().size(); i++)
		{
			if (i != 0)
				message += " ";
			message += target.getJoinedChannels()[i]->getName();
		}
		message += "\r\n";
		break;

	case RPL_INVITING:
		message = prefix + target.getNickname() +
				  " " + channel_name + "\r\n";
		break;

	case ERR_USERONCHANNEL:
		message = prefix + target.getNickname() +
				  " " + channel_name + " :is already on channel\r\n";
		break;

	default:
		DEBUG("reply: unknown code: " << code << "\n");
		break;
	}
	client.appendMessageToSend(message);
}

void Server::reply(int code, Client &client, const std::string &mask)
{
	std::string nickname(client.getNickname());
	std::string message;
	std::string prefix = SSTR(code) + " " + nickname + " ";

	switch (code)
	{
	case RPL_ENDOFWHO:
		message = prefix + mask + " :End of WHO list\r\n";
		break;
	case RPL_NAMREPLY:
		message = prefix + "= " + mask + " :";
		for (size_t i = 0; i < _clients.size(); i++)
		{
			// if client is not invisible
				std::vector<Channel *> channels = _clients[i]->getJoinedChannels();
				int nb_visible_chan = 0;
				for (size_t j = 0; j < channels.size(); j++)
				{
					// if channels[j] is not secret mode OU if requesting client on chan
					nb_visible_chan++;
				}
				if (nb_visible_chan == 0)
				{
					if (i != 0)
						message += " ";
					message += _clients[i]->getNickname();
				}
		}
		message += "\r\n";
		break;
	case RPL_ENDOFNAMES:
		message = prefix + mask + " :End of /NAMES list\r\n";
		break;
	case ERR_NOSUCHCHANNEL:
		message = prefix + mask + " :No such channel\r\n";
		break;
	case ERR_NOSUCHNICK:
		message = prefix + mask + " :No such nick/channel\r\n";
		break;

	case ERR_BADCHANMASK:
		message = prefix + mask + " :Bad Channel Mask\r\n";
		break;

	default:
		DEBUG("reply: unknown code: " << code << "\n");
		break;
	}
	client.appendMessageToSend(message);
}

void Server::reply(int code, Client &client, Channel *channel, const std::string &mask)
{
	std::string nickname(client.getNickname());
	std::string message;
	std::string channel_name = channel == NULL ? "*" : channel->getName();
	std::string prefix = SSTR(code) + " " + nickname + " ";

	switch (code)
	{
	case ERR_USERNOTINCHANNEL:
		message = prefix + mask + " " + channel_name +
				  " :They aren't on that channel\r\n";
		break;

	default:
		DEBUG("reply: unknown code: " << code << "\n");
		break;
	}
	client.appendMessageToSend(message);
}
