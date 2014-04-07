About this Document
====================

This document is intended to provide a simple overview as to how server administration may be performed.

Server Administration from Within
====================

Players within the game realm who have the rights to do so may directly interact with the server internals. The commands to do this are:

`serveradmin auth` This will move you into administration mode. All in-game commands are disabled, and you are given access to server commands.

`serveradmin deauth` This will move you out of administration mode. All administration mode commands will become disabled, and you may play the game as usual.

Server Administration from Outside
====================

The server will expose a minimal JSON/JSONP interface for administration tasks. This interface, by default, can only be accessed from the same machine that the server is running on. Different IPs may be allowed via the configuration file, however this is frowned upon. The recommended way to expose the interface for remote modification is via a website that is running on the same server. Ensure that the website requires secure authentication before exposing the API to the potential administrator. The server will act as if this avenue of administration is available only to localhost, and thus will be incredibly insecure if exposed directly to the internet.

Administration Commands
====================

This list is by no means complete, but there are a few commands that must be included.

`account.create username password email` This command is used to create an account with the given credentials.

`account.credit accountid amount` Credits the given account with some amount of subscription credit.

`account.delete accountid` Deletes the given account.

`account.record.set record value` Sets the given account record.

`account.record.get` Gets the given account record.

`server.option.set option value` Sets the given server option

`server.option.get option1 [, option2[...]]` Gets the given server option(s)

`server.option.append option value` Appends to the given option with the value.

`server.backup` Forces a backup of the server files.