--[[

	I have written this myself to test the stat grammar

]]--

-- assignment
asd = 123
asd1, asd2 = 234, 345

-- do-end
do
	asd3 = 123
end

-- while
while true
do
	asd4 = 123
end

-- repeat
repeat
	asd5=123
until false

-- if-elseif-else
if true
then
	v1=true
elseif true
then
	v1=false
elseif false
then
	v1=true
else
	v1 = true
end

-- for, 2
for asd=123, asd<1
do
	asd=321
end

-- for, 3
for asd=123, asd<1, asd+1
do
	asd=321
end

-- for-in
for asd, asd2 in testfunc()
do
	asd=456
end

-- function 
function testfunc()
	asd=123
end

function testfunc2(test)
	asd=123
end

-- local function
local function testfunc3(test,test2,test3)
	asd=123
end

-- local undefined variable
local asd
local asd2, asd3

-- local defined variable
local asd = 123
local asd2, asd3 = 234, 234

