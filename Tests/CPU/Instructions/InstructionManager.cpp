#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "CPU/Memory/Memory.h"
#include "CPU/Memory/RegisterSet.h"
#include "CPU/Memory/Register.h"
#include "CPU/Instructions/InstructionDecoder.h"
#include "CPU/Instructions/InstructionManager.h"
#include "CPU/Instructions/Instruction.h"

using namespace MCU;

class InstructionManagerTest : public CPPUNIT_NS :: TestFixture {
	CPPUNIT_TEST_SUITE(InstructionManagerTest);
	CPPUNIT_TEST(executeADD1ToRegister);
	CPPUNIT_TEST(executeADDRegisterToRegister);
	CPPUNIT_TEST(executeADDRegisterToRegisterOverflow);
	CPPUNIT_TEST(executeADDRegisterToRegisterZero);
	CPPUNIT_TEST(executeMOVConstantToRegister);
	CPPUNIT_TEST(executeMOVImmediateToRegister);
	CPPUNIT_TEST(executeMOV760r1MOVCLR);
	CPPUNIT_TEST(executeMOVAbsoluteToImmediate);
	CPPUNIT_TEST(executeMOVAutoincrementToIndexed);
	CPPUNIT_TEST(executeMOVIndirectToAbsolute);
	CPPUNIT_TEST(executeJZ);
	CPPUNIT_TEST(executeJNC);
	CPPUNIT_TEST(executeCALL);
	CPPUNIT_TEST(executeRETI);
	CPPUNIT_TEST(executeCMPPositive);
	CPPUNIT_TEST(executeCMPZero);
	CPPUNIT_TEST(executeCMPNegative);
	CPPUNIT_TEST(executeCMPCarry);
	CPPUNIT_TEST_SUITE_END();

	Memory *m;
	RegisterSet *r;
	InstructionDecoder *d;
	Instruction *i;

	public:
		void setUp (void) {
			m = new Memory(120000);
			r = new RegisterSet;
			r->addDefaultRegisters();
			d = new InstructionDecoder(r, m);
			i = new Instruction;
		}

		void tearDown (void) {
			delete d;
			delete i;
			delete m;
			delete r;
		}

		void executeADD1ToRegister() {
			std::string data =
				// 1b 53       	inc	r11 => ADD(.B) #1,r11
				":10F000001B530003B240805A20013F4000000F937E\r\n"
				":040000030000F00009\r\n"
				":00000001FF\r\n";

			r->get(11)->setBigEndian(55);

			m->loadA43(data, r);
			int cycles = d->decodeCurrentInstruction(i);
			cycles += executeInstruction(r, m, i);

			CPPUNIT_ASSERT_EQUAL(1, cycles);
			CPPUNIT_ASSERT_EQUAL((int) 56, (int) r->get(11)->getBigEndian());
		}

		void executeADDRegisterToRegister() {
			std::string data =
				// 0f 51       	add	r1,	r15
				":10F000000F510003B240805A20013F4000000F937E\r\n"
				":040000030000F00009\r\n"
				":00000001FF\r\n";

			r->get(1)->setBigEndian(51);
			r->get(15)->setBigEndian(55);

			m->loadA43(data, r);
			int cycles = d->decodeCurrentInstruction(i);
			cycles += executeInstruction(r, m, i);

			CPPUNIT_ASSERT_EQUAL(1, cycles);
			CPPUNIT_ASSERT_EQUAL((int) 51, (int) r->get(1)->getBigEndian());
			CPPUNIT_ASSERT_EQUAL((int) 106, (int) r->get(15)->getBigEndian());
			CPPUNIT_ASSERT_EQUAL(false, r->get(2)->isBitSet(SR_V));
			CPPUNIT_ASSERT_EQUAL(false, r->get(2)->isBitSet(SR_N));
			CPPUNIT_ASSERT_EQUAL(false, r->get(2)->isBitSet(SR_Z));
		}

