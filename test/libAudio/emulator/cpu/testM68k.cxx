// SPDX-License-Identifier: BSD-3-Clause
// SPDX-FileCopyrightText: 2025 Rachel Mant <git@dragonmux.network>
#include <array>
#include <memory>
#include <substrate/indexed_iterator>
#include <substrate/index_sequence>
#include <crunch++.h>
#include "emulator/cpu/m68k.hxx"
#include "emulator/ram.hxx"
#include "emulator/unitsHelpers.hxx"
#include "testM68kDecodeTable.hxx"

class testM68k final : public testsuite, memoryMap_t<uint32_t, 0x00ffffffU>
{
private:
	motorola68000_t cpu{*this, 8_MHz};

	void testDecode()
	{
		// Run through all 65536 possible instruction values and check they decode properly.
		for (const auto &[insn, decodedOperation] : substrate::indexedIterator_t{instructionMap})
			assertTrue(cpu.decodeInstruction(insn) == decodedOperation);
	}

	void runStep(const bool expectingTrap = false)
	{
		const auto result{cpu.step()};
		assertTrue(result.validInsn);
		if (expectingTrap)
			assertTrue(result.trap);
		else
			assertFalse(result.trap);
		// TODO: Check the cycles taken matches expectations
	}

	void testBranch()
	{
		// Set up 3 branches in each of the 3 instruction encoding forms, checking they jump properly to each other
		// Start with the 8-bit immediate form, then use the 16-bit immediate form, and finally the 32-bit immediate
		// form (6 bytes to encode)
		writeAddress(0x000100U, uint16_t{0x6050U}); // Jump to +0x50 from end of instruction
		writeAddress(0x000152U, uint16_t{0x6000U});
		writeAddress(0x000154U, uint16_t{0xfefcU}); // Jump to -0x104 from end of instruction
		writeAddress(0x000050U, uint16_t{0x60ffU});
		writeAddress(0x000052U, uint32_t{0x000100aeU}); // Jump to +0x100ae from end of instruction
		writeAddress(0x010100U, uint16_t{0x4e75U}); // rts to end the test
		// Set the CPU to execute this sequence
		cpu.executeFrom(0x00000100U, 0x00800000U);
		// Validate starting conditions
		assertEqual(cpu.readProgramCounter(), 0x00000100U);
		assertEqual(cpu.readAddrRegister(7U), 0x007ffffcU);
		assertEqual(cpu.readStatus(), 0x0000U);
		// Step the first instruction and validate
		runStep();
		assertEqual(cpu.readProgramCounter(), 0x00000152U);
		assertEqual(cpu.readAddrRegister(7U), 0x007ffffcU);
		assertEqual(cpu.readStatus(), 0x0000U);
		// Step the second instruction and validate
		runStep();
		assertEqual(cpu.readProgramCounter(), 0x00000050U);
		assertEqual(cpu.readAddrRegister(7U), 0x007ffffcU);
		assertEqual(cpu.readStatus(), 0x0000U);
		// Step the third instruction and validate
		runStep();
		assertEqual(cpu.readProgramCounter(), 0x00010100U);
		assertEqual(cpu.readAddrRegister(7U), 0x007ffffcU);
		assertEqual(cpu.readStatus(), 0x0000U);
		// Step the fourth and final instruction to complete the test
		runStep();
		assertEqual(cpu.readProgramCounter(), 0xffffffffU);
		assertEqual(cpu.readAddrRegister(7U), 0x00800000U);
		assertEqual(cpu.readStatus(), 0x0000U);
	}

