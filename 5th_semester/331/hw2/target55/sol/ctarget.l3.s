leaq    0x28(%rsp), %rdi
leaq    0x8(%rsp), %rsi
ret


-------------------------------------------

ctarget.l3.o:     file format elf64-x86-64


Disassembly of section .text:

0000000000000000 <.text>:
   0:   48 8d 7c 24 28          lea    0x28(%rsp),%rdi
   5:   48 8d 74 24 08          lea    0x8(%rsp),%rsi
   a:   c3                      retq