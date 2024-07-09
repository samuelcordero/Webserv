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

# include "Server.hpp"
# include "Response.hpp"
# include "Request.hpp"
# include <sys/types.h>
# include <sys/socket.h>
# include <sys/epoll.h>
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

# define MAX_EVENTS 128

class Request;
class Server;
class Response;
class Location;

class TCPListener {
	private:
		int							socket_fd;
		int							port;
		struct sockaddr_in			server_addr;
		int							epoll_fd;
		epoll_event					event;
		std::vector<epoll_event>	*events;
		Server						*server;
		std::vector<std::string>	buffers[4096];
		Response					*responses[4096];
		std::pair<Request *, bool>	requests[4096];

		Response	analizer(const Request& request);
		void		readData(int pos);
		void		sendData(int pos);
		void		createResponse(size_t i);
		Response 	Get(std::pair<std::string, std::string> uri_pair, Location &location);
		Response 	Head(std::pair<std::string, std::string> uri_pair, Location &location);
		Response 	Delete(std::pair<std::string, std::string> uri_pair, Location &location);
		Response 	Post(std::pair<std::string, std::string> uri_pair, Location &location, const Request &request);
		std::pair<std::string,
		std::string> splitUri(std::string uri);
	public:
		TCPListener(int port, Server *server);
		~TCPListener();
		TCPListener(const TCPListener& copy, Server *s);
		TCPListener& operator=(const TCPListener& copy);
		void	start();
		void	run();
};

#endif