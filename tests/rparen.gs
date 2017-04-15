;

"A testing program for golfscript. Tests the behavior of the ( operator. " puts
"1's indicate passed tests. " puts

# Integers
4999  ) 5000 = print
-1    )    0 = print
-1000 ) -999 = print

# Arrays
[1 2 3 4 5] ) ] [[1 2 3 4] 5] = print

# Strings
"abcd" ) ] ["abc" 100] = print

# Blocks
{abcd} ) ] [{abc} 100] = print

n
