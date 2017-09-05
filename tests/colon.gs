;

"A testing program for golfscript. Tests the behavior of the : operator." puts
"1's indicate passed tests." puts

# Assigning to an identifier
1:one;
0 one print

# Assigning to whitespace
1: ;
0 print

# Assigning to a builtin function
1:^;
0^print

# Assigning to a string
1:"0";
"0"print

# Assigning to a comment
1:#This is a comment
;0#This is a comment
print

# Assigning to a number
1:0;
0print

# Getting rid of excess zeroes on the stack
;;;;

n
