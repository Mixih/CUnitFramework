/**
 * @file cuf_dep.h
 * @brief CUnitFramework (CUF): Dependency Tracker Interface
 * @details Dependency tracking for the cuf framework, allowing dependency
 * checking for skipping and other operations
 */
#ifndef __CUF_DEP_H__
#define __CUF_DEP_H__

#include <stdbool.h>


/**
 * Struct that holds information about the dependencies of a testcase. Operate
 * on it using the `dependency_*` family of functions
 */
typedef struct {
    bool has_filedeps;    /**< flag indicating if the object has file dependencies*/
    char *filedeps;       /**< `\n` delimited string of file dependencies */
} Dependency;

/**
 * Create a `Dependency` object on the heap and intilize it with default vals.
 *
 * @return pointer to created Dependency object.
 */
Dependency *dependency_create();
/**
 * Register a set of file dependencies to the deps object. If the deps object
 * previous had file dependencies registed. they will be replaced. The set of
 * file dependencies is sepcified as a newline (`\n`) delimited string of valid
 * paths.
 * 
 * @param deps pointer to deps object to modify.
 * @param filedeps newline delimited string of paths to register.
 * @return a pointer to the current deps object to allow function chaining.
 */
Dependency *dependency_reg_filedeps(Dependency *deps, char *filedeps) ;
/**
 * check that the depedencies specified in the deps object are satisfied on the
 * current system.
 *
 * @param deps pointer to deps object to use for check.
 * @return boolean value true if all deps are satisfied, and false if one or
 *         more are not.
 */
bool dependency_check(Dependency *deps);
/**
 * Destoy the Dependency object deps, and free all memory it uses.
 * 
 * @param deps depedency object to destroy.
 */
void dependency_destroy(Dependency *deps);

#endif
