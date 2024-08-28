#!/usr/bin/php
<?php
$data = file_get_contents("php://stdin");

echo "Content-Type: text/html\r\n\r\n";

echo "<html>";
echo "<head><title>PHP Test</title></head>";
echo "<body>";

echo "<h1>This is a PHP Test!</h1>";

echo "<h2>Server Variables</h2>";
echo "<ul>";
foreach ($_SERVER as $key => $value) {
    echo "<li><strong>$key</strong>: $value</li>";
}
echo "</ul>";

echo "<h2>Input Data</h2>";
echo "<pre>$data</pre>";

echo "</body>";
echo "</html>";
?>
