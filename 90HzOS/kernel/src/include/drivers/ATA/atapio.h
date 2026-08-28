#ifndef ATA_H
    #include "../../types.h"

    #define ATA_H

    struct __attribute__((packed))ATA_PCI_Controller {
        u8  BUS;
        u8  DEV;
        u8  FUNC;
    };

    struct ATA_BARs {
        u16 BAR0;
        u16 BAR1;
        u16 BAR2;
        u16 BAR3;
    };
    

    char  ATAControllerExists();
    char  GetATAControllerCount();
    u8* GetATA_PCI_Controller();
    struct ATA_BARs Get_ATA_BARs(u8* BUS_ID, u8* DEV_ID, u8* FUNC_ID, u8 CtrllerID);
    unsigned char SelectDrive(u16 Control_ports, u16 alternate_status, u8 val, u8 CtrllerID);
    char IdentifyATADrive(struct ATA_BARs ATARegs, u8 DriveIdx, char* Output);
    
    char* read_sector();


#endif