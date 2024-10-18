# README

## Overview
This project implements a simple shell in C++ that mimics basic command-line functionalities, including file manipulation, process management, and input/output operations.

## Functions

### File Management
- **`get_file_names()`**: Recursively fetches a list of file names in a specified directory.

### Command Handling
- **`cmd_break()`**: Breaks down a command string into its individual components for easier processing.

### User and System Information
- **`u_name()`**: Retrieves the current user's name.
- **`s_name()`**: Retrieves the system's hostname.
- **`u_pwd()`**: Retrieves the current working directory.

### Process Management
- **`fg_sys_call()`**: Creates a foreground process.
- **`bg_sys_call()`**: Creates a background process.

## Shell Commands

### Change Directory
- **`chdir()`**: Changes the current directory to the specified path.

### Print Working Directory
- **`getcwd()`**: Returns the current working directory.

### Echo
- **`echo`**: Outputs the content that follows the `echo` command.

### List Files
- **`ls`**:
  - **`opendir()`**: Opens a directory for reading.
  - **`readdir()`**: Reads the next item in an open directory.
  - **`closedir()`**: Closes the opened directory.

### System Commands (Foreground/Background)
- **`strdup()`**: Duplicates a string (creates a copy).
- **`fork()`**: Creates a new child process.
- **`execvp()`**: Executes a new process.

### Process Information
- **`pinfo`**:
  - **`getpid()`**: Returns the process ID of the current process.
  - **`tcgetpgrp()`**: Returns the process group ID of the foreground process group associated with the terminal.
  - **`getpgrp()`**: Returns the process group ID of the current process.

### Search Operations
- **`opendir()`**, **`readdir()`**, **`closedir()`**: Similar functionalities as in the List Files section.

### I/O Redirection
- **`open()`**: Opens a file.
- **`read()`**: Reads data from a file.

### Simple Signals
- **CTRL-D**: Triggers end-of-file (EOF) condition using `eof()`.

### Command History
- **`vector`**: Utilizes a vector to store and manage command history.

## Compilation and Execution
1. Compile the project:
   ```bash
   g++ Schell.cpp -o shell
