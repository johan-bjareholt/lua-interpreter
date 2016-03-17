#!/bin/bash

#
# This is essentially just a crash test, not a proper unit test
#

testpass=0
testcount=0

function test() {
	testcount=$(($testcount+1))
	cat $1 | ./lua
	if [ $? -eq 0 ]; then
		testpass=$(($testpass+1))
	else
		failedtests=$failedtests$1
	fi
}

test tests/comments.lua
test tests/stat.lua
test tests/asstest.lua
test tests/longsdlcode.lua

printf "%d/%d tests passed\n" $testpass $testcount
