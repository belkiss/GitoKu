SET(GITOKU_SOURCE
    CFileStatusFilterModel.cpp
    CGitRepo.cpp
    CGitokuWindow.cpp
    gitoku.cpp
)

SET(GITOKU_MOC
    CGitokuWindow.h
    CFileStatusFilterModel.h
)

SET(GITOKU_UI
    ../data/gitoku.ui
)

SET (QT_WRAP_CPP "YES")
SET (QT_WRAP_UI "YES")

QT4_WRAP_UI(GITOKU_UI_H  ${GITOKU_UI})
QT4_WRAP_CPP(GITOKU_MOC_H ${GITOKU_MOC})


INCLUDE(${QT_USE_FILE} ${LIBGIT2_INCLUDE_DIRS})

ADD_DEFINITIONS (${QT_DEFINITIONS})
ADD_DEFINITIONS (-Wall -Wextra)
ADD_DEFINITIONS (-std=c++0x)
ADD_DEFINITIONS (-g)

INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )

ADD_EXECUTABLE(gitoku ${GITOKU_SOURCE} ${GITOKU_MOC_H} ${GITOKU_UI_H})

TARGET_LINK_LIBRARIES(gitoku ${QT_LIBRARIES} ${LIBGIT2_LIBRARIES})

