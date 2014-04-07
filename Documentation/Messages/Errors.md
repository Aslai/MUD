About this Document
====================

This document aims to specify the many error messages that might be used throughout the application.

General Errors
====================

`Error::None` This message is used when no error occurred. This value should equate to 0.

CommStream Errors
====================

`Error::ConnectionFailure` This message is used when the underlying TCP stream could not be established.

`Error::NegotiationFailure` This message is used when the encryption scheme negotiation failed for some reason.

`Error::InvalidScheme` This message is used when the chosen encryption scheme is invalid or unrecognized.

`Error::InvalidHost` This message is used when the specified host name could not be resolved.

`Error::ConnectionRefused` This message is used when the remote host refuses a connection.

`Error::ImportantOperation` This message is used when an important operation is being handled and the stream cannot be shut down.

`Error::BindFailure` This message is used when the software cannot receive connections.

`Error::PartialMessage` This message is used when the received or sent message wasn't delivered completely before the connection was reset.

`Error::NotConnected` This message is used when the stream is not yet connected.
