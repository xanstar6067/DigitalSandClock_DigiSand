#include "settings_storage.h"

#include <stddef.h>
#include <string.h>

#include "main.h"
#include "sand_clock_config.h"
#include "stm32f4xx_hal_flash_ex.h"

#define SETTINGS_STORAGE_MAGIC   0x53434C4BU
#define SETTINGS_STORAGE_VERSION 1U

typedef struct {
    uint32_t magic;
    uint32_t version;
    int32_t seconds;
    uint32_t brightness;
    uint32_t crc;
} StoredSettings;

static uint32_t settings_crc(const StoredSettings *stored) {
    const uint8_t *bytes = (const uint8_t *)stored;
    uint32_t crc = 2166136261UL;

    for (uint32_t i = 0U; i < offsetof(StoredSettings, crc); i++) {
        crc ^= bytes[i];
        crc *= 16777619UL;
    }

    return crc;
}

static bool settings_are_valid(const StoredSettings *stored) {
    if (stored->magic != SETTINGS_STORAGE_MAGIC ||
        stored->version != SETTINGS_STORAGE_VERSION ||
        stored->crc != settings_crc(stored)) {
        return false;
    }

    if (stored->seconds < 0 ||
        stored->seconds > SAND_CLOCK_MAX_SECONDS ||
        stored->brightness > 15U) {
        return false;
    }

    return true;
}

static void settings_pack(StoredSettings *stored,
                          const SettingsStorageData *settings) {
    stored->magic = SETTINGS_STORAGE_MAGIC;
    stored->version = SETTINGS_STORAGE_VERSION;
    stored->seconds = settings->seconds;
    stored->brightness = settings->brightness;
    stored->crc = settings_crc(stored);
}

bool SettingsStorage_Load(SettingsStorageData *settings) {
    const StoredSettings *stored =
        (const StoredSettings *)SAND_CLOCK_SETTINGS_FLASH_ADDRESS;

    if (!settings_are_valid(stored)) {
        return false;
    }

    settings->seconds = (int16_t)stored->seconds;
    settings->brightness = (uint8_t)stored->brightness;
    return true;
}

bool SettingsStorage_Save(const SettingsStorageData *settings) {
    StoredSettings next;
    const StoredSettings *current =
        (const StoredSettings *)SAND_CLOCK_SETTINGS_FLASH_ADDRESS;
    FLASH_EraseInitTypeDef erase = {0};
    uint32_t sector_error = 0U;
    HAL_StatusTypeDef status;

    settings_pack(&next, settings);

    if (settings_are_valid(current) &&
        memcmp(current, &next, sizeof(next)) == 0) {
        return true;
    }

    HAL_FLASH_Unlock();

    erase.TypeErase = FLASH_TYPEERASE_SECTORS;
    erase.Sector = FLASH_SECTOR_7;
    erase.NbSectors = 1U;
    erase.VoltageRange = FLASH_VOLTAGE_RANGE_3;

    status = HAL_FLASHEx_Erase(&erase, &sector_error);
    if (status == HAL_OK) {
        const uint32_t *words = (const uint32_t *)&next;
        uint32_t address = SAND_CLOCK_SETTINGS_FLASH_ADDRESS;

        for (uint32_t i = 0U; i < (sizeof(next) / sizeof(uint32_t)); i++) {
            status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,
                                       address,
                                       words[i]);
            if (status != HAL_OK) {
                break;
            }
            address += sizeof(uint32_t);
        }
    }

    HAL_FLASH_Lock();
    return status == HAL_OK;
}
