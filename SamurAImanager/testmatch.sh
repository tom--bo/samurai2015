#!/bin/sh
manager/gameManager \
    -a players/greedyPlayer0 -p "" -u "" -n "greedy0" -r 1 -s 100 \
    -a players/greedyPlayer1 -p "" -u "" -n "greedy1" -r 2 -s 98 \
    -a players/greedyPlayer2 -p "" -u "" -n "greedy2" -r 3 -s 80 \
    -a players/greedyPlayer0 -p "" -u "" -n "p0" -r 4 -s 70 \
    -a players/greedyPlayer1 -p "" -u "" -n "p1" -r 5 -s 60 \
    -a players/greedyPlayer2  -p "" -u "" -n "p2" -r 6 -s 20 \
    -t

rm SamuraiLog*
