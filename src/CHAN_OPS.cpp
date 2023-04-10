#include "Server.hpp"

// JOIN
// PART
// NAMES
// KICK
// INVITE

void Server::invite(Client &client, const std::vector<std::string> &args)
{
	RUNTIME_MSG("invite function called\n");

	if (args.size() < 3)
	{
		reply(ERR_NEEDMOREPARAMS, client, args);
		return;
	}

	Channel *channel = searchChan(args[2]);
	if (!channel)
	{
		reply(ERR_NOSUCHCHANNEL, client, args[2]);
		return;
	}

	// Check if the client performing the invite is on the channel
	Client *chan_client = channel->searchClient(client.getNickname());
	if (!chan_client)
	{
		reply(ERR_NOTONCHANNEL, client, *channel);
		return;
	}

	// Check if the client performing the invite is an operator
	if (!channel->isOperator(chan_client))
	{
		reply(ERR_CHANOPRIVSNEEDED, client, *channel);
		return;
	}

	std::string target = args[1];
	Client *target_client = searchClient(target);
	// Check if the target client exists
	if (!target_client)
	{
		reply(ERR_NOSUCHNICK, client, target);
		return;
	}
	// Check if the target client is already on the channel
	if (target_client && channel->searchClient(target))
	{
		reply(ERR_USERONCHANNEL, client, channel, *target_client);
		return;
	}

	std::string message = ":" + client.getNickname() +
						  " INVITE " + target + " " +
						  channel->getName() + "\n";

	// Send invite message to the target client
	if (target_client)
		target_client->appendMessageToSend(message);
	// Send invite message to the client performing the invite
	reply(RPL_INVITING, client, channel, *target_client);

	// add the target client to the channel's invited list
	// TO DO
}

void Server::kick(Client &client, const std::vector<std::string> &args)
{
	RUNTIME_MSG("kick function called\n");

	std::string comment = "No comment";
	if (args.size() < 3)
	{
		reply(ERR_NEEDMOREPARAMS, client, args);
		return;
	}

	if (args.size() > 3)
		comment = args[3];

	Channel *channel = searchChan(args[1]);
	if (!channel)
	{
		reply(ERR_NOSUCHCHANNEL, client, args);
		return;
	}

	// Check if the client performing the kick is on the channel
	Client *chan_client = channel->searchClient(client.getNickname());
	if (!chan_client)
	{
		reply(ERR_NOTONCHANNEL, client, *channel);
		return;
	}

	// Check if the client performing the kick is an operator
	if (!channel->isOperator(chan_client))
	{
		reply(ERR_CHANOPRIVSNEEDED, client, *channel);
		return;
	}

	std::string target = args[2];
	Client *target_client = channel->searchClient(target);
	// Check if the target client is on the channel
	if (!target_client)
	{
		reply(ERR_USERNOTINCHANNEL, client, channel, target);
		return;
	}

	std::string message = ":" + client.getNickname() +
						  " KICK " + channel->getName() + " " +
						  target + " :" + comment + "\n";

	client.appendMessageToSend(message);
	// Remove client from channel
	channel->removeClient(target_client);
	target_client->removeChan(channel->getName());

	// Broadcast kick message to all clients in the channel
	channel->fullBroadcast(message);
}

void Server::join(Client &client, const std::vector<std::string> &args)
{
	RUNTIME_MSG("join function called\n");

	if (args.size() == 1)
	{
		reply(ERR_NEEDMOREPARAMS, client, args);
		return;
	}
	if (args[1] == "0")
	{
		// Send PART message to all chans the client is connected to
		return;
	}
	std::vector<std::string> channels = split(args[1], ",");
	for (size_t i = 0; i < channels.size(); i++)
	{
		// Check name >> 476 ERR_BADCHANMASK + return
		if (!Channel::isChannelNameValid(channels[i]))
		{
			reply(ERR_BADCHANMASK, client, channels[i]);
			return;
		}

		Channel *channel = searchChan(channels[i]);
		if (!channel)
			channel = addChan(channels[i], &client);
		channel->addClient("", &client);
		client.addChan(channel);
		// JOIN Message
		std::string message = ":" + client.getNickname() + " JOIN " +
							  channels[i] + "\n";
		// Broadcast join message to all other clients in the channel
		// The message is the same as the one sent to the main client
		channel->fullBroadcast(message);
		// client.appendMessageToSend(message);
		if (channel->getTopic().size() > 0)
		{
			reply(RPL_TOPIC, client, *channel);
			reply(RPL_TOPICWHOTIME, client, *channel);
		}
		reply(RPL_NAMREPLY, client, *channel);
		reply(RPL_ENDOFNAMES, client, *channel);
	}
}

void Server::part(Client &client, const std::vector<std::string> &args)
{
	RUNTIME_MSG("part function called\n");
	if (args.size() < 2)
	{
		reply(ERR_NEEDMOREPARAMS, client, args);
		return;
	}
	std::vector<std::string> channels = split(args[1], ",");
	for (size_t i = 0; i < channels.size(); i++)
	{
		Channel *channel = searchChan(channels[i]);
		if (!channel)
		{
			// 1 ERR_NOSUCHCHANNEL 403 for each channel that does not exist
			reply(ERR_NOSUCHCHANNEL, client, channels[i]);
			continue;
		}
		Client *chan_client = channel->searchClient(client.getNickname());
		if (!chan_client)
		{
			// 1 ERR_NOTONCHANNEL 442 for each channel the client is not on
			reply(ERR_NOTONCHANNEL, client, *channel);
			continue;
		}
		// 1 PART message for each channel the client is leaving
		// BROADCAST TO OTHER CLIENTS IN THE CHANNEL
		std::string message = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getServername() + " PART " + channel->getName();
		if (args.size() > 2)
			message += " " + args[2];
		message += "\r\n";
		channel->fullBroadcast(message);
		client.removeChan(channel->getName());
		removeClientFromChannel(&client, channel);
	}
}

void Server::topic(Client &client, const std::vector<std::string> &args)
{
	if (args.size() < 2)
	{
		reply(ERR_NEEDMOREPARAMS, client, args);
		return ;
	}
	Channel *channel = searchChan(args[1]);
	if (!channel)
		reply(ERR_NOSUCHCHANNEL, client, args[1]);
	else if (!channel->searchClient(client.getNickname()))
		reply(ERR_NOTONCHANNEL, client, *channel);
	else if (args.size() == 2)
	{
		// Send topic of channel args[1] to client
		if (channel->getTopic() == "")
			reply(RPL_NOTOPIC, client, *channel);
		else
		{
			reply(RPL_TOPIC, client, *channel);
			reply(RPL_TOPICWHOTIME, client, *channel);
		}
	}
	else
	{
		// if protected topic mode, verify permissions
		// >> si pas de permission ERR_CHANOPRIVSNEED (482)
		// change topic of chan args[1] to args[2]
		channel->setTopic(args[2]);
		channel->setTopicSetBy(&client);
		channel->setTopicSetAt(time(NULL));
		std::string message = ":" + client.getHostname() + " TOPIC " + channel->getName() + " " + channel->getTopic() + "\r\n";
		channel->fullBroadcast(message);
	}
}
