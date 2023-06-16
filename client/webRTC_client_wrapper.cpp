/**
 * (C) Copyright Elektrobit Automotive GmbH
 * All rights reserved
 * @ Author: Rajeev Soni
 * @ Create Time: 2022-10-03 18:35:15
 * @ Modified by: Your name
 * @ Modified time: 2023-06-15 12:36:35
 * @ Description:
 */

#include "include/webRTC_client_wrapper.h"

webRTC_client_wrapper::webRTC_client_wrapper(const std::string& ipaddress, uint16_t port):
    mIpAddress(ipaddress),mPort(port)
{
    std::cout << __LINE__ << ", " << __PRETTY_FUNCTION__ << std::endl;
    mConnectionStatus = ConnectionStatus::UNINITIALISED;
}

webRTC_client_wrapper::~webRTC_client_wrapper()
{
    std::cout << __LINE__ << ", " << __PRETTY_FUNCTION__ << std::endl;
    rtc::CleanupSSL();
}

// static void *webRTC_client_wrapper::thread_fnc(void* args)
// {
//     std::cout << __LINE__ << ", " << __PRETTY_FUNCTION__ << std::endl;
//     long options = (long)args;
//     std::cout << "inside thread_fnc: "  << options<< std::endl;
//     std::cout << "pc: " << pc << std::endl;
//     init(0);
//     std::cout << "main : After init" << std::endl;
//     CreatePeerConnection();
//     std::cout << "main : After CreatePeerConnection" << std::endl;
//     AddTracks();
//     std::cout << "main : After AddTracks" << std::endl;
//     /*
//     CreateDataChannel();
//     std::cout << "main : After CreateDataChannel" << std::endl;
//     */
//     if(options == 1){
//         CreateOffer();
//         std::cout << "main : After CreateOffer" << std::endl;
//     }

//     return 0;
// }

static void *webRTC_client_wrapper::receiveMessages(void* args)
{
    while (true)
    {
        char buffer[BUFFER_SIZE] = {'\0'};
        buffer[0]='\0';
        int bytesReceived = recv(mSocket_fd, buffer, BUFFER_SIZE, 0);
        buffer[bytesReceived]='\0';
        if (bytesReceived < 0) {
            std::cerr << "Error receiving data\n";
            
        }
        if (bytesReceived == 0) {
            std::cout << "Connection closed by server\n";
            break;
            
        }
        //like this it will be called
        //https://stackoverflow.com/questions/6894977/how-to-call-a-non-static-member-function-from-a-static-member-function-without-p
        OnMessageFromPeer(buffer);
        buffer[0]='\0';
        memset( buffer, '\0', BUFFER_SIZE-1 );
    }
}

/**
 *
 *
 * public methods
 *
 *
*/
void webRTC_client_wrapper::registerCallback(
        const std::shared_ptr<webRTC_wrapper_client_callback>& callback)
{
    mCallback = callback;
}

void webRTC_client_wrapper::init()
{
    rtc::InitializeSSL();
}

