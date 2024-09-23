#include <map>      // make_tuple, get, array
#include <sstream>  // ostringstream
#include <vector>
#include <array>

#include <signal.h>
#include <arpa/inet.h>  // sockaddr_in, inet_pton
#include <execinfo.h>   // backtrace
#include <dlfcn.h>      // dlsym
#include <sys/time.h>   // gettimeofday

// Using the below declarations to be able to call these functions before they are defined in the .cpp
static void ban();
static void unban();
std::tuple<bool, int, int, std::string> banInfoForIp(char* ip);