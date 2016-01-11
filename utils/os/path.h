#ifndef __os__path__h__
#define __os__path__h__

#include <string>
#include <vector>

namespace os {

/*
 * Mimic the python os.path module.
 * It sucks not having all those nice functions for normal use.
 */
namespace path {

std::string join(const std::vector<std::string>&);
std::string normpath(const std::string&);
std::string basename(const std::string&);
std::string dirname(const std::string&);
std::string expanduser(const std::string&);
std::string realpath(const std::string&);
std::string relpath(const std::string&, const std::string&);

ssize_t getsize(const std::string&);

// TODO: The following three functions are not implemented yet.
//bool isfile(const std::string&);
//bool isdir(const std::string&);
bool exists(const std::string&);


} /* namespace path */

} /* namespace os */

#endif /* end of include guard */
