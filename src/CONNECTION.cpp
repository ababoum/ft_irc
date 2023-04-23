#include "Server.hpp"

// PASS
// NICK
// USER
// PING
// QUIT
// OPER

void Server::pass(Client &client, const std::vector<std::string> &args)
{
	std::cout << "pass function called" << std::endl;
	if (!client.isAuthentified() && (client.getNickname() != "" || !client.getUsername().empty()))
		return;
	if (args.size() < 2)
	{
		// 461 ERR_NEEDMOREPARAMS
		reply(ERR_NEEDMOREPARAMS, client, args);
		return;
	}
	if (client.isAuthentified())
	{
		// 462 ERR_ALREADYREGISTRED
		reply(ERR_ALREADYREGISTRED, client, args);
		return;
	}
	client.setPass(args[1] == _password);
}

void Server::nick(Client &client, const std::vector<std::string> &args)
{
	std::cout << "nick function called" << std::endl;
	if (args.size() < 2)
	{
		// 431 ERR_NONICKNAMEGIVEN
		reply(ERR_NONICKNAMEGIVEN, client, args);
		return;
	}
	std::string old_nickname(client.getNickname());
	std::string nickname(args[1]);
	if (!Client::isNicknameValid(nickname))
	{
		// 432 ERR_ERRONEUSNICKNAME
		reply(ERR_ERRONEUSNICKNAME, client, args);
		return;
	}
	if (nickname == client.getNickname())
		return;
	for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if ((*it)->getNickname() == nickname)
		{
			// 433 ERR_NICKNAMEINUSE;
			reply(ERR_NICKNAMEINUSE, client, args);
			return;
		}
	}
	client.setNickname(args[1]);
	if (client.isAuthentified())
		client.appendMessageToSend(":" + old_nickname + " NICK :" + client.getNickname() + "\n");
	else if (client.getUsername() != "")
	{
		authentificate(client);
	}
}

void Server::user(Client &client, const std::vector<std::string> &args)
{
	std::cout << "user function called" << std::endl;
	if (args.size() < 5)
	{
		// 461 ERR_NEEDMOREPARAMS
		reply(ERR_NEEDMOREPARAMS, client, args);
		return;
	}
	if (client.isAuthentified())
	{
		// 462 ERR_ALREADYREGISTRED
		reply(ERR_ALREADYREGISTRED, client, args);
		return;
	}
	client.setUsername(args[1]);
	client.setHostname(args[2]);
	client.setServername(args[3]);
	client.setRealname(args[4]);
	if (!client.isAuthentified() && client.getNickname() != "")
	{
		authentificate(client);
	}
}

void Server::ping(Client &client, const std::vector<std::string> &args)
{
	RUNTIME_MSG("ping function called\n");
	client.appendMessageToSend("PONG " + _name + " " + args[1] + "\n");
}

void Server::quit(Client &client, const std::vector<std::string> &args)
{
	(void)client;
	RUNTIME_MSG("quit function called\n");

	if (args.size() == 1)
		throw std::runtime_error("Quit");
	else
		throw std::runtime_error("Quit :" + args[1]);
}

void Server::oper(Client &client, const std::vector<std::string> &args)
{
	RUNTIME_MSG("oper function called\n");

	if (args.size() < 3)
	{
		reply(ERR_NEEDMOREPARAMS, client, args);
		return;
	}

	// Operators are hard-coded here
	std::map<std::string, std::string> server_ops;
	server_ops["admin"] = "admin";

	std::string name(args[1]);
	std::string password(args[2]);

	if (client.isOperator())
	{
		reply(RPL_YOUREOPER, client, args);
		return ;
	}
	if (server_ops.find(name) != server_ops.end() && server_ops[name] == password)
	{
		client.addMode('o');
		reply(RPL_YOUREOPER, client, args);
	}
	else
	{
		reply(ERR_PASSWDMISMATCH, client, args);
	}
}