	void testBSR()
	{
		// Set up 3 branches in each of the 3 instruction encoding forms, checking they jump properly and then return
		// Start with the 8-bit immediate form, then use the 16-bit immediate form, and finally the 32-bit immediate
		// form (6 bytes to encode)
		writeAddress(0x000200U, uint16_t{0x6150U}); // Jump to +0x50 from end of instruction
		writeAddress(0x000202U, uint16_t{0x6100U});
		writeAddress(0x000204U, uint16_t{0xfefcU}); // Jump to -0x104 from end of instruction u16
		writeAddress(0x000206U, uint16_t{0x61ffU});
		writeAddress(0x000208U, uint32_t{0x000100aeU}); // Jump to +0x100ae from end of instruction u16
		writeAddress(0x00020cU, uint16_t{0x4e75U}); // rts to end the test
		// RTS's for each of the BSRs to hit
		writeAddress(0x000252U, uint16_t{0x4e75U});
		writeAddress(0x000100U, uint16_t{0x4e75U});
		writeAddress(0x0102b6U, uint16_t{0x4e75U});
		// Set the CPU to execute this sequence
		cpu.executeFrom(0x00000200U, 0x00800000U);
		// Validate starting conditions
		assertEqual(cpu.readProgramCounter(), 0x00000200U);
		assertEqual(cpu.readAddrRegister(7U), 0x007ffffcU);
		assertEqual(cpu.readStatus(), 0x0000U);
		// Step the first instruction and validate
		runStep();
		assertEqual(cpu.readProgramCounter(), 0x00000252U);
		assertEqual(cpu.readAddrRegister(7U), 0x007ffff8U);
		assertEqual(cpu.readStatus(), 0x0000U);
		// Step the RTS for that and validate
		runStep();
		assertEqual(cpu.readProgramCounter(), 0x00000202U);
		assertEqual(cpu.readAddrRegister(7U), 0x007ffffcU);
		assertEqual(cpu.readStatus(), 0x0000U);
		// Step the second instruction and validate
		runStep();
		assertEqual(cpu.readProgramCounter(), 0x00000100U);
		assertEqual(cpu.readAddrRegister(7U), 0x007ffff8U);
		assertEqual(cpu.readStatus(), 0x0000U);
		// Step the RTS for that and validate
		runStep();
		assertEqual(cpu.readProgramCounter(), 0x00000206U);
		assertEqual(cpu.readAddrRegister(7U), 0x007ffffcU);
		assertEqual(cpu.readStatus(), 0x0000U);
		// Step the third instruction and validate
		runStep();
		assertEqual(cpu.readProgramCounter(), 0x000102b6U);
		assertEqual(cpu.readAddrRegister(7U), 0x007ffff8U);
		assertEqual(cpu.readStatus(), 0x0000U);
		// Step the RTS for that and validate
		runStep();
		assertEqual(cpu.readProgramCounter(), 0x0000020cU);
		assertEqual(cpu.readAddrRegister(7U), 0x007ffffcU);
		assertEqual(cpu.readStatus(), 0x0000U);
		// Step the fourth and final instruction to complete the test
		runStep();
		assertEqual(cpu.readProgramCounter(), 0xffffffffU);
		assertEqual(cpu.readAddrRegister(7U), 0x00800000U);
		assertEqual(cpu.readStatus(), 0x0000U);
	}

	void testJump()
	{
		// NB: We don't test all EA's here as we get through them in other tests.
		writeAddress(0x000000U, uint16_t{0x4ed0U}); // jmp (a0)
		writeAddress(0x000004U, uint16_t{0x4ee8U});
		writeAddress(0x000006U, uint16_t{0x0100U}); // jmp $0100(a0)
		writeAddress(0x000104U, uint16_t{0x4ef8U});
		writeAddress(0x000106U, uint16_t{0x0110U}); // jmp $0110.w
		writeAddress(0x000110U, uint16_t{0x4ef9U});
		writeAddress(0x000112U, uint16_t{0x0001U});
		writeAddress(0x000114U, uint16_t{0x0000U}); // jmp $00010000.l
		writeAddress(0x010000U, uint16_t{0x4e75U}); // rts to end the test
		// Set the CPU to execute this sequence
		cpu.executeFrom(0x00000000U, 0x00800000U);
		cpu.writeAddrRegister(0U, 0x00000004U);
		// Validate starting conditions
		assertEqual(cpu.readProgramCounter(), 0x00000000U);
		assertEqual(cpu.readAddrRegister(0U), 0x00000004U);
		assertEqual(cpu.readAddrRegister(7U), 0x007ffffcU);
		assertEqual(cpu.readStatus(), 0x0000U);
		// Step the first instruction and validate
		runStep();
		assertEqual(cpu.readProgramCounter(), 0x00000004U);
		assertEqual(cpu.readAddrRegister(7U), 0x007ffffcU);
		assertEqual(cpu.readStatus(), 0x0000U);
		// Step the second instruction and validate
		runStep();
		assertEqual(cpu.readProgramCounter(), 0x00000104U);
		assertEqual(cpu.readAddrRegister(7U), 0x007ffffcU);
		assertEqual(cpu.readStatus(), 0x0000U);
		// Step the third instruction and validate
		runStep();
		assertEqual(cpu.readProgramCounter(), 0x00000110U);
		assertEqual(cpu.readAddrRegister(7U), 0x007ffffcU);
		assertEqual(cpu.readStatus(), 0x0000U);
		// Step the fourth instruction and validate
		runStep();
		assertEqual(cpu.readProgramCounter(), 0x00010000U);
		assertEqual(cpu.readAddrRegister(7U), 0x007ffffcU);
		assertEqual(cpu.readStatus(), 0x0000U);
		// Step the fifth and final instruction to complete the test
		runStep();
		assertEqual(cpu.readProgramCounter(), 0xffffffffU);
		assertEqual(cpu.readAddrRegister(7U), 0x00800000U);
		assertEqual(cpu.readStatus(), 0x0000U);
	}