		void executeADDRegisterToRegisterOverflow() {
			std::string data =
				// 0f 51       	add	r1,	r15
				":10F000000F510003B240805A20013F4000000F937E\r\n"
				":040000030000F00009\r\n"
				":00000001FF\r\n";

			r->get(1)->setBigEndian(32767);
			r->get(15)->setBigEndian(2);

			m->loadA43(data, r);
			int cycles = d->decodeCurrentInstruction(i);
			cycles += executeInstruction(r, m, i);

			CPPUNIT_ASSERT_EQUAL(1, cycles);
			CPPUNIT_ASSERT_EQUAL((int) 32767, (int) r->get(1)->getBigEndian());
			CPPUNIT_ASSERT_EQUAL((int16_t) -32767, (int16_t) r->get(15)->getBigEndian());
			CPPUNIT_ASSERT_EQUAL(true, r->get(2)->isBitSet(SR_V));
			CPPUNIT_ASSERT_EQUAL(true, r->get(2)->isBitSet(SR_N));
			CPPUNIT_ASSERT_EQUAL(false, r->get(2)->isBitSet(SR_Z));
		}

		void executeADDRegisterToRegisterZero() {
			std::string data =
				// 0f 51       	add	r1,	r15
				":10F000000F510003B240805A20013F4000000F937E\r\n"
				":040000030000F00009\r\n"
				":00000001FF\r\n";

			r->get(1)->setBigEndian(-2);
			r->get(15)->setBigEndian(2);

			m->loadA43(data, r);
			int cycles = d->decodeCurrentInstruction(i);
			cycles += executeInstruction(r, m, i);

			CPPUNIT_ASSERT_EQUAL(1, cycles);
			CPPUNIT_ASSERT_EQUAL((int16_t) -2, (int16_t) r->get(1)->getBigEndian());
			CPPUNIT_ASSERT_EQUAL((int16_t) 0, (int16_t) r->get(15)->getBigEndian());
			CPPUNIT_ASSERT_EQUAL(false, r->get(2)->isBitSet(SR_V));
			CPPUNIT_ASSERT_EQUAL(false, r->get(2)->isBitSet(SR_N));
			CPPUNIT_ASSERT_EQUAL(true, r->get(2)->isBitSet(SR_Z));
		}

		void executeMOVConstantToRegister() {
			std::string data =
				// 0b 43       	clr	r11 => mov 0 to r11
				":10F000000B430003B240805A20013F4000000F937E\r\n"
				":040000030000F00009\r\n"
				":00000001FF\r\n";

			r->get(11)->setBigEndian(55);
			CPPUNIT_ASSERT_EQUAL((int) 55, (int) r->get(11)->getBigEndian());

			m->loadA43(data, r);
			int cycles = d->decodeCurrentInstruction(i);
			cycles += executeInstruction(r, m, i);
			CPPUNIT_ASSERT_EQUAL(1, cycles);

			CPPUNIT_ASSERT_EQUAL((int) 0, (int) r->get(11)->getBigEndian());
		}

		void executeMOVImmediateToRegister() {
			std::string data =
				// 31 40 f8 02 	mov	#760,	r1	;#0x02f8
				":10F000003140F802B240805A20013F4000000F937E\r\n"
				":040000030000F00009\r\n"
				":00000001FF\r\n";

			r->get(1)->setBigEndian(55);
			CPPUNIT_ASSERT_EQUAL((int) 55, (int) r->get(1)->getBigEndian());

			m->loadA43(data, r);
			int cycles = d->decodeCurrentInstruction(i);
			cycles += executeInstruction(r, m, i);

			CPPUNIT_ASSERT_EQUAL(2, cycles);
			CPPUNIT_ASSERT_EQUAL((int) 760, (int) r->get(1)->getBigEndian());
		}

		void executeMOV760r1MOVCLR() {
			std::string data =
				// 31 40 f8 02 	mov	#760,	r1	;#0x02f8
				// 0b 43       	clr	r11 => mov 0 to r11
				":10F000003140F8020B43805A20013F4000000F937E\r\n"
				":040000030000F00009\r\n"
				":00000001FF\r\n";

			r->get(1)->setBigEndian(55);
			CPPUNIT_ASSERT_EQUAL((int) 55, (int) r->get(1)->getBigEndian());
			r->get(11)->setBigEndian(55);
			CPPUNIT_ASSERT_EQUAL((int) 55, (int) r->get(11)->getBigEndian());

			m->loadA43(data, r);

			d->decodeCurrentInstruction(i);
			executeInstruction(r, m, i);
			CPPUNIT_ASSERT_EQUAL((int) 760, (int) r->get(1)->getBigEndian());

			d->decodeCurrentInstruction(i);
			executeInstruction(r, m, i);

			CPPUNIT_ASSERT_EQUAL((int) 0, (int) r->get(11)->getBigEndian());
		}

