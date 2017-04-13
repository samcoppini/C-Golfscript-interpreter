;

"A testing program for golfscript. Tests the behavior of the < operator. " puts
"1's indicate passed tests. " puts

# Integers
10 20 < 1 = print
10 10 < 0 = print
10  0 < 0 = print

# Strings
"a" "b"            < 1 = print
"abcdef" "abcdefg" < 1 = print
"" "a"             < 1 = print
"a" "a"            < 0 = print
"b" "a"            < 0 = print
"abcdefg" "abcdef" < 0 = print
"a" ""             < 0 = print

# Blocks
{0 1} {1}   < 1 = print
{0 0} {000} < 1 = print
{} { }      < 1 = print
{} {}       < 0 = print
{000} {0 0} < 0 = print
{1} {0 1}   < 0 = print

# Arrays
[0] [1]             < 1 = print
[] [0]              < 1 = print
[[[]]] [[[1]]]      < 1 = print
["a"] ["b"]         < 1 = print
[[] 0 {}] [[] 0 {}] < 0 = print
[1] [0]             < 0 = print
[0] []              < 0 = print
[[[1]]] [[[]]]      < 0 = print
["b"] ["a"]         < 0 = print

# Slicing on strings
"abcdef" 4 < "abcd" = print
"abcd" 6 < "abcd" = print
"abcdef" -4 < "ab" = print
"abcdef" -8 < "" = print

# Slicing on blocks
{abcdef} 4 < {abcd} = print
{abcd} 6 < {abcd} = print
{abcdef} -4 < {ab} = print
{abcdef} -8 < {} = print

# Slicing on arrays
[0 1 2 3 4 5] 4 < [0 1 2 3] = print
[0 1 2 3] 6 < [0 1 2 3] = print
[0 1 2 3 4 5] -4 < [0 1] = print
[0 1 2 3 4 5] -8 < [] = print
[{abc} "abc" [1 2 [3 4 5]] 3 4 5] 4 < [{abc} "abc" [1 2 [3 4 5]] 3] = print
