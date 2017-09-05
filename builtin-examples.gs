;

{n print}:nl;

"A testing program for golfscript. Tests that the examples given on the "
"builtin documentation page work correctly. 1's indicate passed tests." + puts

"Tests for ~" puts
5~ -6= print
"1 2 +"~ 3= print
{1 2 +}~ 3= print
nl

"Tests for `" puts
1` "1" = print
[1 [2] 'asdf']` "[1 [2] \"asdf\"]" = print
"1"` "\"1\"" = print
{1}` "{1}" = print
nl

"Tests for !" puts
1! 0 = print
{asdf}! 0 = print
""! 1 = print
nl

"Tests for @" puts
1 2 3 4 @ ][1 3 4 2] = print
nl

"Tests for $" puts
1 2 3 4 5 1$ ] [1 2 3 4 5 4] = print
'asdf'$ "adfs" = print
[5 4 3 1 2]{-1*}$ [5 4 3 2 1] = print
nl

"Tests for +" puts
5 7 + 12 = print
'asdf'{1234} + {asdf 1234} = print
[1 2 3][4 5] + [1 2 3 4 5] = print
nl

"Tests for -" puts
1 2-3+ ] [1 -1] = print
1 2 -3+] [1 -1] = print
1 2- 3+] [2] = print
[5 2 5 4 1 1][1 2] - [5 5 4] = print
nl

"Tests for *" puts
2 4 * 8 = print
2 {2*} 5* 64 = print
[1 2 3]2* [1 2 3 1 2 3] = print
3'asdf'* 'asdfasdfasdf' = print
[1 2 3]','* "1,2,3" = print
[1 2 3][4]* [1 4 2 4 3] = print
'asdf'' '* "a s d f" = print
[1 [2] [3 [4 [5]]]]'-'* "1-\x02-\x03\x04\x05" = print
[1 [2] [3 [4 [5]]]][6 7]* [1 6 7 2 6 7 3 [4 [5]]] = print
[1 2 3 4]{+}* 10 = print
'asdf'{+}* 414 = print
nl

"Tests for /" puts
7 3 / 2 = print
[1 2 3 4 2 3 5][2 3] / [[1] [4] [5]] = print
'a s d f'' '/ ["a" "s" "d" "f"] = print
'assdfs' 's'/ ["a" "" "df" ""] = print
[1 2 3 4 5] 2/ [[1 2] [3 4] [5]] = print
0 1 {100<} { .@+ } / ] [89 [1 1 2 3 5 8 13 21 34 55 89]] = print
[1 2 3]{1+}/ ][2 3 4] = print
nl

"Tests for %" puts
7 3 % 1 = print
'assdfs' 's' % ["a" "df"] = print
[1 2 3 4 5] 2 % [1 3 5] = print
[1 2 3 4 5] -1 % [5 4 3 2 1] = print
[1 2 3]{.}% [1 1 2 2 3 3] = print
nl

"Tests for |" puts
5 3 | 7 = print
nl

"Tests for &" puts
[1 1 2 2][1 3]& [1] = print
nl

"Tests for ^" puts
[1 1 2 2][1 3]^ [2 3] = print
nl

"Tests for []" puts
1 2 [\] [2 1] = print
nl

"Tests for \\" puts
1 2 3 \ ] [1 3 2] = print
nl

"Tests for ;" puts
1 2 3 ; ][1 2] = print
nl

"Tests for <" puts
3 4 < 1 = print
"asdf" "asdg" < 1 = print
[1 2 3] 2 < [1 2] = print
{asdf} -1 < {asd} = print
nl

"Tests for >" puts
3 4 > 0 = print
"asdf" "asdg" > 0 = print
[1 2 3] 2 > [3] = print
{asdf} -1 > {f} = print
nl

"Tests for =" puts
3 4 = 0 = print
"asdf" "asdg" = 0 = print
[1 2 3] 2 = 3 = print
nl

"Tests for ," puts
10, [0 1 2 3 4 5 6 7 8 9] = print
10,, 10 = print
10, {3%}, [1 2 4 5 7 8] = print
nl

"Tests for ." puts
1 2 3 . ] [1 2 3 3] = print
nl

"Tests for ?" puts
2 8 ? 256 = print
5 [4 3 5 1] ? 2 = print
[1 2 3 4 5 6] {.* 25=} ? 5 = print
nl

"Tests for (" puts
5 ( 4 = print
[1 2 3]( ] [[2 3] 1] = print
nl

"Tests for )" puts
5 ) 6 = print
[1 2 3] ) ] [[1 2] 3] = print
nl

"Tests for or" puts
5 {1 0/} or 5 = print
nl

"Tests for and" puts
5 {1 1 +} and 2 = print
nl

"Tests for xor" puts
0 [3] xor [3] = print
2 [3] xor 0 = print
nl

"Tests for do" puts
5{1-..}do ] [4 3 2 1 0 0] = print
nl

"Tests for while" puts
5{.}{1-.}while ] [4 3 2 1 0 0] = print
5{.}{1-.}until ] [5] = print
nl

"Tests for if" puts
1 2 3 if 2 = print
0 2 {1.} if ] [1 1] = print
nl

"Tests for abs" puts
-2 abs 2 = print
nl

"Tests for zip" puts
[[1 2 3][4 5 6][7 8 9]]zip [[1 4 7][2 5 8][3 6 9]] = print
['asdf''1234']zip ["a1" "s2" "d3" "f4"] = print
nl

"Tests for base" puts
[1 1 0] 2 base 6 = print
6 2 base [1 1 0] = print
nl