	void testRTS()
	{
		writeAddress(0x000000U, uint16_t{0x4e75U}); // rts
		// Run the solitary rts
		assertTrue(cpu.executeToReturn(0x00000000U, 0x00800000U, false));
		// Then make sure the CPU state matches up
		assertEqual(cpu.readProgramCounter(), 0xffffffffU);
		assertEqual(cpu.readAddrRegister(7U), 0x00800000U);
		assertEqual(cpu.readStatus(), 0x2000U);
	}

	void testRTE()
	{
		writeAddress(0x000100U, uint16_t{0x4e73U}); // rte
		// Set up to execute an exception vector
		cpu.executeFromException(0x00000100U, 0x00800000U, 1U);
		// Validate starting conditions
		assertEqual(cpu.readProgramCounter(), 0x00000100U);
		assertEqual(cpu.readAddrRegister(7U), 0x007ffff8U);
		assertEqual(cpu.readStatus(), 0x2000U);
		// Check that the stacked exception frame is okay
		assertEqual(readAddress<uint16_t>(0x007ffffeU), uint16_t{0x1004U}); // Frame type and vector info
		assertEqual(readAddress<uint32_t>(0x007ffffaU), uint32_t{0xffffffffU}); // Return program counter
		assertEqual(readAddress<uint16_t>(0x007ffff8U), uint16_t{0x0000U}); // Return status register
		// Step the CPU and validate the results line up
		runStep();
		assertEqual(cpu.readProgramCounter(), 0xffffffffU);
		assertEqual(cpu.readAddrRegister(7U), 0x00800000U);
		assertEqual(cpu.readStatus(), 0x0000U);
	}

	void testADD()
	{
		writeAddress(0x000000U, uint16_t{0xd070U});
		writeAddress(0x000002U, uint16_t{0x1204U}); // add.w 4(a0, d1.w*2), d0
		writeAddress(0x000004U, uint16_t{0xd030U});
		writeAddress(0x000006U, uint16_t{0x8800U}); // add.b (a0, a0.l), d0
		writeAddress(0x000008U, uint16_t{0xd088U}); // add.l a0, d0
		writeAddress(0x00000aU, uint16_t{0xd590U}); // add.l d2, (a0)
		writeAddress(0x00000cU, uint16_t{0x4e75U}); // rts to end the test
		writeAddress(0x000100U, uint32_t{0U});
		writeAddress(0x000108U, uint16_t{0x8080U});
		writeAddress(0x000200U, uint8_t{0xc1U});
		// Set the CPU to execute this sequence
		cpu.executeFrom(0x00000000U, 0x00800000U);
		// Set up a0, d0, d1 and d2 to sensible values
		cpu.writeAddrRegister(0U, 0x00000100U);
		cpu.writeDataRegister(0U, 0x40000004U);
		cpu.writeDataRegister(1U, 0x00000002U);
		cpu.writeDataRegister(2U, 0x00000000U);
		// Set the status register to some improbable value that makes it easy to see if it changes
		cpu.writeStatus(0x001fU);
		// Validate starting conditions
		assertEqual(cpu.readProgramCounter(), 0x00000000U);
		assertEqual(cpu.readAddrRegister(7U), 0x007ffffcU);
		// Step the first instruction and validate
		runStep();
		assertEqual(cpu.readProgramCounter(), 0x00000004U);
		assertEqual(cpu.readDataRegister(0U), 0x40008084U);
		assertEqual(cpu.readStatus(), 0x0008U);
		// Step the second instruction and validate
		runStep();
		assertEqual(cpu.readProgramCounter(), 0x00000008U);
		assertEqual(cpu.readDataRegister(0U), 0x40008045U);
		assertEqual(cpu.readStatus(), 0x0013U);
		// Step the third instruction and validate
		runStep();
		assertEqual(cpu.readProgramCounter(), 0x0000000aU);
		assertEqual(cpu.readDataRegister(0U), 0x40008145U);
		assertEqual(cpu.readStatus(), 0x0000U);
		// Step the fourth instruction and validate
		runStep();
		assertEqual(cpu.readProgramCounter(), 0x0000000cU);
		assertEqual(readAddress<uint32_t>(0x00000100U), 0U);
		assertEqual(cpu.readStatus(), 0x0004U);
		// Step the final instruction to complete the test
		runStep();
		assertEqual(cpu.readProgramCounter(), 0xffffffffU);
		assertEqual(cpu.readAddrRegister(7U), 0x00800000U);
	}

