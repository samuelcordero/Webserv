#!/usr/bin/php
<?php
// This is a simple CGI script written in PHP

// Print the HTTP headers
header("Content-Type: text/html");

// Print the HTML content
echo "<html>";
echo "<head><title>Simple PHP CGI Example</title></head>";
echo "<body>";
echo "<h1>Hello, World!</h1>";
echo "<p>This is a simple PHP CGI script.</p>";
echo "</body>";
echo "</html>";
?>
