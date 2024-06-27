# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: bazuara <bazuara@student.42madrid.com>     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/02/04 17:08:31 by sacorder          #+#    #+#              #
#    Updated: 2024/06/27 09:59:19 by bazuara          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = webserv
CC = c++
CPPFLAGS = -Wall -Wextra -Werror -std=c++98 -g3
LDFLAGS = -I incs
SRC =	srcs/config_parser.cpp \
		srcs/Location.cpp \
		srcs/Server.cpp \
		srcs/main.cpp \
		srcs/TCPListener.cpp\
		srcs/Request.cpp
OBJ = $(SRC:.cpp=.o)
TEST_SCRIPT = tests/test.py

.PHONY: all clean fclean re start_server test

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) -o $(NAME) $(OBJ) $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CPPFLAGS) $(LDFLAGS) -c -o $@ $<

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

start_server:
	./$(NAME) &

test: start_server
	python3 $(TEST_SCRIPT)
	pkill -f $(NAME)  # Stop the server after tests

re:: fclean
re:: all
