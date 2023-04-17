#include "Server.hpp"

// KILL
// RESTART


// TODO: TEST KILL
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
    if (!isServerOp(&client))
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
    throw std::runtime_error("Quit :" + args[1]);
}

void Server::restart(Client &client, const std::vector<std::string> &args)
{
    RUNTIME_MSG("restart function called\n");

    (void)args;

    // Check if the client is a server operator
    if (!isServerOp(&client))
    {
        reply(ERR_NOPRIVILEGES, client);
        return;
    }

    // Restart the server
    if (_socket_fd)
    {
        close(_socket_fd);
        _socket_fd = 0;
    }
    usleep(1000000); // Wait 1 second before restarting the server
    _shutting_down = true;
    launch();
}