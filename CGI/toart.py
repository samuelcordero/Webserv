#!/usr/bin/env python3

import sys
import cgi
import cgitb
from art import *

# Enable debugging
cgitb.enable()

# Print the HTTP header
# print("Content-Type: text/html\n")

# Read the input from stdin (for POST request data)
input_data = sys.stdin.read()

# Create the art
Art=text2art(input_data,font='block',chr_ignore=True)

# Generate and print the HTML response
print("<html><head><title>CGI ART Test</title></head><body>")
print(Art)
print("</body></html>")
