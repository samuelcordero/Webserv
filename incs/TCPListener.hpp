/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TCPListener.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agserran <agserran@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/15 12:59:14 by sacorder          #+#    #+#             */
/*   Updated: 2024/06/27 16:23:02 by agserran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef __TCPLISTENER_H__
# define __TCPLISTENER_H__

# include <sys/types.h>
# include <sys/socket.h>
# include <sys/epoll.h>
# include <sys/time.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <vector>
# include <iostream>
# include <cstdlib>
# include <cerrno>
# include <cstdio>
# include <cstring>
# include <utility>
# include <fstream>
# include <ctime>

# include "Server.hpp"
# include "Response.hpp"
# include "Request.hpp"
# include "Client.hpp"
# include "EventManager.hpp"

# define MAX_EVENTS 128

# define CONN_TIMEOUT 30 //timeout for connections in seconds

# define CLIENT 1
# define CGI 2


class Request;
class Server;
class Response;
class Location;
class Client;

class TCPListener {
	private:
		int							socket_fd;
		int							port;
		struct sockaddr_in			server_addr;
		Server						*server;
		/* std::string					buffers[4096];
		Response					*responses[4096];
		std::pair<Request *, bool>	requests[4096];
		long long					last_conn[4096]; */
		Client						clients[4096];
		char						matcher[4096];
		EventManager				*eventManager;

		Response	analizer(const Request& request);
		int			newClient();
		int			readData(int fd);
		int			sendData(int fd);
		void		createResponse(size_t i);
		Response 	Get(std::pair<std::string, std::string> uri_pair, Location &location);
		Response 	Head(std::pair<std::string, std::string> uri_pair, Location &location);
		Response 	Delete(std::pair<std::string, std::string> uri_pair, Location &location);
		Response 	Post(std::pair<std::string, std::string> uri_pair, Location &location, const Request &request);
		std::pair<std::string,
		std::string> splitUri(std::string uri);
		long long	getCurrentEpochMillis();
		bool 		isTimeout(long long startMillis, long long endMillis, int thresholdSeconds);
		void		disconnectClient(int pos);
	public:
		TCPListener(int port, Server *server);
		~TCPListener();
		TCPListener(const TCPListener& copy, Server *s);
		TCPListener& operator=(const TCPListener& copy);
		int		start();
		int		checkEvent(epoll_event ev);
		int		getSocketFd();
		void	setEventManager(EventManager *eventManager);
		//void	run();
};

#endif