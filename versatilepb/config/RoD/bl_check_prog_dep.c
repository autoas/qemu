#include "Std_Types.h"
#include "Std_Debug.h"

Std_ReturnType BL_CheckProgramDependency(uint8_t major, uint8_t minor, uint8_t patch) {
  ASLOG(INFO,
        ("BL_CheckProgramDependency: major = %d, minor = %d, patch = %d\n", major, minor, patch));
  return E_OK;
}
