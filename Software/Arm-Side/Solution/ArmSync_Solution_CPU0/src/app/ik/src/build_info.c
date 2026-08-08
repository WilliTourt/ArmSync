#include "ik/build_info_static.h"
#include "ik/build_info_for_every_compile.h"

const char* ik_build_info_static_version(void) { return "1.1.0." "1"; }
int         ik_build_info_static_build_number(void) { return IK_BUILD_NUMBER; }
const char* ik_build_info_static_host(void) { return "RA8P1"; }
const char* ik_build_info_static_date(void) { return IK_BUILD_TIME; }
const char* ik_build_info_static_commit(void) { return "embedded"; }
const char* ik_build_info_static_compiler(void) { return "GCC"; }
const char* ik_build_info_static_cmake(void) { return "e2studio"; }
const char* ik_build_info_static_all(void) { return "IK 1.1.0 on RA8P1"; }
