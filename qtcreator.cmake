#QtCreator adds
FILE(GLOB_RECURSE LibFiles "${CMAKE_CURRENT_SOURCE_DIR}/wxint/*")
add_custom_target(headers SOURCES ${LibFiles})
