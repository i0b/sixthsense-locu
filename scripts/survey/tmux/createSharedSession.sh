#!/bin/bash

SHARED_FILE="/tmp/tmux/pair_sock"

rm -f $SHARED_FILE
mkdir -p $(dirname $SHARED_FILE)
tmux -S $SHARED_FILE new-session -s shared -d "$(realpath $0 | xargs dirname)/../run.sh"

echo "tmux session created"
