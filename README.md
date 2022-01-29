# Low Level Interpreted -- An attempt to create low level interpreted language

## Ideas:

- Memory slots: 1 slot store a `union{ double, long }`. Functions determine when to accept `long` or `double`
- [0] is reserved for writing output, [1] is reserved for writing system error code, [-1] is reserved for writing current code address(line number)
- Primitive type: Num, Idx, Var, Lbl, Ltl
- No concept of Stack. Plain memory: Positive Memory and Negative Memory
    - Positive Memory(pmem): idx>=0, stores modifiable data
    - Negative Memory(nmem): idx<0, store and referenced by string literals, read only, set by interpreter
- Stack can be simulated by `push pop call ret` functions

## Syntax:

- Comment: everything in the same line after hashtag
- Character: single character wrapped in single quote, converts to Num type which value is its ASCII value
- Reserved symbols: `:,"[]$#`
- Syntax of primitive Type:
    - Num: `-?[1-9][0-9]*(?:.[0-9]+)?`
    - Idx: Integer or Var wrapped in square brackets, no space, can be nested: [[1]] / [[[$aVariable]]]
    - Var: `$name`
    - Sym: `[^\s0-9$#"\[\]:,]+`
    - Ltl: String literal wrapped in double quotes
- Args:
    - Value: {Num, Idx, Var, Lbl(line number)}
    - Ptr: {Idx, Var, Ltl}
    - Writable Ptr: ptr with positive index
        - Ltl returns negative ptr, which is unwritable
    - Symbol: {Sym}
- Statement: `Func: arg1, arg2, arg3...`
- 1 line per statement
 
## Predefined Functions:

```bash
# memory management
mov: des(WPtr), src(Value)  # assignment, read value
cpy: des(WPtr), src(Ptr), size(Value)  # memcpy. When src = Ltl, a new ltl is created and its idx is used as src idx
size:  # write the size of [0]
var: name(Sym), idx(Ptr)  # Creates or update $name with index = idx
loc: ptr(Ptr)  # writes the ptr as value to [0]
allc: size(Value)  # Push slots to pmem
push: idx(WPtr), val(Value)  # increments idx and then assigns val to idx
pop: idx(WPtr)  # decrements pointer
ltof: idx(WPtr)  # change from Long to Double
ftol: idx(WPtr)  # change from Double to Long

# maths, [0] is set as result
# args can be index, var or Num
add: left(Value), right(Value)  # +
sub: left(Value), right(Value)  # -
mul: left(Value), right(Value)  # *
div: left(Value), right(Value)  # /
mod: left(Value), right(Value)  # %
inc: idx(WPtr)  # ++
dec: idx(WPtr)  # --
addf: left(Value), right(Value)  # (float)+
subf: left(Value), right(Value)  # (float)-
mulf: left(Value), right(Value)  # (float)*
divf: left(Value), right(Value)  # (float)/
incf: idx(WPtr)  # (float)++
decf: idx(WPtr)  # (float)--

# cmp, [0] is set to either 0 or 1
eq: left(Value), right(Value)  # ==
ne: left(Value), right(Value)  # !=
gt: left(Value), right(Value)  # >
lt: left(Value), right(Value)  # <
eqf: left(Value), right(Value)  # (float)==
nef: left(Value), right(Value)  # (float)!=
gtf: left(Value), right(Value)  # (float)>
ltf: left(Value), right(Value)  # (float)<

# logic, [0] is set to either 0 or 1
and: left(Value), right(Value)  # &&
or: left(Value), right(Value)  # ||
not: bool(Value)  # !

# control flow
jmp: loc(Value)  # unconditional jmp, accepts label or line number(uint)
jc: cond(Value), lbl(Value)  # jump if cond is true
lbl: lbl(Sym)  # set label.
call: idx(WPtr), lbl(Value)  # increments idx, assigns [-1]+1 to idx, and jumps to lbl
ret: idx(WPtr)  # decrements pointer by one, and jumps to idx

sys: syscallcode(Value)  # pass arguments to registers

# extra
# added for either debug or simplify instructions
print_num: fd(Value), val(Value)

# extern
src: script_name(Sym)  # source another file, load labels and symbols, don't execute
```

## Sys Calls
```
exit(0): exit\_code(Value)
read(1): fd(Value), ptr(WPtr), size(Value)  # appends trailing \0. [0] set to bytes written to mem
write(2): fd(Value), ptr(Ptr), size(Value)  # read and write ASCIIs. [0] set to bytes written to stream
open(3): name(Ptr), option(Value)  # [0] sets to fd. Files are opened in text mode
close(4): fd(Value)
```

## Open option
Number consisting 6 or less digits

 \_ \_ \_ \_ \_ \_<br>
 6 5 4 3 2 1

 1: read<br>
 2: write<br>
 3: append<br>
 4: truncate<br>
 5: create<br>
 6: create\_new<br>

 All digits should be either be 0 or 1, representing boolean value.<br>
 Boolean values are originally passed to std::fs::OpenOptions.<br>
 Read rust docs for more details about each option.<br>

 Example: opening text.txt in read only mode<br>
 ```
 mov: $1, "text.txt"
 mov: $2 ,1
 sys: 3
 ```

 Example: opening text.txt in write-only mode, create file if it does not exists, and will truncate it if it does.
 `open:"text.txt",11010`


## TODO
- [x] Implement all the functions listed in Predefined Function Section (Will not implement fork in near future)

- [ ] Create typed ops for all combinations of types of arguments

- [ ] Change sys ops to sys calls

- [ ] Add tracing JIT (No idea how to implement. Reference: Pypy)

- [ ] Implement Character

- [ ] Write more tests

- [x] Create all the string literals during preprocessing??

- [ ] Fix error report

## Implement note

This interpreter was originally writen in rust. But the performance of the C version is a lot better, so the rust version was abandoned.
