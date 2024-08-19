#!/usr/bin/env python3

import sys
import cgi
import cgitb

# Enable debugging
cgitb.enable()

# Print the HTTP header
print("Content-Type: text/html\r\n\r\n")


# Initialize input_data
input_data = ""

# Try to use cgi.FieldStorage() first
form = cgi.FieldStorage()
if 'input' in form:
    input_data = form.getvalue('input', '').strip()
else:
    # Fallback to reading directly from stdin
    input_data = sys.stdin.read().strip()

# Modify the input (e.g., convert to uppercase)
modified_input = input_data.upper()

# Generate and print the HTML response
print("<html><head><title>CGI Test</title></head><body>")
print(f"<h1>Modified input in uppercase: {modified_input}</h1>")
print("</body></html>")
