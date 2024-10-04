lw x28, x0, 512
li x31, 3
sw x0, 512, x31
add x28, x0, x31
lw x27, x0, 512
lw x28, x0, 513
li x31, 32
add x29, x0, x31
li x30, 12
add x31, x0, x29
mul x31, x31, x30
add x29, x0, x31
add x30, x27, x0
add x28, x0, x30
li x31, 8
add x30, x0, x28
mul x30, x30, x31
add x31, x0, x29
add x31, x31, x30
sw x0, 513, x31
add x28, x0, x31
# COND START 10
lw x28, x0, 513
add x31, x28, x0
add x29, x0, x31
li x30, 0
add x31, x0, x29
seq x31, x31, x30
li x30, 0
seq x31, x31, x30
li x30, 0
beq x31, x30, 20
# BODY START 20
lw x28, x0, 513
add x31, x28, x0
add x29, x0, x31
li x30, 10
add x31, x0, x29
rem x31, x31, x30
add x29, x0, x31
li x30, 48
add x31, x0, x29
add x31, x31, x30
ewrite x31
lw x28, x0, 513
add x31, x28, x0
add x29, x0, x31
li x30, 10
add x31, x0, x29
div x31, x31, x30
sw x0, 513, x31
add x28, x0, x31
jal x0, -30
# BODY FINISH
ebreak
