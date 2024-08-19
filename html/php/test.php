#!/usr/bin/php
<?php
// This is a simple CGI script written in PHP

// Print the HTTP headers
echo "Content-Type: text/html\r\n\r\n";

// Print the HTML content
echo "<html>";
echo "<head><title>Simple PHP CGI Example</title></head>";
echo "<body>";
echo "<h1>Hello, World!</h1>";
echo "<p>This is a simple PHP CGI script.</p>";

echo "<pre>";
echo "Raw POST data: ";
echo file_get_contents("php://input");
echo "</pre>";

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
