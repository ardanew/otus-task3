add_executable(allocator main.cpp pool_allocator.h forward_list.h)

include(GNUInstallDirs)
install(
    TARGETS allocator COMPONENT app
    PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
