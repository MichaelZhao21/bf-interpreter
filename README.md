# BF Interpreter in C :)

This is just a simple interpreter I'm writing for [Brainfuck](https://esolangs.org/wiki/Brainfuck). This is purely for fun and to help me relearn C (and learn BF along the way). I will also be putting various BF programs in the [examples](/examples/) folder, so feel free to look through those. (most come from [http://www.brainfuck.org/](http://www.brainfuck.org/)!)

## Execution

To compile the program, simply use gcc:

```
gcc -o run run.c
```

Then, you can use the run executable to run your BF program. Let's say I want to run `examples/helloworld.bf`:

```
./run examples/helloworld.bf
```

## Implementation

Currently, I have implemented the basic BF characters, with a few optimizations:

| Command | Description                                                         |
| ------- | ------------------------------------------------------------------- |
| `>`     | Move the pointer to the right                                       |
| `<`     | Move the pointer to the left                                        |
| `+`     | Increment the memory cell at the pointer                            |
| `-`     | Decrement the memory cell at the pointer                            |
| `.`     | Output the character signified by the cell at the pointer           |
| `,`     | Input a character and store it in the cell at the pointer           |
| `[`     | Jump past the matching `]` if the cell at the pointer is 0          |
| `]`     | Jump back to the matching `[` if the cell at the pointer is nonzero |

> Copied from https://esolangs.org/wiki/Brainfuck

In addition to these base elements, there are a few specifications to my interpreter:

* All memory cells start with a value of `0`, and the initial pointer will also start at memory cell `0`
* Negative addresses of memory cells will crash the program, but there is no upper bound to the number of cells (except for the limits of your computer's memory)
* Each memory cell will be a `signed int` from C, so it can hold 32 bits, for values between `-2147483648` and `2147483647` (on older systems this may be 16 bits, but it will depend on the architecture and os -- though for most modern systems it should be 32 bit)
* Following standard convention, the `[+]` and `[-]` wrapping loops will be evaluated to `0`
* Newline characters will be a standard C newline character, which maps to `10` (LF)
* EOF will simply leave memory cells untouched

## Approach

I'll follow a simplified interpreter design with 3 different stages:

1. **Lexer** that reads the code and turns it into tokens
2. **Optimizer** that takes the tokens and creates optimizations, such as the 0 loops
3. **Interpreter** that interprets the tokens and runs the program
