# Skalculator

Skalculator is a simple GUI RPN calculator using only X11.

To know more about Reverse Polish Notation (RPN), read [this link](https://en.wikipedia.org/wiki/Reverse_Polish_notation).

## Features

Not much in terms of features:
- Double floating point precision
- 7 operations: `+` `-` `*` `/` `%` `^` `1/x`
- Stack operations: `AC` `POP` `SWAP` `ENTER` (push)
- Non-interactive input mode

## Dependencies
Skalculator depends only on C standard libraries and X11, but you'll need `make` and a C compiler aliased as `cc` to build the program.

## Building Skalculator
Go to the project's root folder, then run:
```sh
make
```

## Running Skalculator
There are 3 main ways to use the program:

### Just open the calculator

Simply run the program without any additional arguments
```sh
./skalculator
```

### Pre-compute argument

Run the program with an additional string argument
```sh
./skalculator [argument]
```

Example:
```sh
./skalculator "5 4 3 SWAP - *"
```
This automatically types `5` `ENTER` `4` `ENTER` `3` `SWAP` `-` `*` and display the result.

### Load script

Run the program with `-f` or `--file` option, followed by the file name:
```sh
./skalculator --file [script]
```

Suppose you have a file called `myscript.ska` with the contents:
```
5 4 3 SWAP - *
```

You can automatically run this file upon opening skalculator by callint it via:
```sh
./skalculator --file myscript.ska
```
The result should be the same as the argument example.

All the supported tokens for argument and script modes are:
- Numbers: `329` `-256` `2.38e10`
- Operations: `+` `-` `*` `/` `%` `^` `1/x`
- Stack commands: `AC` `POP` `SWAP` `ENTER`
- Other commands: `C` `+/-`

### Create little programs with Skalculator

Combining Skalculator and CLI tools, it is possible to write very simple programs. See `run_example.sh` inside `examples` folder.

Example:
```sh
./examples/run_example.sh 1.570795
```
This program runs a Taylor's series approximation for `sin(x)`. Therefore, since `PI/2 ~ 1.570795`, Skalculator will open showing `1.00001`, which is an approximation for `sine(PI/2)`.

Experiment running the program with other values, such as `3.14159`.

## License
```
BSD 3-Clause License
Copyright (c) 2024, Felipe V. Calderan
All rights reserved.
See the full license inside LICENSE.txt file
```
