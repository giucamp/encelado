
#include "common.h"
#include <iostream>

void assert_failed(const char * i_source_file, const char * i_function, int i_line, const char * i_expr)
{
    std::cerr << "assert failed in " << i_source_file << " (" << i_line << ")\n";
    std::cerr << "function: " << i_function << "\n";
    std::cerr << "expression: " << i_expr << std::endl;

#ifdef _MSC_VER
    __debugbreak();
#elif defined(__GNUC__)
    __builtin_trap();
#else
    assert(false);
#endif
}


#if defined(__linux__) && !defined(__ANDROID__)
#    include <execinfo.h>
#    include <signal.h>
#    include <stdlib.h>
#    include <unistd.h>
#endif

#if defined(__linux__) && !defined(__ANDROID__)
// https://stackoverflow.com/questions/77005/how-to-generate-a-stacktrace-when-my-gcc-c-app-crashes
void seg_fault_handler(int sig)
{

    void * array[256];
    size_t size = backtrace(array, 256);

    // print out all the frames to stderr
    fprintf(stderr, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    exit(1);
}
#endif

int main()
{
#if defined(__linux__) && !defined(__ANDROID__)
    signal(SIGSEGV, seg_fault_handler);
#endif

    ediacaran_test::type_list_tests();
    ediacaran_test::class_tests();
    ediacaran_test::type_tests();
    ediacaran_test::qualified_type_ptr_tests();
    ediacaran_test::string_tests();
}