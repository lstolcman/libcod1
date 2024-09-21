// Using this header file to be able to call functions before they are defined
static void ban();
static void unban();
std::tuple<bool, int, int, std::string> banInfoForIp(char* ip);