;

"A testing program for golfscript. Tests the behavior of the ! operator. " puts
"1's indicate passed tests. " puts

# Integers
0 ! print
1 ! 0 = print

# Arrays
[] ! print
[0] ! 0 = print

# Strings
"" ! print
"0" ! 0 = print

# Blocks
{} ! print
{0} ! 0 = print

n
