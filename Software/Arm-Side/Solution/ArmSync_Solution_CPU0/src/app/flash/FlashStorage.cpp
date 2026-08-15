#include "FlashStorage.h"
#include "hal_data.h"

/* RA critical-section helpers (provided by BSP) */
#include "fsp_common_api.h"

bool FlashStorage::init() {
    if (_opened) return true;
    fsp_err_t err = R_OSPI_B_Open(g_ospi0.p_ctrl, g_ospi0.p_cfg);
    _opened = (err == FSP_SUCCESS);
    return _opened;
}

void FlashStorage::waitUntilWip() const {
    spi_flash_status_t status;
    status.write_in_progress = true;
    uint32_t timeout = UINT32_MAX;
    while ((status.write_in_progress) && (--timeout > 0)) {
        (void)R_OSPI_B_StatusGet(g_ospi0.p_ctrl, &status);
    }
}

bool FlashStorage::eraseSector(uint32_t addr) {
    if (!_opened) return false;
    FSP_CRITICAL_SECTION_DEFINE;
    FSP_CRITICAL_SECTION_ENTER;
    fsp_err_t err = R_OSPI_B_Erase(g_ospi0.p_ctrl, (uint8_t *)(FLASH_BASE + addr), SECTOR_SIZE);
    FSP_CRITICAL_SECTION_EXIT;
    if (err != FSP_SUCCESS) return false;
    waitUntilWip();
    return true;
}

bool FlashStorage::write(uint32_t addr, const uint8_t *data, uint32_t len) {
    if (!_opened) return false;
    if (len == 0) return true;

    uint8_t *dest = (uint8_t *)(FLASH_BASE + addr);
    uint32_t off  = 0;
    while (off < len) {
        // OSPI constraints per R_OSPI_B_Write call:
        //   (1) must stay within one 64-byte page (page_offset = addr & 63)
        //   (2) byte_count must be a multiple of the CPU access length (8)
        // So each chunk is capped to the current page AND floored to 8 bytes.
        uint32_t page_rem = WRITE_CHUNK - (((uint32_t)(dest + off)) & (WRITE_CHUNK - 1));
        uint32_t chunk    = (len - off) < page_rem ? (len - off) : page_rem;
        chunk &= ~(WRITE_ALIGN - 1U);          // multiple of 8

        if (chunk == 0) return false;          // unaligned tail we cannot express

        FSP_CRITICAL_SECTION_DEFINE;
        FSP_CRITICAL_SECTION_ENTER;
        fsp_err_t err = R_OSPI_B_Write(g_ospi0.p_ctrl, &data[off], dest + off, chunk);
        FSP_CRITICAL_SECTION_EXIT;
        if (err != FSP_SUCCESS) return false;

        waitUntilWip();
        off += chunk;
    }

    // Fresh data is in the part; drop the (potentially stale) D-cache lines now
    // so later XIP reads don't return cached garbage.
    invalidateDCache(addr, len);
    return true;
}

void FlashStorage::read(uint32_t addr, uint8_t *buf, uint32_t len) const {
    invalidateDCache(addr, len);
    const uint8_t *src = (const uint8_t *)(FLASH_BASE + addr);
    for (uint32_t i = 0; i < len; i++) buf[i] = src[i];
}

void FlashStorage::invalidateDCache(uint32_t addr, uint32_t len) {
    SCB_InvalidateDCache_by_Addr((volatile void *)(FLASH_BASE + addr), len);
}
