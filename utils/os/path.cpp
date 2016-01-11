#include "path.h"

#include <string>
#include <sstream>

#include <errno.h>
#include <libgen.h>
#include <stdlib.h>
#include <sys/stat.h>

namespace os {

namespace path {

std::string join(const std::vector<std::string> &v)
{
    if (v.empty())
        return std::string();

    std::string out = v[0];
    const size_t num = v.size();
    for(size_t i = 1; i < num; i++) {
        out += "/";
        out += v[i];
    }

    return out;
}

std::string normpath(const std::string &path)
{
    if (path.empty())
        return std::string();

    const std::string first_char = (path[0] == '/')  ?  "/"  :  "";
    std::vector<std::string> out_toks;

    std::stringstream ss(path);
    std::string tok;
    while( std::getline(ss, tok, '/') ) {
        if (tok.empty()) {
            continue;
        } else if (tok == ".") {
            continue;
        } else if (tok == "..") {
            if (out_toks.empty())
                goto exit_failure;
            out_toks.pop_back();
        } else {
            out_toks.push_back(tok);
        }
    }

    return first_char + ::os::path::join(out_toks);

exit_failure:
    fprintf(stderr,"os::path::normpath: Cannot\n");
    return path;

}

std::string basename(const std::string &path)
{
    char *buf = strdup(path.c_str());
    std::string out = ::basename(buf);
    free(buf);
    return out;
}

std::string dirname(const std::string &path)
{
    char *buf = strdup(path.c_str());
    std::string out = ::dirname(buf);
    free(buf);
    return out;
}

std::string realpath(const std::string &path)
{
    char *buf = ::realpath(path.c_str(), NULL);
    if (!buf)
        return std::string();

    const std::string out = buf;
    free(buf);
    return out;
}

/*
 * Caveat:
 *      The python os.path.expanduser(path) documentation says:
 *              On Unix and Windows, return the argument with an initial
 *              component of ~ or ~user replaced by that user‘s home directory.
 *
 *              On Unix, an initial ~ is replaced by the environment variable
 *              HOME if it is set; otherwise the current user’s home directory
 *              is looked up in the password directory through the built-in
 *              module pwd. An initial ~user is looked up directly in the
 *              password directory.
 *
 *              On Windows, HOME and USERPROFILE will be used if set, otherwise
 *              a combination of HOMEPATH and HOMEDRIVE will be used. An initial
 *              ~user is handled by stripping the last directory component from
 *              the created user path derived above.
 *
 *              If the expansion fails or if the path does not begin with a
 *              tilde, the path is returned unchanged.
 *
 *      This implementation only replaces leading ~ with HOME on UNIX/Linux.
 */
std::string expanduser(const std::string &path)
{
    if (path.empty() || path[0] != '~')
        return std::string(path);

    char *buf = getenv("HOME");
    if (!buf)
        return std::string(path);

    std::string out = buf;
    out += std::string(path.data() + 1, path.size() - 1);
    return out;
}

std::string relpath(const std::string&, const std::string&);

ssize_t getsize(const std::string &path)
{
    struct stat st;
    if (::stat(path.c_str(), &st) == 0)
        return st.st_size;
    fprintf(stderr, "Cannot determine size of %s: %s\n",
            path.c_str(), ::strerror(errno));
    return -1;
}

bool isfile(const std::string&);
bool isdir(const std::string&);

bool exists(const std::string &path)
{
    struct stat st;
    return ::stat(path.c_str(), &st) == 0;
}


} /* namespace path */

} /* namespace os */