void webRTC_client_wrapper::connectToServer(const std::string& ipaddress, uint32_t port)
{
    std::cout << __LINE__ << ", " << __PRETTY_FUNCTION__ << std::endl;
     mConnectionStatus = ConnectionStatus::CONNECTING;
    mSocket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (mSocket_fd == -1) {
        std::cout << "connectToServer::Socket creation error" << std::endl;.
        mConnectionStatus = ConnectionStatus::UNINITIALISED;
        exit(EXIT_FAILURE);
    }

    mServerAddress.sin_family = AF_INET;
    mServerAddress.sin_port = htons(port); //  port number
    mServerAddress.sin_addr.s_addr = inet_addr(ipaddress); // server address
    if (connect(mSocket_fd, (struct sockaddr*)&mServerAddress, sizeof(mServerAddress)) == -1) {
       std::cerr << "connectToServer::Error connecting to server\n";
        mConnectionStatus = ConnectionStatus::UNINITIALISED;
        exit(EXIT_FAILURE);
    }

    // Using Google's STUN server.
    webrtc::PeerConnectionInterface::IceServer ice_server;
    ice_server.uri = "stun:stun.l.google.com:19302";

    //configuration.enable_dtls_srtp = true;
    configuration.sdp_semantics = webrtc::SdpSemantics::kUnifiedPlan;
    configuration.servers.push_back(ice_server);
    std::cout << "connectToServer::after push_back 1" << std::endl;

    //configuration.enable_dtls_srtp = true;
    if (!signaling_thread_.get()) {
        signaling_thread_ = rtc::Thread::CreateWithSocketServer();
        signaling_thread_->Start();
    }

    if(signaling_thread_.get()){
        std::cout << "connectToServer::signaling_thread_ is good " << std::endl;
    }

    peer_connection_factory = webrtc::CreatePeerConnectionFactory(
        nullptr /* network_thread */, nullptr /* worker_thread */,
        signaling_thread_.get(), nullptr /* default_adm */,
        webrtc::CreateBuiltinAudioEncoderFactory(),
        webrtc::CreateBuiltinAudioDecoderFactory(),
        std::make_unique<webrtc::VideoEncoderFactoryTemplate<
        webrtc::LibvpxVp8EncoderTemplateAdapter,
        webrtc::LibvpxVp9EncoderTemplateAdapter,
        webrtc::OpenH264EncoderTemplateAdapter,
        webrtc::LibaomAv1EncoderTemplateAdapter>>(),
        std::make_unique<webrtc::VideoDecoderFactoryTemplate<
        webrtc::LibvpxVp8DecoderTemplateAdapter,
        webrtc::LibvpxVp9DecoderTemplateAdapter,
        webrtc::OpenH264DecoderTemplateAdapter,
        webrtc::Dav1dDecoderTemplateAdapter>>(),
        nullptr /* audio_mixer */, nullptr /* audio_processing */
    );

    if (!peer_connection_factory) {
        std::cout << "connectToServer::Error,Failed to initialize PeerConnectionFactory " << std::endl;
        exit(EXIT_FAILURE);
    }
    CreatePeerConnection();
    std::cout << "connectToServer:: After CreatePeerConnection" << std::endl;
    addTracks();
    std::cout << "connectToServer:: After addTracks" << std::endl;
    CreateOffer();
    std::cout << "connectToServer::successfully created peerconnection and connected to server" << std::endl;
    mConnectionStatus = ConnectionStatus::CONNECTED;
}

void webRTC_client_wrapper::disconnectFromServer()
{
    std::cout << __LINE__ << ", " << __PRETTY_FUNCTION__ << std::endl;
    mConnectionStatus = ConnectionStatus::DISCONNECTING;
}

ConnectionStatus webRTC_client_wrapper::getConnectionStatus() const
{
    return mConnectionStatus;
}

/**
 *
 * Internal webRTC helper functions
 *
 *
*/
bool webRTC_client_wrapper::createPeerConnection()
{
    peer_connection = peer_connection_factory->CreatePeerConnection(configuration, nullptr, nullptr, this);
    if(peer_connection == nullptr){
        std::cout << " CreatePeerConnection null"  << std::endl;
        return nullptr;
    }
    return peer_connection;
}

void webRTC_client_wrapper::createDataChannel()
{
    webrtc::DataChannelInit config;
    // Configuring DataChannel.
    config.ordered = true;
    data_channel = peer_connection->CreateDataChannel("data_channel", &config);
    data_channel->RegisterObserver(this);
}

