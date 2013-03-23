#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "CPU/Memory/Memory.h"
#include "CPU/Memory/RegisterSet.h"
#include "CPU/Memory/Register.h"
#include "CPU/Instructions/InstructionDecoder.h"
#include "CPU/Instructions/Instruction.h"

class InstructionDecoderTest : public CPPUNIT_NS :: TestFixture{
	CPPUNIT_TEST_SUITE(InstructionDecoderTest);
	CPPUNIT_TEST(decodeADD1ToRegister);
	CPPUNIT_TEST(decodeADDRegisterToRegister);
	CPPUNIT_TEST(decodeMOVConstantToRegister);
	CPPUNIT_TEST(decodeMOVImmediateToRegister);
	CPPUNIT_TEST(decodeMOV760r1MOVCLR);
	CPPUNIT_TEST(decodeMOVAbsoluteToImmediate);
	CPPUNIT_TEST(decodeMOVAutoincrementToIndexed);
	CPPUNIT_TEST(decodeMOVIndirectToAbsolute);
	CPPUNIT_TEST(decodeJZ);
	CPPUNIT_TEST(decodeJNC);
	CPPUNIT_TEST(decodeCALL);
	CPPUNIT_TEST(decodeRETI);
	CPPUNIT_TEST(decodeCMP);
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

		void decodeADD1ToRegister() {
			std::string data =
				// 1b 53       	inc	r11 => ADD(.B) #1,r11
				":10F000001B530003B240805A20013F4000000F937E\r\n"
				":040000030000F00009\r\n"
				":00000001FF\r\n";

			r->get(11)->set(55);

			m->loadA43(data, r);
			int inc = d->decodeCurrentInstruction(i);

			CPPUNIT_ASSERT_EQUAL(1, inc); // constants are treat as registers
			CPPUNIT_ASSERT_EQUAL((int) Instruction2, (int) i->type);
			CPPUNIT_ASSERT_EQUAL((int) 5, (int) i->opcode);
			CPPUNIT_ASSERT(i->getSrc());
			CPPUNIT_ASSERT_EQUAL((int) 1, (int) i->getSrc()->getBigEndian());
			CPPUNIT_ASSERT(i->getDst());
			CPPUNIT_ASSERT_EQUAL((int) 55, (int) i->getDst()->get());
		}

		void decodeADDRegisterToRegister() {
			std::string data =
				// 0f 51       	add	r1,	r15
				":10F000000F510003B240805A20013F4000000F937E\r\n"
				":040000030000F00009\r\n"
				":00000001FF\r\n";

			r->get(1)->set(51);
			r->get(15)->set(55);

			m->loadA43(data, r);
			int inc = d->decodeCurrentInstruction(i);

			CPPUNIT_ASSERT_EQUAL(1, inc);
			CPPUNIT_ASSERT_EQUAL((int) Instruction2, (int) i->type);
			CPPUNIT_ASSERT_EQUAL((int) 5, (int) i->opcode);
			CPPUNIT_ASSERT(i->getSrc());
			CPPUNIT_ASSERT_EQUAL((int) 51, (int) i->getSrc()->get());
			CPPUNIT_ASSERT(i->getDst());
			CPPUNIT_ASSERT_EQUAL((int) 55, (int) i->getDst()->get());
		}

		void decodeMOVConstantToRegister() {
			std::string data =
				// 0b 43       	clr	r11 => mov 0 to r11
				":10F000000B430003B240805A20013F4000000F937E\r\n"
				":040000030000F00009\r\n"
				":00000001FF\r\n";

			r->get(11)->set(55);

			m->loadA43(data, r);
			int inc = d->decodeCurrentInstruction(i);

			CPPUNIT_ASSERT_EQUAL(1, inc);
			CPPUNIT_ASSERT_EQUAL((int) Instruction2, (int) i->type);
			CPPUNIT_ASSERT_EQUAL((int) 4, (int) i->opcode);
			CPPUNIT_ASSERT(i->getSrc());
			CPPUNIT_ASSERT_EQUAL((int) 0, (int) i->getSrc()->get());
			CPPUNIT_ASSERT(i->getDst());
			CPPUNIT_ASSERT_EQUAL((int) 55, (int) i->getDst()->get());
		}

		void decodeMOVImmediateToRegister() {
			std::string data =
				// 31 40 f8 02 	mov	#760,	r1	;#0x02f8
				":10F000003140F802B240805A20013F4000000F937E\r\n"
				":040000030000F00009\r\n"
				":00000001FF\r\n";

			r->get(1)->set(55);

			m->loadA43(data, r);
			int inc = d->decodeCurrentInstruction(i);

			CPPUNIT_ASSERT_EQUAL(2, inc);
			CPPUNIT_ASSERT_EQUAL((int) Instruction2, (int) i->type);
			CPPUNIT_ASSERT_EQUAL((int) 4, (int) i->opcode);
			CPPUNIT_ASSERT(i->getSrc());
			CPPUNIT_ASSERT_EQUAL((int) 760, (int) i->getSrc()->getBigEndian());
			CPPUNIT_ASSERT(i->getDst());
			CPPUNIT_ASSERT_EQUAL((int) 55, (int) i->getDst()->get());
		}

