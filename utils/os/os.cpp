#include "os.h"

#include <sys/stat.h>
#include <sys/types.h>

namespace os {

int mkdir(const std::string &path)
{
    return ::mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

} /* namespace os */
