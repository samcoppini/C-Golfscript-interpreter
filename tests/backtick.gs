;

"A testing program for golfscript. Tests the behavior of the ` operator. " puts
"1's indicate passed tests. " puts

# Integers
1 ` "1" = print

# Strings
"abc" ` "\"abc\"" = print

# Blocks
{abc} ` "{abc}" = print

# Arrays
[] ` "[]" = print
[[1] "abc" {0} 1971] ` "[[1] \"abc\" {0} 1971]" = print

["abc" [1 "abc" {0} []] 1 {99}] `````
"\"\\\"\\\\\\\"\\\\\\\\\\\\\\\"[\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\"abc\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\" [1 \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\"abc\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\" {0} []] 1 {99}]\\\\\\\\\\\\\\\"\\\\\\\"\\\"\""
= print
