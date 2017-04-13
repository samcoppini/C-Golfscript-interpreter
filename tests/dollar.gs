;

"A testing program for golfscript. Tests the behavior of the $ operator. " puts
"1's indicate passed tests. " puts

# Integers
2 1 0  0  $ 0 = print
       1  $ 1 = print
       2  $ 2 = print
       -1 $ 2 = print
       -2 $ 1 = print
       -3 $ 0 = print
       3 $ ] [2 1 0] = print
       -99 $ ] [] = print

# Strings
"euyg4u873ukbjbssagh\x00\n\f\xFF\xF0k2jgujadjh" $
"\x00\n\f23478aabbdeggghhjjjjkkssuuuuy\xF0\xFF" = print

# Arrays
["abcd" {1111} "ab" "1" "11" [97 98 99] "11111" [49 49 49]] $
["1" "11" [49 49 49] {1111} "11111" "ab" [97 98 99] "abcd"] = print
[27 62652 8326735 733 -283 82752 81 0 -2723 -373 -4726 376] $
[-4726 -2723 -373 -283 0 27 81 376 733 62652 82752 8326735] = print

# Sorting by a mapping
[1982 52653 7387 516728 9383 67353 -12673 0 236 -9262] {-1*} $
[516728 67353 52653 9383 7387 1982 236 0 -9262 -12673] = print
["abc" [0 0 0 0 0 0 0] "ab" "abcd" "abbbba"] {,} $
["ab" "abc" "abcd" "abbbba" [0 0 0 0 0 0 0]] = print
