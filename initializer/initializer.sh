#!/bin/bash

echo "Checking if the command_not_found_handle is in the .bashrc file..."
if ! grep -q 'command_not_found_handle' ~/.bashrc; then
	echo "Initializer: the command_not_found_handle is not in the .bashrc file. Appending it..."
	cat ./initializer/command_not_found_handle.sh >> ~/.bashrc
else
	echo "Initializer: the command_not_found_handle is already in the .bashrc file"
fi