	void testADDA()
	{
		writeAddress(0x000000U, uint16_t{0xd0e1U}); // adda.w -(a1), a0
		writeAddress(0x000002U, uint16_t{0xd3c0U}); // adda.l d0, a1
		writeAddress(0x000004U, uint16_t{0x4e75U}); // rts to end the test
		writeAddress(0x000100U, uint16_t{0xc100U});
		// Set the CPU to execute this sequence
		cpu.executeFrom(0x00000000U, 0x00800000U);
		// Set up a0, a1 and d0 to sensible values
		cpu.writeAddrRegister(0U, 0x00000010U);
		cpu.writeAddrRegister(1U, 0x00000102U);
		cpu.writeDataRegister(0U, 0x40000004U);
		// Set the status register to some improbable value that makes it easy to see if it changes
		cpu.writeStatus(0x001fU);
		// Validate starting conditions
		assertEqual(cpu.readProgramCounter(), 0x00000000U);
		assertEqual(cpu.readAddrRegister(7U), 0x007ffffcU);
		// Step the first instruction and validate
		runStep();
		assertEqual(cpu.readProgramCounter(), 0x00000002U);
		assertEqual(cpu.readAddrRegister(0U), 0xffffc110U);
		assertEqual(cpu.readAddrRegister(1U), 0x00000100U);
		assertEqual(cpu.readStatus(), 0x001fU);
		// Step the second instruction and validate
		runStep();
		assertEqual(cpu.readProgramCounter(), 0x00000004U);
		assertEqual(cpu.readAddrRegister(1U), 0x40000104U);
		assertEqual(cpu.readStatus(), 0x001fU);
		// Step the final instruction to complete the test
		runStep();
		assertEqual(cpu.readProgramCounter(), 0xffffffffU);
		assertEqual(cpu.readAddrRegister(7U), 0x00800000U);
	}

	void testADDI()
	{
		writeAddress(0x000000U, uint16_t{0x0618U});
		writeAddress(0x000002U, uint16_t{0x0004U}); // addi.b #4, (a0)+
		writeAddress(0x000004U, uint16_t{0x0658U});
		writeAddress(0x000006U, uint16_t{0x8001U}); // addi.w #$8001, (a0)+
		writeAddress(0x000008U, uint16_t{0x0698U});
		writeAddress(0x00000aU, uint32_t{0xfeed4ca7U}); // addi.l #$feed4ca7, (a0)+
		writeAddress(0x00000eU, uint16_t{0x0628U});
		writeAddress(0x000010U, uint16_t{0x00bcU});
		writeAddress(0x000012U, uint16_t{0xfff9U}); // addi.b #$bc, -7(a0)
		writeAddress(0x000014U, uint16_t{0x4e75U}); // rts to end the test
		writeAddress(0x000100U, uint8_t{0x40U});
		writeAddress(0x000101U, uint16_t{0x7ffeU});
		writeAddress(0x000103U, uint32_t{0x80000001U});
		// Set the CPU to execute this sequence
		cpu.executeFrom(0x00000000U, 0x00800000U);
		// Set up a0 to point to the data pool at +0x0100U
		cpu.writeAddrRegister(0U, 0x00000100U);
		// Set the status register to some improbable value that makes it easy to see if it changes
		cpu.writeStatus(0x001fU);
		// Validate starting conditions
		assertEqual(cpu.readProgramCounter(), 0x00000000U);
		assertEqual(cpu.readAddrRegister(7U), 0x007ffffcU);
		// Step the first instruction and validate
		runStep();
		assertEqual(cpu.readProgramCounter(), 0x00000004U);
		assertEqual(cpu.readAddrRegister(0U), 0x00000101U);
		assertEqual(readAddress<uint8_t>(0x00000100U), 0x44U);
		assertEqual(cpu.readStatus(), 0x0000U);
		// Step the second instruction and validate
		runStep();
		assertEqual(cpu.readProgramCounter(), 0x00000008U);
		assertEqual(cpu.readAddrRegister(0U), 0x00000103U);
		assertEqual(readAddress<uint16_t>(0x00000101U), 0xffffU);
		assertEqual(cpu.readStatus(), 0x0008U);
		// Step the third instruction and validate
		runStep();
		assertEqual(cpu.readProgramCounter(), 0x0000000eU);
		assertEqual(cpu.readAddrRegister(0U), 0x00000107U);
		assertEqual(readAddress<uint32_t>(0x00000103U), 0x7eed4ca8U);
		assertEqual(cpu.readStatus(), 0x0013U);
		// Step the fourth instruction and validate
		runStep();
		assertEqual(cpu.readProgramCounter(), 0x00000014U);
		assertEqual(readAddress<uint8_t>(0x00000100U), 0x00U);
		assertEqual(cpu.readStatus(), 0x0015U);
		// Step the final instruction to complete the test
		runStep();
		assertEqual(cpu.readProgramCounter(), 0xffffffffU);
		assertEqual(cpu.readAddrRegister(7U), 0x00800000U);
	}