		void decodeMOV760r1MOVCLR() {
			std::string data =
				// 31 40 f8 02 	mov	#760,	r1	;#0x02f8
				// 0b 43       	clr	r11 => mov 0 to r11
				":10F000003140F8020B43805A20013F4000000F937E\r\n"
				":040000030000F00009\r\n"
				":00000001FF\r\n";

			r->get(1)->set(55);
			r->get(11)->set(65);

			m->loadA43(data, r);
			int inc = d->decodeCurrentInstruction(i);

			CPPUNIT_ASSERT_EQUAL(2, inc);
			CPPUNIT_ASSERT_EQUAL((int) Instruction2, (int) i->type);
			CPPUNIT_ASSERT_EQUAL((int) 4, (int) i->opcode);
			CPPUNIT_ASSERT(i->getSrc());
			CPPUNIT_ASSERT_EQUAL((int) 760, (int) i->getSrc()->getBigEndian());
			CPPUNIT_ASSERT(i->getDst());
			CPPUNIT_ASSERT_EQUAL((int) 55, (int) i->getDst()->get());

			inc = d->decodeCurrentInstruction(i);

			CPPUNIT_ASSERT_EQUAL(1, inc);
			CPPUNIT_ASSERT_EQUAL((int) Instruction2, (int) i->type);
			CPPUNIT_ASSERT_EQUAL((int) 4, (int) i->opcode);
			CPPUNIT_ASSERT(i->getSrc());
			CPPUNIT_ASSERT_EQUAL((int) 0, (int) i->getSrc()->get());
			CPPUNIT_ASSERT(i->getDst());
			CPPUNIT_ASSERT_EQUAL((int) 65, (int) i->getDst()->get());
		}

		void decodeMOVAbsoluteToImmediate() {
			std::string data =
				// b2 40 80 5a 	mov	#23168,	&0x0120	;#0x5a80
				// 20 01
				":10F00000B240805A2001805A20013F4000000F937E\r\n"
				":040000030000F00009\r\n"
				":00000001FF\r\n";

			m->setBigEndian(0x0120, 55);

			m->loadA43(data, r);
			int inc = d->decodeCurrentInstruction(i);

			CPPUNIT_ASSERT_EQUAL(5, inc);
			CPPUNIT_ASSERT_EQUAL((int) Instruction2, (int) i->type);
			CPPUNIT_ASSERT_EQUAL((int) 4, (int) i->opcode);
			CPPUNIT_ASSERT(i->getSrc());
			CPPUNIT_ASSERT_EQUAL((int) 23168, (int) i->getSrc()->getBigEndian());
			CPPUNIT_ASSERT(i->getDst());
			CPPUNIT_ASSERT_EQUAL((int) 55, (int) i->getDst()->getBigEndian());
		}

		void decodeMOVAutoincrementToIndexed() {
			std::string data =
				// b1 4f 00 00 	mov	@r15+,	0(r1)	;0x0000(r1)
				":10F00000B14F00002001805A20013F4000000F937E\r\n"
				":040000030000F00009\r\n"
				":00000001FF\r\n";

			m->setBigEndian(0x0120, 55);
			r->get(15)->setBigEndian(0x0120);

			m->setBigEndian(0x0130, 50);
			r->get(1)->setBigEndian(0x0130);

			m->loadA43(data, r);
			int inc = d->decodeCurrentInstruction(i);

			CPPUNIT_ASSERT_EQUAL(5, inc);
			CPPUNIT_ASSERT_EQUAL((int) Instruction2, (int) i->type);
			CPPUNIT_ASSERT_EQUAL((int) 4, (int) i->opcode);
			CPPUNIT_ASSERT(i->getSrc());
			CPPUNIT_ASSERT_EQUAL((int) 55, (int) i->getSrc()->getBigEndian());
			CPPUNIT_ASSERT_EQUAL((int) 0x0122, (int) r->get(15)->getBigEndian());
			CPPUNIT_ASSERT(i->getDst());
			CPPUNIT_ASSERT_EQUAL((int) 50, (int) i->getDst()->getBigEndian());
		}

