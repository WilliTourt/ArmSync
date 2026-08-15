#pragma once

#include <stdint.h>
#include <stddef.h>

/**
 * @brief Thin C++ wrapper over the OSPI-B SPI NOR Flash driver (W25Q256JV).
 *
 * The Winbond W25Q256JV (32 MB) on the RA8P1 carrier board is memory-mapped
 * at FLASH_BASE (0x80000000): reads are plain XIP pointer dereferences, while
 * writes/erases go through the R_OSPI_B_* API. This class hides the low-level
 * details (command sequences, WIP polling, D-cache maintenance, critical
 * sections) behind a small read/write/erase interface so higher layers (e.g.
 * the recording/playback task) don't need to know about the hardware.
 *
 * Thread-safety: these methods are meant to be called from a single RTOS task
 * (recording task) — not from ISRs and not concurrently. Each operation
 * internally disables interrupts around the actual OSPI transaction.
 */
class FlashStorage {
    public:
        // Ctor is trivial; call init() once before using the instance.
        FlashStorage() = default;

        // Open the OSPI-B driver. Returns true on success.
        bool init();

        // Erase one 4 KB sector covering `addr`. Returns true on success.
        bool eraseSector(uint32_t addr);

        // Write `len` bytes (must be 64-aligned chunks, `len` a multiple of 64)
        // to `addr`. The target sector(s) must already be erased. Returns true on success.
        bool write(uint32_t addr, const uint8_t *data, uint32_t len);

        // Memory-mapped (XIP) read of `len` bytes from `addr` into `buf`.
        // Invalidate the D-cache first so we fetch fresh data from the part.
        void read(uint32_t addr, uint8_t *buf, uint32_t len) const;

        // Return the memory-mapped address for `addr` (for direct pointer reads).
        static uint8_t *map(uint32_t addr) { return (uint8_t *)(FLASH_BASE + addr); }

        // Invalidate D-cache for the mapped region — call before reading after a write.
        static void invalidateDCache(uint32_t addr, uint32_t len);

        // --- W25Q256JV / OSPI constants ---------------------------------
        static constexpr uint32_t FLASH_BASE     = 0x80000000UL;
        static constexpr uint32_t SECTOR_SIZE    = 0x1000UL;   // 4 KB sector
        static constexpr uint32_t PAGE_SIZE      = 0x1000UL;   // 4 KB page program max
        static constexpr uint32_t WRITE_CHUNK    = 64U;        // aligned program chunk
        static constexpr uint32_t WRITE_ALIGN    = 8U;         // CPU access length (must be multiple)
        static constexpr uint32_t FLASH_TOTAL    = 0x02000000UL; // 32 MB

    private:
        void waitUntilWip() const;
        bool _opened = false;
};
