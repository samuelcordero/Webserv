#!/usr/bin/env python3

import sys
import cgi
import cgitb

# Enable debugging
cgitb.enable()

# Print the HTTP header
print("Content-Type: text/html\n")

# Read the input from stdin (for POST request data)
input_data = sys.stdin.read()

# Modify the input (e.g., convert to uppercase)
modified_input = input_data.upper()

# Generate and print the HTML response
print("<html><head><title>CGI Test</title></head><body>")
print(f"<h1>Modified input: {modified_input}</h1>")
print("</body></html>")