void webRTC_client_wrapper::addTracks()
{
    if (!peer_connection->GetSenders().empty()) {
      return;  // Already added tracks.
    }
  
    rtc::scoped_refptr<webrtc::AudioTrackInterface> audio_track(
        peer_connection_factory->CreateAudioTrack(
            kAudioLabel, peer_connection_factory->CreateAudioSource(
                             cricket::AudioOptions()).get()));
    auto result_or_error = peer_connection->AddTrack(audio_track, {kStreamId});
    if (!result_or_error.ok()) {
      std::cout << "Failed to add audio track to PeerConnection: "
                        << result_or_error.error().message()<< std::endl;
    }
  
    rtc::scoped_refptr<CapturerTrackSource> video_device =
        CapturerTrackSource::Create();
    if (video_device) {
      rtc::scoped_refptr<webrtc::VideoTrackInterface> video_track_(
          peer_connection_factory->CreateVideoTrack(video_device, kVideoLabel));
      
      result_or_error = peer_connection->AddTrack(video_track_, {kStreamId});
      if (!result_or_error.ok()) {
        std::cout << "Failed to add video track to PeerConnection: "
                          << result_or_error.error().message()<< std::endl;
      }
    } else {
      std::cout << "OpenVideoCaptureDevice failed" << std::endl;
    }
}

