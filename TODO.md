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
* Without password OK
* With wrong password OK

## NICK
* Without nickname OK
* With nickname already used OK
* With nickname with forbidden characters OK
* With nickname too long

## JOIN
* Without channel OK
* With channel already joined OK nc, voir irssi
* After being kicked OK si le comportement est de pouvoir revenir
* An invite-only channel without being invited OK
* An invite-only channel after being invited OK
* After parting, join again and see if the operator privileges are still there >> Operator privileges not here, normal ?

## PART
* Without channel OK
* With channel not joined OK
* With channel joined OK

## KICK >>> Problem with broadcast ? L'operateur recoit 2 messages, le kické n'en recoit aucun.
* Without channel OK
* Without nickname OK
* With channel not joined OK
* With channel joined OK execpt bad broadcast
* With nickname not in channel OK
* With nickname in channel OK
* Without operator privileges OK
* With operator privileges OK

## PRIVMSG
* To multiple targets (channels and/or users) Cassé avec plusieurs users ou plusieurs channels:
`PRIVMSG mriant2,mriant3 coucou
`401 mriant mriant2,mriant3 :No such nick/channel

## NOTICE
* To multiple targets (channels and/or users) Aussi cassé avec plusieurs cibles
`NOTICE mriant2,mriant3 coucou

## WHO
* Without channel OK
* With channel not joined renvoie la liste, c'est normal ?
Format de réponse bizarre
`352 mriant #test3 mriant2 d mriant2 H@:0 d 
Il manque <host> ou <server>
* With channel joined Pareil
* With channel joined and with a nickname Segfault !

## WHOIS
* Without nickname OK

## INVITE
* Without channel OK
* Without nickname OK
* With channel not joined OK
* With channel joined OK
* With nickname not in channel OK
* With nickname in channel OK
* Without operator privileges OK
* With operator privileges OK

## MODE

## TOPIC

## LIST

## NAMES

## QUIT

## PING

## RESTART
* Without operator privileges OK
RESTART quitte mais ne relance pas

## OPER
* With wrong credentials OK
* With correct credentials OK
