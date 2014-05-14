About this Document
====================

This document is intended to provide a simple overview as to how an end user can create an account.

Creating a User Account via the Website
====================

To create an account, a given user should be able to visit a website for the game, and submit their information to a script-driven website, which will make a server admin request to the server. The server handles the actual registration of the account, and the information will get returned to the server-sided script via JSON. This can then be used to handle things like duplicate account names, etc.

Creating a User Account via the Game Client
====================

To create an account via the game client, the user should only have to click a prominent button once the client is launched that takes them to a registration page in the client. Information such as the password should be shared with the server in an encrypted fasion, in addition to any transport security that there might be. 