	void testADDQ()
	{
		writeAddress(0x000000U, uint16_t{0x5a04U}); // addq.b #5, d4
		writeAddress(0x000002U, uint16_t{0x5848U}); // addq.w #4, a0
		writeAddress(0x000004U, uint16_t{0x5083U}); // addq.l #8, d3
		writeAddress(0x000006U, uint16_t{0x4e75U}); // rts to end the test
		// Set the CPU to execute this sequence
		cpu.executeFrom(0x00000000U, 0x00800000U);
		// Set up a0, d3 and d4 to sensible values
		cpu.writeAddrRegister(0U, 0x000000fcU);
		cpu.writeDataRegister(3U, 0x7ffffff8U);
		cpu.writeDataRegister(4U, 0x000000ffU);
		// Set the status register to some improbable value that makes it easy to see if it changes
		cpu.writeStatus(0x001fU);
		// Validate starting conditions
		assertEqual(cpu.readProgramCounter(), 0x00000000U);
		assertEqual(cpu.readAddrRegister(7U), 0x007ffffcU);
		// Step the first instruction and validate
		runStep();
		assertEqual(cpu.readProgramCounter(), 0x00000002U);
		assertEqual(cpu.readDataRegister(4U), 0x00000004U);
		assertEqual(cpu.readStatus(), 0x0011U);
		// Step the second instruction and validate
		runStep();
		assertEqual(cpu.readProgramCounter(), 0x00000004U);
		assertEqual(cpu.readAddrRegister(0U), 0x00000100U);
		// As this is done to an address register, the status flags should not change
		assertEqual(cpu.readStatus(), 0x0011U);
		// Step the third instruction and validate
		runStep();
		assertEqual(cpu.readProgramCounter(), 0x00000006U);
		assertEqual(cpu.readDataRegister(3U), 0x80000000U);
		assertEqual(cpu.readStatus(), 0x000aU);
		// Step the final instruction to complete the test
		runStep();
		assertEqual(cpu.readProgramCounter(), 0xffffffffU);
		assertEqual(cpu.readAddrRegister(7U), 0x00800000U);
	}

	void testANDI()
	{
		writeAddress(0x000000U, uint16_t{0x0238U});
		writeAddress(0x000002U, uint16_t{0x0024U});
		writeAddress(0x000004U, uint16_t{0x0100U}); // andi.b #$24, $0100.w
		writeAddress(0x000006U, uint16_t{0x0258U});
		writeAddress(0x000008U, uint16_t{0x0000U}); // andi.w #0, (a0)+
		writeAddress(0x00000aU, uint16_t{0x0281U});
		writeAddress(0x00000cU, uint16_t{0x8421U});
		writeAddress(0x00000eU, uint16_t{0x1248U}); // andi.l #$84211248, d1
		writeAddress(0x000010U, uint16_t{0x4e75U}); // rts to end the test
		writeAddress(0x000100U, uint8_t{0x29U});
		// Set the CPU to execute this sequence
		cpu.executeFrom(0x00000000U, 0x00800000U);
		// Set up a0 and d1 to sensible values
		cpu.writeAddrRegister(0U, 0x00000100U);
		cpu.writeDataRegister(1U, 0xfeedaca7U);
		// Set the status register to some improbable value that makes it easy to see if it changes
		cpu.writeStatus(0x001fU);
		// Validate starting conditions
		assertEqual(cpu.readProgramCounter(), 0x00000000U);
		assertEqual(cpu.readAddrRegister(7U), 0x007ffffcU);
		// Step the first instruction and validate
		runStep();
		assertEqual(cpu.readProgramCounter(), 0x00000006U);
		assertEqual(readAddress<uint8_t>(0x000100U), uint8_t{0x20U});
		assertEqual(cpu.readStatus(), 0x0010U);
		// Reset the status register to a different improbable value that makes it easy to see if it changes
		cpu.writeStatus(0x000fU);
		// Step the second instruction and validate
		runStep();
		assertEqual(cpu.readProgramCounter(), 0x0000000aU);
		assertEqual(cpu.readAddrRegister(0U), 0x00000102U);
		assertEqual(readAddress<uint16_t>(0x000100U), uint16_t{0U});
		assertEqual(cpu.readStatus(), 0x0004U);
		// Step the third instruction and validate
		runStep();
		assertEqual(cpu.readProgramCounter(), 0x00000010U);
		assertEqual(cpu.readDataRegister(1U), 0x84210000U);
		assertEqual(cpu.readStatus(), 0x0008U);
		// Step the final instruction to complete the test
		runStep();
		assertEqual(cpu.readProgramCounter(), 0xffffffffU);
		assertEqual(cpu.readAddrRegister(7U), 0x00800000U);
	}