		void executeMOVAbsoluteToImmediate() {
			std::string data =
				// b2 40 80 5a 	mov	#23168,	&0x0120	;#0x5a80
				// 20 01
				":10F00000B240805A2001805A20013F4000000F937E\r\n"
				":040000030000F00009\r\n"
				":00000001FF\r\n";

			m->setBigEndian(0x0120, 55);
			CPPUNIT_ASSERT_EQUAL((int) 55, (int) m->getBigEndian(0x0120));

			m->loadA43(data, r);
			int cycles = d->decodeCurrentInstruction(i);
			cycles += executeInstruction(r, m, i);

			CPPUNIT_ASSERT_EQUAL(5, cycles);
			CPPUNIT_ASSERT_EQUAL((int) 23168, (int) m->getBigEndian(0x0120));
		}

		void executeMOVAutoincrementToIndexed() {
			std::string data =
				// b1 4f 00 00 	mov	@r15+,	0(r1)	;0x0000(r1)
				":10F00000B14F00002001805A20013F4000000F937E\r\n"
				":040000030000F00009\r\n"
				":00000001FF\r\n";

			m->setBigEndian(0x0120, 55);
			CPPUNIT_ASSERT_EQUAL((int) 55, (int) m->getBigEndian(0x0120));
			r->get(15)->setBigEndian(0x0120);

			m->setBigEndian(0x0130, 50);
			CPPUNIT_ASSERT_EQUAL((int) 50, (int) m->getBigEndian(0x0130));
			r->get(1)->setBigEndian(0x0130);

			m->loadA43(data, r);
			int cycles = d->decodeCurrentInstruction(i);
			cycles += executeInstruction(r, m, i);

			CPPUNIT_ASSERT_EQUAL(5, cycles);
			// r15 should be incremented by 2
			CPPUNIT_ASSERT_EQUAL((int) 0x0122, (int) r->get(15)->getBigEndian());
			CPPUNIT_ASSERT_EQUAL((int) 55, (int) m->getBigEndian(0x0130));
		}

		void executeMOVIndirectToAbsolute() {
			std::string data =
				// e2 4f 21 00 	mov.b	@r15,	&0x0021
				":10F00000E24F21002001805A20013F4000000F937E\r\n"
				":040000030000F00009\r\n"
				":00000001FF\r\n";

			m->setBigEndian(0x0120, 0x50);
			r->get(15)->setBigEndian(0x0120);

			// words are located at even addresses, so we change 0x0020 here
			// which changes also 0x0021.
			m->setBigEndian(0x0020, 0xffff);
			CPPUNIT_ASSERT_EQUAL((int) 0xffff, (int) m->getBigEndian(0x0020));
			CPPUNIT_ASSERT_EQUAL((int) 0xff, (int) m->getByte(0x0021));

			m->loadA43(data, r);
			int cycles = d->decodeCurrentInstruction(i);
			// write 50 to 0x0021
			cycles += executeInstruction(r, m, i);

			CPPUNIT_ASSERT_EQUAL(5, cycles);
			// register not incremented
			CPPUNIT_ASSERT_EQUAL((int) 0x0120, (int) r->get(15)->getBigEndian());
			CPPUNIT_ASSERT_EQUAL((int) 0x50ff, (int) m->getBigEndian(0x0020));
			CPPUNIT_ASSERT_EQUAL((int) 0x50, (int) m->getByte(0x0021));
		}

