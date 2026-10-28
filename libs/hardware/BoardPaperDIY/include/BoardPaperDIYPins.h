#pragma once

// M5Stack PaperDIY (K159) pin map. Source: M5Stack's PaperDIY GPIO table,
// cross-checked against M5GFX's board_M5PaperDIY autodetect (src/M5GFX.cpp).
// The EPD bus is identical to the PaperS3's; only the power path differs.

// ED047TC1 8-bit parallel data bus, D0..D7.
#define PAPERDIY_EP_D0 6
#define PAPERDIY_EP_D1 14
#define PAPERDIY_EP_D2 7
#define PAPERDIY_EP_D3 12
#define PAPERDIY_EP_D4 9
#define PAPERDIY_EP_D5 11
#define PAPERDIY_EP_D6 8
#define PAPERDIY_EP_D7 10

#define PAPERDIY_EP_SPH 13  // XSTL
#define PAPERDIY_EP_CL 16   // XCL
#define PAPERDIY_EP_LE 15   // XLE
#define PAPERDIY_EP_SPV 17
#define PAPERDIY_EP_CKV 18
#define PAPERDIY_EP_OE 45   // M5GFX drives OE on GPIO45 (the docs list G45 as FPC MCLK)
#define PAPERDIY_EP_BST_EN 46  // boost enable; M5GFX's pin_pwr. Also needs PM1 GPIO2 (EPD_PWR) high.

