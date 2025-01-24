// SPDX-License-Identifier: BSD-3-Clause
// SPDX-FileCopyrightText: 2025 Rachel Mant <git@dragonmux.network>
#ifndef EMULATOR_ATARI_STE_HXX
#define EMULATOR_ATARI_STE_HXX

#include "memoryMap.hxx"
#include "cpu/68000.hxx"

// M68k has a 24-bit address bus, but we can't directly represent that, so use a 32-bit address value instead.
struct atariSTe_t : memoryMap_t<uint32_t>
{
private:
	motorola68000_t cpu{*this};

public:
	atariSTe_t() noexcept;
};

#endif /*EMULATOR_ATARI_STE_HXX*/
