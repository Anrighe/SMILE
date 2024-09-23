command_not_found_handle() {

    declare attempted_command="$1"
    smile -i $attempted_command
    return 0
}
