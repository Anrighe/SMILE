#!/bin/bash

# Checking if the command_not_found_handle is in the .bashrc file
if ! grep -q 'command_not_found_handle' ~/.bashrc; then
	cat ./initializer/command_not_found_handle.sh >> ~/.bashrc
fi