	void testCLR()
	{
		writeAddress(0x000000U, uint16_t{0x4200U}); // clr.b d0
		writeAddress(0x000002U, uint16_t{0x4240U}); // clr.w d0
		writeAddress(0x000004U, uint16_t{0x4280U}); // clr.l d0
		writeAddress(0x000006U, uint16_t{0x4e75U}); // rts to end the test
		// Set the CPU to execute this sequence
		cpu.executeFrom(0x00000000U, 0x00800000U);
		// Set up d0 to a sensible value
		cpu.writeDataRegister(0U, 0xfeedaca7U);
		// Set the status register to some improbable value that makes it easy to see if it changes
		cpu.writeStatus(0x001bU);
		// Validate starting conditions
		assertEqual(cpu.readProgramCounter(), 0x00000000U);
		assertEqual(cpu.readAddrRegister(7U), 0x007ffffcU);
		// Step the first instruction and validate
		runStep();
		assertEqual(cpu.readProgramCounter(), 0x00000002U);
		assertEqual(cpu.readDataRegister(0U), 0xfeedac00U);
		assertEqual(cpu.readStatus(), 0x0014U);
		// Reset the status register to a different improbable value that makes it easy to see if it changes
		cpu.writeStatus(0x000fU);
		// Step the second instruction and validate
		runStep();
		assertEqual(cpu.readProgramCounter(), 0x00000004U);
		assertEqual(cpu.readDataRegister(0U), 0xfeed0000U);
		assertEqual(cpu.readStatus(), 0x0004U);
		// Step the third instruction and validate
		runStep();
		assertEqual(cpu.readProgramCounter(), 0x00000006U);
		assertEqual(cpu.readDataRegister(0U), 0x00000000U);
		assertEqual(cpu.readStatus(), 0x0004U);
		// Step the final instruction to complete the test
		runStep();
		assertEqual(cpu.readProgramCounter(), 0xffffffffU);
		assertEqual(cpu.readAddrRegister(7U), 0x00800000U);
	}

