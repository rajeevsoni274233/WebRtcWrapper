# WebRtcWrapper
Wrting a wrapper on top of webrtc lib

# Download and Install Devtools
Open a terminal by pressing Ctrl+Alt+T or searching for "Terminal" in the applications menu.

1. Update the package lists for upgrades and new package installations by running the following command:
    sudo apt update

2. Install the necessary build tools and dependencies required to compile WebRTC by running the following command:
    sudo apt install build-essential

3. Install additional dependencies required by WebRTC by running the following command:
    sudo apt install libssl-dev libasound2-dev libpulse-dev libjpeg62-dev libvpx-dev

4. Clone the WebRTC source code repository using the following command:
    git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git

5. Add the depot_tools directory to your system's PATH environment variable. This can be done by adding the       following line to your ~/.bashrc file:
    export PATH="$PATH:${HOME}/depot_tools"

6. Refresh the terminal to apply the updated PATH by running the following command:
    source ~/.bashrc

# Download WebRtc and follow the Instructions
https://webrtc.github.io/webrtc-org/native-code/development/


# some important links
https://hsf-training.github.io/hsf-training-cmake-webpage/04-targets/index.html
https://medium.com/@abdularis/how-to-compile-native-webrtc-from-source-for-android-d0bac8e4c933


# Build cmds on linux or ubuntu
$ mkdir build
$ cd build
$ cmake ..
$ make
