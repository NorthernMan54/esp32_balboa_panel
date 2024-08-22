#include "restartReason.h"

String getLastRestartReason()
{

  String espResetReason = "";
  esp_reset_reason_t reason = esp_reset_reason();

  switch (reason)
  {
  case ESP_RST_UNKNOWN:
    espResetReason = "Reset reason can not be determined";
    break;

  case ESP_RST_POWERON:
    espResetReason = "Reset due to power-on event";
    break;

  case ESP_RST_EXT:
    espResetReason = "Reset by external pin (not applicable for ESP32)";
    break;

  case ESP_RST_SW:
    espResetReason = "Software reset via esp_restart";
    break;

  case ESP_RST_PANIC:
    espResetReason = "Software reset due to exception/panic";
    break;

  case ESP_RST_INT_WDT:
    espResetReason = "Reset (software or hardware) due to interrupt watchdog";
    break;

  case ESP_RST_TASK_WDT:
    espResetReason = "Reset due to task watchdog";
    break;

  case ESP_RST_WDT:
    espResetReason = "Reset due to other watchdogs";
    break;

  case ESP_RST_DEEPSLEEP:
    espResetReason = "Reset after exiting deep sleep mode";
    break;

  case ESP_RST_BROWNOUT:
    espResetReason = "Brownout reset (software or hardware)";
    break;

  case ESP_RST_SDIO:
    espResetReason = "Reset over SDIO";
    break;

  default:
    break;
  }

return espResetReason;
}
