#include "Server.hpp"

// PRIVMSG
// NOTICE

static void _bot(Client &client, const std::string &msg, const std::string &serv_name)
{
	std::string answer = "I'm a bot 🤖, I don't understand your message.";

	if (to_lower(msg) == "hello")
	{
		answer = "Hello " + client.getNickname() + "!";
	}
	else if (to_lower(msg) == "help")
	{
		answer =
			"Find all about this IRC server here: https://modern.ircdocs.horse/";
	}

	// Send the message to the target
	std::string sender = std::string("bot!bot@" + serv_name);
	std::string message = ":" + sender +
						  " PRIVMSG " + client.getNickname() + " :" + answer + "\r\n";
	client.appendMessageToSend(message);
}

/**
 * @brief NOTICE message handler
 * Automatic replies must never be sent in response to a NOTICE message.
 **/
void Server::notice(Client &client, const std::vector<std::string> &args)
{
	RUNTIME_MSG("notice function called\n");

	if (args.size() < 3)
	{
		return;
	}
	// Check if the target is a channel
	if (args[1][0] == '#')
	{
		Channel *channel = searchChan(args[1]);
		if (!channel)
		{
			return;
		}
		// Check if the client is in the channel
		Client *chan_client = channel->searchClient(client.getNickname());
		if (!chan_client)
		{
			return;
		}
		// Send the message to all the clients in the channel
		std::string message = ":" + client.getSource() +
							  " NOTICE " + args[1] + " :" + args[2] + "\r\n";
		channel->broadcast(message, chan_client);
	}
	else
	{
		// Check if the target is the bot
		if (args[1] == "bot")
		{
			_bot(client, args[2], _name);
			return;
		}

		// Check if the target exists
		Client *target = searchClient(args[1]);
		if (!target)
		{
			return;
		}
		// Abort if the message is self-sent
		if (target == &client)
			return;
		// Send the message to the target
		std::string message = ":" + client.getSource() +
							  " NOTICE " + args[1] + " :" + args[2] + "\r\n";
		target->appendMessageToSend(message);
	}
}

/**
 * @brief PRIVMSG message handler
 * Automatic replies must never be sent in response to a PRIVMSG message.
 **/
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
		std::string message = ":" + client.getSource() +
							  " PRIVMSG " + args[1] + " :" + args[2] + "\r\n";
		channel->broadcast(message, chan_client);
	}
	else
	{
		// Check if the target is the bot
		if (args[1] == "bot")
		{
			_bot(client, args[2], _name);
			return;
		}
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
		std::string message = ":" + client.getSource() +
							  " PRIVMSG " + args[1] + " :" + args[2] + "\r\n";
		target->appendMessageToSend(message);
	}
}
