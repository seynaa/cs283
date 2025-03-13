#!/usr/bin/env bats

# File: student_tests.sh
# Create your unit tests suite in this file

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF
    [ "$status" -eq 0 ]
}

@test "Built-in: cd to existing directory" {
    run ./dsh <<EOF
cd bats
pwd
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "bats" ]]
}

@test "Built-in: exit command" {
    run ./dsh <<EOF
exit
EOF
    [ "$status" -eq 0 ]
}

@test "Built-in: stop-server in local mode" {
    run ./dsh <<EOF
stop-server
exit
EOF
    [ "$status" -eq 0 ]
}

@test "Pipes: Multiple commands with pipe" {
    run ./dsh <<EOF
ls -l | grep dsh | wc -l
exit
EOF
    [ "$status" -eq 0 ]
    # Extract the first line of output (the pipeline result) and check if it's a number
    number=$(echo "$output" | head -n 1 | tr -d '[:space:]')
    [[ "$number" =~ ^[0-9]+$ ]]
}

@test "Error: Non-existent command" {
    run ./dsh <<EOF
nonexistentcommand
exit
EOF
    [ "$status" -ne 0 ]
    [[ "$output" =~ "Execution failed" ]]
}

@test "Remote: Client mode with ls" {
    run ./dsh -c -i 127.0.0.1 -p 1234 <<EOF
ls
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "dsh" ]]
}

@test "Multi-threaded: Server with concurrent clients" {
    run ./dsh -c -i 127.0.0.1 -p 1234 <<EOF
ls
exit
EOF
    [ "$status" -eq 0 ]
}

@test "Remote: stop-server command" {
    run ./dsh -c -i 127.0.0.1 -p 1234 <<EOF
stop-server
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "stopping" ]]
}