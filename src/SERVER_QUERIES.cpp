#include "Server.hpp"

// MODE

// Command: MODE
//   Parameters: <target> [<modestring> [<mode arguments>...]]

static const std::string CHANNEL_MODES = "oit";
// o value = nickname of client
// i no value
// t no value
static const std::string USER_MODES = "io";
// o no value
// i no value

static const std::string parseUserModestring(const std::string &modestring)
{
	bool add = true;
	std::string plus;
	std::string minus;

	for (size_t i = 0; i < modestring.size(); i++)
	{
		if (modestring[i] == '+')
			add = true;
		else if (modestring[i] == '-')
			add = false;
		else
		{
			if (plus.find(modestring[i]) == std::string::npos && minus.find(modestring[i]) == std::string::npos) 
			{
				if (add)
					plus += modestring[i];
				else
					minus += modestring[i];
			}
		}
	}
	return std::string("+") + plus + std::string("-") + minus;
}

static void parseChannelModestring(const std::string &modestring, const std::vector<std::string> &args, std::string &modes, std::string &parameters_modes)
{
	bool add = true;
	std::string plus;
	std::string minus;
	size_t args_index = 3;

	(void)args;
	for (size_t i = 0; i < modestring.size(); i++)
	{
		if (modestring[i] == '+')
			add = true;
		else if (modestring[i] == '-')
			add = false;
		else
		{
			if (modestring[i] == 'o' && args.size() > args_index)
			{
				if (add)
					parameters_modes += "+o";
				else
					parameters_modes += "-o";
				parameters_modes += args[args_index];
				args_index++;
			}
			else if (plus.find(modestring[i]) == std::string::npos && minus.find(modestring[i]) == std::string::npos) 
			{
				if (add)
					plus += modestring[i];
				else
					minus += modestring[i];
			}
		}
	}
	modes = std::string("+") + plus + std::string("-") + minus + parameters_modes;
}


std::string Server::applyModestring(const std::string &modes, const std::string &params_modes, Client &client, Channel &channel)
{
	bool add = true;
	bool (Channel::*f[])(char c, std::string nickname) = {
		&Channel::removeMode,
		&Channel::addMode};
	std::string message = "+";
	std::string parameters;
	std::string nickname;

	// Add simple modes
	for (size_t i = 1; i < modes.size(); i++)
	{
		if (modes[i] == '-')
		{
			add = false;
			message += "-";
		}
		// if the mode is not known to the server retourner une erreur
		else if (std::find(CHANNEL_MODES.begin(), CHANNEL_MODES.end(), modes[i]) == CHANNEL_MODES.end())
		{
			reply(ERR_UNKNOWNMODE, client, std::string(1, modes[i]));
		}
		// sinon changer le mode et ajouter au message
		else
		{
			if ((channel.*f[add])(modes[i], ""))
				message += modes[i];
		}
	}
	// Add modes that require parameters
	for (size_t i = 0; i < params_modes.size(); i++)
	{
		if (params_modes[i] == '+')
		{
			add = true;
			message += "+";
		}
		else if (params_modes[i] == '-')
		{
			add = false;
			message += "-";
		}
		// enregistrer le nickname du client sur qui le mode o est appelé
		else
		{
			while (i < params_modes.size() && params_modes[i] != '+' && params_modes[i] != '-')
			{
				nickname += params_modes[i];
				i++;
			}
			// vérifier si le client existe sur le channel
			Client *client_target = channel.searchClient(nickname);
			if (!client_target)
				reply(ERR_NOTONCHANNEL, client, channel);
			else
			{
				if ((channel.*f[add])('o', nickname))
				{
					message += "o";
					parameters += " " + nickname;
				}
			}
			nickname.clear();
			i--;
		}
	}
	// Trim message from double + and -
	for (size_t i = 1; i <= message.size(); i++)
	{
		if ((i == message.size() || message[i] == '+' || message[i] == '-') && 
			(message[i - 1] == '+' || message[i - 1] == '-'))
		{
			message.erase(i - 1, 1);
			i--;
		}
	}
	for (size_t i = 1; i <= parameters.size(); i++)
	{
		if ((i == parameters.size() || parameters[i] == '+' || parameters[i] == '-') && 
			(parameters[i - 1] == '+' || parameters[i - 1] == '-'))
		{
			parameters.erase(i - 1, 1);
			i--;
		}
	}
	return message + parameters;
}

std::string Server::applyModestring(const std::string &modes, Client &client)
{
	bool add = true;
	std::string message;
	bool (Client::*f[])(char c) = {
		&Client::removeMode,
		&Client::addMode};

	for (size_t i = 0; i < modes.size(); i++)
	{
		if (modes[i] == '+')
		{
			add = true;
			message += "+";
		}
		else if (modes[i] == '-')
		{
			add = false;
			message += "-";
		}
		// If one or more modes sent are not implemented on the server, the server MUST apply the modes that are implemented, and then send the ERR_UMODEUNKNOWNFLAG (501) in reply along with the MODE message.
		else if (std::find(USER_MODES.begin(), USER_MODES.end(), modes[i]) == USER_MODES.end())
		{
			reply(ERR_UMODEUNKNOWNFLAG, client);
		}
		else
		{
			if ((client.*f[add])(modes[i]))
			{
				message += std::string(1, modes[i]);
			}
		}
	}
	// Trim message from double + and -
	for (size_t i = 1; i <= message.size(); i++)
	{
		if ((i == message.size() || message[i] == '+' || message[i] == '-') && 
			(message[i - 1] == '+' || message[i - 1] == '-'))
		{
			message.erase(i - 1, 1);
			i--;
		}
	}
	return message;
}

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
			// the user sending the command MUST have appropriate channel privileges on the target channel to change the modes given.
			// If a user does not have appropriate privileges to change modes on the target channel, the server MUST NOT process the message, and ERR_CHANOPRIVSNEEDED (482) numeric is returned.
			if (!channel->isOperator(&client))
			{
				// ERR_CHANOPRIVSNEEDED 482
				reply(ERR_CHANOPRIVSNEEDED, client, *channel);
				return;
			}
			else
			{
				std::string modes; 
				std::string parameters_mode;
				std::string message; 
			
				parseChannelModestring(args[2], args, modes, parameters_mode);
				message = applyModestring(modes, parameters_mode, client, *channel);
				if (message.size() != 0)
					client.appendMessageToSend(":" + client.getSource() + " MODE " + target + " " + message + "\r\n");
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
				// the supplied modes will be applied, and a MODE message will be sent to the user containing the changed modes. 
				std::string modestring = parseUserModestring(args[2]);
				std::string message = applyModestring(modestring, client);
				if (message.size() != 0)
					client.appendMessageToSend(":" + client.getSource() + " MODE " + target + " " + message + "/r/n");
			}
		}
	}

}