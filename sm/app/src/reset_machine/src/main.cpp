// Disclaimer
//
// This work (specification and/or software implementation) and the material
// contained in it, as released by AUTOSAR, is for the purpose of information
// only. AUTOSAR and the companies that have contributed to it shall not be
// liable for any use of the work.
//
// The material contained in this work is protected by copyright and other
// types of intellectual property rights. The commercial exploitation of the
// material contained in this work requires a license to such intellectual
// property rights.
//
// This work may be utilized or reproduced without any modification, in any
// form or by any means, for informational purposes only. For any other
// purpose, no part of the work may be utilized or reproduced, in any form
// or by any means, without permission in writing from the publisher.
//
// The work has been developed for automotive applications only. It has
// neither been developed, nor tested for non-automotive applications.
//
// The word AUTOSAR and the AUTOSAR logo are registered trademarks.
// --------------------------------------------------------------------------

/// ================================================================
///
/// File description:
/// ----------------
/// @file       main.cpp
/// @brief      Adaptive Platform Test Applications
/// @details    a test, which just prints the time
/// @details    Implementation of ResetMachine
/// @date       2024-06-25
/// @author     zhibo.han
/// @version    1.2.0
///
/// ================================================================

#include <fcntl.h>   // COMPATIBLE_WARN: Pipe required for simulating machine restart in test environment
#include <unistd.h>  // COMPATIBLE_WARN: Pipe required for simulating machine restart in test environment

#include <cstdio>
#include <cstring>
#include <iostream>
#include <thread>
#include <vector>

#include "function_thread_safe.h"
#include "isoft/ara_fsh/filesystem_hierarchy.h"

using std::cout;
using std::endl;

/*___________________________________________________________________________________________________________
 * main
 */
int main(int argc, char** argv)
{
    // Do not buffer output
    setvbuf(stdout, nullptr, _IONBF, 0);

    (void)argc;
    (void)argv;
    cout << "start...................." << endl;
    int ret = 0;
    // string command = "reboot";
    // ret = system(command.c_str());
    // string command = "shutdown -r +0";
    // system(command.c_str());

    // cout << "before reboot" << endl;
    // ret = execlp("reboot", "reboot", NULL);
    // cout << "after reboot and ret" << ret << endl;

    // // Find Execution Management and send signal SIGINT to it
    // char buf[512];
    // memset(buf, 0, 512);
    // // FILE *cmd_pipe = popen("pidof -s execution-manager", "r");
    // FILE *cmd_pipe = popen("pidof emd", "r");
    // fgets(buf, 512, cmd_pipe);
    // // pid_t pid = strtoul(buf, NULL, 10);
    // pclose(cmd_pipe);

    // vector<pid_t> pids;
    // std::string s = buf;
    // std::string delimiter = " ";

    // size_t pos = 0;
    // std::string token;
    // while ((pos = s.find(delimiter)) != std::string::npos) {
    //     token = s.substr(0, pos);
    //     pid_t pid = strtoul(token.c_str(), NULL, 10);
    //     pids.push_back(pid);
    //     s.erase(0, pos + delimiter.length());
    // }
    // if(!s.empty()) {
    //   pid_t pid = strtoul(s.c_str(), NULL, 10);
    //   pids.push_back(pid);
    // }

    // int pidNum = pids.size();
    // cout<<"main get pidNum:" << pidNum << endl;
    // if(pids.size() != 0) {

    //   for(auto pid:pids) {
    //     cout<<"try to send SIGINT to pid:" << pid << endl;
    //     kill(pid, SIGINT);
    //   }

    //   cout<<"try to call ara-loader...................." << endl;

    //   // ara directory
    //   isoft::ara_fsh::Platform fsh;
    //   std::string araSysRootDir = fsh.GetSysroot().c_str();

    //   cout<<"get araSysRootDir:" << araSysRootDir.c_str();

    //   string araLoaderCommand = araSysRootDir + "/ara/boot/StartAraLoader.sh" + " " + araSysRootDir;
    //   system(araLoaderCommand.c_str());
    // }

    // ara directory
    isoft::ara_fsh::Platform fsh;
    std::string araSysTmpDir = fsh.GetSysTmpDir();

    cout << "get araSysTmpDir:" << araSysTmpDir.c_str() << std::endl;

    // pipe address
    std::string pipePath = araSysTmpDir + "/reset-machine-fifo";
    std::cout << "before open pipePath:" << pipePath << "." << std::endl;

    int pipe = -1;
    while (-1 == pipe) {
        // Open FIFO for write only
        pipe = open(pipePath.c_str(), O_WRONLY | O_NONBLOCK | O_CLOEXEC);
        if (-1 == pipe) {
            std::cout << "Fail to open, errno:" << ara::core::internal::safe_strerror(errno)
                      << ". reset_machine_daemon may not be up. Try again later." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        } else {
            break;
        }
    }

    // Check if writable
    int nfd = pipe + 1;
    fd_set wfd;
    struct timeval tv
    {
    };
    tv.tv_sec  = 0;
    tv.tv_usec = 0;
    FD_ZERO(&wfd);       // NOLINT
    FD_SET(pipe, &wfd);  // NOLINT
    tv.tv_sec  = 1;
    tv.tv_usec = 0;
    int retval = select(nfd, nullptr, &wfd, nullptr, &tv);
    if (-1 == retval) {
        std::cout << "Fail to select, errno:" << ara::core::internal::safe_strerror(errno) << std::endl;
    } else if (0 == retval) {
        std::cout << "Can't write in 1s, State Management may be down." << std::endl;
    } else {
        // Data area to be sent
        const char* temp = "ResetMachine\r\n";
        size_t len       = strlen(temp);
        std::cout << "Try to write temp:" << temp << std::endl;
        size_t tot = 0;
        while (tot < len) {
            ssize_t num = write(pipe, temp + tot, len - tot);
            if (num < 0) {
                std::cout << "Fail to write, errno:" << ara::core::internal::safe_strerror(errno) << std::endl;
                break;
            }
            if (0 == num) {
                std::cout << "read, num is 0" << std::endl;
            } else {
                std::cout << "read, return num:" << num << std::endl;
            }

            tot += static_cast< size_t >(num);
        }
    }

    close(pipe);

    cout << "end...................." << endl;
    return ret;
}
