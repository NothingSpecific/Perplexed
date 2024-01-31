#!/bin/bash

cd SDL/
if [[ "$1" == "--autogen" ]]; then
	./autogen.sh
fi
./configure
make
