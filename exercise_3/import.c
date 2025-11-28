#include <stdint.h>

static __attribute((noinline)) uint64_t resolve_import(const char* module, const char* function)
{
    /* Convention: a0 = module ptr, a1 = function name ptr, a7 = syscall code (10105)
     * result returned in a0. Use +r for a0 to read the returned handle. */
    register uint64_t a0 asm("a0") = (uint64_t)module;
    register uint64_t a1 asm("a1") = (uint64_t)function;
    register uint64_t a2 asm("a2") = 0;
    register uint64_t a7 asm("a7") = 10105;
    asm volatile("ecall" : "+r"(a0) : "r"(a1), "r"(a2), "r"(a7) : "memory");
    return a0;
}

static __attribute((noinline)) uint64_t host_call(uint64_t fn, uint64_t args[13])
{
    /* host_call convention (see riscvm):
     * a0 = function pointer/address to call
     * a1 = pointer to args array (13 uint64_t values)
     * a7 = syscall code (20000)
     * returns value in a0
     */
    register uint64_t a0 asm("a0") = fn;
    register uint64_t a1 asm("a1") = (uint64_t)args;
    register uint64_t a7 asm("a7") = 20000;
    asm volatile("ecall" : "+r"(a0) : "r"(a1), "r"(a7) : "memory");
    return a0;
}

static __attribute((noinline)) uint64_t exit(int exit_code)
{
    register uint64_t a0 asm("a0") = exit_code;
    register uint64_t a1 asm("a1") = 0; // unused
    register uint64_t a7 asm("a7") = 10000;
    asm volatile("ecall" : "+r"(a0) : "r"(a1), "r"(a7) : "memory");
    return a0;
}

void _start() __attribute__((section(".text.start")));

void _start()
{
    /* Resolve the host's `puts` and call it to print a message. */
    const char message[] = "Hello from RISC-V!";
    /* module = NULL (0) to search global symbols, function = "puts" */
    uint64_t puts_fn = resolve_import(0, "puts");

    uint64_t args[13] = {0};
    args[0]           = (uint64_t)message;
    host_call(puts_fn, args);

    exit(0);
    asm volatile("ebreak");
}
