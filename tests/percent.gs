;

"A testing program for golfscript. Tests the behavior of the % operator. " puts
"1's indicate passed tests." puts

# Integers
6547110749597242042892482550166185500531458154388279349880441722664169647648019942769688355796826934033556944526174891328347860249
7268799894776010965213129140049183177094755486915943043646681574202763071626567476
%
3413626398632373550993151312819405764075191836133415457618372267142130280424610849
= print

# String indexing with integers
"abcdefg" 2 % "aceg" = print
"123456789" -3 % "963" = print

# Array indexing with integers
["abc" [1 4 5 2] "b" 9 "bcd" 10] 2 % ["abc" "b" "bcd"] = print
["d" [1 2] "e" 2 4 5 6] -3 % [6 2 "d"] = print

# Splitting a string with another string
"abbababbabbbabbababababbaba" "ab" % ["b" "b" "bb" "b" "b" "a"] = print

# Splitting a string with an array
"abbababbabbbabbababababbaba" [97 98] % [[98] [98] [98 98] [98] [98] [97]] = print

# Splitting an array with another array
["a" "bbbb" "b" "a" "a" "c" "a" "d" "a"] ["a"] %
[["bbbb" "b"] ["c"] ["d"]] = print

# Mapping over an array with a block
[0 2 4 6 8] {"a"*} % ["" "aa" "aaaa" "aaaaaa" "aaaaaaaa"] = print

# Mapping over a string with a block
"abcdefgh" {.} % "aabbccddeeffgghh" = print

n
