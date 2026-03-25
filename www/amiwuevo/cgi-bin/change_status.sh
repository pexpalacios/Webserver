#!/bin/sh

#!needs to change the databs alive from 1 to 0. When it's 0 replace the script.js for a different .js that blocks all buttons
echo "Bash CGI executed"
echo "Content-type: text/plain"
echo ""

ALIVE_FILE="/home/penpalac/Desktop/Webserv/database/alive.txt"

# Check write permission
if [ ! -w "$ALIVE_FILE" ]; then
	echo "Error: No write permission for $ALIVE_FILE"
	exit 1
fi

# Overwrite alive.txt with 0
echo 0 > "$ALIVE_FILE"

if [ $? -eq 0 ]; then
	echo "Status changed: alive.txt set to 0."
else
	echo "Error: Could not write to $ALIVE_FILE."
fi

echo "0"