		void decodeMOVIndirectToAbsolute() {
			std::string data =
				// e2 4f 21 00 	mov.b	@r15,	&0x0021
				":10F00000E24F21002001805A20013F4000000F937E\r\n"
				":040000030000F00009\r\n"
				":00000001FF\r\n";

			m->setBigEndian(0x0120, 55);
			r->get(15)->setBigEndian(0x0120);

			m->setBigEndian(0x0021, 50);

			m->loadA43(data, r);
			int inc = d->decodeCurrentInstruction(i);

			CPPUNIT_ASSERT_EQUAL(5, inc);
			CPPUNIT_ASSERT_EQUAL((int) Instruction2, (int) i->type);
			CPPUNIT_ASSERT_EQUAL((int) 4, (int) i->opcode);
			CPPUNIT_ASSERT(i->getSrc());
			CPPUNIT_ASSERT_EQUAL((int) 55, (int) i->getSrc()->getBigEndian());
			CPPUNIT_ASSERT_EQUAL((int) 0x0120, (int) r->get(15)->getBigEndian());
			CPPUNIT_ASSERT(i->getDst());
			CPPUNIT_ASSERT_EQUAL((int) 50, (int) i->getDst()->getBigEndian());
		}

		void decodeJZ() {
			std::string data =
				// 05 24       	jz	$+12     	;abs 0xf01c
				":10F00000052421002001805A20013F4000000F937E\r\n"
				":040000030000F00009\r\n"
				":00000001FF\r\n";

			m->loadA43(data, r);
			int inc = d->decodeCurrentInstruction(i);

			CPPUNIT_ASSERT_EQUAL(2, inc);
			CPPUNIT_ASSERT_EQUAL((int) InstructionCond, (int) i->type);
			CPPUNIT_ASSERT_EQUAL((int) 1, (int) i->opcode);
			CPPUNIT_ASSERT_EQUAL((int) 10, (int) i->offset);
		}

		void decodeJNC() {
			std::string data =
				// fc 2b       	jnc	$-6      	;abs 0xf03c
				":10F00000FC2B21002001805A20013F4000000F937E\r\n"
				":040000030000F00009\r\n"
				":00000001FF\r\n";

			m->loadA43(data, r);
			int inc = d->decodeCurrentInstruction(i);

			CPPUNIT_ASSERT_EQUAL(2, inc);
			CPPUNIT_ASSERT_EQUAL((int) InstructionCond, (int) i->type);
			CPPUNIT_ASSERT_EQUAL((int) 2, (int) i->opcode);
			CPPUNIT_ASSERT_EQUAL((int) -8, (int) i->offset);
		}

		void decodeCALL() {
			std::string data =
				// b0 12 36 f0 	call	#0xf036	
				":10F00000B01236F02001805A20013F4000000F937E\r\n"
				":040000030000F00009\r\n"
				":00000001FF\r\n";

			m->loadA43(data, r);
			int inc = d->decodeCurrentInstruction(i);

			CPPUNIT_ASSERT_EQUAL(2, inc);
			CPPUNIT_ASSERT_EQUAL((int) Instruction1, (int) i->type);
			CPPUNIT_ASSERT_EQUAL((int) 5, (int) i->opcode);
			CPPUNIT_ASSERT(i->getDst());
			CPPUNIT_ASSERT_EQUAL((int) 0xf036, (int) i->getDst()->getBigEndian());
		}

		void decodeRETI() {
			std::string data =
				// 00 13       	reti
				":10F00000001336F02001805A20013F4000000F937E\r\n"
				":040000030000F00009\r\n"
				":00000001FF\r\n";

			m->loadA43(data, r);
			int inc = d->decodeCurrentInstruction(i);

			CPPUNIT_ASSERT_EQUAL(1, inc);
			CPPUNIT_ASSERT_EQUAL((int) Instruction1, (int) i->type);
			CPPUNIT_ASSERT_EQUAL((int) 6, (int) i->opcode);
// 			CPPUNIT_ASSERT(!i->getDst());
		}

		void decodeCMP() {
			std::string data =
				//f00e:	0f 93       	tst	r15 = cmp #0, r15
				":10F000000F9321002001805A20013F4000000F937E\r\n"
				":040000030000F00009\r\n"
				":00000001FF\r\n";

			r->get(15)->set(55);

			m->loadA43(data, r);
			int inc = d->decodeCurrentInstruction(i);

			CPPUNIT_ASSERT_EQUAL(1, inc);
			CPPUNIT_ASSERT_EQUAL((int) Instruction2, (int) i->type);
			CPPUNIT_ASSERT_EQUAL((int) 9, (int) i->opcode);
			CPPUNIT_ASSERT(i->getSrc());
			CPPUNIT_ASSERT_EQUAL((int) 0, (int) i->getSrc()->get());
			CPPUNIT_ASSERT(i->getDst());
			CPPUNIT_ASSERT_EQUAL((int) 55, (int) i->getDst()->get());
		}

};

CPPUNIT_TEST_SUITE_REGISTRATION (InstructionDecoderTest);