	void testLEA()
	{
		writeAddress(0x000000U, uint16_t{0x43d0U}); // lea (a0), a1
		writeAddress(0x000002U, uint16_t{0x43e8U});
		writeAddress(0x000004U, uint16_t{0xfeedU}); // lea $feed(a0), a1
		writeAddress(0x000006U, uint16_t{0x43f0U});
		writeAddress(0x000008U, uint16_t{0x0c10U}); // lea $10(a0, d0.l*4), a1
		writeAddress(0x00000aU, uint16_t{0x43f8U});
		writeAddress(0x00000cU, uint16_t{0xca15U}); // lea ($ca15).w, a1
		writeAddress(0x00000eU, uint16_t{0x43f9U});
		writeAddress(0x000010U, uint32_t{0x0badf00dU}); // lea ($0badf00d).l, a1
		writeAddress(0x000014U, uint16_t{0x43faU});
		writeAddress(0x000016U, uint16_t{0xfffaU}); // lea -6(pc), a1
		writeAddress(0x000018U, uint16_t{0x43fbU});
		writeAddress(0x00001aU, uint16_t{0x0727U});
		writeAddress(0x00001cU, uint16_t{0x0026U});
		writeAddress(0x00001eU, uint32_t{0xf00daca1U}); // lea $f00daca1($26[pc], d0.w*8), a1
		writeAddress(0x000022U, uint16_t{0x4e75U}); // rts to end the test
		writeAddress(0x000040U, uint32_t{0xdeadbeefU});
		// Set the CPU to execute this sequence
		cpu.executeFrom(0x00000000U, 0x00800000U);
		// Set up a0 and d0 to sensible values
		cpu.writeAddrRegister(0U, 0x00000100U);
		cpu.writeDataRegister(0U, 0x00000200U);
		// Set the status register to some improbable value that makes it easy to see if it changes
		cpu.writeStatus(0x001fU);
		// Step the first instruction and validate
		runStep();
		assertEqual(cpu.readProgramCounter(), 0x00000002U);
		assertEqual(cpu.readAddrRegister(1U), 0x00000100U);
		assertEqual(cpu.readStatus(), 0x001fU);
		// Step the second instruction and validate
		runStep();
		assertEqual(cpu.readProgramCounter(), 0x00000006U);
		assertEqual(cpu.readAddrRegister(1U), 0xffffffedU);
		assertEqual(cpu.readStatus(), 0x001fU);
		// Step the third instruction and validate
		runStep();
		assertEqual(cpu.readProgramCounter(), 0x0000000aU);
		assertEqual(cpu.readAddrRegister(1U), 0x00000910U);
		assertEqual(cpu.readStatus(), 0x001fU);
		// Step the fourth instruction and validate
		runStep();
		assertEqual(cpu.readProgramCounter(), 0x0000000eU);
		assertEqual(cpu.readAddrRegister(1U), 0xffffca15U);
		assertEqual(cpu.readStatus(), 0x001fU);
		// Step the fifth instruction and validate
		runStep();
		assertEqual(cpu.readProgramCounter(), 0x00000014U);
		assertEqual(cpu.readAddrRegister(1U), 0x0badf00dU);
		assertEqual(cpu.readStatus(), 0x001fU);
		// Step the sixth instruction and validate
		runStep();
		assertEqual(cpu.readProgramCounter(), 0x00000018U);
		assertEqual(cpu.readAddrRegister(1U), 0x00000010U);
		assertEqual(cpu.readStatus(), 0x001fU);
		// Step the seventh instruction and validate
		runStep();
		assertEqual(cpu.readProgramCounter(), 0x00000022U);
		// 0xdeadbeefU + 0x00000200U * 8U + 0xf00daca1U
		assertEqual(cpu.readAddrRegister(1U), 0xcebb7b90U);
		assertEqual(cpu.readStatus(), 0x001fU);
		// Step the final instruction to complete the test
		runStep();
		assertEqual(cpu.readProgramCounter(), 0xffffffffU);
		assertEqual(cpu.readAddrRegister(7U), 0x00800000U);
	}

	void testOR()
	{
		writeAddress(0x000000U, uint16_t{0x8080U}); // or.l d0, d0
		writeAddress(0x000002U, uint16_t{0x8050U}); // or.w (a0), d0
		writeAddress(0x000004U, uint16_t{0x803cU});
		writeAddress(0x000006U, uint16_t{0x0040U}); // or.b #$40, d0
		writeAddress(0x000008U, uint16_t{0x8199U}); // or.l d0, (a1)+
		writeAddress(0x00000aU, uint16_t{0x4e75U}); // rts to end the test
		writeAddress(0x000100U, uint32_t{0x84000000U});
		// Set the CPU to execute this sequence
		cpu.executeFrom(0x00000000U, 0x00800000U);
		// Re-use the 3rd instruction as the data for the 2nd, and set a reasonable destination for the 4th
		cpu.writeAddrRegister(0U, 0x00000004U);
		cpu.writeAddrRegister(1U, 0x00000100U);
		// Make sure d0 starts out as 0 for this sequence
		cpu.writeDataRegister(0U, 0U);
		// Set the carry and overflow bits in the status register so we can observe them being cleared
		// And the extend bit to make sure that's left alone
		cpu.writeStatus(0x0013U);
		// Validate starting conditions
		assertEqual(cpu.readProgramCounter(), 0x00000000U);
		assertEqual(cpu.readDataRegister(0U), 0x00000000U);
		assertEqual(cpu.readAddrRegister(0U), 0x00000004U);
		assertEqual(cpu.readAddrRegister(1U), 0x00000100U);
		assertEqual(cpu.readAddrRegister(7U), 0x007ffffcU);
		assertEqual(cpu.readStatus(), 0x0013U);
		// Step the first instruction and validate
		runStep();
		assertEqual(cpu.readProgramCounter(), 0x00000002U);
		assertEqual(cpu.readDataRegister(0U), 0x00000000U);
		assertEqual(cpu.readStatus(), 0x0014U);
		// Step the second instruction and validate
		runStep();
		assertEqual(cpu.readProgramCounter(), 0x00000004U);
		assertEqual(cpu.readDataRegister(0U), 0x0000803cU);
		assertEqual(cpu.readStatus(), 0x0018U);
		// Step the third instruction and validate
		runStep();
		assertEqual(cpu.readProgramCounter(), 0x00000008U);
		assertEqual(cpu.readDataRegister(0U), 0x0000807cU);
		assertEqual(cpu.readStatus(), 0x0010U);
		// Step the fourth instruction and validate
		runStep();
		assertEqual(cpu.readProgramCounter(), 0x0000000aU);
		assertEqual(cpu.readDataRegister(0U), 0x0000807cU);
		assertEqual(cpu.readAddrRegister(1U), 0x00000104U);
		assertEqual(readAddress<uint32_t>(0x000100U), uint32_t{0x8400807cU});
		assertEqual(cpu.readStatus(), 0x0018U);
		// Step the fifth and final instruction to complete the test
		runStep();
		assertEqual(cpu.readProgramCounter(), 0xffffffffU);
		assertEqual(cpu.readAddrRegister(7U), 0x00800000U);
	}

