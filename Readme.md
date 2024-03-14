# Matt_daemon

42 School Project


## Overview

The goal of this project is to create a daemon that binds to port 4242 and log incoming message to a file in the path `/var/log/matt_daemon.log`.  

### Requirements

- The daemon must run as root  
- The daemon must be able to handle multiple clients at the same time but limit active connections to 3
- The daemon must write a lock file in `/var/lock/matt_daemon` to prevent multiple instances from running
- The daemon must be able to handle signals and quit on signal reception 
- The `quit` command must be implemented to stop the daemon
- Any other command must be logged in the log file with a timestamp in the format `YYYY-MM-DD HH:MM:SS`


## Installation & Run

A Makefile is provided to compile the project, just run the `make` command  


## Usage


### Start the daemon

```bash
sudo ./Matt_daemon
```

### Send a message to the daemon

```bash
echo "Hello" | nc localhost 4242
```

### Stop the daemon

- Send the `quit` command to the daemon
```bash
echo "quit" | nc localhost 4242
```

- Send a signal to the daemon

```bash
ps ax | grep Matt_daemon
kill -2 <pid>
```