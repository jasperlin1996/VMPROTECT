#include "vmtest.hpp"

int main()
{
    std::cout << R"(
 _   ____  _______________ _____ _____ _____ _____ _____ 
| | | |  \/  || ___ \ ___ \  _  |_   _|  ___/  __ \_   _|
| | | | .  . || |_/ / |_/ / | | | | | | |__ | /  \/ | |  
| | | | |\/| ||  __/|    /| | | | | | |  __|| |     | |  
\ \_/ / |  | || |   | |\ \\ \_/ / | | | |___| \__/\ | |  
 \___/\_|  |_/\_|   \_| \_|\___/  \_/ \____/ \____/ \_/  
                                                         
                                                         
 _____ _____ _____ _____ _____                           
|_   _|  ___/  ___|_   _/  ___|                          
  | | | |__ \ `--.  | | \ `--.                           
  | | |  __| `--. \ | |  `--. \                          
  | | | |___/\__/ / | | /\__/ /                          
  \_/ \____/\____/  \_/ \____/                           
                                                         
    )" << std::endl;
    std::cout << "version 0.4.051021.1436\n" << std::endl;

    if(VMCPU::bIsOnTest) {
        VMTest *vmt = new VMTest();

        std::cout << "[INFO] TESTS VMCPU STARTED" << std::endl;
        if(vmt->testVM()) std::cout << "\t[INFO] ALL TESTS PASS" << std::endl;
        else std::cout << "\t[ERROR] SOME TESTS FAILED!" << std::endl;

        delete vmt;
    }
    else{
        std::cout << "\t[ERROR] CANNOT START VMCPU'S TESTS!" << std::endl;
    }
    return 0;
}