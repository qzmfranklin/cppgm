#ifndef __os__os__h__
#define __os__os__h__

#include <string>

namespace os {

/*
 * Create directory with mode 775.
 * Return 0 if successful.
 * Return -1 otherwise, and errno is set to indicate the error.
 */
int mkdir(const std::string&);

} /* namespace os */

#endif /* end of include guard <`1`> */
