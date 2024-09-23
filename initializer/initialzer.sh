#!/bin/bash

echo "Initializer script: checking if the command_not_found_handle is in the .bashrc file..."
if ! grep -q 'command_not_found_handle' ~/.bashrc; then
	echo "Initializer script: adding the command_not_found_handle in the .bashrc file. Appending it..."
	cat ./initializer/command_not_found_handle.sh >> ~/.bashrc
else
	echo "Initializer script: the command_not_found_handle is already present in the .bashrc file"
fi