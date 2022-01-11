#include "User.hpp"

int	main(int argc, char **argv)
{
	if (argc != 3)
		std::cerr << "Wrong number of arguments" << std::endl;

	User	foo("%123", "123", "123", "123");
	return 0;
}
