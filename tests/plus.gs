;

"A testing program for golfscript. Tests the behavior of the + operator. " puts
"1's indicate passed tests. " puts

# Integers
12 6 + 18 = print
-10 -4 + -14 = print

# Arrays
[1 0 "aaa" {1} [3 4 {8}]] [9 62 -1 [29]] +
[1 0 "aaa" {1} [3 4 {8}] 9 62 -1 [29]] = print

# Strings
"abdj\x00  " "\x00shdjuy3" + "abdj\x00  \x00shdjuy3" = print

# Blocks
{1 [1 2 3] "abcd"} {"efgh" 1 39 [1]} + {1 [1 2 3] "abcd" "efgh" 1 39 [1]} = print

# Tests for correct behavior when types are coerced

# Integers and arrays
1 [2 3] + [1 2 3] = print
[1 2] 3 + [1 2 3] = print

# Integers and strings
1 "234" + "1234" = print
"123" 4 + "1234" = print

# Integers and blocks
1 {2 3 4} + {1 2 3 4} = print
{1 2 3} 4 + {1 2 3 4} = print

# Arrays and strings
[48 49 50 32 0] "0123" + "012 \x000123" = print
"0123" [48 49 50 32 0] + "0123012 \x00" = print

# Arrays and blocks
[48 49 50 32 0] {1 2  3} + {48 49 50 32 0 1 2  3} = print
{ 1 2 3 } [48 49 50 32 0] + { 1 2 3  48 49 50 32 0} = print

# Strings and blocks
"abc 0 1    2" {1 2 4} + {abc 0 1    2 1 2 4} = print
{1 2 4} "abc 0 1    2" + {1 2 4 abc 0 1    2} = print
