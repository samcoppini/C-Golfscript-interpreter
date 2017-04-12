;

"A testing program for golfscript. Tests the behavior of the ? operator. " puts
"1's indicate passed tests." puts

# Exponentation
11 16 ? 45949729863572161 = print
-3 19 ? -1162261467 = print

# Finding the index of an element
19 ["a" "19" {19} [19] 19] ? 4 = print
19 ["19" [19] -18 1817626 [19 [19]]] ? -1 = print
"abbaabbab" "bab" ? 6 = print
"ababbabab" "bbb" ? -1 = print
"1" [49 [49 1 "1"] {1} "1"] ? 2 = print
"1" [49 [0] {11} "1"] ? 3 = print
"1" [49 [49] {1} "1"] ? 1 = print
"1" [49 [48] {0} "0"] ? -1 = print
"" [0 []] ? 1 = print
"abc" [[97 "b" 99]] ? -1 = print
[0 "0" "" [0 0] {0} [0]] [0] ? 5 = print
[0] [0 "0" "" [0 0] {0} [0]] ? -1 = print

# Finding first element for which a condition is true
[3 4 1 2 9 99 999 999] {8>} ? 9 = print
[0 1 2 3 4 6 7 8 9] {5=} ? ] [] = print
"abcdefghijkl" {99 =} ? 99 = print
"abcdefghijkl" {111 =} ? ] [] = print
{99 =}  {abcdefghijkl} ? 99 = print
{111 =} {abcdefghijkl} ? ] [] = print
