file(GLOB UTIL_FILES "./src/utils/*.c")

idf_component_register(SRCS "./src/Firmware.c" ${UTIL_FILES}
                    INCLUDE_DIRS "./include/")