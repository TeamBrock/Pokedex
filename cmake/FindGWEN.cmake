SET(GWEN_INCLUDE_DIR thirdparty/gwen/include)

FIND_LIBRARY(GWEN_LIBRARY_RELEASE GWEN
	HINTS
	thirdparty/gwen/lib
	thirdparty/gwen/build/Release
)

FIND_LIBRARY(GWEN_LIBRARY_DEBUG GWEN
	HINTS
	thirdparty/gwen/lib
	thirdparty/gwen/build/DEBUG
)

IF(CMAKE_BUILD_TYPE EQUAL "DEBUG")
   	SET(GWEN_LIBRARIES ${GWEN_LIBRARY_DEBUG})
ELSE()
	SET(GWEN_LIBRARIES ${GWEN_LIBRARY_RELEASE})
ENDIF()
