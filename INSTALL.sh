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
echo "version 0.1.211007.1837"
echo "#####################################"

echo ""
echo -n "Path to deploy VMPROTECT: "
read pathToInstall

vmDirName="/VMPROTECT"
fullPathToDeploy="$pathToInstall$vmDirName"

# #####################################
# ############## STAGE 1 ##############
echo "*****************************"
echo "STAGE 1 - check if required programs are installed"
echo "*****************************"
if ! [ -x "$(command -v python3)" ];
then
        echo -e "  \e[1;33mWARNING: python3 could not be found\e[0m"
        if [ $EUID != 0 ]; then
                sudo "$0" "$@"
        fi
        sudo apt install python3 -y
else
        echo -e "  python3 is installed - \e[96myes\e[0m"
fi

if python3 -c 'import pkgutil; exit(not pkgutil.find_loader("tkinter"))'; then
        echo -e '  tkinter is installed - \e[96myes\e[0m'
else
        echo -e '  \e[1;33mWARNING: tkinter could not be found\e[0m'
        if [ $EUID != 0 ]; then
                sudo "$0" "$@"
        fi 
        sudo apt-get install python3-tk -y
fi

if ! [ -x "$(command -v make)" ];
then
        echo -e "  \e[1;33mWARNING: make could not be found\e[0m"
        if [ $EUID != 0 ]; then
                sudo "$0" "$@"
        fi
        sudo apt install build-essential -y
else
        echo -e "  make is installed - \e[96myes\e[0m"
fi

if ! [ -x "$(command -v g++)" ];
then
        echo -e "  \e[1;33mWARNING: g++ could not be found\e[0m"
        if [ $EUID != 0 ]; then
                sudo "$0" "$@"
        fi
        sudo apt install build-essential -y
else
        echo -e "  g++ is installed - \e[96myes\e[0m"
fi

if ! [ -x "$(command -v nasm)" ];
then
        echo -e "  \e[1;33mWARNING: nasm could not be found\e[0m"
        if [ $EUID != 0 ]; then
                sudo "$0" "$@"
        fi
        sudo apt install nasm -y
else
        echo -e "  nasm is installed - \e[96myes\e[0m"
fi
# #####################################

# #####################################
# ############## STAGE 2 ##############
echo "*****************************"
echo "STAGE 2 - copy files"
echo "*****************************"
mkdir $fullPathToDeploy
cp -v -R ./VMCore $fullPathToDeploy/VMCore
cp -v -R ./Editor $fullPathToDeploy/Editor
cp -v -R ./Debugger $fullPathToDeploy/Debugger
cp -v -R ./SharedCode $fullPathToDeploy/SharedCode
cp -v ./Compiler/vm.inc $fullPathToDeploy/vm.inc
cp -v ./VMPROTECT.py $fullPathToDeploy/VMPROTECT.py
chmod +x $fullPathToDeploy/VMPROTECT.py
# #####################################

# #####################################
# ############## STAGE 3 ##############
echo "*****************************"
echo "STAGE 3 - test the environment"
echo "*****************************"
make -C $fullPathToDeploy/VMCore buildtests
make -C $fullPathToDeploy/VMCore runtests
printf "\n\n"
make -C $fullPathToDeploy/VMCore clean
make -C $fullPathToDeploy/VMCore build
# #####################################

printf "\n"
echo -e "\e[96mFINISHED!\e[0m"