bool webRTC_client_wrapper::createOffer()
{
    if (peer_connection.get() == nullptr) {
        peer_connection_factory = nullptr;
        std::cout  << ":" << std::this_thread::get_id() << ":"<< "Error on CreatePeerConnection." << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "inside CreateOffer " << std::endl;
    peer_connection->CreateOffer(this, webrtc::PeerConnectionInterface::RTCOfferAnswerOptions());
    std::cout << "inside CreateOffer 1" << std::endl;
    return true;
}

void webRTC_client_wrapper::deletePeerConnection()
{
    peer_connection = nullptr;
    signaling_thread_->Stop();
}

void webRTC_client_wrapper::send(const char *msg)
{
    std::lock_guard<std::mutex> lock(mtx);
    send(socket_fd, msg, strlen(msg), 0);
    sleep(5);
}

void webRTC_client_wrapper::registerObserver()
{
    data_channel->RegisterObserver(this);
}

std::string webRTC_client_wrapper::truncateReadMessage(std::string msg)
{
    std::string msglp;
    if(msg[0] == '\0')
    {
        std::cout<<"webRTC_client_wrapper::truncateReadMessage : empty string " << std::endl;
        return "";
    }
    for(int i = 0; i < msg.length(); i++)
    {
        if(msg[i] == '}')
        {
            for(int j=i+1; j < msg.length(); j++)
            {
                
                msglp += msg[j];
            }
            std::cout<<"webRTC_client_wrapper::truncateReadMessage returns : " << msglp<< std::endl;
            break;
        }
    }
    return msglp;
}

void webRTC_client_wrapper::OnMessageFromPeer(std::string message)
{
    std::cout << __LINE__ << ", " << __PRETTY_FUNCTION__ << std::endl;
    Json::Reader reader;
    Json::Value jmessage;
    while(message.length() != 0)
    {
        if (!reader.parse(message, jmessage)) {
            std::cout << "Received unknown message. " << message << std::endl;
            return;
        }
        std::string type_str;
        std::string json_object;

        rtc::GetStringFromJsonObject(jmessage, kSessionDescriptionTypeName,
                                    &type_str);
        if (!type_str.empty())
        {
            absl::optional<webrtc::SdpType> type_maybe =
                webrtc::SdpTypeFromString(type_str);
            if (!type_maybe) {
                std::cout << "Unknown SDP type: " << type_str;
                return;
            }
            webrtc::SdpType type = *type_maybe;
            std::string sdp;
            if (!rtc::GetStringFromJsonObject(jmessage, kSessionDescriptionSdpName,
                                                &sdp)) {
                std::cout << "Can't parse received session description message." << std::endl;
                return;
            }
            webrtc::SdpParseError error;
            std::unique_ptr<webrtc::SessionDescriptionInterface> session_description =
                webrtc::CreateSessionDescription(type, sdp, &error);
            if (!session_description) {
                std::cout << "Can't parse received session description message. "
                                    "SdpParseError was: "
                                << error.description << std::endl;
                return;
            }
            std::cout << " Received session description :" << sdp << std::endl;
            peer_connection->SetRemoteDescription(
                SetSessionDescriptionObserver::Create(),
                session_description.release());
            if (type == webrtc::SdpType::kOffer) {
                peer_connection->CreateAnswer(
                    this, webrtc::PeerConnectionInterface::RTCOfferAnswerOptions());
            }
        } else 
        {
            std::cout << "Received ice = " << jmessage << std::endl;
            std::string sdp_mid;
            int sdp_mlineindex = 0;
            std::string sdp;
            if (!rtc::GetStringFromJsonObject(jmessage, kCandidateSdpMidName,
                                                &sdp_mid) ||
                !rtc::GetIntFromJsonObject(jmessage, kCandidateSdpMlineIndexName,
                                            &sdp_mlineindex) ||
                !rtc::GetStringFromJsonObject(jmessage, kCandidateSdpName, &sdp)) {
                std::cout << "Can't parse received message."<< std::endl;
                return;
            }
            webrtc::SdpParseError error;
            std::unique_ptr<webrtc::IceCandidateInterface> candidate(
                webrtc::CreateIceCandidate(sdp_mid, sdp_mlineindex, sdp, &error));
            std::cout << "sdp_mid = " << sdp_mid << "sdp_mlineindex = " << sdp_mlineindex << "sdp = " << sdp << std::endl;
            if (!candidate.get()) {
                std::cout << "Can't parse received candidate message. "
                                    "SdpParseError was: "
                                << error.description << std::endl;
                return;
            }
            if (!peer_connection->AddIceCandidate(candidate.get())) {
                std::cout << "Failed to apply the received candidate" << std::endl;
            }
            std::cout << " inside OnMessageFromPeer Received candidate :" << message << std::endl;
        }
        message = TruncateReadMessage(message);
        std::cout << "Truncated message : " << message << std::endl;
    }
}

/*
 *
 * DataChannelObserver implementations
 *
**/
void webRTC_client_wrapper::OnStateChange()
{
    std::cout << __LINE__ << ", " << __PRETTY_FUNCTION__ << std::endl;
    assert(data_channel);
    std::cout << "DataChannel OnStateChange() state = " << data_channel->state() << std::endl;
    // based on the datachannel state we will change our internal state
    // mConnectionStatus = static_cast<int32_t>(data_channel->state());
}

void webRTC_client_wrapper::OnMessage(const webrtc::DataBuffer& buffer)
{
    std::cout << __LINE__ << ", " << __PRETTY_FUNCTION__ << std::endl;
    //callback to send the video data to client binary
    // mCallback->receiveData(buffer);
    std::cout << "[info] message received :" << std::string(buffer.data.data<char>(), buffer.data.size()) << std::endl;
}

/*
 *
 * PeerConnectionObserver implementations
 *
**/
void webRTC_client_wrapper::OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state)
{
    std::cout << __LINE__ << ", " << __PRETTY_FUNCTION__ << std::endl;
}

void webRTC_client_wrapper::OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> channel)
{
    std::cout << __LINE__ << ", " << __PRETTY_FUNCTION__ << std::endl;
    data_channel = channel;
    data_channel->RegisterObserver(this);
    std::cout << "OnDataChannel end" << std::endl;
}

void webRTC_client_wrapper::OnRenegotiationNeeded()
{
    std::cout << __LINE__ << ", " << __PRETTY_FUNCTION__ << std::endl;
}

void webRTC_client_wrapper::OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state)
{
    std::cout << __LINE__ << ", " << __PRETTY_FUNCTION__ << std::endl;
    if(new_state == webrtc::PeerConnectionInterface::kIceConnectionCompleted)
    {
        std::cout << "OnIceConnectionChange =  kIceConnectionCompleted";
        //data_channel->RegisterObserver(this);
        // std::cout << "OnIceConnectionChange datachannel registered";

    }
}

