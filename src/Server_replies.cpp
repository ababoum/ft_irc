#include "Server.hpp"

void Server::reply(int code, Client &client, const std::vector<std::string>& args)
{
	std::string message;

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
		message = "461 " + client.getNickname() + args[0] + " :Not enough parameters\r\n";
		break;
	case ERR_ALREADYREGISTRED:
		message = "462 " + client.getNickname() + " :You may not reregister\r\n";
		break;
	case ERR_PASSWDMISMATCH:
		message = "464 " + client.getNickname() + " :Password incorrect\r\n";
		break;
	
	default:
		DEBUG("reply: unknown code: " << code << "\n");
		break;
	}
	client.appendMessageToSend(message);
}