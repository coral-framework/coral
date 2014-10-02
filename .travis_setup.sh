# Travis VMs usually run an outdated version of Ubuntu.
# This script makes Travis compatible with up-to-date dev environments.

# Update GCC and CMake
sudo add-apt-repository -y ppa:kalakris/cmake # for CMake 2.8.11
sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test # for GCC 4.8
sudo apt-get -qq update
sudo apt-get -qq install cmake g++-4.8
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.8 90

# Install Lua and LuaRocks.
# LUA must be set to "Lua 5.1", "Lua 5.2" or "LuaJIT 2.0".

LUA51_VERSION='5.1.5'
LUA52_VERSION='5.2.3'
LUAJIT_VERSION='2.0.3'
LUAROCKS_VERSION='2.2.0'

if [ "$LUA" == "LuaJIT 2.0" ]; then
  curl http://luajit.org/download/LuaJIT-$LUAJIT_VERSION.tar.gz | tar xz
  cd LuaJIT-$LUAJIT_VERSION
  make && sudo make install INSTALL_TSYMNAME=lua;
else
  if [ "$LUA" == "Lua 5.1" ]; then
    curl http://www.lua.org/ftp/lua-$LUA51_VERSION.tar.gz | tar xz
    cd lua-$LUA51_VERSION;
  elif [ "$LUA" == "Lua 5.2" ]; then
    curl http://www.lua.org/ftp/lua-$LUA52_VERSION.tar.gz | tar xz
    cd lua-$LUA52_VERSION;
  fi
  sudo make linux install;
fi

cd ..
curl http://luarocks.org/releases/luarocks-$LUAROCKS_VERSION.tar.gz | tar xz
cd luarocks-$LUAROCKS_VERSION

if [ "$LUA" == "LuaJIT 2.0" ]; then
  ./configure --with-lua-include=/usr/local/include/luajit-2.0;
else
  ./configure;
fi

make && sudo make install
cd ..
