#include "FlashStorage.h"
#include "hal_data.h"

/* RA critical-section helpers (provided by BSP) */
#include "fsp_common_api.h"

namespace {
// Send the WINBOND WRITE ENABLE (0x06) command. The R_OSPI_B_Write non-DMA
// path does NOT issue write-enable itself; the W25Q256JV requires WEL set
// before every program/erase operation (and erase clears it). The CPKHMI
// qspi example sends 0x06 before each write burst, so we mirror that here.
bool flashWriteEnable() {
    spi_flash_direct_transfer_t dt = {};
    dt.command        = 0x06U;
    dt.command_length = 0x1U;
    fsp_err_t err = R_OSPI_B_DirectTransfer(
        g_ospi0.p_ctrl, &dt, SPI_FLASH_DIRECT_TRANSFER_DIR_WRITE);
    return (err == FSP_SUCCESS);
}
}

bool FlashStorage::init() {
    if (_opened) return true;
    fsp_err_t err = R_OSPI_B_Open(g_ospi0.p_ctrl, g_ospi0.p_cfg);
    _opened = (err == FSP_SUCCESS);
    return _opened;
}

void FlashStorage::waitUntilWip() const {
    spi_flash_status_t status;
    status.write_in_progress = true;
    uint32_t timeout = 0xFFFFFFFF;
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

        // W25Q256JV needs WRITE ENABLE (WEL) set before each program operation;
        // the R_OSPI_B_Write non-DMA path does not do this itself, and erase
        // clears WEL. Issue 0x06 so the direct-store doesn't fault.
        if (!flashWriteEnable()) {
            return false;
        }

        FSP_CRITICAL_SECTION_DEFINE;
        FSP_CRITICAL_SECTION_ENTER;
        fsp_err_t err = R_OSPI_B_Write(g_ospi0.p_ctrl, &data[off], dest + off, chunk);
        FSP_CRITICAL_SECTION_EXIT;
        if (err != FSP_SUCCESS) return false;

        waitUntilWip();

        // Give the OSPI part a short settle time between consecutive program
        // operations. Continuous back-to-back writes were observed to fault;
        // a small delay after WIP lets the state machine fully reset.
        R_BSP_SoftwareDelay(1, BSP_DELAY_UNITS_MILLISECONDS);

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
