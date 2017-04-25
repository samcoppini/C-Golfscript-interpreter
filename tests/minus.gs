;

"A testing program for golfscript. Tests the behavior of the - operator. " puts
"1's indicate passed tests. " puts

# Integers
7657054118062213451514667490700296350060529522193244247306636924894346913339414124064481945782139724187637014708246159392442716963
8084851358747042843051738723271014379384551399516129353573115895214077042985887878413538523677429373434272064340889606451124286671
- -427797240684829391537071232570718029324021877322885106266478970319730129646473754349056577895289649246635049632643447058681569708
= print

# Arrays
[1 2 3 4 "4" [1 2 3] 5 3 6] [2 2 2 4 8 5] - [1 3 "4" [1 2 3] 3 6] = print
["abc"] [[97 98 99]] - [] = print

# Strings
"aaaabbbcdef" "aceeeeggg" - "bbbdf" = print

# Blocks
{aaaabbbcdef} {aceeeeggg} - {bbbdf} = print

# Tests for correct behavior when types are coerced

# Integers and arrays
1 [1 2] - [] = print
11 [1 2] - [11] = print
[1 2] 1 - [2] = print
[1 2] 11 - [1 2] = print

# Integers and strings
123552 "1224" - "355" = print
"1224" 123552 - "4" = print

# Integers and blocks
123552 {1224} - {355} = print
{1224} 123552 - {4} = print

# Arrays and strings
[48 0 49 49 50 50 51] "1363" - "0\x0022" = print
"1363 7" [48 0 49 49 50 50 51] - "6 7" = print

# Arrays and blocks
["ab" 1 2 [51 52 52]] {3 b 5} - {a1244} = print
{3 b 5} ["ab" 1 2 [51 52 52]] - {5} = print

# Strings and blocks
"aaaabbbcdef" {aceeeeggg} - {bbbdf} = print
{aceeeeggg} "aaaabbbcdef" - {ggg} = print

n
