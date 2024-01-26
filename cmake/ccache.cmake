# enable compiler cache
find_program(CCACHE_FOUND ccache)
if (CCACHE_FOUND)
    message(STATUS "ccache enabled")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
else ()
    message(WARNING "ccache not found")
endif (CCACHE_FOUND)