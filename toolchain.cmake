# the name of the target operating system
set(CMAKE_SYSTEM_NAME Linux)

# which compilers to use for C and C++
set(CMAKE_C_COMPILER   mips-openwrt-linux-musl-gcc)
set(CMAKE_CXX_COMPILER mips-openwrt-linux-musl-g++)

# where is the target environment located
set(CMAKE_FIND_ROOT_PATH  /opt/toolchains/openwrt-ath79/staging_dir/toolchain-mips_24kc_gcc-7.5.0_musl)

# adjust the default behavior of the FIND_XXX() commands:
# search programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH)

# search headers and libraries in the target environment
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)