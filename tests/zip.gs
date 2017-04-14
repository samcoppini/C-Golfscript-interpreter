;

"A testing program for golfscript. Tests the behavior of the zip function." puts
"1's indicate passed tests." puts

[[0 1]] zip [[0] [1]] = print
[[0 1] [0 1] [0 1]] zip [[0 0 0] [1 1 1]] = print
[[0 1 2 3 4 5][0 1 2 3 4][0 1 2 3][0 1 2][0 1][0]] zip
[[0 0 0 0 0 0] [1 1 1 1 1] [2 2 2 2] [3 3 3] [4 4] [5]] = print
[[] [0] [0 1] [2]] zip [[0 0 2] [1]] = print
["abcd" "efgh"] zip ["ae" "bf" "cg" "dh"] = print
["aaa" "bbbbbb" "cccc" "" "ee" "fffff"] zip
["abcef" "abcef" "abcf" "bcf" "bf" "b"] = print
[{aaa} "bbbbbb" "cccc" "" "ee" "fffff"] zip
[{abcef} {abcef} {abcf} {bcf} {bf} {b}] = print
[{aaa} "bbbbbb" "cccc" [0 1 2] "ee" "fffff"] zip
"[{abc\x00ef} {abc\x01ef} {abc\x02f} {bcf} {bf} {b}]"~ = print
[["abcd" "bb"] "abcd" [99 99 99] {17262} [[[]]]] zip
[["abcd" 97 99 49 [[]]] ["bb" 98 99 55] [99 99 50] [100 54] [50]] = print
["abcdef" [98 99 48 49 50 51 52 53] {726267338}] zip
["ab7" "bc2" "c06" "d12" "e26" "f37" "43" "53" "8"] = print
