#!/usr/bin/env python3

import sys
import cgi
import cgitb
import html
from art import *

# Enable debugging
cgitb.enable()

# Print the HTTP header
print("Content-Type: text/html\n")

# Initialize input_data
input_data = ""

# Try to use cgi.FieldStorage() first
form = cgi.FieldStorage()
if 'art' in form:
    input_data = form.getvalue('art', '').strip()
else:
    # Fallback to reading directly from stdin
    input_data = sys.stdin.read().strip()

# Create the art if input_data is not empty
if input_data:
    art = text2art(input_data, font='block', chr_ignore=True)
else:
    art = "No input provided."

# Escape HTML special characters
escaped_art = html.escape(art)

# Generate and print the HTML response
print("<html><head><title>CGI ART Test</title></head><body>")
print("<h1>Debug Information</h1>")
print("<p>Form keys: {}</p>".format(list(form.keys())))
print("<p>Input data: {}</p>".format(input_data))
print("<pre>")
print(escaped_art)
print("</pre>")
print("</body></html>")
