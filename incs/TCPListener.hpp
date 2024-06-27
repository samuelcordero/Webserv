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

# define MAX_EVENTS 128

class Request;
class Server;
class Response;

class TCPListener {
	private:
		int							socket_fd;
		int							port;
		struct sockaddr_in			server_addr;
		int							epoll_fd;
		epoll_event					event;
		std::vector<epoll_event>	*events;
		Server						*server;

		void	mock_handler(int client_socket_fd);
	public:
		TCPListener(int port, Server *server);
		~TCPListener();
		TCPListener(const TCPListener& copy, Server *s);
		TCPListener& operator=(const TCPListener& copy);
		void	start();
		void	run();
		Response	analizer(const Request& request);
};

#endif