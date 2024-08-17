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

// Check if form data has been sent
if ($_SERVER["REQUEST_METHOD"] == "POST") {
    echo "<h2>Form Data Received:</h2>";
    echo "<ul>";
    foreach ($_POST as $key => $value) {
        echo "<li><strong>" . htmlspecialchars($key) . ":</strong> " . htmlspecialchars($value) . "</li>";
    }
    echo "</ul>";
} else {
    echo "<p>No form data submitted.</p>";
}

echo "</body>";
echo "</html>";
?>
