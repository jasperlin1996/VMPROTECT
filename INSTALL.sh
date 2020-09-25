#!/bin/bash

cat << "EOF"
 __   ____  __ ___ ___  ___ _____ ___ ___ _____                                                
 \ \ / /  \/  | _ \ _ \/ _ \_   _| __/ __|_   _|                                               
  \ V /| |\/| |  _/   / (_) || | | _| (__  | |                                                 
   \_/ |_|  |_|_| |_|_\\___/ |_| |___\___| |_|                _                            _   
  __| |_____ _____| |___ _ __ _ __  ___ _ _| |_   ___ _ ___ _(_)_ _ ___ _ _  _ __  ___ _ _| |_ 
 / _` / -_) V / -_) / _ \ '_ \ '  \/ -_) ' \  _| / -_) ' \ V / | '_/ _ \ ' \| '  \/ -_) ' \  _|
 \__,_\___|\_/\___|_\___/ .__/_|_|_\___|_||_\__| \___|_||_\_/|_|_| \___/_||_|_|_|_\___|_||_\__|
 (_)_ _  __| |_ __ _| | |_|_ _ _                                                               
 | | ' \(_-<  _/ _` | | / -_) '_|                                                              
 |_|_||_/__/\__\__,_|_|_\___|_|                                                                

EOF
echo "version 0.1.200924.0730"
echo "#####################################"

echo ""
echo -n "Write path to deploy: "
read pathToInstall

vmDirName="/VMPROTECT"
fullPathToDeploy="$pathToInstall$vmDirName"

# #####################################
# ############## STAGE 1 ##############
echo "STAGE 1 - check if require programs exist"
if ! [ -x "$(command -v python3)" ];
then
        echo "  \e[1;31mWARNING: python3 could not be found\e[0m"
        if [ $EUID != 0 ]; then
                sudo "$0" "$@"
        fi
        sudo apt install python3 -y
else
        echo "  python3 installed - \e[96myes\e[0m"
fi

if ! [ -x "$(command -v make)" ];
then
        echo "  \e[1;31mWARNING: make could not be found\e[0m"
        if [ $EUID != 0 ]; then
                sudo "$0" "$@"
        fi
        sudo apt install build-essential -y
else
        echo "  make installed - \e[96myes\e[0m"
fi

if ! [ -x "$(command -v g++)" ];
then
        echo "  \e[1;31mWARNING: g++ could not be found\e[0m"
        if [ $EUID != 0 ]; then
                sudo "$0" "$@"
        fi
        sudo apt install build-essential -y
else
        echo "  g++ installed - \e[96myes\e[0m"
fi

if ! [ -x "$(command -v nasm)" ];
then
        echo "  \e[1;33mWARNING: nasm could not be found\e[0m"
        if [ $EUID != 0 ]; then
                sudo "$0" "$@"
        fi
        sudo apt install nasm -y
else
        echo "  nasm installed - \e[96myes\e[0m"
fi
# #####################################

# #####################################
# ############## STAGE 2 ##############
echo "STAGE 2 - copy files"
mkdir $fullPathToDeploy
cp -v -R ./VMCore $fullPathToDeploy/VMCore
cp -v -R ./Editor $fullPathToDeploy/Editor
cp -v -R ./Debugger $fullPathToDeploy/Debugger
cp -v ./Compiler/vm.inc $fullPathToDeploy/vm.inc
cp -v ./VMPROTECT.py $fullPathToDeploy/VMPROTECT.py
chmod +x $fullPathToDeploy/VMPROTECT.py
# #####################################

# #####################################
# ############## STAGE 3 ##############
echo "STAGE 3 - test environment"
make -C $fullPathToDeploy/VMCore buildtests
make -C $fullPathToDeploy/VMCore runtests
echo "\n\n"
make -C $fullPathToDeploy/VMCore clean
make -C $fullPathToDeploy/VMCore build
# #####################################

echo "\n"
echo "\e[96mFINISHED!\e[0m"