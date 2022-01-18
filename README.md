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
jmp: loc(Sym | Value)  # unconditional jmp, accepts label or line number(uint)
jc: cond(Value), lbl(Sym)  # jump if cond is true
lbl: lbl(Sym)  # set label.
call: idx(WPtr), lbl(Sym)  # increments idx, assigns [-1]+1 to idx, and jumps to lbl
ret: idx(WPtr)  # decrements pointer by one, and jumps to idx

# sys
exit: exit_code(Value)
fork: ???
read: fd(Value), ptr(WPtr), size(Value)  # appends trailing \0. [0] set to bytes written to mem
write: fd(Value), ptr(Ptr), size(Value)  # read and write ASCIIs. [0] set to bytes written to stream
open: name(Ptr | Sym), option(Value)  # [0] sets to fd. Files are opened in text mode
close: fd(Value)

# extra
# added for either debug or simplify instructions
print_num: fd(Value), val(Value)

# extern
src: script_name(Sym)  # source another file, load labels and symbols, don't execute
```

## TODO
- [x] Implement all the functions listed in Predefined Function Section (Will not implement fork in near future)

- [ ] Implement Character

- [ ] Write more tests

- [ ] Create all the string literals during preprocessing??

- [ ] Fix error report

- [ ] Move argc check to preprocess time

## Implement note

This interpreter was originally writen in rust.
