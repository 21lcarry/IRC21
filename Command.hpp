#pragma once
#include "ircserv.hpp"
#include "Server.hpp"
#include "User.hpp"
#ifdef __APPLE__
#define IRC_NOSIGNAL SO_NOSIGPIPE
#else
#define IRC_NOSIGNAL MSG_NOSIGNAL
#endif

class Server;

class Command {
    public:
        Command(Server &server, User &user);
        ~Command() {}
	bool	isEqualToRegex(std::string mask, std::string subString);
        int handleRequest();
    private:
        Command();
        Command(const Command &o);
        Command &operator=(const Command &o);

        int _executeCommand(std::string &prefix, std::string &command, std::vector<std::string> &param);
        void _initCommandMap();

        std::map<std::string, int (Command::*)(std::string &, std::vector<std::string> &)> _commandMap;
        /*COMMANDS*/
        /*register*/
        int _cmdPASS(std::string &prefix, std::vector<std::string> &param); //done
        int _cmdNICK(std::string &prefix, std::vector<std::string> &param); // done 
        int _cmdUSER(std::string &prefix, std::vector<std::string> &param); // done
        int _cmdOPER(std::string &prefix, std::vector<std::string> &param);
        int _cmdQUIT(std::string &prefix, std::vector<std::string> &param);
        /*users*/
        int _cmdPRIVMSG_(std::string &prefix, std::vector<std::string> &param, std::string command = "PRIVMSG");
		int _cmdPRIVMSG(std::string &prefix, std::vector<std::string> &param);
        int _cmdAWAY(std::string &prefix, std::vector<std::string> &param);
        int _cmdNOTICE(std::string &prefix, std::vector<std::string> &param);
        int _cmdWHO(std::string &prefix, std::vector<std::string> &param);
        int _cmdWHOIS(std::string &prefix, std::vector<std::string> &param);
        int _cmdWHOWAS(std::string &prefix, std::vector<std::string> &param);
        /*channel*/
        int _cmdMODE(std::string &prefix, std::vector<std::string> &param);
        int _cmdTOPIC(std::string &prefix, std::vector<std::string> &param);
        int _cmdJOIN(std::string &prefix, std::vector<std::string> &param);
        int _cmdINVITE(std::string &prefix, std::vector<std::string> &param);
        int _cmdKICK(std::string &prefix, std::vector<std::string> &param);
        int _cmdPART(std::string &prefix, std::vector<std::string> &param);
        int _cmdNAMES(std::string &prefix, std::vector<std::string> &param);
        int _cmdLIST(std::string &prefix, std::vector<std::string> &param);
        /*other*/
        int _cmdWALLOPS(std::string &prefix, std::vector<std::string> &param);
        int _cmdPING(std::string &prefix, std::vector<std::string> &param);
        int _cmdISON(std::string &prefix, std::vector<std::string> &param);
        int _cmdUSERHOST(std::string &prefix, std::vector<std::string> &param);
        int _cmdVERSION(std::string &prefix, std::vector<std::string> &param);
        int _cmdINFO(std::string &prefix, std::vector<std::string> &param);
        int _cmdADMIN(std::string &prefix, std::vector<std::string> &param);
        int _cmdTIME(std::string &prefix, std::vector<std::string> &param);
        int _cmdPONG(std::string &prefix, std::vector<std::string> &param);
        /*************************/
        
        bool _validateNick(std::string &nick);
        bool _nickInUse(std::string &nick);
        int _authorization();
     //   int _errorSend(User &user, int code, std::string param1 = "", std::string param2 = "");
        void _notifyUsers(const std::string &msg);
        Server  &_server;
        User    &_user;
};