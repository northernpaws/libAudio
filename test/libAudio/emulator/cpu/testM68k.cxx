// SPDX-License-Identifier: BSD-3-Clause
// SPDX-FileCopyrightText: 2025 Rachel Mant <git@dragonmux.network>
#include <array>
#include <memory>
#include <substrate/indexed_iterator>
#include <crunch++.h>
#include "emulator/cpu/m68k.hxx"
#include "emulator/ram.hxx"
#include "emulator/unitsHelpers.hxx"

constexpr std::array<decodedOperation_t, 65536U> instructionMap
{{
	{instruction_t::ori, 0U, 0U, {}, 0U, 0U, 0U},
	{instruction_t::ori, 0U, 1U, {}, 0U, 0U, 0U},
	{instruction_t::ori, 0U, 2U, {}, 0U, 0U, 0U},
	{instruction_t::ori, 0U, 3U, {}, 0U, 0U, 0U},
	{instruction_t::ori, 0U, 4U, {}, 0U, 0U, 0U},
	{instruction_t::ori, 0U, 5U, {}, 0U, 0U, 0U},
	{instruction_t::ori, 0U, 6U, {}, 0U, 0U, 0U},
	{instruction_t::ori, 0U, 7U, {}, 0U, 0U, 0U},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::ori, 0U, 0U, {}, 0U, 0U, 2U},
	{instruction_t::ori, 0U, 1U, {}, 0U, 0U, 2U},
	{instruction_t::ori, 0U, 2U, {}, 0U, 0U, 2U},
	{instruction_t::ori, 0U, 3U, {}, 0U, 0U, 2U},
	{instruction_t::ori, 0U, 4U, {}, 0U, 0U, 2U},
	{instruction_t::ori, 0U, 5U, {}, 0U, 0U, 2U},
	{instruction_t::ori, 0U, 6U, {}, 0U, 0U, 2U},
	{instruction_t::ori, 0U, 7U, {}, 0U, 0U, 2U},
	{instruction_t::ori, 0U, 0U, {}, 0U, 0U, 3U},
	{instruction_t::ori, 0U, 1U, {}, 0U, 0U, 3U},
	{instruction_t::ori, 0U, 2U, {}, 0U, 0U, 3U},
	{instruction_t::ori, 0U, 3U, {}, 0U, 0U, 3U},
	{instruction_t::ori, 0U, 4U, {}, 0U, 0U, 3U},
	{instruction_t::ori, 0U, 5U, {}, 0U, 0U, 3U},
	{instruction_t::ori, 0U, 6U, {}, 0U, 0U, 3U},
	{instruction_t::ori, 0U, 7U, {}, 0U, 0U, 3U},
	{instruction_t::ori, 0U, 0U, {}, 0U, 0U, 4U},
	{instruction_t::ori, 0U, 1U, {}, 0U, 0U, 4U},
	{instruction_t::ori, 0U, 2U, {}, 0U, 0U, 4U},
	{instruction_t::ori, 0U, 3U, {}, 0U, 0U, 4U},
	{instruction_t::ori, 0U, 4U, {}, 0U, 0U, 4U},
	{instruction_t::ori, 0U, 5U, {}, 0U, 0U, 4U},
	{instruction_t::ori, 0U, 6U, {}, 0U, 0U, 4U},
	{instruction_t::ori, 0U, 7U, {}, 0U, 0U, 4U},
	{instruction_t::ori, 0U, 0U, {}, 0U, 0U, 5U},
	{instruction_t::ori, 0U, 1U, {}, 0U, 0U, 5U},
	{instruction_t::ori, 0U, 2U, {}, 0U, 0U, 5U},
	{instruction_t::ori, 0U, 3U, {}, 0U, 0U, 5U},
	{instruction_t::ori, 0U, 4U, {}, 0U, 0U, 5U},
	{instruction_t::ori, 0U, 5U, {}, 0U, 0U, 5U},
	{instruction_t::ori, 0U, 6U, {}, 0U, 0U, 5U},
	{instruction_t::ori, 0U, 7U, {}, 0U, 0U, 5U},
	{instruction_t::ori, 0U, 0U, {}, 0U, 0U, 6U},
	{instruction_t::ori, 0U, 1U, {}, 0U, 0U, 6U},
	{instruction_t::ori, 0U, 2U, {}, 0U, 0U, 6U},
	{instruction_t::ori, 0U, 3U, {}, 0U, 0U, 6U},
	{instruction_t::ori, 0U, 4U, {}, 0U, 0U, 6U},
	{instruction_t::ori, 0U, 5U, {}, 0U, 0U, 6U},
	{instruction_t::ori, 0U, 6U, {}, 0U, 0U, 6U},
	{instruction_t::ori, 0U, 7U, {}, 0U, 0U, 6U},
	{instruction_t::ori, 0U, 0U, {}, 0U, 0U, 7U},
	{instruction_t::ori, 0U, 1U, {}, 0U, 0U, 7U},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::ori, 0U, 8U, {}, 1U, 0U, 0U, 2U},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::ori, 0U, 0U, {}, 1U, 0U, 0U},
	{instruction_t::ori, 0U, 1U, {}, 1U, 0U, 0U},
	{instruction_t::ori, 0U, 2U, {}, 1U, 0U, 0U},
	{instruction_t::ori, 0U, 3U, {}, 1U, 0U, 0U},
	{instruction_t::ori, 0U, 4U, {}, 1U, 0U, 0U},
	{instruction_t::ori, 0U, 5U, {}, 1U, 0U, 0U},
	{instruction_t::ori, 0U, 6U, {}, 1U, 0U, 0U},
	{instruction_t::ori, 0U, 7U, {}, 1U, 0U, 0U},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::ori, 0U, 0U, {}, 1U, 0U, 2U},
	{instruction_t::ori, 0U, 1U, {}, 1U, 0U, 2U},
	{instruction_t::ori, 0U, 2U, {}, 1U, 0U, 2U},
	{instruction_t::ori, 0U, 3U, {}, 1U, 0U, 2U},
	{instruction_t::ori, 0U, 4U, {}, 1U, 0U, 2U},
	{instruction_t::ori, 0U, 5U, {}, 1U, 0U, 2U},
	{instruction_t::ori, 0U, 6U, {}, 1U, 0U, 2U},
	{instruction_t::ori, 0U, 7U, {}, 1U, 0U, 2U},
	{instruction_t::ori, 0U, 0U, {}, 1U, 0U, 3U},
	{instruction_t::ori, 0U, 1U, {}, 1U, 0U, 3U},
	{instruction_t::ori, 0U, 2U, {}, 1U, 0U, 3U},
	{instruction_t::ori, 0U, 3U, {}, 1U, 0U, 3U},
	{instruction_t::ori, 0U, 4U, {}, 1U, 0U, 3U},
	{instruction_t::ori, 0U, 5U, {}, 1U, 0U, 3U},
	{instruction_t::ori, 0U, 6U, {}, 1U, 0U, 3U},
	{instruction_t::ori, 0U, 7U, {}, 1U, 0U, 3U},
	{instruction_t::ori, 0U, 0U, {}, 1U, 0U, 4U},
	{instruction_t::ori, 0U, 1U, {}, 1U, 0U, 4U},
	{instruction_t::ori, 0U, 2U, {}, 1U, 0U, 4U},
	{instruction_t::ori, 0U, 3U, {}, 1U, 0U, 4U},
	{instruction_t::ori, 0U, 4U, {}, 1U, 0U, 4U},
	{instruction_t::ori, 0U, 5U, {}, 1U, 0U, 4U},
	{instruction_t::ori, 0U, 6U, {}, 1U, 0U, 4U},
	{instruction_t::ori, 0U, 7U, {}, 1U, 0U, 4U},
	{instruction_t::ori, 0U, 0U, {}, 1U, 0U, 5U},
	{instruction_t::ori, 0U, 1U, {}, 1U, 0U, 5U},
	{instruction_t::ori, 0U, 2U, {}, 1U, 0U, 5U},
	{instruction_t::ori, 0U, 3U, {}, 1U, 0U, 5U},
	{instruction_t::ori, 0U, 4U, {}, 1U, 0U, 5U},
	{instruction_t::ori, 0U, 5U, {}, 1U, 0U, 5U},
	{instruction_t::ori, 0U, 6U, {}, 1U, 0U, 5U},
	{instruction_t::ori, 0U, 7U, {}, 1U, 0U, 5U},
	{instruction_t::ori, 0U, 0U, {}, 1U, 0U, 6U},
	{instruction_t::ori, 0U, 1U, {}, 1U, 0U, 6U},
	{instruction_t::ori, 0U, 2U, {}, 1U, 0U, 6U},
	{instruction_t::ori, 0U, 3U, {}, 1U, 0U, 6U},
	{instruction_t::ori, 0U, 4U, {}, 1U, 0U, 6U},
	{instruction_t::ori, 0U, 5U, {}, 1U, 0U, 6U},
	{instruction_t::ori, 0U, 6U, {}, 1U, 0U, 6U},
	{instruction_t::ori, 0U, 7U, {}, 1U, 0U, 6U},
	{instruction_t::ori, 0U, 0U, {}, 1U, 0U, 7U},
	{instruction_t::ori, 0U, 1U, {}, 1U, 0U, 7U},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal}, // XXX: Should be ORI to SR
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::ori, 0U, 0U, {}, 2U, 0U, 0U},
	{instruction_t::ori, 0U, 1U, {}, 2U, 0U, 0U},
	{instruction_t::ori, 0U, 2U, {}, 2U, 0U, 0U},
	{instruction_t::ori, 0U, 3U, {}, 2U, 0U, 0U},
	{instruction_t::ori, 0U, 4U, {}, 2U, 0U, 0U},
	{instruction_t::ori, 0U, 5U, {}, 2U, 0U, 0U},
	{instruction_t::ori, 0U, 6U, {}, 2U, 0U, 0U},
	{instruction_t::ori, 0U, 7U, {}, 2U, 0U, 0U},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::ori, 0U, 0U, {}, 2U, 0U, 2U},
	{instruction_t::ori, 0U, 1U, {}, 2U, 0U, 2U},
	{instruction_t::ori, 0U, 2U, {}, 2U, 0U, 2U},
	{instruction_t::ori, 0U, 3U, {}, 2U, 0U, 2U},
	{instruction_t::ori, 0U, 4U, {}, 2U, 0U, 2U},
	{instruction_t::ori, 0U, 5U, {}, 2U, 0U, 2U},
	{instruction_t::ori, 0U, 6U, {}, 2U, 0U, 2U},
	{instruction_t::ori, 0U, 7U, {}, 2U, 0U, 2U},
	{instruction_t::ori, 0U, 0U, {}, 2U, 0U, 3U},
	{instruction_t::ori, 0U, 1U, {}, 2U, 0U, 3U},
	{instruction_t::ori, 0U, 2U, {}, 2U, 0U, 3U},
	{instruction_t::ori, 0U, 3U, {}, 2U, 0U, 3U},
	{instruction_t::ori, 0U, 4U, {}, 2U, 0U, 3U},
	{instruction_t::ori, 0U, 5U, {}, 2U, 0U, 3U},
	{instruction_t::ori, 0U, 6U, {}, 2U, 0U, 3U},
	{instruction_t::ori, 0U, 7U, {}, 2U, 0U, 3U},
	{instruction_t::ori, 0U, 0U, {}, 2U, 0U, 4U},
	{instruction_t::ori, 0U, 1U, {}, 2U, 0U, 4U},
	{instruction_t::ori, 0U, 2U, {}, 2U, 0U, 4U},
	{instruction_t::ori, 0U, 3U, {}, 2U, 0U, 4U},
	{instruction_t::ori, 0U, 4U, {}, 2U, 0U, 4U},
	{instruction_t::ori, 0U, 5U, {}, 2U, 0U, 4U},
	{instruction_t::ori, 0U, 6U, {}, 2U, 0U, 4U},
	{instruction_t::ori, 0U, 7U, {}, 2U, 0U, 4U},
	{instruction_t::ori, 0U, 0U, {}, 2U, 0U, 5U},
	{instruction_t::ori, 0U, 1U, {}, 2U, 0U, 5U},
	{instruction_t::ori, 0U, 2U, {}, 2U, 0U, 5U},
	{instruction_t::ori, 0U, 3U, {}, 2U, 0U, 5U},
	{instruction_t::ori, 0U, 4U, {}, 2U, 0U, 5U},
	{instruction_t::ori, 0U, 5U, {}, 2U, 0U, 5U},
	{instruction_t::ori, 0U, 6U, {}, 2U, 0U, 5U},
	{instruction_t::ori, 0U, 7U, {}, 2U, 0U, 5U},
	{instruction_t::ori, 0U, 0U, {}, 2U, 0U, 6U},
	{instruction_t::ori, 0U, 1U, {}, 2U, 0U, 6U},
	{instruction_t::ori, 0U, 2U, {}, 2U, 0U, 6U},
	{instruction_t::ori, 0U, 3U, {}, 2U, 0U, 6U},
	{instruction_t::ori, 0U, 4U, {}, 2U, 0U, 6U},
	{instruction_t::ori, 0U, 5U, {}, 2U, 0U, 6U},
	{instruction_t::ori, 0U, 6U, {}, 2U, 0U, 6U},
	{instruction_t::ori, 0U, 7U, {}, 2U, 0U, 6U},
	{instruction_t::ori, 0U, 0U, {}, 2U, 0U, 7U},
	{instruction_t::ori, 0U, 1U, {}, 2U, 0U, 7U},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::chk2_cmp2, 0U, 0U, {operationFlags_t::immediateNotRegister}, 0U, 0U, 2U, 2U},
	{instruction_t::chk2_cmp2, 0U, 1U, {operationFlags_t::immediateNotRegister}, 0U, 0U, 2U, 2U},
	{instruction_t::chk2_cmp2, 0U, 2U, {operationFlags_t::immediateNotRegister}, 0U, 0U, 2U, 2U},
	{instruction_t::chk2_cmp2, 0U, 3U, {operationFlags_t::immediateNotRegister}, 0U, 0U, 2U, 2U},
	{instruction_t::chk2_cmp2, 0U, 4U, {operationFlags_t::immediateNotRegister}, 0U, 0U, 2U, 2U},
	{instruction_t::chk2_cmp2, 0U, 5U, {operationFlags_t::immediateNotRegister}, 0U, 0U, 2U, 2U},
	{instruction_t::chk2_cmp2, 0U, 6U, {operationFlags_t::immediateNotRegister}, 0U, 0U, 2U, 2U},
	{instruction_t::chk2_cmp2, 0U, 7U, {operationFlags_t::immediateNotRegister}, 0U, 0U, 2U, 2U},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::chk2_cmp2, 0U, 0U, {operationFlags_t::immediateNotRegister}, 0U, 0U, 5U, 2U},
	{instruction_t::chk2_cmp2, 0U, 1U, {operationFlags_t::immediateNotRegister}, 0U, 0U, 5U, 2U},
	{instruction_t::chk2_cmp2, 0U, 2U, {operationFlags_t::immediateNotRegister}, 0U, 0U, 5U, 2U},
	{instruction_t::chk2_cmp2, 0U, 3U, {operationFlags_t::immediateNotRegister}, 0U, 0U, 5U, 2U},
	{instruction_t::chk2_cmp2, 0U, 4U, {operationFlags_t::immediateNotRegister}, 0U, 0U, 5U, 2U},
	{instruction_t::chk2_cmp2, 0U, 5U, {operationFlags_t::immediateNotRegister}, 0U, 0U, 5U, 2U},
	{instruction_t::chk2_cmp2, 0U, 6U, {operationFlags_t::immediateNotRegister}, 0U, 0U, 5U, 2U},
	{instruction_t::chk2_cmp2, 0U, 7U, {operationFlags_t::immediateNotRegister}, 0U, 0U, 5U, 2U},
	{instruction_t::chk2_cmp2, 0U, 0U, {operationFlags_t::immediateNotRegister}, 0U, 0U, 6U, 2U},
	{instruction_t::chk2_cmp2, 0U, 1U, {operationFlags_t::immediateNotRegister}, 0U, 0U, 6U, 2U},
	{instruction_t::chk2_cmp2, 0U, 2U, {operationFlags_t::immediateNotRegister}, 0U, 0U, 6U, 2U},
	{instruction_t::chk2_cmp2, 0U, 3U, {operationFlags_t::immediateNotRegister}, 0U, 0U, 6U, 2U},
	{instruction_t::chk2_cmp2, 0U, 4U, {operationFlags_t::immediateNotRegister}, 0U, 0U, 6U, 2U},
	{instruction_t::chk2_cmp2, 0U, 5U, {operationFlags_t::immediateNotRegister}, 0U, 0U, 6U, 2U},
	{instruction_t::chk2_cmp2, 0U, 6U, {operationFlags_t::immediateNotRegister}, 0U, 0U, 6U, 2U},
	{instruction_t::chk2_cmp2, 0U, 7U, {operationFlags_t::immediateNotRegister}, 0U, 0U, 6U, 2U},
	{instruction_t::chk2_cmp2, 0U, 0U, {operationFlags_t::immediateNotRegister}, 0U, 0U, 7U, 2U},
	{instruction_t::chk2_cmp2, 0U, 1U, {operationFlags_t::immediateNotRegister}, 0U, 0U, 7U, 2U},
	{instruction_t::chk2_cmp2, 0U, 2U, {operationFlags_t::immediateNotRegister}, 0U, 0U, 7U, 2U},
	{instruction_t::chk2_cmp2, 0U, 3U, {operationFlags_t::immediateNotRegister}, 0U, 0U, 7U, 2U},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::btst, 0U, 0U, {}, 0U, 0U, 0U},
	{instruction_t::btst, 0U, 1U, {}, 0U, 0U, 0U},
	{instruction_t::btst, 0U, 2U, {}, 0U, 0U, 0U},
	{instruction_t::btst, 0U, 3U, {}, 0U, 0U, 0U},
	{instruction_t::btst, 0U, 4U, {}, 0U, 0U, 0U},
	{instruction_t::btst, 0U, 5U, {}, 0U, 0U, 0U},
	{instruction_t::btst, 0U, 6U, {}, 0U, 0U, 0U},
	{instruction_t::btst, 0U, 7U, {}, 0U, 0U, 0U},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::btst, 0U, 0U, {}, 0U, 0U, 2U},
	{instruction_t::btst, 0U, 1U, {}, 0U, 0U, 2U},
	{instruction_t::btst, 0U, 2U, {}, 0U, 0U, 2U},
	{instruction_t::btst, 0U, 3U, {}, 0U, 0U, 2U},
	{instruction_t::btst, 0U, 4U, {}, 0U, 0U, 2U},
	{instruction_t::btst, 0U, 5U, {}, 0U, 0U, 2U},
	{instruction_t::btst, 0U, 6U, {}, 0U, 0U, 2U},
	{instruction_t::btst, 0U, 7U, {}, 0U, 0U, 2U},
	{instruction_t::btst, 0U, 0U, {}, 0U, 0U, 3U},
	{instruction_t::btst, 0U, 1U, {}, 0U, 0U, 3U},
	{instruction_t::btst, 0U, 2U, {}, 0U, 0U, 3U},
	{instruction_t::btst, 0U, 3U, {}, 0U, 0U, 3U},
	{instruction_t::btst, 0U, 4U, {}, 0U, 0U, 3U},
	{instruction_t::btst, 0U, 5U, {}, 0U, 0U, 3U},
	{instruction_t::btst, 0U, 6U, {}, 0U, 0U, 3U},
	{instruction_t::btst, 0U, 7U, {}, 0U, 0U, 3U},
	{instruction_t::btst, 0U, 0U, {}, 0U, 0U, 4U},
	{instruction_t::btst, 0U, 1U, {}, 0U, 0U, 4U},
	{instruction_t::btst, 0U, 2U, {}, 0U, 0U, 4U},
	{instruction_t::btst, 0U, 3U, {}, 0U, 0U, 4U},
	{instruction_t::btst, 0U, 4U, {}, 0U, 0U, 4U},
	{instruction_t::btst, 0U, 5U, {}, 0U, 0U, 4U},
	{instruction_t::btst, 0U, 6U, {}, 0U, 0U, 4U},
	{instruction_t::btst, 0U, 7U, {}, 0U, 0U, 4U},
	{instruction_t::btst, 0U, 0U, {}, 0U, 0U, 5U},
	{instruction_t::btst, 0U, 1U, {}, 0U, 0U, 5U},
	{instruction_t::btst, 0U, 2U, {}, 0U, 0U, 5U},
	{instruction_t::btst, 0U, 3U, {}, 0U, 0U, 5U},
	{instruction_t::btst, 0U, 4U, {}, 0U, 0U, 5U},
	{instruction_t::btst, 0U, 5U, {}, 0U, 0U, 5U},
	{instruction_t::btst, 0U, 6U, {}, 0U, 0U, 5U},
	{instruction_t::btst, 0U, 7U, {}, 0U, 0U, 5U},
	{instruction_t::btst, 0U, 0U, {}, 0U, 0U, 6U},
	{instruction_t::btst, 0U, 1U, {}, 0U, 0U, 6U},
	{instruction_t::btst, 0U, 2U, {}, 0U, 0U, 6U},
	{instruction_t::btst, 0U, 3U, {}, 0U, 0U, 6U},
	{instruction_t::btst, 0U, 4U, {}, 0U, 0U, 6U},
	{instruction_t::btst, 0U, 5U, {}, 0U, 0U, 6U},
	{instruction_t::btst, 0U, 6U, {}, 0U, 0U, 6U},
	{instruction_t::btst, 0U, 7U, {}, 0U, 0U, 6U},
	{instruction_t::btst, 0U, 0U, {}, 0U, 0U, 7U},
	{instruction_t::btst, 0U, 1U, {}, 0U, 0U, 7U},
	{instruction_t::btst, 0U, 2U, {}, 0U, 0U, 7U},
	{instruction_t::btst, 0U, 3U, {}, 0U, 0U, 7U},
	{instruction_t::btst, 0U, 4U, {}, 0U, 0U, 7U},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::bchg, 0U, 0U, {}, 0U, 0U, 0U},
	{instruction_t::bchg, 0U, 1U, {}, 0U, 0U, 0U},
	{instruction_t::bchg, 0U, 2U, {}, 0U, 0U, 0U},
	{instruction_t::bchg, 0U, 3U, {}, 0U, 0U, 0U},
	{instruction_t::bchg, 0U, 4U, {}, 0U, 0U, 0U},
	{instruction_t::bchg, 0U, 5U, {}, 0U, 0U, 0U},
	{instruction_t::bchg, 0U, 6U, {}, 0U, 0U, 0U},
	{instruction_t::bchg, 0U, 7U, {}, 0U, 0U, 0U},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::bchg, 0U, 0U, {}, 0U, 0U, 2U},
	{instruction_t::bchg, 0U, 1U, {}, 0U, 0U, 2U},
	{instruction_t::bchg, 0U, 2U, {}, 0U, 0U, 2U},
	{instruction_t::bchg, 0U, 3U, {}, 0U, 0U, 2U},
	{instruction_t::bchg, 0U, 4U, {}, 0U, 0U, 2U},
	{instruction_t::bchg, 0U, 5U, {}, 0U, 0U, 2U},
	{instruction_t::bchg, 0U, 6U, {}, 0U, 0U, 2U},
	{instruction_t::bchg, 0U, 7U, {}, 0U, 0U, 2U},
	{instruction_t::bchg, 0U, 0U, {}, 0U, 0U, 3U},
	{instruction_t::bchg, 0U, 1U, {}, 0U, 0U, 3U},
	{instruction_t::bchg, 0U, 2U, {}, 0U, 0U, 3U},
	{instruction_t::bchg, 0U, 3U, {}, 0U, 0U, 3U},
	{instruction_t::bchg, 0U, 4U, {}, 0U, 0U, 3U},
	{instruction_t::bchg, 0U, 5U, {}, 0U, 0U, 3U},
	{instruction_t::bchg, 0U, 6U, {}, 0U, 0U, 3U},
	{instruction_t::bchg, 0U, 7U, {}, 0U, 0U, 3U},
	{instruction_t::bchg, 0U, 0U, {}, 0U, 0U, 4U},
	{instruction_t::bchg, 0U, 1U, {}, 0U, 0U, 4U},
	{instruction_t::bchg, 0U, 2U, {}, 0U, 0U, 4U},
	{instruction_t::bchg, 0U, 3U, {}, 0U, 0U, 4U},
	{instruction_t::bchg, 0U, 4U, {}, 0U, 0U, 4U},
	{instruction_t::bchg, 0U, 5U, {}, 0U, 0U, 4U},
	{instruction_t::bchg, 0U, 6U, {}, 0U, 0U, 4U},
	{instruction_t::bchg, 0U, 7U, {}, 0U, 0U, 4U},
	{instruction_t::bchg, 0U, 0U, {}, 0U, 0U, 5U},
	{instruction_t::bchg, 0U, 1U, {}, 0U, 0U, 5U},
	{instruction_t::bchg, 0U, 2U, {}, 0U, 0U, 5U},
	{instruction_t::bchg, 0U, 3U, {}, 0U, 0U, 5U},
	{instruction_t::bchg, 0U, 4U, {}, 0U, 0U, 5U},
	{instruction_t::bchg, 0U, 5U, {}, 0U, 0U, 5U},
	{instruction_t::bchg, 0U, 6U, {}, 0U, 0U, 5U},
	{instruction_t::bchg, 0U, 7U, {}, 0U, 0U, 5U},
	{instruction_t::bchg, 0U, 0U, {}, 0U, 0U, 6U},
	{instruction_t::bchg, 0U, 1U, {}, 0U, 0U, 6U},
	{instruction_t::bchg, 0U, 2U, {}, 0U, 0U, 6U},
	{instruction_t::bchg, 0U, 3U, {}, 0U, 0U, 6U},
	{instruction_t::bchg, 0U, 4U, {}, 0U, 0U, 6U},
	{instruction_t::bchg, 0U, 5U, {}, 0U, 0U, 6U},
	{instruction_t::bchg, 0U, 6U, {}, 0U, 0U, 6U},
	{instruction_t::bchg, 0U, 7U, {}, 0U, 0U, 6U},
	{instruction_t::bchg, 0U, 0U, {}, 0U, 0U, 7U},
	{instruction_t::bchg, 0U, 1U, {}, 0U, 0U, 7U},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::bclr, 0U, 0U, {}, 0U, 0U, 0U},
	{instruction_t::bclr, 0U, 1U, {}, 0U, 0U, 0U},
	{instruction_t::bclr, 0U, 2U, {}, 0U, 0U, 0U},
	{instruction_t::bclr, 0U, 3U, {}, 0U, 0U, 0U},
	{instruction_t::bclr, 0U, 4U, {}, 0U, 0U, 0U},
	{instruction_t::bclr, 0U, 5U, {}, 0U, 0U, 0U},
	{instruction_t::bclr, 0U, 6U, {}, 0U, 0U, 0U},
	{instruction_t::bclr, 0U, 7U, {}, 0U, 0U, 0U},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::bclr, 0U, 0U, {}, 0U, 0U, 2U},
	{instruction_t::bclr, 0U, 1U, {}, 0U, 0U, 2U},
	{instruction_t::bclr, 0U, 2U, {}, 0U, 0U, 2U},
	{instruction_t::bclr, 0U, 3U, {}, 0U, 0U, 2U},
	{instruction_t::bclr, 0U, 4U, {}, 0U, 0U, 2U},
	{instruction_t::bclr, 0U, 5U, {}, 0U, 0U, 2U},
	{instruction_t::bclr, 0U, 6U, {}, 0U, 0U, 2U},
	{instruction_t::bclr, 0U, 7U, {}, 0U, 0U, 2U},
	{instruction_t::bclr, 0U, 0U, {}, 0U, 0U, 3U},
	{instruction_t::bclr, 0U, 1U, {}, 0U, 0U, 3U},
	{instruction_t::bclr, 0U, 2U, {}, 0U, 0U, 3U},
	{instruction_t::bclr, 0U, 3U, {}, 0U, 0U, 3U},
	{instruction_t::bclr, 0U, 4U, {}, 0U, 0U, 3U},
	{instruction_t::bclr, 0U, 5U, {}, 0U, 0U, 3U},
	{instruction_t::bclr, 0U, 6U, {}, 0U, 0U, 3U},
	{instruction_t::bclr, 0U, 7U, {}, 0U, 0U, 3U},
	{instruction_t::bclr, 0U, 0U, {}, 0U, 0U, 4U},
	{instruction_t::bclr, 0U, 1U, {}, 0U, 0U, 4U},
	{instruction_t::bclr, 0U, 2U, {}, 0U, 0U, 4U},
	{instruction_t::bclr, 0U, 3U, {}, 0U, 0U, 4U},
	{instruction_t::bclr, 0U, 4U, {}, 0U, 0U, 4U},
	{instruction_t::bclr, 0U, 5U, {}, 0U, 0U, 4U},
	{instruction_t::bclr, 0U, 6U, {}, 0U, 0U, 4U},
	{instruction_t::bclr, 0U, 7U, {}, 0U, 0U, 4U},
	{instruction_t::bclr, 0U, 0U, {}, 0U, 0U, 5U},
	{instruction_t::bclr, 0U, 1U, {}, 0U, 0U, 5U},
	{instruction_t::bclr, 0U, 2U, {}, 0U, 0U, 5U},
	{instruction_t::bclr, 0U, 3U, {}, 0U, 0U, 5U},
	{instruction_t::bclr, 0U, 4U, {}, 0U, 0U, 5U},
	{instruction_t::bclr, 0U, 5U, {}, 0U, 0U, 5U},
	{instruction_t::bclr, 0U, 6U, {}, 0U, 0U, 5U},
	{instruction_t::bclr, 0U, 7U, {}, 0U, 0U, 5U},
	{instruction_t::bclr, 0U, 0U, {}, 0U, 0U, 6U},
	{instruction_t::bclr, 0U, 1U, {}, 0U, 0U, 6U},
	{instruction_t::bclr, 0U, 2U, {}, 0U, 0U, 6U},
	{instruction_t::bclr, 0U, 3U, {}, 0U, 0U, 6U},
	{instruction_t::bclr, 0U, 4U, {}, 0U, 0U, 6U},
	{instruction_t::bclr, 0U, 5U, {}, 0U, 0U, 6U},
	{instruction_t::bclr, 0U, 6U, {}, 0U, 0U, 6U},
	{instruction_t::bclr, 0U, 7U, {}, 0U, 0U, 6U},
	{instruction_t::bclr, 0U, 0U, {}, 0U, 0U, 7U},
	{instruction_t::bclr, 0U, 1U, {}, 0U, 0U, 7U},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::bset, 0U, 0U, {}, 0U, 0U, 0U},
	{instruction_t::bset, 0U, 1U, {}, 0U, 0U, 0U},
	{instruction_t::bset, 0U, 2U, {}, 0U, 0U, 0U},
	{instruction_t::bset, 0U, 3U, {}, 0U, 0U, 0U},
	{instruction_t::bset, 0U, 4U, {}, 0U, 0U, 0U},
	{instruction_t::bset, 0U, 5U, {}, 0U, 0U, 0U},
	{instruction_t::bset, 0U, 6U, {}, 0U, 0U, 0U},
	{instruction_t::bset, 0U, 7U, {}, 0U, 0U, 0U},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::bset, 0U, 0U, {}, 0U, 0U, 2U},
	{instruction_t::bset, 0U, 1U, {}, 0U, 0U, 2U},
	{instruction_t::bset, 0U, 2U, {}, 0U, 0U, 2U},
	{instruction_t::bset, 0U, 3U, {}, 0U, 0U, 2U},
	{instruction_t::bset, 0U, 4U, {}, 0U, 0U, 2U},
	{instruction_t::bset, 0U, 5U, {}, 0U, 0U, 2U},
	{instruction_t::bset, 0U, 6U, {}, 0U, 0U, 2U},
	{instruction_t::bset, 0U, 7U, {}, 0U, 0U, 2U},
	{instruction_t::bset, 0U, 0U, {}, 0U, 0U, 3U},
	{instruction_t::bset, 0U, 1U, {}, 0U, 0U, 3U},
	{instruction_t::bset, 0U, 2U, {}, 0U, 0U, 3U},
	{instruction_t::bset, 0U, 3U, {}, 0U, 0U, 3U},
	{instruction_t::bset, 0U, 4U, {}, 0U, 0U, 3U},
	{instruction_t::bset, 0U, 5U, {}, 0U, 0U, 3U},
	{instruction_t::bset, 0U, 6U, {}, 0U, 0U, 3U},
	{instruction_t::bset, 0U, 7U, {}, 0U, 0U, 3U},
	{instruction_t::bset, 0U, 0U, {}, 0U, 0U, 4U},
	{instruction_t::bset, 0U, 1U, {}, 0U, 0U, 4U},
	{instruction_t::bset, 0U, 2U, {}, 0U, 0U, 4U},
	{instruction_t::bset, 0U, 3U, {}, 0U, 0U, 4U},
	{instruction_t::bset, 0U, 4U, {}, 0U, 0U, 4U},
	{instruction_t::bset, 0U, 5U, {}, 0U, 0U, 4U},
	{instruction_t::bset, 0U, 6U, {}, 0U, 0U, 4U},
	{instruction_t::bset, 0U, 7U, {}, 0U, 0U, 4U},
	{instruction_t::bset, 0U, 0U, {}, 0U, 0U, 5U},
	{instruction_t::bset, 0U, 1U, {}, 0U, 0U, 5U},
	{instruction_t::bset, 0U, 2U, {}, 0U, 0U, 5U},
	{instruction_t::bset, 0U, 3U, {}, 0U, 0U, 5U},
	{instruction_t::bset, 0U, 4U, {}, 0U, 0U, 5U},
	{instruction_t::bset, 0U, 5U, {}, 0U, 0U, 5U},
	{instruction_t::bset, 0U, 6U, {}, 0U, 0U, 5U},
	{instruction_t::bset, 0U, 7U, {}, 0U, 0U, 5U},
	{instruction_t::bset, 0U, 0U, {}, 0U, 0U, 6U},
	{instruction_t::bset, 0U, 1U, {}, 0U, 0U, 6U},
	{instruction_t::bset, 0U, 2U, {}, 0U, 0U, 6U},
	{instruction_t::bset, 0U, 3U, {}, 0U, 0U, 6U},
	{instruction_t::bset, 0U, 4U, {}, 0U, 0U, 6U},
	{instruction_t::bset, 0U, 5U, {}, 0U, 0U, 6U},
	{instruction_t::bset, 0U, 6U, {}, 0U, 0U, 6U},
	{instruction_t::bset, 0U, 7U, {}, 0U, 0U, 6U},
	{instruction_t::bset, 0U, 0U, {}, 0U, 0U, 7U},
	{instruction_t::bset, 0U, 1U, {}, 0U, 0U, 7U},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::andi, 0U, 0U, {}, 0U, 0U, 0U},
	{instruction_t::andi, 0U, 1U, {}, 0U, 0U, 0U},
	{instruction_t::andi, 0U, 2U, {}, 0U, 0U, 0U},
	{instruction_t::andi, 0U, 3U, {}, 0U, 0U, 0U},
	{instruction_t::andi, 0U, 4U, {}, 0U, 0U, 0U},
	{instruction_t::andi, 0U, 5U, {}, 0U, 0U, 0U},
	{instruction_t::andi, 0U, 6U, {}, 0U, 0U, 0U},
	{instruction_t::andi, 0U, 7U, {}, 0U, 0U, 0U},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::andi, 0U, 0U, {}, 0U, 0U, 2U},
	{instruction_t::andi, 0U, 1U, {}, 0U, 0U, 2U},
	{instruction_t::andi, 0U, 2U, {}, 0U, 0U, 2U},
	{instruction_t::andi, 0U, 3U, {}, 0U, 0U, 2U},
	{instruction_t::andi, 0U, 4U, {}, 0U, 0U, 2U},
	{instruction_t::andi, 0U, 5U, {}, 0U, 0U, 2U},
	{instruction_t::andi, 0U, 6U, {}, 0U, 0U, 2U},
	{instruction_t::andi, 0U, 7U, {}, 0U, 0U, 2U},
	{instruction_t::andi, 0U, 0U, {}, 0U, 0U, 3U},
	{instruction_t::andi, 0U, 1U, {}, 0U, 0U, 3U},
	{instruction_t::andi, 0U, 2U, {}, 0U, 0U, 3U},
	{instruction_t::andi, 0U, 3U, {}, 0U, 0U, 3U},
	{instruction_t::andi, 0U, 4U, {}, 0U, 0U, 3U},
	{instruction_t::andi, 0U, 5U, {}, 0U, 0U, 3U},
	{instruction_t::andi, 0U, 6U, {}, 0U, 0U, 3U},
	{instruction_t::andi, 0U, 7U, {}, 0U, 0U, 3U},
	{instruction_t::andi, 0U, 0U, {}, 0U, 0U, 4U},
	{instruction_t::andi, 0U, 1U, {}, 0U, 0U, 4U},
	{instruction_t::andi, 0U, 2U, {}, 0U, 0U, 4U},
	{instruction_t::andi, 0U, 3U, {}, 0U, 0U, 4U},
	{instruction_t::andi, 0U, 4U, {}, 0U, 0U, 4U},
	{instruction_t::andi, 0U, 5U, {}, 0U, 0U, 4U},
	{instruction_t::andi, 0U, 6U, {}, 0U, 0U, 4U},
	{instruction_t::andi, 0U, 7U, {}, 0U, 0U, 4U},
	{instruction_t::andi, 0U, 0U, {}, 0U, 0U, 5U},
	{instruction_t::andi, 0U, 1U, {}, 0U, 0U, 5U},
	{instruction_t::andi, 0U, 2U, {}, 0U, 0U, 5U},
	{instruction_t::andi, 0U, 3U, {}, 0U, 0U, 5U},
	{instruction_t::andi, 0U, 4U, {}, 0U, 0U, 5U},
	{instruction_t::andi, 0U, 5U, {}, 0U, 0U, 5U},
	{instruction_t::andi, 0U, 6U, {}, 0U, 0U, 5U},
	{instruction_t::andi, 0U, 7U, {}, 0U, 0U, 5U},
	{instruction_t::andi, 0U, 0U, {}, 0U, 0U, 6U},
	{instruction_t::andi, 0U, 1U, {}, 0U, 0U, 6U},
	{instruction_t::andi, 0U, 2U, {}, 0U, 0U, 6U},
	{instruction_t::andi, 0U, 3U, {}, 0U, 0U, 6U},
	{instruction_t::andi, 0U, 4U, {}, 0U, 0U, 6U},
	{instruction_t::andi, 0U, 5U, {}, 0U, 0U, 6U},
	{instruction_t::andi, 0U, 6U, {}, 0U, 0U, 6U},
	{instruction_t::andi, 0U, 7U, {}, 0U, 0U, 6U},
	{instruction_t::andi, 0U, 0U, {}, 0U, 0U, 7U},
	{instruction_t::andi, 0U, 1U, {}, 0U, 0U, 7U},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::andi, 0U, 8U, {}, 1U, 0U, 0U, 2U},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::andi, 0U, 0U, {}, 1U, 0U, 0U},
	{instruction_t::andi, 0U, 1U, {}, 1U, 0U, 0U},
	{instruction_t::andi, 0U, 2U, {}, 1U, 0U, 0U},
	{instruction_t::andi, 0U, 3U, {}, 1U, 0U, 0U},
	{instruction_t::andi, 0U, 4U, {}, 1U, 0U, 0U},
	{instruction_t::andi, 0U, 5U, {}, 1U, 0U, 0U},
	{instruction_t::andi, 0U, 6U, {}, 1U, 0U, 0U},
	{instruction_t::andi, 0U, 7U, {}, 1U, 0U, 0U},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::andi, 0U, 0U, {}, 1U, 0U, 2U},
	{instruction_t::andi, 0U, 1U, {}, 1U, 0U, 2U},
	{instruction_t::andi, 0U, 2U, {}, 1U, 0U, 2U},
	{instruction_t::andi, 0U, 3U, {}, 1U, 0U, 2U},
	{instruction_t::andi, 0U, 4U, {}, 1U, 0U, 2U},
	{instruction_t::andi, 0U, 5U, {}, 1U, 0U, 2U},
	{instruction_t::andi, 0U, 6U, {}, 1U, 0U, 2U},
	{instruction_t::andi, 0U, 7U, {}, 1U, 0U, 2U},
	{instruction_t::andi, 0U, 0U, {}, 1U, 0U, 3U},
	{instruction_t::andi, 0U, 1U, {}, 1U, 0U, 3U},
	{instruction_t::andi, 0U, 2U, {}, 1U, 0U, 3U},
	{instruction_t::andi, 0U, 3U, {}, 1U, 0U, 3U},
	{instruction_t::andi, 0U, 4U, {}, 1U, 0U, 3U},
	{instruction_t::andi, 0U, 5U, {}, 1U, 0U, 3U},
	{instruction_t::andi, 0U, 6U, {}, 1U, 0U, 3U},
	{instruction_t::andi, 0U, 7U, {}, 1U, 0U, 3U},
	{instruction_t::andi, 0U, 0U, {}, 1U, 0U, 4U},
	{instruction_t::andi, 0U, 1U, {}, 1U, 0U, 4U},
	{instruction_t::andi, 0U, 2U, {}, 1U, 0U, 4U},
	{instruction_t::andi, 0U, 3U, {}, 1U, 0U, 4U},
	{instruction_t::andi, 0U, 4U, {}, 1U, 0U, 4U},
	{instruction_t::andi, 0U, 5U, {}, 1U, 0U, 4U},
	{instruction_t::andi, 0U, 6U, {}, 1U, 0U, 4U},
	{instruction_t::andi, 0U, 7U, {}, 1U, 0U, 4U},
	{instruction_t::andi, 0U, 0U, {}, 1U, 0U, 5U},
	{instruction_t::andi, 0U, 1U, {}, 1U, 0U, 5U},
	{instruction_t::andi, 0U, 2U, {}, 1U, 0U, 5U},
	{instruction_t::andi, 0U, 3U, {}, 1U, 0U, 5U},
	{instruction_t::andi, 0U, 4U, {}, 1U, 0U, 5U},
	{instruction_t::andi, 0U, 5U, {}, 1U, 0U, 5U},
	{instruction_t::andi, 0U, 6U, {}, 1U, 0U, 5U},
	{instruction_t::andi, 0U, 7U, {}, 1U, 0U, 5U},
	{instruction_t::andi, 0U, 0U, {}, 1U, 0U, 6U},
	{instruction_t::andi, 0U, 1U, {}, 1U, 0U, 6U},
	{instruction_t::andi, 0U, 2U, {}, 1U, 0U, 6U},
	{instruction_t::andi, 0U, 3U, {}, 1U, 0U, 6U},
	{instruction_t::andi, 0U, 4U, {}, 1U, 0U, 6U},
	{instruction_t::andi, 0U, 5U, {}, 1U, 0U, 6U},
	{instruction_t::andi, 0U, 6U, {}, 1U, 0U, 6U},
	{instruction_t::andi, 0U, 7U, {}, 1U, 0U, 6U},
	{instruction_t::andi, 0U, 0U, {}, 1U, 0U, 7U},
	{instruction_t::andi, 0U, 1U, {}, 1U, 0U, 7U},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal}, // XXX: Should be ORI to SR
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::andi, 0U, 0U, {}, 2U, 0U, 0U},
	{instruction_t::andi, 0U, 1U, {}, 2U, 0U, 0U},
	{instruction_t::andi, 0U, 2U, {}, 2U, 0U, 0U},
	{instruction_t::andi, 0U, 3U, {}, 2U, 0U, 0U},
	{instruction_t::andi, 0U, 4U, {}, 2U, 0U, 0U},
	{instruction_t::andi, 0U, 5U, {}, 2U, 0U, 0U},
	{instruction_t::andi, 0U, 6U, {}, 2U, 0U, 0U},
	{instruction_t::andi, 0U, 7U, {}, 2U, 0U, 0U},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::andi, 0U, 0U, {}, 2U, 0U, 2U},
	{instruction_t::andi, 0U, 1U, {}, 2U, 0U, 2U},
	{instruction_t::andi, 0U, 2U, {}, 2U, 0U, 2U},
	{instruction_t::andi, 0U, 3U, {}, 2U, 0U, 2U},
	{instruction_t::andi, 0U, 4U, {}, 2U, 0U, 2U},
	{instruction_t::andi, 0U, 5U, {}, 2U, 0U, 2U},
	{instruction_t::andi, 0U, 6U, {}, 2U, 0U, 2U},
	{instruction_t::andi, 0U, 7U, {}, 2U, 0U, 2U},
	{instruction_t::andi, 0U, 0U, {}, 2U, 0U, 3U},
	{instruction_t::andi, 0U, 1U, {}, 2U, 0U, 3U},
	{instruction_t::andi, 0U, 2U, {}, 2U, 0U, 3U},
	{instruction_t::andi, 0U, 3U, {}, 2U, 0U, 3U},
	{instruction_t::andi, 0U, 4U, {}, 2U, 0U, 3U},
	{instruction_t::andi, 0U, 5U, {}, 2U, 0U, 3U},
	{instruction_t::andi, 0U, 6U, {}, 2U, 0U, 3U},
	{instruction_t::andi, 0U, 7U, {}, 2U, 0U, 3U},
	{instruction_t::andi, 0U, 0U, {}, 2U, 0U, 4U},
	{instruction_t::andi, 0U, 1U, {}, 2U, 0U, 4U},
	{instruction_t::andi, 0U, 2U, {}, 2U, 0U, 4U},
	{instruction_t::andi, 0U, 3U, {}, 2U, 0U, 4U},
	{instruction_t::andi, 0U, 4U, {}, 2U, 0U, 4U},
	{instruction_t::andi, 0U, 5U, {}, 2U, 0U, 4U},
	{instruction_t::andi, 0U, 6U, {}, 2U, 0U, 4U},
	{instruction_t::andi, 0U, 7U, {}, 2U, 0U, 4U},
	{instruction_t::andi, 0U, 0U, {}, 2U, 0U, 5U},
	{instruction_t::andi, 0U, 1U, {}, 2U, 0U, 5U},
	{instruction_t::andi, 0U, 2U, {}, 2U, 0U, 5U},
	{instruction_t::andi, 0U, 3U, {}, 2U, 0U, 5U},
	{instruction_t::andi, 0U, 4U, {}, 2U, 0U, 5U},
	{instruction_t::andi, 0U, 5U, {}, 2U, 0U, 5U},
	{instruction_t::andi, 0U, 6U, {}, 2U, 0U, 5U},
	{instruction_t::andi, 0U, 7U, {}, 2U, 0U, 5U},
	{instruction_t::andi, 0U, 0U, {}, 2U, 0U, 6U},
	{instruction_t::andi, 0U, 1U, {}, 2U, 0U, 6U},
	{instruction_t::andi, 0U, 2U, {}, 2U, 0U, 6U},
	{instruction_t::andi, 0U, 3U, {}, 2U, 0U, 6U},
	{instruction_t::andi, 0U, 4U, {}, 2U, 0U, 6U},
	{instruction_t::andi, 0U, 5U, {}, 2U, 0U, 6U},
	{instruction_t::andi, 0U, 6U, {}, 2U, 0U, 6U},
	{instruction_t::andi, 0U, 7U, {}, 2U, 0U, 6U},
	{instruction_t::andi, 0U, 0U, {}, 2U, 0U, 7U},
	{instruction_t::andi, 0U, 1U, {}, 2U, 0U, 7U},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
	{instruction_t::illegal},
}};

class testM68k final : public testsuite, memoryMap_t<uint32_t>
{
private:
	motorola68000_t cpu{*this, 8_MHz};

	void testDecode()
	{
		// Run through all 65536 possible instruction values and check they decode properly.
		for (const auto &[insn, decodedOperation] : substrate::indexedIterator_t{instructionMap})
		{
			if (insn == 704U)
				break;
			assertTrue(cpu.decodeInstruction(insn) == decodedOperation);
		}
	}

public:
	CRUNCH_VIS testM68k() noexcept : testsuite{}, memoryMap_t<uint32_t>{}
	{
		//addressMap[{0x000000, 0x080000}] = std::make_unique<ram_t<uint32_t, 512_KiB>>();
	}

	void registerTests() final
	{
		CXX_TEST(testDecode)
	}
};

CRUNCH_API void registerCXXTests() noexcept;
void registerCXXTests() noexcept
{
	registerTestClasses<testM68k>();
}
