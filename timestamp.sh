#!/bin/sh
commits=`git log | grep -i "^commit" | awk '{print $2};'`
echo $commits
for commit in $commits; do
  echo $commit
done
