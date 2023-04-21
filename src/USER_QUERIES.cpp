#include "Server.hpp"

// WHO
// WHOIS

void Server::who(Client &client, const std::vector<std::string> &args)
{
	RUNTIME_MSG("who function called\n");

	if (args.size() == 1)
	{
		reply(ERR_NEEDMOREPARAMS, client, args);
		return;
	}

	// WHO command can have multiple arguments
	for (size_t i = 1; i < args.size(); i++)
	{
		// Check if the argument is a channel name
		if (args[i][0] == '#')
		{
			// Check if the channel exists
			Channel *found = NULL;
			for (size_t j = 0; j < _channels.size(); j++)
			{
				if (_channels[j]->getName() == args[i])
				{
					found = _channels[j];
					for (size_t k = 0; k < found->getClients().size(); k++)
					{
						// the client is listed only if he is not invisible
						// or if he is in the same channel as the client who sent the command
						if (!found->getClients()[k].second->isInvisible() ||
							found->searchClient(client.getNickname()))
						{
							reply(RPL_WHOREPLY, client, found, *found->getClients()[k].second);
						}
					}
					reply(RPL_ENDOFWHO, client, args[i]);
				}
			}
			if (!found)
			{
				reply(ERR_NOSUCHCHANNEL, client, args[i]);
			}
		}
		else
		{
			// Check if the user exists
			bool found = false;
			for (size_t j = 0; j < _clients.size(); j++)
			{
				if (_clients[j]->getNickname() == args[i])
				{
					found = true;
					reply(RPL_WHOREPLY, client, NULL, *_clients[j]);
					reply(RPL_ENDOFWHO, client, args[i]);
				}
			}
			if (!found)
			{
				// 401
				reply(ERR_NOSUCHNICK, client, args[i]);
			}
		}
	}
}

void Server::whois(Client &client, const std::vector<std::string> &args)
{
	RUNTIME_MSG("who function called\n");

	if (args.size() == 1)
	{
		reply(ERR_NONICKNAMEGIVEN, client, args);
		return;
	}

	// WHOIS command can have multiple arguments
	// An arg can be a comma-separated list of nicknames
	for (size_t i = 1; i < args.size(); i++)
	{
		std::vector<std::string> nicks = split(args[i], ',');
		for (size_t j = 0; j < nicks.size(); j++)
		{
			// Check if the nickname exists
			Client *found = searchClient(nicks[j]);
			if (found)
			{
				reply(RPL_WHOISUSER, client, NULL, *_clients[j]);
				reply(RPL_WHOISCHANNELS, client, NULL, *_clients[j]);
				if (_clients[j]->isOperator())
					reply(RPL_WHOISOPERATOR, client, NULL, *_clients[j]);
				// We can add more if necessary...
			}
			else
			{
				reply(ERR_NOSUCHNICK, client, args[i]);
			}
		}
	}
	reply(RPL_ENDOFWHOIS, client, args);
}