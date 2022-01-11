NAME =	ircserv

TEST =	test

SRCS =	main.cpp\
		Server.cpp\
		User.cpp\
		UserInfo.cpp\
		UserHistory.cpp

SRC1 =	main1.cpp\
		User.cpp

COMP = clang++ -g -std=c++98

OBJS = $(SRCS:.cpp=.o)

OBJS1 = $(SRC1:.cpp=.o)

all:	$(NAME)

$(NAME):	$(OBJS)
			$(COMP) $(OBJS) -o $(NAME)

test:	$(OBJS1)
		$(COMP)	$(OBJS1)	-o $(TEST)

clean:
			rm -f $(OBJS) $(OBJS1)

fclean:		clean
			rm -f $(NAME) $(TEST)

re:			fclean all

.PHONY:	re fclean clean all