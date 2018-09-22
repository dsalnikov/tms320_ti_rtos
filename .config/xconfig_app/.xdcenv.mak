#
_XDCBUILDCOUNT = 0
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = C:/ti/tirtos_c2000_2_16_01_14/packages;C:/ti/tirtos_c2000_2_16_01_14/products/tidrivers_c2000_2_16_01_13/packages;C:/ti/tirtos_c2000_2_16_01_14/products/bios_6_45_02_31/packages;C:/ti/tirtos_c2000_2_16_01_14/products/ndk_2_25_00_09/packages;C:/ti/tirtos_c2000_2_16_01_14/products/uia_2_00_05_50/packages;C:/ti/tirtos_c2000_2_16_01_14/products/bios_6_41_00_26/packages;C:/ti/tirtos_c2000_2_16_01_14/products/ipc_3_30_01_12/packages;C:/ti/tirtos_c2000_2_16_01_14/products/ndk_2_24_01_18/packages;C:/ti/tirtos_c2000_2_16_01_14/products/uia_2_00_02_39/packages;C:/ti/ccsv8/ccs_base;C:/Users/lamazavr/git/tms320_ti_rtos/.config
override XDCROOT = C:/ti/xdctools_3_32_02_25_core
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = C:/ti/tirtos_c2000_2_16_01_14/packages;C:/ti/tirtos_c2000_2_16_01_14/products/tidrivers_c2000_2_16_01_13/packages;C:/ti/tirtos_c2000_2_16_01_14/products/bios_6_45_02_31/packages;C:/ti/tirtos_c2000_2_16_01_14/products/ndk_2_25_00_09/packages;C:/ti/tirtos_c2000_2_16_01_14/products/uia_2_00_05_50/packages;C:/ti/tirtos_c2000_2_16_01_14/products/bios_6_41_00_26/packages;C:/ti/tirtos_c2000_2_16_01_14/products/ipc_3_30_01_12/packages;C:/ti/tirtos_c2000_2_16_01_14/products/ndk_2_24_01_18/packages;C:/ti/tirtos_c2000_2_16_01_14/products/uia_2_00_02_39/packages;C:/ti/ccsv8/ccs_base;C:/Users/lamazavr/git/tms320_ti_rtos/.config;C:/ti/xdctools_3_32_02_25_core/packages;..
HOSTOS = Windows
endif
