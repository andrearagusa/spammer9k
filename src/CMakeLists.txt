set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__FILENAME__='\"$(subst ${CMAKE_SOURCE_DIR}/,,$(abspath $<))\"'")

include_directories(.)
include_directories(sha1)
include_directories(sqlite3)

# Make build flags compiler specific
if (CMAKE_COMPILER_IS_GNUCC)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        #target_compile_options(spammer9k PRIVATE -fexceptions -Wall -Wextra -pedantic -ggdb -O0)
        set (CMAKE_Cxx_FLAGS "-D_REETRANT -ggdb -fPIC -Wall -Wextra -pedantic -O0")
    elseif( CMAKE_BUILD_TYPE STREQUAL "Release" )
        #target_compile_options(spammer9k PRIVATE -fexceptions -DNDEBUG -fPIC -Wall -Wextra -pedantic -O3)
        set (CMAKE_Cxx_FLAGS "-D_REETRANT -DNDEBUG -fPIC -Wall -Wextra -pedantic -O3")
    endif ()
endif (CMAKE_COMPILER_IS_GNUCC)

set (CMAKE_CXX_STANDARD_LIBRARIES "-lcurl -lboost_system -ldl -lpthread -lcpprest -lssl -lcrypto -lstdc++fs")
#target_link_libraries(spammer9k -lcurl -lboost_system -ldl -lpthread -lcpprest -lssl -lcrypto -lstdc++fs)

add_executable(spammer9k
        sha1/sha1.c sha1/sha1.h
        sqlite3/sqlite3.c sqlite3/sqlite3.h
        CAccount.cpp CAccount.h
        CMail.cpp CMail.h
        CMailDispatcher.cpp CMailDispatcher.h
        CMailHandler.cpp CMailHandler.h
        CProvider.cpp CProvider.h
        CRestListener.cpp CRestListener.h
        RestServer.cpp RestServer.h
        CStatistics.cpp CStatistics.h
        main.cpp
        Singleton.h
        Utils.cpp Utils.h
        CConfig.cpp CConfig.h
        CSigHandler.cpp CSigHandler.h
        CTemplate.cpp CTemplate.h
        CTemplateManager.cpp CTemplateManager.h
        CLog.cpp CLog.h
        CNewsletter.cpp CNewsletter.h)

# The rule to install daemon binary
install (TARGETS spammer9k
        RUNTIME
        DESTINATION bin)