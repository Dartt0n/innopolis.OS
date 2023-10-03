# enable history in non-iteractive shell
HISTFILE=~/.bash_history
set -o history

history | tail -n 10 > ~/week01/ex2.txt
