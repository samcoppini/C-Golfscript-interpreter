;

"A testing program for golfscript. Tests the behavior of the ( operator. " puts
"1's indicate passed tests. " puts

# Integers
5000 (  4999 = print
0    (    -1 = print
-999 ( -1000 = print

# Arrays
[1 2 3 4 5] ( ] [[2 3 4 5] 1] = print

# Strings
"abcd" ( ] ["bcd" 97] = print

# Blocks
{abcd} ( ] [{bcd} 97] = print
