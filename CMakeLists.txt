# ESP-IDF component manifest for M5Utility.
# Upstream: https://github.com/m5stack/M5Utility (originally Arduino library;
# pure C++ utilities + freertos/esp_timer based compatibility shims, no
# Arduino headers required when ARDUINO is not defined).
#
# Dependencies are PRIV_REQUIRES (implementation-only, not exposed via public
# headers) — used solely in src/m5_utility/compatibility_feature.cpp:
#   freertos   : FreeRTOS.h, task.h, vTaskDelay, xPortInIsrContext, pdMS_TO_TICKS
#   esp_timer  : esp_timer_get_time
#   esp_rom    : esp_rom_delay_us
file(GLOB_RECURSE SRCS "src/m5_utility/*.cpp")
idf_component_register(
    SRCS ${SRCS}
    INCLUDE_DIRS "src"
    PRIV_REQUIRES freertos esp_timer esp_rom
)
