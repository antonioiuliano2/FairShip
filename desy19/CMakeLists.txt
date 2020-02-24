# Create a library called "libnutaudet” which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${BASE_INCLUDE_DIRECTORIES}
${CMAKE_SOURCE_DIR}/shipdata
${CMAKE_SOURCE_DIR}/charmdet
${CMAKE_SOURCE_DIR}/desy19
${CMAKE_SOURCE_DIR}/online
${CMAKE_SOURCE_DIR}/genfit/core/include
${CMAKE_SOURCE_DIR}/genfit/trackReps/include
${CMAKE_SOURCE_DIR}/millepede
${ROOT_INCLUDE_DIR}
)

include_directories( ${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
${GENFIT_LIBRARY_DIR}

)

link_directories( ${LINK_DIRECTORIES})

set(SRCS
EmuDESYTarget.cxx
EmuDESYPoint.cxx
SciFiDESY.cxx
SciFiDESYPoint.cxx
DESYContFact.cxx
)

Set(HEADERS )
Set(LINKDEF DESYLinkDef.h)
Set(LIBRARY_NAME desy19)
Set(DEPENDENCIES Base ShipData GeoBase ParBase Geom Core genfit millepede)

GENERATE_LIBRARY()