	void testORI()
	{
		writeAddress(0x000000U, uint16_t{0x0038U});
		writeAddress(0x000002U, uint16_t{0x0024U});
		writeAddress(0x000004U, uint16_t{0x0100U}); // ori.b #$24, $0100.w
		writeAddress(0x000006U, uint16_t{0x0040U});
		writeAddress(0x000008U, uint16_t{0x0000U}); // ori.w #0, d0
		writeAddress(0x00000aU, uint16_t{0x0080U});
		writeAddress(0x00000cU, uint16_t{0x8000U});
		writeAddress(0x00000eU, uint16_t{0x0001U}); // ori.l #$80000001, d0
		writeAddress(0x000010U, uint16_t{0x4e75U}); // rts to end the test
		writeAddress(0x000100U, uint8_t{0x01U});
		// Set the CPU to execute this sequence
		cpu.executeFrom(0x00000000U, 0x00800000U);
		// Set up d0 so we get zero = true from the second instruction, but get something interesting for the 3rd
		cpu.writeDataRegister(0U, 0x05a00000U);
		// Set the carry and overflow bits in the status register so we can observe them being cleared
		// And the extend bit to make sure that's left alone
		cpu.writeStatus(0x0013U);
		// Validate starting conditions
		assertEqual(cpu.readProgramCounter(), 0x00000000U);
		assertEqual(cpu.readAddrRegister(7U), 0x007ffffcU);
		assertEqual(cpu.readStatus(), 0x0013U);
		// Step the first instruction and validate
		runStep();
		assertEqual(cpu.readProgramCounter(), 0x00000006U);
		assertEqual(readAddress<uint8_t>(0x000100U), uint8_t{0x25U});
		assertEqual(cpu.readStatus(), 0x0010U);
		// Step the second instruction and validate
		runStep();
		assertEqual(cpu.readProgramCounter(), 0x0000000aU);
		assertEqual(cpu.readDataRegister(0U), 0x05a00000U);
		assertEqual(cpu.readStatus(), 0x0014U);
		// Step the third instruction and validate
		runStep();
		assertEqual(cpu.readProgramCounter(), 0x00000010U);
		assertEqual(cpu.readDataRegister(0U), 0x85a00001U);
		assertEqual(cpu.readStatus(), 0x0018U);
		// Step the final instruction to complete the test
		runStep();
		assertEqual(cpu.readProgramCounter(), 0xffffffffU);
		assertEqual(cpu.readAddrRegister(7U), 0x00800000U);
	}

public:
	CRUNCH_VIS testM68k() noexcept : testsuite{}, memoryMap_t<uint32_t, 0x00ffffffU>{}
	{
		// Check all the CPU preconditions before any execution begins
		for (const auto &reg : substrate::indexSequence_t{8U})
			assertEqual(cpu.readDataRegister(reg), 0U);
		for (const auto &reg : substrate::indexSequence_t{8U})
			assertEqual(cpu.readAddrRegister(reg), 0U);
		assertEqual(cpu.readProgramCounter(), 0xffffffffU);
		assertEqual(cpu.readStatus(), 0x2000U);

		// Register some memory for the tests to use
		addressMap[{0x000000U, 0x800000U}] = std::make_unique<ram_t<uint32_t, 8_MiB>>();
	}

	void registerTests() final
	{
		CXX_TEST(testDecode)
		CXX_TEST(testBranch)
		CXX_TEST(testBSR)
		CXX_TEST(testJump)
		CXX_TEST(testRTS)
		CXX_TEST(testRTE)
		CXX_TEST(testADD)
		CXX_TEST(testADDA)
		CXX_TEST(testADDI)
		CXX_TEST(testADDQ)
		CXX_TEST(testANDI)
		CXX_TEST(testCLR)
		CXX_TEST(testLEA)
		CXX_TEST(testOR)
		CXX_TEST(testORI)
	}
};

CRUNCH_API void registerCXXTests() noexcept;
void registerCXXTests() noexcept
{
	registerTestClasses<testM68k>();
}
