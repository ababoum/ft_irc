# GENERAL TO-DO LIST

* Change the Makefile back to c++ compiler at the end of the development
* Users modes : io
* Channels modes : oit


# Points of attention


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

# Command tests to perform

## PASS
* Without password
* With wrong password

## NICK
* Without nickname
* With nickname already used
* With nickname with forbidden characters
* With nickname too long

## JOIN
* Without channel
* With channel already joined
* After being kicked
* An invite-only channel without being invited
* An invite-only channel after being invited
* After parting, join again and see if the operator privileges are still there

## PART
* Without channel
* With channel not joined
* With channel joined

## KICK
* Without channel
* Without nickname
* With channel not joined
* With channel joined
* With nickname not in channel
* With nickname in channel
* Without operator privileges
* With operator privileges

## PRIVMSG
* To multiple targets (channels and/or users)

## NOTICE
* To multiple targets (channels and/or users)

## WHO
* Without channel
* With channel not joined
* With channel joined
* With channel joined and with a nickname

## WHOIS
* Without nickname

## INVITE
* Without channel
* Without nickname
* With channel not joined
* With channel joined
* With nickname not in channel
* With nickname in channel
* Without operator privileges
* With operator privileges

## MODE

## TOPIC

## LIST

## NAMES

## QUIT

## PING

## RESTART
* Without operator privileges

## OPER
* With wrong credentials
* With correct credentials
