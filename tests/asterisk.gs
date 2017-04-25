;

"A testing program for golfscript. Tests the behavior of the * operator. " puts
"1's indicate passed tests." puts

# Integers
9664032343174822628070702335296303076584200708209481604592376393514851465325234280217
-697679182935662630702834003333132157804625354405109549220005252037383512535564392040
*
-6742394189050027459559685473513589895163259498770110088695989481024344869885063555220111270090061628645589060973681789816126492310054829055012403039115820258984604272680
= print

# Integers and blocks
1 {.1+} 9 * ] [1 2 3 4 5 6 7 8 9 10] = print

# Integers and arrays
[1 [2 3 4] "5" {6 7} "8"] 2 * [1 [2 3 4] "5" {6 7} "8" 1 [2 3 4] "5" {6 7} "8"]
= print

# Integers and strings
"abcdefgh" 5 * "abcdefghabcdefghabcdefghabcdefghabcdefgh" = print

# Joining an array with an array
[1 [2 3] "abc" [5 [6 [7] 8]] [9 7] 10] [1 "2" [3]] *
"\x01\x012\x03\x02\x03\x012\x03abc\x012\x03\x05\x06\a\b\x012\x03\t\a\x012\x0310"
= print

# Joining an array with a string
[1 "b" [1 2 3] [9 [a b]] "c" {d}] "," * "{1,b,\x01\x02\x03,\t,c, d}" ~ = print

# Joining a string with a string
"abcdef" ", " * "a, b, c, d, e, f" = print

# Folding over an array with a block
["3" 2 "5" {4} [6] 7] {+} * {325 4 6 7} = print

# Folding over a string with a block
"abcdef" {*9-} * 968587765155 = print

# Folding over a block with a block
{*9-} {abcdef} * 968587765155 = print

n
