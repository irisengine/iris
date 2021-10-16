////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

.intel_syntax noprefix

.section __TEXT,__text

/**
 * Change stack, performs all necessary patching such that the function can
 * return to the caller.
 *
 * @param rdi
 *   Pointer to new stack.
 */
.globl _change_stack
_change_stack:

push rax # stack alignment

mov r9, rbp
mov r8, [rbp] # get base-pointer for previous frame
sub r8, rbp # get offset between this and previous frame

mov rdx, rbp
sub rdx, rsp # get size of current frame
mov rax, rdi # preserve new stack address

# copy old stack to new stack
cld
mov rsi, rsp
mov rcx, 1024
rep movsb

mov rsp, rax # switch to new stack
mov rbp, rax # copy stack to base pointer
add rbp, rdx # restore stack frame by offseting base pointer the same amout as
             # when we entered the function

# patch base pointer for previous frame to point back into our new stack, this
# allows us one function return after this function
mov rax, r8
add r8, rbp
mov [rbp], r8

pop rax # stack alignment

ret

/**
 * Save current context.
 *
 * @param context
 *   Pointer to struct to save registers.
 */
.globl _save_context
_save_context:
mov [rdi], rbx
mov [rdi + 8], rbp
mov [rdi + 16], rsp
lea rax, [rip+end] # store end of this function in rip
mov [rdi + 24], rax
mov [rdi + 32], r12
mov [rdi + 40], r13
mov [rdi + 48], r14
mov [rdi + 56], r15
end:
ret

/**
 * Restore context.
 *
 * @param context
 *   Pointer to struct with registers to restore.
 */
.globl _restore_context
_restore_context:
mov rbx, [rdi]
mov rbp, [rdi + 8]
mov rsp, [rdi + 16]
mov r12, [rdi + 32]
mov r13, [rdi + 40]
mov r14, [rdi + 48]
mov r15, [rdi + 56]
mov rax, [rdi + 24]
jmp rax


