#include "Server.hpp"

// KILL

void Server::kill(Client &client, const std::vector<std::string> &args)
{
    RUNTIME_MSG("kill function called\n");

    if (args.size() < 3)
    {
        reply(ERR_NEEDMOREPARAMS, client, args);
        return;
    }

    std::string nickname(args[1]);
    std::string message(args[2]);

    // check if the client has the right to kill
    if (!client.isOperator())
    {
        reply(ERR_NOPRIVILEGES, client);
        return;
    }

    // check if the client exists
    Client *target = searchClient(nickname);
    if (!target)
    {
        reply(ERR_NOSUCHNICK, client, args[1]);
        return;
    }

    // Broadcast the kill message to all the clients in the channels where the target is
    std::string quit_msg = ":" + client.getNickname() + " KILL " +
                           target->getNickname() + " :" + message + "\r\n";

    for (size_t i = 0; i < _channels.size(); i++)
    {
        for (size_t j = 0; j < _channels[i]->getClients().size(); j++)
        {
            if (_channels[i]->getClients()[j].second->getNickname() == target->getNickname())
            {
                _channels[i]->broadcast(quit_msg, target);
            }
        }
    }

    // Send the kill message to the target
    std::string kill_msg = "ERROR :Closing Link: ircserv (Killed (" +
                           client.getNickname() + " (" + args[2] + ")))\r\n";

    target->appendMessageToSend(kill_msg);
    // remove the client from the server
    target->setFatalError();
}