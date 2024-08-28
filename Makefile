# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: sacorder <sacorder@student.42madrid.com    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/02/04 17:08:31 by sacorder          #+#    #+#              #
#    Updated: 2024/08/28 11:23:34 by sacorder         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = webserv
CC = c++
CPPFLAGS = -Wall -Wextra -Werror -std=c++98 -g3
LDFLAGS = -I incs
SRC =	srcs/CgiHandler.cpp \
		srcs/Client.cpp \
		srcs/config_parser.cpp \
		srcs/Controller.cpp \
		srcs/Errors.cpp \
		srcs/EventManager.cpp \
		srcs/extraFunctions.cpp \
		srcs/Http.cpp \
		srcs/Indexer.cpp \
		srcs/Location.cpp \
		srcs/main.cpp \
		srcs/Request.cpp \
		srcs/Response.cpp \
		srcs/Server.cpp \
		srcs/TCPListener/TCPListener.cpp \
		srcs/TCPListener/TCPListenerClientUtils.cpp \
		srcs/TCPListener/TCPListenerConnMethods.cpp \
		srcs/TCPListener/TCPListenerUtils.cpp 


OBJS_DIR = objs
OBJ = $(patsubst srcs/%, $(OBJS_DIR)/%, $(SRC:.cpp=.o))

TEST_SCRIPT = tests/test.py

.PHONY: all clean fclean re start_server test

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) -o $(NAME) $(OBJ) $(LDFLAGS)

$(OBJS_DIR)/%.o: srcs/%.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(LDFLAGS) -c -o $@ $<

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)
# start the server and redirects all outputs to null
start_server: $(NAME)
	@./$(NAME) config/tester.conf 2> /dev/null 1> /dev/null &

kill_server:
	@ps aux | grep $(NAME) | grep -v grep | awk '{print $$2}' | xargs kill -9

test: start_server
	@touch html/empty/file
	@pytest $(TEST_SCRIPT)
	@pkill $(NAME)

re:: fclean
re:: all
