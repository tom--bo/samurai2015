#!/bin/sh
manager/gameManager \
    -a players/greedyPlayer0 -p "" -u "" -n "greedy0" -r 1 -s 100 \
    -a players/greedyPlayer1 -p "" -u "" -n "greedy1" -r 2 -s 98 \
    -a players/greedyPlayer2 -p "" -u "" -n "greedy2" -r 3 -s 70 \
    -a players/greedyPlayer3 -p "" -u "" -n "greedy3" -r 3 -s 60 \
    -a players/greedyPlayer4 -p "" -u "" -n "greedy4" -r 3 -s 50 \
    -a players/greedyPlayer5 -p "" -u "" -n "greedy5" -r 3 -s 40 \
    -t

rm SamuraiLog*