		void executeJZ() {
			std::string data =
				// f010: 05 24       	jz	$+12     	;abs 0xf01c
				":10F01000052421002001805A20013F4000000F937E\r\n"
				":040000030000F01009\r\n"
				":00000001FF\r\n";

			m->loadA43(data, r);
			int cycles = d->decodeCurrentInstruction(i);
			r->get(2)->setBit(SR_Z, 0);
			uint16_t pc = r->get(0)->getBigEndian();
			executeInstruction(r, m, i);

			CPPUNIT_ASSERT_EQUAL(pc, r->get(0)->getBigEndian());

			r->get(2)->setBit(SR_Z, 1);
			cycles += executeInstruction(r, m, i);
			CPPUNIT_ASSERT_EQUAL(2, cycles);

			// PC has been incremented by two during decodeCurrentInstruction,
			// so we have to decrement here.
			pc += 12 - 2;
			CPPUNIT_ASSERT_EQUAL((uint16_t)0xf01c, r->get(0)->getBigEndian());
			CPPUNIT_ASSERT_EQUAL(pc, r->get(0)->getBigEndian());
		}

		void executeJNC() {
			std::string data =
				//f042: fc 2b       	jnc	$-6      	;abs 0xf03c
				":10F04200FC2B21002001805A20013F4000000F937E\r\n"
				":040000030000F04209\r\n"
				":00000001FF\r\n";

			m->loadA43(data, r);
			r->get(2)->setBit(SR_C, 1);
			d->decodeCurrentInstruction(i);
			uint16_t pc = r->get(0)->getBigEndian();
			executeInstruction(r, m, i);

			CPPUNIT_ASSERT_EQUAL(pc, r->get(0)->getBigEndian());

			r->get(2)->setBit(SR_C, 0);
			executeInstruction(r, m, i);

			// PC has been incremented by two during decodeCurrentInstruction,
			// so we have to decrement here.
			pc -= 6 + 2;
			CPPUNIT_ASSERT_EQUAL((uint16_t)0xf03c, r->get(0)->getBigEndian());
			CPPUNIT_ASSERT_EQUAL((uint16_t)pc, r->get(0)->getBigEndian());
		}

		void executeCALL() {
			std::string data =
				// b0 12 36 f0 	call	#0xf036	
				":10F00000B01236F02001805A20013F4000000F937E\r\n"
				":040000030000F00009\r\n"
				":00000001FF\r\n";

			m->loadA43(data, r);
			int cycles = d->decodeCurrentInstruction(i);
			uint16_t pc = r->get(0)->getBigEndian();

			r->get(1)->setBigEndian(0x0300); // SP
			cycles += executeInstruction(r, m, i);

			CPPUNIT_ASSERT_EQUAL(5, cycles);
			CPPUNIT_ASSERT_EQUAL((uint16_t)0xf036, r->get(0)->getBigEndian());
			CPPUNIT_ASSERT_EQUAL((uint16_t)(0x0300 - 2), r->get(1)->getBigEndian());
			CPPUNIT_ASSERT_EQUAL(pc, m->getBigEndian(0x0300 - 2));

		}

		void executeRETI() {
			// TODO:
// 			std::string data =
// 				// 00 13       	reti
// 				":10F00000001336F02001805A20013F4000000F937E\r\n"
// 				":040000030000F00009\r\n"
// 				":00000001FF\r\n";
// 
// 			m->loadA43(data, r);
// 			int inc = d->decodeCurrentInstruction(i);
// 
// 			CPPUNIT_ASSERT_EQUAL(1, inc);
// 			CPPUNIT_ASSERT_EQUAL((int) Instruction1, (int) i->type);
// 			CPPUNIT_ASSERT_EQUAL((int) 6, (int) i->opcode);
		}

		void executeCMPPositive() {
			std::string data =
				//f00e:	0f 93       	tst	r15 = cmp #0, r15
				":10F000000F9321002001805A20013F4000000F937E\r\n"
				":040000030000F00009\r\n"
				":00000001FF\r\n";

			r->get(15)->setBigEndian(1);

			m->loadA43(data, r);
			int cycles = d->decodeCurrentInstruction(i);
			cycles += executeInstruction(r, m, i);

			CPPUNIT_ASSERT_EQUAL(1, cycles);
			CPPUNIT_ASSERT_EQUAL(false, r->get(2)->isBitSet(SR_V));
			CPPUNIT_ASSERT_EQUAL(false, r->get(2)->isBitSet(SR_N));
			CPPUNIT_ASSERT_EQUAL(false, r->get(2)->isBitSet(SR_Z));
		}

