;

"A testing program for golfscript. Tests the behavior of the / operator. " puts
"1's indicate passed tests." puts

# Integers
9 4 / 2 = print

# Splitting an array into groups of a certain size
[[1 2 [3]] "a" "b" {1} [1 2] "?" 0 1 "2" 4] 3 /
[[[1 2 [3]] "a" "b"] [{1} [1 2] "?"] [0 1 "2"] [4]] = print
[1 2 [3 4] 5 {6} {7 8} [9]] -2 / [[[9] {7 8}] [{6} 5] [[3 4] 2] [1]] = print

# Splitting a string into groups
"abcdefghij" 3 / ["abc" "def" "ghi" "j"] = print
"0123456789abcdef" -5 / ["fedcb" "a9876" "54321" "0"] = print

# Split a string based on a delimiter
"1, 2,3 , , , 4 5, " ", " / ["1" "2,3 " "" "" "4 5" ""] = print

# Splitting an array with an array
["a" "b" "b" "a" "b" "b" "b" "a" "a" "b"] ["a" "b"] /
[[] ["b"] ["b" "b" "a"] []] = print

# Splitting a string with an array
"abbababbbabbab" [97 98] / [[] [98] [] [98 98] [98] []] = print

# Executing a block over an array
[1 "abcd" [1 "abc"] {100} 0 -1] {2*} / ]
[2 "abcdabcd" [1 "abc" 1 "abc"] 100 100 0 -2] = print

# Executing a block over a string
"abcdef" {32-} / ] [65 66 67 68 69 70]  = print

# Unfold operation (block/block)
1 {100<} {2 *} / [1 2 4 8 16 32 64] = print

n
