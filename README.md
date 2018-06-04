# stackl
Stack based virtual machine interpreter and a C compiler.

This development effort is intended to support an undergradutate Operating 
Systems class.  It has been used in both the Operating Systems and Compilers
classes at Oregon Institute of Technology for several years. 

For the Compilers class, students develop a compiler that outputs stackl 
assembly code. This allows students to perform code generation without the 
need to learn a complex assembly language.

For the Operating Systems class, students are able to write interrupt service
routines, system calls, process schedulers, etc. Since the interpreter supports
both user and priviliged modes, students get the experience of writing kernel code without the need for dedicated hardware (or a dedicated VM).

This software is furnished under the terms of the MIT license. See LICENSE.txt 
for details.
