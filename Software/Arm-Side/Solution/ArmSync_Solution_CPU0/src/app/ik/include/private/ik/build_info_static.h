#ifndef IK_VTABLE_BUILD_INFO_STATIC_H
#define IK_VTABLE_BUILD_INFO_STATIC_H
#include "ik/build_info.h"
C_BEGIN
IK_PRIVATE_API const char* ik_build_info_static_version(void);
IK_PRIVATE_API int ik_build_info_static_build_number(void);
IK_PRIVATE_API const char* ik_build_info_static_host(void);
IK_PRIVATE_API const char* ik_build_info_static_date(void);
IK_PRIVATE_API const char* ik_build_info_static_commit(void);
IK_PRIVATE_API const char* ik_build_info_static_compiler(void);
IK_PRIVATE_API const char* ik_build_info_static_cmake(void);
IK_PRIVATE_API const char* ik_build_info_static_all(void);
#define IK_BUILD_INFO_STATIC_IMPL \
    ik_build_info_static_version, \
    ik_build_info_static_build_number, \
    ik_build_info_static_host, \
    ik_build_info_static_date, \
    ik_build_info_static_commit, \
    ik_build_info_static_compiler, \
    ik_build_info_static_cmake, \
    ik_build_info_static_all
C_END
#endif
