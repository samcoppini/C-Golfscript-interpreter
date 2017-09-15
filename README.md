C Golfscript Interpreter
========================
An interpreter for the esoteric programming language [Golfscript](http://www.golfscript.com/golfscript/), written in C.

## Usage:
    Usage: golf.exe [--run script | file | --help]
    --help           display this help message
    --run script     execute script passed in as string on the command line

## Building
Download the source by using the following command in your command prompt:
```sh
$ git clone https://github.com/samcoppini/C-Golfscript-interpreter.git
```
or, alternatively, just download a [zip file of the source code](https://github.com/samcoppini/C-Golfscript-interpreter/archive/master.zip).

After downloading it, simply use `make` to create the executable. Requires a C11 compatible compiler. Then run `make test` to make sure everything works, or if you changed something and want to make sure nothing broke.
