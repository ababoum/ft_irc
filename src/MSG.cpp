#include "Server.hpp"

// PRIVMSG

void Server::privmsg(Client &client, const std::vector<std::string> &args)
{
	RUNTIME_MSG("privmsg function called\n");

	if (args.size() < 3)
	{
		reply(ERR_NEEDMOREPARAMS, client, args);
		return;
	}
	// Check if the target is a channel
	if (args[1][0] == '#')
	{
		Channel *channel = searchChan(args[1]);
		if (!channel)
		{
			reply(ERR_NOSUCHCHANNEL, client, args[1]);
			return;
		}
		// Check if the client is in the channel
		Client *chan_client = channel->searchClient(client.getNickname());
		if (!chan_client)
		{
			reply(ERR_NOTONCHANNEL, client, args[1]);
			return;
		}
		// Send the message to all the clients in the channel
		std::string message = ":" + client.getNickname() +
							  " PRIVMSG " + args[1] + " :" + args[2] + "\r\n";
		channel->broadcast(message, chan_client);
	}
	else
	{
		// Check if the target exists
		Client *target = searchClient(args[1]);
		if (!target)
		{
			reply(ERR_NOSUCHNICK, client, args[1]);
			return;
		}
		// Abort if the message is self-sent
		if (target == &client)
			return;
		// Send the message to the target
		std::string message = ":" + client.getNickname() +
							  " PRIVMSG " + args[1] + " :" + args[2] + "\r\n";
		target->appendMessageToSend(message);
	}
}

