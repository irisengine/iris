/**
 * This is an incomplete file and is intended to be included in
 * include/jobs/context.h
 *
 * *DO NOT* include this file directly
 */

/**
 * x86_64 context, registers that need to be preserved when suspending a
 * function.
 */
struct Context
{
    void *rbx = nullptr;
    void *rbp = nullptr;
    void *rsp = nullptr;
    void *rip = nullptr;
    void *r12 = nullptr;
    void *r13 = nullptr;
    void *r14 = nullptr;
    void *r15 = nullptr;
};
