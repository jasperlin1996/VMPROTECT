#ifndef _MAIN_HPP
#define _MAIN_HPP

#include <iostream>

typedef uint8_t BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;

#define MAGIC_NUMBER (uint16_t)0x566d
#define MODE_DEBUG "debug"
#define MODE_EXEC "exec"

BYTE* loadProtectedCode(int &, std::string);

#endif //_MAIN_HPP