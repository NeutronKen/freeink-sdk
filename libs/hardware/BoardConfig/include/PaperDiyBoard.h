#pragma once

// M5Stack PaperDIY — board bring-up over the M5PM1 PMIC, single owner.
//
// The PaperDIY routes its panel rail (EPD_PWR = PM1 GPIO2), charge status
// (PM1 GPIO3) and the 3V3_L2 rail that feeds the microSD slot and both user
// buttons through the PM1 on the internal I2C bus (SDA41/SCL42). Header-only and
// idempotent: BoardConfig::holdPowerRails() calls ensureBooted() first thing in
// setup(), and the display config calls it again before touching the bus.

#include <Arduino.h>

#include "M5Pm1.h"

namespace freeink {
namespace paperdiy {

constexpr uint8_t PM1_GPIO_EPD_PWR = 2;   // panel + driver rail, HIGH = on
constexpr uint8_t PM1_GPIO_CHG_STAT = 3;  // LOW = charging

inline bool ensureBooted() {
  static bool booted = false;
  static bool ok = false;
  if (booted) return ok;
  booted = true;
  using namespace m5pm1;
  constexpr uint8_t kEpdPwr = 1 << PM1_GPIO_EPD_PWR;
  constexpr uint8_t kChgStat = 1 << PM1_GPIO_CHG_STAT;

  beginBus();
  clearWakeSource();
  // PWR_CFG is cleared by the PM1 on reset/shutdown, so establish it every boot.
  // CHG_EN charges the 140 mAh cell. DCDC_EN is the 3V3_L2 rail for the microSD
  // slot and the two user buttons. BOOST (5V out) and LDO (RGB) stay off.
  bool good = updateReg(REG_PWR_CFG, BOOST_EN | LDO_EN, CHG_EN | DCDC_EN);
  // Take over the single-click reset so a power-button click reaches firmware;
  // the PMIC's long-hold download escape stays enabled.
  configureAppPowerButton();

  // EPD_PWR as push-pull output HIGH (same sequence as M5GFX/M5Unified).
  good &= updateReg(REG_GPIO_FUNC0, 0b11 << (PM1_GPIO_EPD_PWR * 2), 0);
  good &= updateReg(REG_GPIO_MODE, 0, kEpdPwr);
  good &= updateReg(REG_GPIO_DRV, kEpdPwr, 0);
  good &= updateReg(REG_GPIO_OUT, 0, kEpdPwr);
  // CHG_STAT as pulled-up input.
  good &= updateReg(REG_GPIO_FUNC0, 0b11 << (PM1_GPIO_CHG_STAT * 2), 0);
  good &= updateReg(REG_GPIO_MODE, kChgStat, 0);
  good &= updateReg(REG_GPIO_PULL0, 0b11 << (PM1_GPIO_CHG_STAT * 2), 0b01 << (PM1_GPIO_CHG_STAT * 2));

  delay(10);  // let the 3V3_L2 and panel rails settle
  ok = good;
  return ok;
}

// Returns true once per power-button click (the button reaches only the PM1).
// Rate-limited: each poll is two paced I2C transactions to a firmware-emulated slave.
inline bool pollPowerButtonClicked(unsigned long nowMs) {
  static unsigned long lastPollMs = 0;
  constexpr unsigned long POLL_INTERVAL_MS = 250;
  if (nowMs - lastPollMs < POLL_INTERVAL_MS) return false;
  lastPollMs = nowMs;
  if (!ensureBooted()) return false;
  uint8_t status = 0;
  if (!m5pm1::readButtonStatus(&status)) return false;
  return (status & m5pm1::BTN_EVENT) != 0;
}

// Drops the panel rail and asks the PM1 to shut the board down. On USB the board
// stays powered; treat as best-effort there.
inline bool powerOff() {
  if (!ensureBooted()) return false;
  m5pm1::updateReg(m5pm1::REG_GPIO_OUT, 1 << PM1_GPIO_EPD_PWR, 0);
  return m5pm1::requestShutdown();
}

}  // namespace paperdiy
}  // namespace freeink
