#include "../../include/drivers/PCI/PCI.h"
#include "../../include/types.h"
#include "../../include/drivers/ports/ports.h"
#include "../../include/vga/stdio.h"

u32 read_PCI(u8 BUS_ID, u8 DEV_ID, u8 FUNC_ID, u8 REG_OFFSET){
    u32 ret = 0;
    u32 PCI_ID = 0;
    PCI_ID = (1 << 31) | (BUS_ID << 16) | (DEV_ID << 11) | (FUNC_ID << 8) | REG_OFFSET;
    outl(PCI_COMMAND, PCI_ID);
    ret = inl(PCI_DATA);
    return ret;
}

struct PCIDev_Descriptor GetDevInfo(u8 BUS_ID, u8 DEV_ID, u8 FUNC_ID){
    // Init
    struct PCIDev_Descriptor Dev;
    Dev.VENDOR_ID = 0;
    Dev.DEVICE_ID = 0;
    Dev.INTERFACE = 0;
    Dev.INTERRUPT = 0;
    Dev.REVISION  = 0;
    Dev.SUBCLASS  = 0;
    Dev.CLASS     = 0;

    u32 Info = 0;
    Info = read_PCI(BUS_ID, DEV_ID, FUNC_ID, 0x00);
    if (Info == 0x0000 || Info == 0xFFFF || Info == 0xFFFFFFFF){
        return Dev;
    }
    Dev.VENDOR_ID = (u16)Info;
    Dev.DEVICE_ID = (u16)(Info >> 16);

    Info = read_PCI(BUS_ID, DEV_ID, FUNC_ID, 0x08);
    Dev.REVISION = (u8)Info;
    Dev.INTERFACE = (u8)(Info >> 8);
    Dev.SUBCLASS = (u8)(Info >> 16);
    Dev.CLASS = (u8)(Info >> 24);

    Info = read_PCI(BUS_ID, DEV_ID, FUNC_ID, 0x3C);
    Dev.INTERRUPT = (u8)Info;
    return Dev;
}

void fillVendors(char** Vendors_str, u16* VendorsID){
    char* VendorsStr[15] = {"Intel", "AMD", "NVIDIA", "Realtek", "Samsung", "SK hynix", "MSI", "Gigabyte", "ASUS", "Dell", "HP", "Broadcom", "QEMU", "VMware", 0};
    u16 VendorIDs[15] = {0x8086, 0x1022, 0x10DE, 0x10EC, 0x144D, 0x1C5C, 0x1462, 0x1458, 0x1043, 0x1028, 0x103C, 0x14E4, 0x1234, 0x15AD, 0};
    for (u32 i = 0; *(VendorsStr + i) != 0; ++i){
        *(Vendors_str + i) = *(VendorsStr + i);
        *(VendorsID + i) = *(VendorIDs + i);
    }
}

u8 CheckMultifun(u8 BUS_ID, u8 DEV_ID){
    u32 Info = read_PCI(BUS_ID, DEV_ID, 0, 0x0E);   // Check if multifun
    if ((u8)Info == (1 << 7)){                      //
        return 1;
    }
    else {
        return 0;
    }
}