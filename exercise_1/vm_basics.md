# Exercise 1: VM Obfuscation Basics

**Change the working directory:**
```sh
cd exercise_1
```

In this exercise we are going to explore the concept of VM obfuscation.

1. Build `minivm.cpp`:
```sh
   clang-20 -O3 -fno-slp-vectorize -std=c++17 minivm.cpp -o minivm
```
2. Run the VM (`./minivm 1 2 3 4`) and do the following exercises:
   - Extract the active VM bytecode (look at the source code, starting from `main` at the bottom)
     `07 04 00 01 06 05 02 03 02 06 04 05 01 06`
   - How many registers does the VM have in the context?
     256
   - Make a list of all available opcodes, their index and what they do.

     | Opcode | Instruction | Description |
     |--------|-------------|-------------|
     | 0 | LABEL | Marqueur de label (pour les sauts) |
     | 1 | RET reg | Retourne la valeur du registre |
     | 2 | ADD dst, op1, op2 | dst = op1 + op2 |
     | 3 | MOVIMM dst, imm64 | Charge une valeur immédiate 64 bits |
     | 4 | CMP dst, op1, op2 | dst = 1 si op1 == op2, sinon 0 |
     | 5 | JCC cond, label | Saut conditionnel si cond ≠ 0 |
     | 6 | XOR dst, op1, op2 | dst = op1 ^ op2 |
     | 7 | OR dst, op1, op2 | dst = op1 \| op2 |
     | 8 | MUL dst, op1, op2 | dst = op1 * op2 |

   - What does the active VM bytecode do? Write it as C pseudocode.
```c
     return (r0 | r1) + (r2 ^ r3);
```

3. For this exercise you will write your own bytecode. Look at the commented-out example code to figure out how to do it.
   - Implement a function that adds the first two arguments together: `return a + b`
```sh
     python3 minivm.py --bytecode "02 03 00 01 01 03" 3 2
```
   - Implement a function that multiplies the first two arguments together: `return a * b`
```sh
     python3 minivm.py --bytecode "08 03 00 01 01 03" 3 2
```
   - Implement a function that implements: `return a - b`

     **Explication :**
```
     -x = (~x) + 1   (complément à deux)
     ~x = x XOR 0xFFFFFFFFFFFFFFFF

     Donc : a - b = a + (-b)
                  = a + (~b + 1)
                  = a + ((b XOR 0xFFFFFFFFFFFFFFFF) + 1)
```

     **Bytecode :**
```
     03 0A FF FF FF FF FF FF FF FF    # MOVIMM r10, 0xFFFFFFFFFFFFFFFF
     06 02 01 0A                      # XOR r2, r1, r10  (r2 = ~r1)
     03 03 01 00 00 00 00 00 00 00    # MOVIMM r3, 1
     02 02 02 03                      # ADD r2, r2, r3   (r2 = -r1)
     02 02 00 02                      # ADD r2, r0, r2   (r2 = r0 - r1)
     01 02                            # RET r2
```

     **Solution :**
```sh
     python3 minivm.py --bytecode "03 0A FF FF FF FF FF FF FF FF 06 02 01 0A 03 03 01 00 00 00 00 00 00 00 02 02 02 03 02 02 00 02 01 02" 7 3
```

   - Implement a function that implements: `return a == 42 ? 1337 : 0`

     *TODO*

If you finish early, here are a few bonus exercises (not required):

4. Implement `fib(n)` in VM bytecode. You can add new opcodes if necessary.

Do these exercises for `minivm-tailcall.cpp` (make sure to recompile):

5. Analyze the `minivm` binary in your favorite disassembler/decompiler. Would it be difficult to write a disassembler for the VM bytecode if the opcodes were switched around?
6. Which C++ attribute is responsible for the `jmp reg` opcode at the end of the handler?