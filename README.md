# utaipfwd
Small NMEA to TAIP forwarder for routers


https://github.com/kosma/minmea
https://github.com/benhoyt/inih

https://xdevs.com/doc/Trimble/sveight/sv8_app_c.pdf
https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/

https://archive.openwrt.org/releases/19.07.10/targets/ath79/generic/
https://archive.openwrt.org/releases/19.07.10/targets/ath79/generic/openwrt-sdk-19.07.10-ath79-generic_gcc-7.5.0_musl.Linux-x86_64.tar.xz
https://openwrt.org/toh/gl.inet/gl-x750
https://openwrt.org/docs/guide-developer/toolchain/install-buildsystem
https://openwrt.org/docs/guide-developer/toolchain/using_the_sdk


extract SDK to /opt/toolchains/openwrt-mips

```
mkdir build
cd build
cmake ..
cd ..
mkdir build-mips
cd buils-mips
cmake .. --toolchain toolchain.cmake
```