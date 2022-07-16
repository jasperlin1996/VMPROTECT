#ifndef _SCREEN_VM_HPP
#define _SCREEN_VM_HPP

#include <iostream>

enum class SCREEN_MODE { NORMAL = 0, HEX };

class Screen {
    public:
        template<typename DTVM>
        static void vmPrint(DTVM d, SCREEN_MODE sm, bool newLine) { 
            switch(sm) {
                case SCREEN_MODE::NORMAL:
                    std::cout << d;
                    break;
                case SCREEN_MODE::HEX:
                    std::cout << std::hex << d;
                    break;
            }
            if(newLine) std::cout << "\n";
        }
};

#endif //_SCREEN_VM_HPP