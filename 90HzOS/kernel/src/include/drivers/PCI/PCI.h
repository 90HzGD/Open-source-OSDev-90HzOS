#include "../../types.h"

#ifndef PCI_H
    #define PCI_H

    #define PCI_COMMAND     0xCF8
    #define PCI_DATA        0xCFC
    #define BUS_COUNT       256
    #define DEV_COUNT       32
    #define FUNC_COUNT      8

    struct PCIDev_Descriptor {
        u16 VENDOR_ID;
        u16 DEVICE_ID;
        u8  REVISION;
        u8  INTERFACE;
        u8  SUBCLASS;
        u8  CLASS;
        u8  INTERRUPT;
    };

    struct PCIVendors {
        char* Vendors_str[15];
        u16 VendorsID[15];
    };

    u32 read_PCI(u8 BUS_ID, u8 DEV_ID, u8 FUNC_ID, u8 REG_OFFSET);
    struct PCIDev_Descriptor GetDevInfo(u8 BUS_ID, u8 DEV_ID, u8 FUNC_ID);
    void fillVendors(char** Vendors_str, u16* VendorsID);
    u8 CheckMultifun(u8 BUS_ID, u8 DEV_ID);

#endif