		void executeCMPZero() {
			std::string data =
				//f00e:	0f 93       	tst	r15 = cmp #0, r15
				":10F000000F9321002001805A20013F4000000F937E\r\n"
				":040000030000F00009\r\n"
				":00000001FF\r\n";

			r->get(15)->setBigEndian(0);

			m->loadA43(data, r);
			d->decodeCurrentInstruction(i);
			executeInstruction(r, m, i);

			CPPUNIT_ASSERT_EQUAL(false, r->get(2)->isBitSet(SR_V));
			CPPUNIT_ASSERT_EQUAL(false, r->get(2)->isBitSet(SR_N));
			CPPUNIT_ASSERT_EQUAL(true, r->get(2)->isBitSet(SR_Z));
		}

		void executeCMPNegative() {
			std::string data =
				//f00e:	0f 93       	tst	r15 = cmp #0, r15
				":10F000000F9321002001805A20013F4000000F937E\r\n"
				":040000030000F00009\r\n"
				":00000001FF\r\n";

			r->get(15)->setBigEndian(-1);

			m->loadA43(data, r);
			d->decodeCurrentInstruction(i);
			executeInstruction(r, m, i);

			CPPUNIT_ASSERT_EQUAL(false, r->get(2)->isBitSet(SR_V));
			CPPUNIT_ASSERT_EQUAL(true, r->get(2)->isBitSet(SR_N));
			CPPUNIT_ASSERT_EQUAL(false, r->get(2)->isBitSet(SR_Z));
		}

		void executeCMPCarry() {
			std::string data =
				// f038:	0e 9f       	cmp	r15,	r14	// r14 - r15
				":10F000000E9F21002001805A20013F4000000F937E\r\n"
				":040000030000F00009\r\n"
				":00000001FF\r\n";

			m->loadA43(data, r);
			d->decodeCurrentInstruction(i);

			r->get(15)->setBigEndian(2);
			r->get(14)->setBigEndian(0);
			executeInstruction(r, m, i);

			CPPUNIT_ASSERT_EQUAL(false, r->get(2)->isBitSet(SR_V));
			CPPUNIT_ASSERT_EQUAL(true, r->get(2)->isBitSet(SR_N));
			CPPUNIT_ASSERT_EQUAL(false, r->get(2)->isBitSet(SR_Z));
			CPPUNIT_ASSERT_EQUAL(false, r->get(2)->isBitSet(SR_C));

			r->get(15)->setBigEndian(2);
			r->get(14)->setBigEndian(1);
			executeInstruction(r, m, i);

			CPPUNIT_ASSERT_EQUAL(false, r->get(2)->isBitSet(SR_V));
			CPPUNIT_ASSERT_EQUAL(true, r->get(2)->isBitSet(SR_N));
			CPPUNIT_ASSERT_EQUAL(false, r->get(2)->isBitSet(SR_Z));
			CPPUNIT_ASSERT_EQUAL(false, r->get(2)->isBitSet(SR_C));

			r->get(15)->setBigEndian(2);
			r->get(14)->setBigEndian(2);
			executeInstruction(r, m, i);

			CPPUNIT_ASSERT_EQUAL(false, r->get(2)->isBitSet(SR_V));
			CPPUNIT_ASSERT_EQUAL(false, r->get(2)->isBitSet(SR_N));
			CPPUNIT_ASSERT_EQUAL(true, r->get(2)->isBitSet(SR_Z));
			CPPUNIT_ASSERT_EQUAL(true, r->get(2)->isBitSet(SR_C));

			r->get(15)->setBigEndian(2);
			r->get(14)->setBigEndian(3);
			executeInstruction(r, m, i);

			CPPUNIT_ASSERT_EQUAL(false, r->get(2)->isBitSet(SR_V));
			CPPUNIT_ASSERT_EQUAL(false, r->get(2)->isBitSet(SR_N));
			CPPUNIT_ASSERT_EQUAL(false, r->get(2)->isBitSet(SR_Z));
			CPPUNIT_ASSERT_EQUAL(true, r->get(2)->isBitSet(SR_C));
		}

};

CPPUNIT_TEST_SUITE_REGISTRATION (InstructionManagerTest);
