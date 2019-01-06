#!/bin/bash

# This script check if we're inside a git repo and outputs a
# description of the current commit and the possible changes
# to an const wchar string in a .h file

clean=""
commit_num=""
file="git_commit_rev.h"

# Check if we are inside a git working tree
if [[ $(git rev-parse --is-inside-work-tree) -eq "true" ]]
then
	# Check if there are any (interesting) uncommited changes
	if [[ $(git status --porcelain | grep -E "\.?\w+\.(cc|h|sh|py|ya?ml)") ]]
	then
	    clean="+"
	fi

	# Check the git revision and output to file
	commit_num=$(git describe --tags --always --abbrev=4)
else
	commit_num="out of tree"
fi

echo "constexpr wchar_t git_commit_rev[] = L\"$commit_num$clean\";" > $file
