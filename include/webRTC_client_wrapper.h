/**
 * (C) Copyright Elektrobit Automotive GmbH
 * All rights reserved
 * @ Author: Rajeev Soni
 * @ Create Time: 2022-10-03 18:35:15
 * @ Modified by: Your name
 * @ Modified time: 2023-06-14 16:44:41
 * @ Description:
 */
#ifndef WEBRTC_CLIENT_WRAPPER_H_
#define WEBRTC_CLIENT_WRAPPER_H_

#include <string>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <cassert>
#include <memory>
#include <thread>
#include <mutex>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "rtc_base/third_party/sigslot/sigslot.h"
#include "webRTC_wrapper_client_ callback.h"
#include "rtc_base/strings/json.h"
#include "webRTC_utility.h"
#include "test_code.h"

class webRTC_client_wrapper : public webrtc::PeerConnectionObserver,
                                public webrtc::CreateSessionDescriptionObserver,
                                public webrtc::DataChannelObserver,
                                public sigslot::has_slots<>
{
public:
    //construtor and distructor
    webRTC_client_wrapper(const std::string& ipaddress, uint16_t port);
    ~webRTC_client_wrapper();

    //public APIs
    void registerCallback(const std::shared_ptr<webRTC_wrapper_client_callback>& callback);
    void init();
    void connectToServer(const std::string& ipaddress, uint32_t port);
    void disconnectFromServer();
    ConnectionStatus getConnectionStatus() const;

private:
    //static functions for threading
    // static void *thread_fnc(void* args);
    static void *receiveMessages(void* args);

    //Internal webRTC helper functions
    bool createPeerConnection();
    void createDataChannel();
    void addTracks();
    bool createOffer();
    void deletePeerConnection();
    void send(const char *msg);
    void registerObserver();
    std::string truncateReadMessage(std::string msg);
    void OnMessageFromPeer(std::string message);

    //DataChannelObserver implementations
    void OnStateChange() override{}
    void OnMessage(const webrtc::DataBuffer& buffer) override {}

    //PeerConnectionObserver implementations
    void OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state) override {}
    void OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> channel) override {}
    void OnRenegotiationNeeded() override {}
    void OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state) override {}
    void OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state) override {}
    void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) override {}
    void OnAddStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) override {}
    void OnAddTrack(rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver, const std::vector<rtc::scoped_refptr<webrtc::MediaStreamInterface>>& streams) override {}
    void OnRemoveStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) override {}
    void OnIceConnectionReceivingChange(bool receiving) override {}

    //CreateSessionDescriptionObserver implementations
    void OnSuccess(webrtc::SessionDescriptionInterface* desc) override;
    void OnFailure(webrtc::RTCError error) override;

    void AddRef() const override;
    rtc::RefCountReleaseStatus Release() const override;

    //member variables
    std::string mIpAddress;
    uint16_t mPort;
    std::mutex mtx;
    int32_t mSocket_fd;
    ConnectionStatus mConnectionStatus;
    struct sockaddr_in mServerAddress;
    std::shared_ptr<webRTC_wrapper_client_callback> mCallback;
    rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel;
    rtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection;
};

#endif //WEBRTC_CLIENT_WRAPPER_H_
