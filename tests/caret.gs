;

"A testing program for golfscript. Tests the behavior of the ^ operator. " puts
"1's indicate passed tests." puts

# Integers
12 6 ^ 10 = print
-10 -4 ^ 10 = print

# Arrays
[3 4 5 "a" [99] 5 5][5 5 2 2 "b" "c" 1 2 3] ^ [4 "a" 2 "b" 1] = print

# Strings
"344555""5522123" ^ "421" = print

# Blocks
{344555}{5522123} ^ {421} = print

# Tests for correct behavior when types are coerced

# Integers and arrays
12 [1 2] ^ [12 1 2] = print
1 [1 2] ^ [2] = print

# Integers and strings
123 "1 2" ^ "3 " = print

# Integers and blocks
123 {1 2} ^ {3 } = print

# Arrays and strings
[50 48 49 48] "4 1 2 4" ^ "04 " = print

# Arrays and blocks
[48 49 48 32 50] {4 1 2} ^ {893501} = print

# Strings and blocks
"1 2 3"{4 2 2 4 6} ^ {1346} = print

n
