;

"A testing program for golfscript. Tests the behavior of the | operator. " puts
"1's indicate passed tests." puts

# Integers
12 6 | 14 = print
-10 -4 | -2 = print

# Arrays
[3 4 5 "a" 5 5][5 5 2 "b" 2 1 2 3] | [3 4 5 "a" 2 "b" 1] = print

# Strings
"344555""5522123" | "34521" = print

# Blocks
{344555}{5522123} | {34521} = print

# Tests for correct behavior when types are coerced

# Integers and arrays
12 [1 2] | [12 1 2] = print
1 [1 2] | [1 2] = print

# Integers and strings
123 "1 2" | "123 " = print

# Integers and blocks
123 {1 2} | {123 } = print

# Arrays and strings
[50 48 49 48] "4 1 2 4" | "2014 " = print

# Arrays and blocks
[48 49 48 32 50] {4 1 2} | {48 932501} = print

# Strings and blocks
"1 2 3"{4 2 2 4 6} | {1 2346} = print
