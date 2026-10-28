#include <BoardPaperDIYPins.h>
#include <PaperDiyBoard.h>
#include <LgfxEpdConfig.h>

namespace {

// OE (GPIO45) and BST_EN (GPIO46) are plain GPIOs sequenced by Bus_EPD's stock
// power control, but the panel rail itself (PM1 GPIO2) must be up first. M5GFX
// raises it at autodetect and leaves it on; do the same, once, before the bus
// takes the pins.
bool prepareEpdPower() { return freeink::paperdiy::ensureBooted(); }

}  // namespace

namespace freeink {

const LgfxEpdConfig& m5PaperDiyLgfxConfig() {
  static const LgfxEpdConfig cfg = {
      {PAPERDIY_EP_D0, PAPERDIY_EP_D1, PAPERDIY_EP_D2, PAPERDIY_EP_D3, PAPERDIY_EP_D4, PAPERDIY_EP_D5,
       PAPERDIY_EP_D6, PAPERDIY_EP_D7},
      PAPERDIY_EP_SPH,
      PAPERDIY_EP_SPV,
      PAPERDIY_EP_OE,
      PAPERDIY_EP_LE,
      PAPERDIY_EP_CL,
      PAPERDIY_EP_CKV,
      PAPERDIY_EP_BST_EN,
      16000000,  // busHz — M5GFX's bus_speed for this board
      8,         // linePadding — M5GFX's line_padding
      0,         // rotation: SDK native landscape (960x540); flip to 2 if "up" is inverted on hardware
      {&prepareEpdPower, nullptr, nullptr},
  };
  return cfg;
}

}  // namespace freeink
