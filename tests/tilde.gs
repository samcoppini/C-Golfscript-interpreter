;

"A testing program for golfscript. Tests the behavior of the ~ operator. " puts
"1's indicate passed tests. " puts

# Integers
0 ~ -1 = print
-1 ~ 0 = print
72867 ~ -72868 = print
-72868 ~ 72867 = print

# Arrays
[1 2 3 4] ~ ] [1 2 3 4] = print
[] ~ ] [] = print

# Strings
5 ".*" ~ 25 = print
1 "" ~ print

# Blocks
5 {.*} ~ 25 = print
1 {} ~ print

n
