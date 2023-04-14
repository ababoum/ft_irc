# GENERAL TO-DO LIST

* Commands to broadcast to channel : JOIN PART KICK MODE PRIVMSG NOTICE QUIT
* Implement the MODE command
* Implement the operator commands: MODE KILL
* Change the Makefile back to c++ compiler at the end of the development
* Concatenate the client commands when they are sent in parts or if the buffer is too long
* Check the channel name value according to the RFC
* Once MODE is implemented, review JOIN, TOPIC, NAMES, RPL_NAMEREPLY, LIST
* Users modes : io
* Channels modes : oit


# Points of attention

* What happens when a channel is empty? (everyone left) >> The channel is deleted


# Useful commands and information

Launch easily IRSSI with a ready-connection
```bash
irssi -c 127.0.0.1 -p 6667 -w lala -n mynick
```

# How to transfer a file within IRSSI
```bash
/DCC SEND sarah "cat.png"
/DCC GET bob "cat.png"
```