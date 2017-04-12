;

"A testing program for golfscript. Tests the behavior of the & operator. " puts
"1's indicate passed tests." puts

# Integers
12 6 & 4 = print
-10 -4 & -12 = print

# Arrays
[3 4 5 "a" 5 5 "c"][5 5 2 2 1 "b" 2 3 [99]] & [3 5 "c"] = print

# Strings
"344555""5522123" & "35" = print

# Blocks
{344555}{5522123} & {35} = print

# Tests for correct behavior when types are coerced

# Integers and arrays
12 [1 2] & [] = print
1 [1 2] & [1] = print

# Integers and strings
123 "1 2" & "12" = print

# Integers and blocks
123 {1 2} & {12} = print

# Arrays and strings
[50 48 49 48] "4 1 2 4" & "21" = print

# Arrays and blocks
[48 49 48 32 50] {4 1 2} & {4 2} = print

# Strings and blocks
"1 2 3"{4 2 2 4 6} & { 2} = print
