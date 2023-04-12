#include "Server.hpp"

// MODE

// Command: MODE
//   Parameters: <target> [<modestring> [<mode arguments>...]]

void Server::mode(Client &client, const std::vector<std::string> &args)
{
	if (args.size() < 2)
	{
		reply(ERR_NEEDMOREPARAMS, client, args);
		return;
	}
	else
	{
		std::string target = args[1];
		// Channel mode
		if (target[0] == '#' || target[0] == '&')
		{
			Channel *channel = searchChan(target);
			// If <target> is a channel that does not exist on the network,
			if (!channel)
			{
				// the ERR_NOSUCHCHANNEL (403) numeric is returned.
				reply(ERR_NOSUCHCHANNEL, client, target);
				return;
			}
			// If <modestring> is not given,		
			if (args.size() == 2)
			{
				// the RPL_CHANNELMODEIS (324) numeric is returned.
				reply(RPL_CHANNELMODEIS, client, *channel);
				return;
			}
			// If <modestring> is given,
			else
			{
				std::string modestring = args[2];
				// the user sending the command MUST have appropriate channel privileges on the target channel to change the modes given.

				// If a user does not have appropriate privileges to change modes on the target channel, the server MUST NOT process the message, and ERR_CHANOPRIVSNEEDED (482) numeric is returned.
				// If the user has permission to change modes on the target, the supplied modes will be applied based on the type of the mode (see below).
				// For type A, B, and C modes, arguments will be sequentially obtained from <mode arguments>.
				// If a type B or C mode does not have a parameter when being set, the server MUST ignore that mode.
				// If a type A mode has been sent without an argument, the contents of the list MUST be sent to the user, unless it contains sensitive information the user is not allowed to access.
				// When the server is done processing the modes, a MODE command is sent to all members of the channel containing the mode changes.
				// Servers MAY choose to hide sensitive information when sending the mode changes.
				
			}
		}
		// User mode
		else
		{
			Client *client_target = searchClient(target);
			// If <target> is a nickname that does not exist on the network,
			if (!client_target)
			{
				// the ERR_NOSUCHNICK (401) numeric is returned. 
				reply(ERR_NOSUCHNICK, client, target);
				return;
			}
			// If <target> is a different nick than the user who sent the command,
			if (target != client.getNickname())
			{
				// the ERR_USERSDONTMATCH (502) numeric is returned.
				reply(ERR_USERSDONTMATCH, client, target);
				return;
			}
			// If <modestring> is not given,
			if (args.size() == 2)
			{
				// the RPL_UMODEIS (221) numeric is sent back containing the current modes of the target user.
				reply(RPL_UMODEIS, client);
				return;
			}
			// If <modestring> is given,
			else
			{
				std::string modestring = args[2];
				// the supplied modes will be applied, and a MODE message will be sent to the user containing the changed modes. 
				
				// If one or more modes sent are not implemented on the server, the server MUST apply the modes that are implemented, and then send the ERR_UMODEUNKNOWNFLAG (501) in reply along with the MODE message.
				
			}
		}
	}

}