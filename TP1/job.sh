#!/bin/bash

function prendre_peripherique()
{
   local nom="$1"
   while ! mkdir "$nom" 2>/dev/null; do
      sleep 0.01s
   done
}

prendre_peripherique coucou
sleep 2
rmdir coucou
for ((I=0; I<666667; I++))
do J=I
done
sleep 2
exit 0

