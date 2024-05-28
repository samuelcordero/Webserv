/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TCPListener.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sacorder <sacorder@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/15 12:59:14 by sacorder          #+#    #+#             */
/*   Updated: 2024/05/28 18:03:45 by sacorder         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef __TCPLISTENER_H__
# define __TCPLISTENER_H__

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

# define MAX_EVENTS 128

class TCPListener {
	private:
		int							socket_fd;
		int							port;
		struct sockaddr_in			server_addr;
		int							epoll_fd;
		epoll_event					event;
		std::vector<epoll_event>	*events;

		void	mock_handler(int client_socket_fd);
	public:
		TCPListener(int port);
		~TCPListener();
		void	run();
};

#endif