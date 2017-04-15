;

"A testing program for golfscript. Tests the behavior of the , operator. " puts
"1's indicate passed tests." puts

# Creating integer arrays
10 , [0 1 2 3 4 5 6 7 8 9] = print
0 , [] = print
-10 , [] = print

# Getting the size of an array or string
[] , 0 = print
[1 2 3 4 5 6 7 8 9] , 9 = print
"" , 0 = print
"1\x0023456789" , 10 = print

# Filter function
[0 1 2 3 4 5 6 7 8 9] {3 % 0 =} , [0 3 6 9] = print
[] {5 *} , [] = print
"abcdefghi" {[.] [100 100] > } , "efghi" = print
"" {88 *} , [] = print
{0 1 2 3 4 5 6 7} {32 >} , {01234567} = print
{} {0p} , {} = print

n
