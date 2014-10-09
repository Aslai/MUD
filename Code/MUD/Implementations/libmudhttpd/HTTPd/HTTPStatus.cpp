#include <string>
#include "HTTPd/HTTPStatus.hpp"

namespace MUD{
    static struct{int code; std::string text; std::string message;} HTTPstatuses[] = {

        {100,    "Continue",                            "Continue"} ,
        {101,    "Switching Protocols",                 "Switching Protocols"} ,
        {102,    "Processing",                          "Processing"} ,

        {200,    "OK",                                  "OK"} ,
        {201,    "Created",                             "The resource was created"} ,
        {202,    "Accepted",                            "The resource was accepted"} ,
        {203,    "Non-Authoritative Information",       "Non-Authoritative Information"} ,
        {204,    "No Content",                          "No content in this resource"} ,
        {205,    "Reset Content",                       "Reset content in this resource"} ,
        {206,    "Partial Content",                     "Partial content supplied"} ,
        {207,    "Multi-Status",                        "Multi-Status"} ,
        {208,    "Already Reported",                    "Already Reported"} ,
        {226,    "IM Used",                             "IM Used"} ,

        {300,    "Multiple Choices",                    "Multiple choices provided"} ,
        {301,    "Moved Permanently",                   "Moved Permanently"} ,
        {302,    "Found",                               "Found"} ,
        {303,    "See Other",                           "See Other"} ,
        {304,    "Not Modified",                        "Not Modified"} ,
        {305,    "Use Proxy",                           "Use Proxy"} ,
        {306,    "Switch Proxy",                        "Switch Proxy"} ,
        {307,    "Temporary Redirect",                  "Temporary Redirect"} ,
        {308,    "Permanent Redirect ",                 "Permanent Redirect "} ,

        {400,    "Bad Request",                         "Your request made to the server could not be understood"} ,
        {401,    "Unauthorized",                        "You are not authorized to view this resource"} ,
        {402,    "Payment Required",                    "Money must be tendered before viewing this resource"} ,
        {403,    "Forbidden",                           "This resource is forbidden to the likes of you"} ,
        {404,    "Not Found",                           "The requested resource could not be found"} ,
        {405,    "Method Not Allowed",                  "Method Not Allowed"} ,
        {406,    "Not Acceptable",                      "Not Acceptable"} ,
        {407,    "Proxy Authentication Required",       "You must be authenticated via a proxy to continue"} ,
        {408,    "Request Timeout",                     "The request you made for this resource has timed out. Please try again later"} ,
        {409,    "Conflict",                            "There was a conflict when dealing with your request"} ,
        {410,    "Gone",                                "Gone with the wind"} ,
        {411,    "Length Required",                     "A length must be provided to use this feature"} ,
        {412,    "Precondition Failed",                 "Precondition Failed"} ,
        {413,    "Request Entity Too Large",            "Request Entity Too Large"} ,
        {414,    "Request-URI Too Long",                "The requested resource could not be provided as the request URI was too large"} ,
        {415,    "Unsupported Media Type",              "Unsupported Media Type"} ,
        {416,    "Requested Range Not Satisfiable",     "Requested Range Not Satisfiable"} ,
        {417,    "Expectation Failed",                  "Expectation Failed"} ,
        {418,    "I'm a teapot",                        "I'm a little teapot, short and stout.<br/>Here is my handle, here is my spout.<br/>When I get steamed up I start to shout:<br/>Tip me over and pour me out!"} ,
        {419,    "Authentication Timeout",              "Authentication Timeout"} ,
        {420,    "Method Failure",                      "Method Failure"} ,
        {420,    "Enhance Your Calm",                   "Enhance Your Calm"} ,
        {422,    "Unprocessable Entity",                "Unprocessable Entity"} ,
        {423,    "Locked",                              "Locked"} ,
        {424,    "Failed Dependency",                   "Failed Dependency"} ,
        {425,    "Unordered Collection",                "Unordered Collection"} ,
        {426,    "Upgrade Required",                    "Upgrade Required"} ,
        {428,    "Precondition Required",               "Precondition Required"} ,
        {429,    "Too Many Requests",                   "Too Many Requests"} ,
        {431,    "Request Header Fields Too Large",     "Request Header Fields Too Large"} ,
        {440,    "Login Timeout",                       "Login Timeout"} ,
        {444,    "No Response",                         "No Response"} ,
        {449,    "Retry With",                          "Retry With"} ,
        {450,    "Blocked by Windows Parental Controls","Blocked by Windows Parental Controls"} ,
        {451,    "Unavailable For Legal Reasons",       "Unavailable For Legal Reasons"} ,
        {451,    "Redirect",                            "Redirect"} ,
        {494,    "Request Header Too Large",            "Request Header Too Large"} ,
        {495,    "Cert Error",                          "Cert Error"} ,
        {496,    "No Cert",                             "No Cert"} ,
        {497,    "HTTP to HTTPS",                       "HTTP to HTTPS"} ,
        {499,    "Client Closed Request ",              "Client Closed Request "} ,

        {500,    "Internal Server Error",               "Internal Server Error"} ,
        {501,    "Not Implemented",                     "Not Implemented"} ,
        {502,    "Bad Gateway",                         "Bad Gateway"} ,
        {503,    "Service Unavailable",                 "Service Unavailable"} ,
        {504,    "Gateway Timeout",                     "Gateway Timeout"} ,
        {505,    "HTTP Version Not Supported",          "HTTP Version Not Supported"} ,
        {506,    "Variant Also Negotiates",             "Variant Also Negotiates"} ,
        {507,    "Insufficient Storage",                "Insufficient Storage"} ,
        {508,    "Loop Detected",                       "Loop Detected"} ,
        {509,    "Bandwidth Limit Exceeded",            "Bandwidth Limit Exceeded"} ,
        {510,    "Not Extended",                        "Not Extended"} ,
        {511,    "Network Authentication Required",     "Network Authentication Required"} ,
        {520,    "Origin Error",                        "Origin Error"} ,
        {521,    "Web server is down",                  "Web server is down"} ,
        {522,    "Connection timed out",                "Connection timed out"} ,
        {523,    "Proxy Declined Request",              "Proxy Declined Request"} ,
        {524,    "A timeout occurred",                  "A timeout occurred"} ,
        {598,    "Network read timeout error",          "Network read timeout error"} ,
        {599,    "Network connect timeout error",       "Network connect timeout error"} ,

        {0,      "",                                    "" }
    };

    DLL_EXPORT std::string GetMessageFromHTTPStatus( int status ) {
        //Get the proper string to send with a given HTTP status code.
        for( int i = 0; HTTPstatuses[i].code != 0; i ++ ) {
            if( status == HTTPstatuses[i].code ) {
                return HTTPstatuses[i].text;
            }
        }
        return "Unknown";
    }
    DLL_EXPORT std::string GetDetailedMessageFromHTTPStatus( int status ) {
        //Get the proper string to send with a given HTTP status code.
        for( int i = 0; HTTPstatuses[i].code != 0; i ++ ) {
            if( status == HTTPstatuses[i].code ) {
                return HTTPstatuses[i].message;
            }
        }
        return "Unknown status code";
    }

}
