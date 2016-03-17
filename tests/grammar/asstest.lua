--[[

	This is taken from the mechani.se course site

]]--

-- D
print(13+(22-11)/1*44)

-- C
x = 7
y = (10+x)*2
z = y - x

-- B, I/O and control flow
print "enter a number:"
n = io.read("*number")
factorial = 1
x=1
for i = 2,n do
	  x = x * i
end
io.write("factorial of ")
io.write(n)
io.write(" is ")
print(x)

-- B, Conditional control flow
print "enter a number:"
n = io.read("*number")
for i = 2,n do
  if n%i==0 then
    io.write(i)
    io.write(" is a factor of ")
    print(n)
  end
end

-- A, first alternative

list = { 5, 6, 1, 2, 9, 14, 2, 15, 6, 7, 8, 97 }
itemCount=#list
repeat
  hasChanged = false
  itemCount=itemCount - 1
  for i = 1, itemCount do
    if list[i] > list[i + 1] then
      list[i], list[i + 1] = list[i + 1], list[i]
      hasChanged = true
    end
  end
until hasChanged == false
for i = 1,itemCount do
  print (list[i])
end

-- A, second alternative
function fibonacci(n)
    if n<3 then
        return 1
    else
        return fibonacci(n-1) + fibonacci(n-2)
    end
end

for n = 1, 16 do
    io.write(fibonacci(n), ", ")
end
io.write("...\n")
