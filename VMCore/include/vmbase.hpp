#ifndef _VM_BASE_HPP
#define _VM_HPP

class VMBase {
    protected:
        // Opcodes functions
        virtual void funcNop() = 0;
        virtual void funcEE() = 0;
        virtual void funcMov() = 0;
        virtual void funcMovmb() = 0;
        virtual void funcMovmw() = 0;
        virtual void funcMovb() = 0;
        virtual void funcMovw() = 0;
        virtual void funcMovbm() = 0;
        virtual void funcMovwm() = 0;
        virtual void funcMovmrb() = 0;
        virtual void funcMovmrw() = 0;
        virtual void funcMovmd() = 0;
        virtual void funcMovd() = 0;
        virtual void funcMovdm() = 0;
        virtual void funcMovmrd() = 0;
        virtual void funcJmp() = 0;
        virtual void funcJz() = 0;
        virtual void funcJnz() = 0;
        virtual void funcJae() = 0;
        virtual void funcJbe() = 0;
        virtual void funcJb() = 0;
        virtual void funcJa() = 0;
        virtual void funcAdvr() = 0;
        virtual void funcAdrr() = 0;
        virtual void funcAdrrl() = 0;
        virtual void funcSubvr() = 0;
        virtual void funcSubrr() = 0;
        virtual void funcSubrrl() = 0;
        virtual void funcXor() = 0;
        virtual void funcXorl() = 0;
        virtual void funcNot() = 0;
        virtual void funcNotb() = 0;
        virtual void funcAdvrd() = 0;
        virtual void funcSubvrd() = 0;
        virtual void funcShr() = 0;
        virtual void funcShl() = 0;
        virtual void funcCmp() = 0;
        virtual void funcCmpl() = 0;
        virtual void funcVmSysbus() = 0;
        virtual void funcPush() = 0;
        virtual void funcPop() = 0;
        virtual void funcClSt() = 0;
        virtual void funcSetSp() = 0;
};

#endif //_VM_BASE_HPP