void webRTC_client_wrapper::OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state)
{
    std::cout << __LINE__ << ", " << __PRETTY_FUNCTION__ << std::endl;
}

void webRTC_client_wrapper::OnIceCandidate(const webrtc::IceCandidateInterface* candidate)
{
    std::cout << __LINE__ << ", " << __PRETTY_FUNCTION__ << std::endl;
    std::cout << "OnIceCandidate : " <<  candidate->sdp_mline_index() << std::endl;

    Json::StyledWriter writer;
    Json::Value jmessage;

    jmessage[kCandidateSdpMidName] = candidate->sdp_mid();
    jmessage[kCandidateSdpMlineIndexName] = candidate->sdp_mline_index();
    std::string sdp;
    if (!candidate->ToString(&sdp)) {
      std::cout << "Failed to serialize candidate" << std::endl;
      return;
    }
    jmessage[kCandidateSdpName] = sdp;
    std::string jsonstring = writer.write(jmessage);
    const char *iceCandidate = jsonstring.c_str();
    Send(iceCandidate);
}

void webRTC_client_wrapper::OnAddStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream)
{
    std::cout << __LINE__ << ", " << __PRETTY_FUNCTION__ << std::endl;
}

void webRTC_client_wrapper::OnAddTrack(rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver,
        const std::vector<rtc::scoped_refptr<webrtc::MediaStreamInterface>>& streams)
{
    std::cout << __LINE__ << ", " << __PRETTY_FUNCTION__ << std::endl;
    std::cout << "TRACK RECEIVED"  << std::endl;
    if (receiver->track()->kind() == webrtc::MediaStreamTrackInterface::kVideoKind) {
        // Cast the track to a video track
        rtc::scoped_refptr<webrtc::VideoTrackInterface> video_track(
        static_cast<webrtc::VideoTrackInterface*>(receiver->track().get()));
    }
}

void webRTC_client_wrapper::OnRemoveStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream)
{
    std::cout << __LINE__ << ", " << __PRETTY_FUNCTION__ << std::endl;
}

void webRTC_client_wrapper::OnIceConnectionReceivingChange(bool receiving)
{
    std::cout << __LINE__ << ", " << __PRETTY_FUNCTION__ << std::endl;
}

/**
 * 
 * CreateSessionDescriptionObserver implementations
 * 
*/
void webRTC_client_wrapper::OnSuccess(webrtc::SessionDescriptionInterface *desc)
{
    std::cout << __LINE__ << ", " << __PRETTY_FUNCTION__ << std::endl;
    if(desc){
        peer_connection->SetLocalDescription(SetSessionDescriptionObserver::Create(), desc);
        std::cout << "Local desc set" << std::endl;
        std::string sdp,rsdp;
        desc->ToString(&sdp);
        Json::StyledWriter writer;
        Json::Value jmessage;
        jmessage[kSessionDescriptionTypeName] = webrtc::SdpTypeToString(desc->GetType());
        jmessage[kSessionDescriptionSdpName] = sdp;
        std::string jsonstring = writer.write(jmessage);
        const char* offer = jsonstring.c_str();
        std::cout << "offer :" << offer <<std::endl;
        Send(offer);
        //send(mSocket_fd, offer, strlen(offer), 0);
    }
}

void webRTC_client_wrapper::OnFailure(webrtc::RTCError error)
{
    std::cout << __LINE__ << ", " << __PRETTY_FUNCTION__ << std::endl;
    std::cout<<"[error] err:"<<error.message()<<std::endl;
    assert(false);
}

void webRTC_client_wrapper::AddRef() const override
{
    std::cout << __LINE__ << ", " << __PRETTY_FUNCTION__ << std::endl;
}

rtc::RefCountReleaseStatus webRTC_client_wrapper::Release() const override
{
    std::cout << __LINE__ << ", " << __PRETTY_FUNCTION__ << std::endl;
}