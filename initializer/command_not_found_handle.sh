command_not_found_handle() {

    declare attempted_command="$1"
    /usr/bin/smile --i $attempted_command
    return 0
}
