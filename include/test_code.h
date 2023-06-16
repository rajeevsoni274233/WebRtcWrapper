/**
 * (C) Copyright Elektrobit Automotive GmbH
 * All rights reserved
 * @ Author: Rajeev Soni
 * @ Create Time: 2022-10-03 18:35:15
 * @ Modified by: Your name
 * @ Modified time: 2023-06-14 12:33:20
 * @ Description:
 */
#ifndef TEST_CODE_H_
#define TEST_CODE_H_

#include <string>

constexpr char IP_ADDRESS[] = "127.0.0.1";
#define PORT 8888

namespace webrtc {

using DataBuffer = std::string;

class PeerConnectionObserver{
public:

};

class CreateSessionDescriptionObserver{
public:

};

class DataChannelObserver{
public:
    virtual void OnStateChange() = 0;
    virtual void onMessage() = 0;
};

}
#endif // TEST_CODE_H_