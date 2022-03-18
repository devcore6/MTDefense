AC_INIT([cucktdserver], [1.0])

CPPFLAGS="$CPPFLAGS -D__SERVER__=1 -I../Src/Include"
LIBS="$LIBS -pthread"

AC_CONFIG_SRCDIR([../Src/enet/callbacks.c])
AC_CONFIG_SRCDIR([../Src/enet/compress.c])
AC_CONFIG_SRCDIR([../Src/enet/host.c])
AC_CONFIG_SRCDIR([../Src/enet/list.c])
AC_CONFIG_SRCDIR([../Src/enet/packet.c])
AC_CONFIG_SRCDIR([../Src/enet/peer.c])
AC_CONFIG_SRCDIR([../Src/enet/protocol.c])
AC_CONFIG_SRCDIR([../Src/enet/unix.c])
AC_CONFIG_SRCDIR([../Src/enet/enet/callbacks.h])
AC_CONFIG_SRCDIR([../Src/enet/enet/enet.h])
AC_CONFIG_SRCDIR([../Src/enet/enet/list.h])
AC_CONFIG_SRCDIR([../Src/enet/enet/protocol.h])
AC_CONFIG_SRCDIR([../Src/enet/enet/time.h])
AC_CONFIG_SRCDIR([../Src/enet/enet/types.h])
AC_CONFIG_SRCDIR([../Src/enet/enet/unix.h])
AC_CONFIG_SRCDIR([../Src/enet/enet/utility.h])
AC_CONFIG_SRCDIR([../Src/Engine/Command.cpp])
AC_CONFIG_SRCDIR([../Src/Engine/Command.hpp])
AC_CONFIG_SRCDIR([../Src/Engine/Engine.hpp])
AC_CONFIG_SRCDIR([../Src/Engine/GL.cpp])
AC_CONFIG_SRCDIR([../Src/Engine/GL.hpp])
AC_CONFIG_SRCDIR([../Src/Engine/Map.cpp])
AC_CONFIG_SRCDIR([../Src/Engine/Map.hpp])
AC_CONFIG_SRCDIR([../Src/Engine/Network.cpp])
AC_CONFIG_SRCDIR([../Src/Engine/Network.hpp])
AC_CONFIG_SRCDIR([../Src/Engine/Server.hpp])
AC_CONFIG_SRCDIR([../Src/Engine/Server-Main.cpp])
AC_CONFIG_SRCDIR([../Src/Engine/Texture.cpp])
AC_CONFIG_SRCDIR([../Src/Engine/Texture.hpp])
AC_CONFIG_SRCDIR([../Src/Engine/Tools.hpp])
AC_CONFIG_SRCDIR([../Src/Game/Game.hpp])
AC_CONFIG_SRCDIR([../Src/Game/Server.cpp])
AC_CONFIG_SRCDIR([../Src/Game/Tower.cpp])
AC_CONFIG_SRCDIR([../Src/Include/enet/callbacks.h])
AC_CONFIG_SRCDIR([../Src/Include/enet/enet.h])
AC_CONFIG_SRCDIR([../Src/Include/enet/list.h])
AC_CONFIG_SRCDIR([../Src/Include/enet/protocol.h])
AC_CONFIG_SRCDIR([../Src/Include/enet/time.h])
AC_CONFIG_SRCDIR([../Src/Include/enet/types.h])
AC_CONFIG_SRCDIR([../Src/Include/enet/unix.h])
AC_CONFIG_SRCDIR([../Src/Include/enet/utility.h])

AC_CONFIG_AUX_DIR([.aux])

AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_PROG_CC
AC_PROG_CXX

echo -n "checking for c++23 compiler... "
echo "int main() { return 0; }" > __test.cpp
if $CXX -std=c++2b __test.cpp -o __test > /dev/null 2>&1; then
	echo "$CXX"
	CPPFLAGS="$CPPFLAGS -std=c++2b"
	rm -f __test.cpp __test
else
	echo "no"
	rm -f __test.cpp
	echo "fatal: c++23 compiler not found"
	exit
fi

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
