#include "inc/MainWindow.h"
#include "inc/webRTC_client_wrapper.h"
#include "inc/webRTC_wrapper_client_ callback.h"
#include "inc/webRTC_utility.h"

int main(){
    webRTC_client_wrapper *pc = new webRTC_client_wrapper("127.0.0.1",8888);
    pc->init();
    std::thread client(&pc->connectToServer());
    client.join();

    //code will not reach here
    return 0;
}