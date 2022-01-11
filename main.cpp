#include "ircserv.hpp"
#include "Server.hpp"

int	main(int argc, char **argv)
{
	int port = std::atoi(argv[1]);

	if (argc != 3)
	{
		std::cerr << "ircserv: Wrong number of arguments" << std::endl;
		exit(EXIT_FAILURE);
	}
	if (port < 1024 || port > 49151)
	{
		std::cerr << "ircserv: Incorrect or reserved port." << std::endl;
		exit(EXIT_FAILURE);
	}

	Server server(argv[2], port);
	server.run();
	
	return 0;
}
