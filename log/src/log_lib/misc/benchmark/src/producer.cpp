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
/// @file       producer.cpp
/// @brief
/// @details
/// @date       2025-09-17
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <chrono>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <thread>

// TLV protocol structure definition (fixed length 1024 bytes)
struct TLVMessage
{
    uint32_t type;     // 4-byte type
    uint32_t length;   // 4-byte length
    char value[1016];  // 1016-byte data
};

// Ensure the TLV structure size is 1024 bytes
static_assert(sizeof(TLVMessage) == 1024, "TLVMessage must be 1024 bytes");

const char *SERVER_IP = "127.0.0.1";
const int PORT        = 8888;

int main(std::int32_t argc, char *argv[])
{
    if (argc < 2) {
        std::cerr << "Parameters required" << std::endl;
        return -1;
    }
    int sockfd;
    struct sockaddr_in servaddr;
    socklen_t len;
    TLVMessage buffer;

    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Initialize server address structure
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port   = htons(PORT);

    // Convert IP address
    if (inet_pton(AF_INET, SERVER_IP, &servaddr.sin_addr) <= 0) {
        perror("invalid address/address not supported");
        exit(EXIT_FAILURE);
    }

    len = sizeof(servaddr);
    std::int64_t msgCount{atoi(argv[1])};
    // Calculate the interval between each print per second (microseconds)
    auto intervalUs       = 1000000 / msgCount;  // 1 second = 1000000 microseconds
    std::int64_t lastTime = msgCount * 10;
    // Send several test messages
    for (int i = 0; i < lastTime; ++i) {
        // Prepare the message
        buffer.type         = htonl(0x00000001);  // Data type
        std::string message = "Hello from producer! Message " + std::to_string(i + 1);
        buffer.length       = htonl(message.size());
        strncpy(buffer.value, message.c_str(), sizeof(buffer.value) - 1);
        buffer.value[sizeof(buffer.value) - 1] = '\0';  // Ensure the string is terminated

        // Send the message
        sendto(sockfd, &buffer, sizeof(TLVMessage), MSG_CONFIRM, (const struct sockaddr *)&servaddr, len);
        std::cout << "Sent message: " << message << std::endl;

        std::this_thread::sleep_for(std::chrono::microseconds(intervalUs - 3));
    }

    close(sockfd);
    return 0;
}
