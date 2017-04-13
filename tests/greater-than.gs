;

"A testing program for golfscript. Tests the behavior of the > operator. " puts
"1's indicate passed tests. " puts

# Integers
10 20 > 0 = print
10 10 > 0 = print
10  0 > 1 = print

# Strings
"a" "b"            > 0 = print
"abcdef" "abcdefg" > 0 = print
"" "a"             > 0 = print
"a" "a"            > 0 = print
"b" "a"            > 1 = print
"abcdefg" "abcdef" > 1 = print
"a" ""             > 1 = print

# Blocks
{0 1} {1}   > 0 = print
{0 0} {000} > 0 = print
{} { }      > 0 = print
{} {}       > 0 = print
{000} {0 0} > 1 = print
{1} {0 1}   > 1 = print

# Arrays
[0] [1]             > 0 = print
[] [0]              > 0 = print
[[[]]] [[[1]]]      > 0 = print
["a"] ["b"]         > 0 = print
[[] 0 {}] [[] 0 {}] > 0 = print
[1] [0]             > 1 = print
[0] []              > 1 = print
[[[1]]] [[[]]]      > 1 = print
["b"] ["a"]         > 1 = print

# Slicing strings
"abcdef"  2 > "cdef" = print
"abcdef"  8 > "" = print
"abcdef" -2 > "ef" = print
"abcdef" -8 > "abcdef" = print

# Slicing blocks
{abcdef}  2 > {cdef} = print
{abcdef}  8 > {} = print
{abcdef} -2 > {ef} = print
{abcdef} -8 > {abcdef} = print

# Slicing arrays
[0 1 2 3 4 5] 4 > [4 5] = print
[0 1 2 3] 6 > [] = print
[0 1 2 3 4 5] -4 > [2 3 4 5] = print
[0 1 2 3 4 5] -8 > [0 1 2 3 4 5] = print
[{abc} "abc" [1 2 [3 4 5]] 3 4 5] 2 > [[1 2 [3 4 5]] 3 4 5] = print
