;

"A testing program for golfscript. Tests the behavior of the = operator." puts
"1's indicate passed tests." puts

# Integers
0 0  =   print
0 1  = ! print
-1 1 = ! print

# Strings/Arrays/Blocks
"abcdef" "abcdef"      =   print
"abcdef" "abcdeg"      = ! print
{abcdef} {abcdef}      =   print
{abcdef} {abcdeg}      = ! print
[0 1 22] [0 1 22]      =   print
[0 1 22] [0 1 21]      = ! print
[[[[]]]] [[[[]]]]      =   print
[[0] [0]] [[0] [0]]    =   print
[0 1 2] [[0] [1] [2]]  = ! print
[97 98 99] "abc"       =   print
[97 98 99] "abcd"      = ! print
[97 98 99 100] "abc"   = ! print
[97 98 99 "d"] "abcd"  = ! print
["abcdef"] "abcdef"    = ! print
[97 98 99] {abc}       =   print
[97 98 99] {abcd}      = ! print
[97 98 99 100] {abc}   = ! print
[97 98 99 {d}] {abcd}  = ! print
"abcdef" {abcdef}      =   print
"abcdefgh" {abcdef}    = ! print
"abcdef" {abcdefgh}    = ! print

# Selecting individual array/string/block elements
"abcdef"     1 = 98   = print
"abcdef"     6 = ] [] = print
"abcdef"    -1 = 102  = print
"abcdef"    -7 = ] [] = print
{abcdef}     1 = 98   = print
{abcdef}     6 = ] [] = print
{abcdef}    -1 = 102  = print
{abcdef}    -7 = ] [] = print
[0 "1" [2]]  1 = "1"  = print
[0 "1" [2]]  3 = ] [] = print
[0 "1" [2]] -1 = [2]  = print
[0 "1" [2]] -4 = ] [] = print

n
