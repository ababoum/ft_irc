#include "Server.hpp"

// WHO
// WHOIS


// Warning: what happens if we query about both a channel and a nickname?
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
						reply(RPL_WHOREPLY, client, found, *found->getClients()[k]);
					}
					reply(RPL_ENDOFWHO, client, args[i]);
				}
			}
			if (!found)
			{
				// 403
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
	for (size_t i = 1; i < args.size(); i++)
	{
		// Check if the nickname exists
		bool found = false;
		for (size_t j = 0; j < _clients.size(); j++)
		{
			if (_clients[j]->getNickname() == args[i])
			{
				found = true;
				reply(RPL_WHOISUSER, client, NULL, *_clients[j]);
				reply(RPL_WHOISCHANNELS, client, NULL, *_clients[j]);
				// We can add more if necessary...
			}
		}
		if (!found)
		{
			// 401
			reply(ERR_NOSUCHNICK, client, args[i]);
		}
	}
	reply(RPL_ENDOFWHOIS, client, args);
}