// SPDX-License-Identifier: BSD-3-Clause
// SPDX-FileCopyrightText: 2025 Rachel Mant <git@dragonmux.network>
#include <cstdint>
#include <random>
#include <substrate/span>
#include "ym2149.hxx"

ym2149_t::ym2149_t(const uint32_t clockFreq) noexcept : clockedPeripheral_t<uint32_t>{clockFreq},
	rng{std::random_device{}()}, rngDistribution{0U, 1U}
{
	for (auto &channel : channels)
		channel.resetEdgeState(rng, rngDistribution);
}

void ym2149_t::readAddress(const uint32_t address, substrate::span<uint8_t> data) const noexcept
{
	// Only admit 8- and 16-bit reads
	if (data.size_bytes() > 2U)
		return;
	// We only respond to reads on address 0
	if (address != 0U)
		return;
	// Where to read from depends on the register selection, so..
	switch (selectedRegister)
	{
		// Current channel frequency rough/fine
		case 0U:
		case 1U:
		case 2U:
		case 3U:
		case 4U:
		case 5U:
		{
			// Turn the register selection into a channel number and rough adjustment indicator
			const auto channel{static_cast<size_t>(selectedRegister >> 1U)};
			const auto roughAdjustment{(selectedRegister & 1U) != 0U};
			data[0U] = channels[channel].readFrequency(roughAdjustment);
			break;
		}
		// Current noise frequency
		case 6U:
			data[0U] = noisePeriod;
			break;
		// Current mixer configuration
		case 7U:
			data[0U] = mixerConfig;
			break;
		// Current channel level
		case 8U:
		case 9U:
		case 10U:
		{
			// Turn the register selection into a channel number
			const size_t channel{selectedRegister - 8U};
			data[0U] = channels[channel].level;
			break;
		}
		// Envelope frequency fine adjustment
		case 11U:
			data[0U] = static_cast<uint8_t>(envelopePeriod);
			break;
		// Envelope frequency rough adjustment
		case 12U:
			data[0U] = static_cast<uint8_t>(envelopePeriod >> 8U);
			break;
		// Envelope shape adjustment
		case 13U:
			data[0U] = envelopeShape;
			break;
		// I/O port data
		case 14U:
		case 15U:
		{
			// Turn the register selection into a port number (repesenting A vs B)
			const auto port{selectedRegister & 1U};
			data[0U] = ioPort[port];
			break;
		}
	}
}

void ym2149_t::writeAddress(const uint32_t address, const substrate::span<uint8_t> &data) noexcept
{
	// Only admit 8- and 16-bit writes
	if (data.size_bytes() > 2U)
		return;
	// The address to write determines if we're making a register selection or writing a register
	switch (address)
	{
		case 0U: // Register selection
			// Only the bottom 4 bits of the selection are valid/used
			selectedRegister = data[0U] & 0x0fU;
			break;
		case 2U: // Register write
			// Where we write depends on the register selection, so..
			switch (selectedRegister)
			{
				// Channel frequency rough/fine adjustment
				case 0U:
				case 1U:
				case 2U:
				case 3U:
				case 4U:
				case 5U:
				{
					// Turn the register selection into a channel number and rough adjustment indicator
					const auto channel{static_cast<size_t>(selectedRegister >> 1U)};
					const auto roughAdjustment{(selectedRegister & 1U) != 0U};
					// Select the channel and write the adjustment
					channels[channel].writeFrequency(data[0U], roughAdjustment);
					break;
				}
				// Noise frequency adjustment
				case 6U:
					// Register is actually only 5 bit, so discard the upper 3
					noisePeriod = data[0U] & 0x1fU;
					break;
				// Mixer configuration adjustment
				case 7U:
					mixerConfig = data[0U];
					break;
				// Channel level adjustment
				case 8U:
				case 9U:
				case 10U:
				{
					// Turn the register selection into a channel number
					const size_t channel{selectedRegister - 8U};
					// Adjust that channel's levels (only 5 bits valid, discard the upper 3)
					channels[channel].level = data[0U] & 0x1fU;
					break;
				}
				// Envelope frequency fine adjustment
				case 11U:
					envelopePeriod &= 0xff00U;
					envelopePeriod |= data[0U];
					break;
				// Envelope frequency rough adjustment
				case 12U:
					envelopePeriod &= 0x00ffU;
					envelopePeriod |= data[0U] << 8U;
					break;
				// Envelope shape adjustment
				case 13U:
					envelopeShape = data[0U] & 0x0fU;
					break;
				// I/O port data
				case 14U:
				case 15U:
				{
					// Turn the register selection into a port number (repesenting A vs B)
					const auto port{selectedRegister & 1U};
					ioPort[port] = data[0U];
					break;
				}
			}
			break;
	}
}

bool ym2149_t::clockCycle() noexcept
{
	// If we should update the FSM in this cycle, do so
	if (cyclesTillUpdate == 0U)
		updateFSM();
	// The internal state machine runs at 1/8th the device clock speed
	// (presume that ~SEL is low, yielding f(T) = (f(Master) / 2) / (16 * TP)
	// which simplifies to f(T) = f(Master) / (8 * TP))
	cyclesTillUpdate = (cyclesTillUpdate + 1U) & 7U;
	return true;
}

void ym2149_t::updateFSM() noexcept
{
	// Step all the channels forward one internal cycle
	for (auto &channel : channels)
		channel.step();

	// Step the envelope generator forward one internal cycle
	// If the envelope counter exceeds the period of the envelope
	if (++envelopeCounter >= envelopePeriod)
	{
		// Reset the counter
		envelopeCounter = 0U;
		// Update the envelope position (64 possible positions)
		envelopePosition = (envelopePosition + 1U) & 0x3fU;
	}

	// Step the noise generator forward one internal cycle, noting it runs
	// at half speed to everything else. If the noise counter exceeds the
	// period of the noise to be generated
	if ((++noiseCounter) >= noisePeriod)
	{
		// Reset the counter
		noiseCounter = 0U;
		// Set the noise state if the 0th and 2nd bits of the noise LFSR are the same value as each other
		noiseState = (noiseLFSR ^ (noiseLFSR >> 2U)) & 1U;
		// Shift the new noise state in as single bit on the LHS of the noise LFSR
		noiseLFSR >>= 1U;
		noiseLFSR |= noiseState ? (1U << 16U) : 0U;
	}
}

bool ym2149_t::sampleReady() const noexcept
{
	return true;
}

namespace ym2149
{
	void channel_t::resetEdgeState(std::minstd_rand &rng, std::uniform_int_distribution<uint8_t> &dist) noexcept
		{ edgeState = static_cast<bool>(dist(rng)); }

	void channel_t::writeFrequency(const uint8_t value, const bool roughAdjust) noexcept
	{
		if (roughAdjust)
		{
			period &= 0x00ff0U;
			// Only 4 bits valid, throw away the upper 4
			period |= (value & 0x0fU) << 8U;
		}
		else
		{
			period &= 0xff00U;
			period |= value;
		}
	}

	uint8_t channel_t::readFrequency(const bool roughAdjust) const noexcept
	{
		if (roughAdjust)
			return static_cast<uint8_t>(period >> 8U);
		else
			return static_cast<uint8_t>(period);
	}

	void channel_t::step() noexcept
	{
		// If our channel's period counter exceeds the period of the current tone frequency
		if (++counter >= period)
		{
			// Reset the counter and update the edge states
			counter = 0U;
			edgeState = !edgeState;
		}
	}
} // namespace ym2149
