
# Webserv

![webserv](./webserv.png)

## Overview

**Webserv** is a high-performance HTTP server built from the ground up in C++ 98. This project is a culmination of our deep dive into the workings of the HTTP protocol, designed and implemented entirely by [samuelcordero](https://github.com/samuelcordero), [bazuara](https://github.com/bazuara), and [agserran](https://github.com/agserran). The server is capable of handling multiple simultaneous connections, serving static content, executing CGI scripts, and managing HTTP requests and responses efficiently.

This work represents a comprehensive effort to master HTTP server implementation in C++ 98, leveraging the principles of robust, non-blocking I/O operations, and dynamic server configuration.

## Features

- **Comprehensive HTTP Support**: Implements essential HTTP methods (GET, POST, DELETE).
- **Efficient I/O Handling**: Utilizes non-blocking I/O with `poll()` or its equivalent.
- **Dynamic Configuration**: Offers customizable configuration files, similar to NGINX, allowing users to define ports, error pages, routes, and more.
- **CGI Execution**: Capable of executing CGI scripts for dynamic content generation.
- **Static Content Serving**: Efficiently serves static HTML content.
- **Robust Error Handling**: Implements accurate HTTP response status codes and custom error pages.
- **Session Management**: Supports cookies and session handling for advanced user management.

## Repository Structure

- **srcs/**: Contains the main source files for the server implementation.
- **incs/**: Includes header files defining the structures and classes used across the project.
- **config/**: Sample configuration files to guide users in setting up their own configurations.
- **html/**: Static HTML content to demonstrate the server's capability.
- **CGI/**: Sample CGI scripts that can be executed by the server.
- **tests/**: Test scripts and files to validate the server's functionality under various scenarios.

## Getting Started

### Prerequisites

- A C++ 98 compliant compiler.
- Make utility.
- A UNIX-like operating system.

### Installation

Clone the repository and compile the server using the provided Makefile:

```bash
git clone https://github.com/samuelcordero/Webserv.git
cd Webserv
make
```

### Usage

Start the server with a custom configuration file:

```bash
./webserv [configuration file]
```

### Configuration

Webserv uses a configuration file to control its behavior. This file allows you to set up:

- Ports and hostnames
- Default and custom error pages
- HTTP method restrictions per route
- Directory listings
- CGI script execution
- File upload paths

Refer to the example configuration files in the `config/` directory for details.

### Testing and Validation

To ensure robustness and reliability, the server has been tested with:

- **Web Browsers**: Compatibility across multiple web browsers.
- **Telnet and NGINX**: Validation against NGINX behavior and manual testing via Telnet.
- **Automated Scripts**: Python and C++ scripts available in the `tests/` directory.
