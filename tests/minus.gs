;

"A testing program for golfscript. Tests the behavior of the - operator. " puts
"1's indicate passed tests. " puts

# Integers
12 6 - 6 = print
-10 -4 --6 = print

# Arrays
[1 2 3 4 "4" [1 2 3] 5 3 6] [2 2 2 4 8 5] - [1 3 "4" [1 2 3] 3 6] = print
["abc"] [[97 98 99]] - [] = print

# Strings
"aaaabbbcdef" "aceeeeggg" - "bbbdf" = print

# Blocks
{aaaabbbcdef} {aceeeeggg} - {bbbdf} = print

# Tests for correct behavior when types are coerced

# Integers and arrays
1 [1 2] - [] = print
11 [1 2] - [11] = print
[1 2] 1 - [2] = print
[1 2] 11 - [1 2] = print

# Integers and strings
123552 "1224" - "355" = print
"1224" 123552 - "4" = print

# Integers and blocks
123552 {1224} - {355} = print
{1224} 123552 - {4} = print

# Arrays and strings
[48 0 49 49 50 50 51] "1363" - "0\x0022" = print
"1363 7" [48 0 49 49 50 50 51] - "6 7" = print

# Arrays and blocks
["ab" 1 2 [51 52 52]] {3 b 5} - {a1244} = print
{3 b 5} ["ab" 1 2 [51 52 52]] - {5} = print

# Strings and blocks
"aaaabbbcdef" {aceeeeggg} - {bbbdf} = print
{aceeeeggg} "aaaabbbcdef" - {ggg} = print
