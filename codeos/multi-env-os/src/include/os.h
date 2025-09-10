#ifndef OS_H
#define OS_H

#include <stddef.h>

// System initialization and management
int initialize_system(void);  // Returns 0 on success, non-zero on failure
void start_kernel(void);      // Returns 0 on success, non-zero on failure
void shutdown_system(void);   // Returns 0 on success, non-zero on failure

// Environment setup functions
void setup_python_env(void);  // Returns 0 on success, non-zero on failure
void setup_cpp_env(void);     // Returns 0 on success, non-zero on failure
void setup_nodejs_env(void);  // Returns 0 on success, non-zero on failure

// Application launch functions
int launch_browser(void);    // Returns process ID on success, -1 on failure
void launch_editor(void);     // Returns process ID on success, -1 on failure
void launch_terminal(void);   // Returns process ID on success, -1 on failure
void launch_python_env(void); // Returns process ID on success, -1 on failure
void launch_cpp_env(void); // Returns process ID on success, -1 on failure
void launch_nodejs_env(void); // Returns process ID on success, -1 on failure

// System status functions
int get_system_status(void); // Returns 0 if system is healthy, non-zero otherwise

// Process management structures and constants
#define MAX_PROCESS_NAME 256
#define MAX_PROCESSES 1024
#define PROCESS_PRIORITY_LOW 0
#define PROCESS_PRIORITY_NORMAL 1
#define PROCESS_PRIORITY_HIGH 2

typedef struct {
    int pid;                      // Process ID
    char name[MAX_PROCESS_NAME];  // Process name
    int priority;                 // Process priority
    int status;                   // Process status (running, suspended, terminated)
    unsigned long memory_usage;   // Memory usage in bytes
    unsigned long cpu_time;       // CPU time used
} Process;

// Memory management structures
typedef struct {
    unsigned long total_memory;   // Total memory available in bytes
    unsigned long used_memory;    // Memory currently in use in bytes
    unsigned long peak_memory;    // Peak memory usage in bytes
    int fragmentation;           // Memory fragmentation percentage
} MemoryInfo;

// Memory management functions
void* allocate_memory(size_t size);           // Returns NULL on failure
int free_memory(void* ptr);                   // Returns 0 on success
int get_total_memory(void);                   // Returns total memory in bytes
int get_used_memory(void);                    // Returns used memory in bytes
int init_memory(void);                        // Returns 0 on success

// Process management functions
int create_process(const char* name, int priority);  // Returns process ID or -1 on failure
int terminate_process(int pid);                      // Returns 0 on success
int suspend_process(int pid);                        // Returns 0 on success
int resume_process(int pid);                         // Returns 0 on success
int get_process_count(void);                         // Returns number of active processes
int init_process_manager(void);                      // Returns 0 on success

// Error handling
int get_last_error(void);                           // Returns last error code
const char* get_error_message(int error_code);      // Returns error message string

#endif // OS_H