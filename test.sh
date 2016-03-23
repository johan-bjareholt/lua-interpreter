#!/bin/bash

#
# This is essentially just a crash test, not a proper unit test
#

gtestpass=0
gtestcount=0

function gtest() {
	gtestcount=$(($gtestcount+1))
	echo "Grammar testing $1"
	./lua -s $1
	if [ $? -eq 0 ]; then
		gtestpass=$(($gtestpass+1))
	else
		echo "FAILED: grammar test $1"
		failedtests=$failedtests$1
	fi
}

gtest tests/grammar/comments.lua
gtest tests/grammar/stat.lua
gtest tests/grammar/asstest.lua
gtest tests/grammar/longsdlcode.lua
gtest tests/grammar/sockhttp.lua
gtest tests/grammar/sockurl.lua

printf "%d/%d grammar tests passed\n" $gtestpass $gtestcount


itestcount=0
itestpass=0

function itest(){
	itestcount=$(($itestcount+1))
	echo "Interpretation testing $1"
	./lua $1
	if [ $? -eq 0 ]; then
		itestpass=$(($itestpass+1))
	else
		echo "FAILED: interpretation test $1"
		failedtests=$failedtests$1
	fi
}

itest tests/interpret/ass_d.lua
itest tests/interpret/ass_c.lua
itest tests/interpret/for.lua
itest tests/interpret/ass_b1.lua

printf "%d/%d interpretation tests passed\n" $itestpass $itestcount
