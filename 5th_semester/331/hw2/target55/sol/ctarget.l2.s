movq $0x36d52185, %rdi
movq %rdi, %rsi
sarq $0x2, %rsi
ret

-------------------------------------------

ctarget.l2.o:     file format elf64-x86-64


Disassembly of section .text:

0000000000000000 <.text>:
   0:   48 c7 c7 85 21 d5 36    mov    $0x36d52185,%rdi
   7:   48 89 fe                mov    %rdi,%rsi
   a:   48 c1 fe 02             sar    $0x2,%rsi
   e:   c3                      retq
