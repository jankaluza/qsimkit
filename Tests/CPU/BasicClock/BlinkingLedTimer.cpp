#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "CPU/Memory/Memory.h"
#include "CPU/Memory/RegisterSet.h"
#include "CPU/Memory/Register.h"
#include "CPU/Instructions/InstructionDecoder.h"
#include "CPU/Instructions/InstructionManager.h"
#include "CPU/Instructions/Instruction.h"
#include "CPU/Interrupts/InterruptManager.h"
#include "CPU/BasicClock/Timer.h"
#include "CPU/BasicClock/TimerFactory.h"
#include "CPU/BasicClock/BasicClock.h"
#include "CPU/BasicClock/ACLK.h"
#include "CPU/BasicClock/SMCLK.h"
#include "CPU/BasicClock/VLO.h"
#include "CPU/BasicClock/DCO.h"
#include "CPU/BasicClock/LFXT1.h"
#include "CPU/Variants/Variant.h"
#include "CPU/Variants/VariantManager.h"
#include "CPU/Pins/PinManager.h"

#include <algorithm>

namespace MSP430 {

class DummyTimerFactory : public TimerFactory {
	public:
		DCO *createDCO(Memory *mem, Variant *variant) { return new DCO(mem, variant); }
		VLO *createVLO() { return new VLO(); }
};

#if 0

Disassembly of section .text:

0000f800 <__init_stack>:
    f800:       31 40 80 02     mov     #640,   r1      ;#0x0280

0000f804 <__low_level_init>:
    f804:       15 42 20 01     mov     &0x0120,r5
    f808:       75 f3           and.b   #-1,    r5      ;r3 As==11
    f80a:       35 d0 08 5a     bis     #23048, r5      ;#0x5a08

0000f80e <__do_copy_data>:
    f80e:       3f 40 00 00     mov     #0,     r15     ;#0x0000
    f812:       0f 93           tst     r15
    f814:       07 24           jz      $+16            ;abs 0xf824
    f816:       82 45 20 01     mov     r5,     &0x0120
    f81a:       2f 83           decd    r15
    f81c:       9f 4f 8a f8     mov     -1910(r15),512(r15);0xf88a(r15), 0x0200(r15)
    f820:       00 02 
    f822:       f9 23           jnz     $-12            ;abs 0xf816

0000f824 <__do_clear_bss>:
    f824:       3f 40 02 00     mov     #2,     r15     ;#0x0002
    f828:       0f 93           tst     r15
    f82a:       06 24           jz      $+14            ;abs 0xf838
    f82c:       82 45 20 01     mov     r5,     &0x0120
    f830:       1f 83           dec     r15
    f832:       cf 43 00 02     mov.b   #0,     512(r15);r3 As==00, 0x0200(r15)
    f836:       fa 23           jnz     $-10            ;abs 0xf82c

0000f838 <main>:
    f838:       b2 40 80 5a     mov     #23168, &0x0120 ;#0x5a80
    f83c:       20 01 
    f83e:       b2 d0 c4 02     bis     #708,   &0x0160 ;#0x02c4
    f842:       60 01 
    f844:       b2 40 e8 03     mov     #1000,  &0x0172 ;#0x03e8
    f848:       72 01 
    f84a:       b2 40 10 00     mov     #16,    &0x0162 ;#0x0010
    f84e:       62 01 
    f850:       b2 d0 10 00     bis     #16,    &0x0160 ;#0x0010
    f854:       60 01 
    f856:       f2 d0 40 00     bis.b   #64,    &0x0022 ;#0x0040
    f85a:       22 00 
    f85c:       f2 d0 40 00     bis.b   #64,    &0x0021 ;#0x0040
    f860:       21 00 
    f862:       32 d2           eint
    f864:       ff 3f           jmp     $+0             ;abs 0xf864

0000f866 <__stop_progExec__>:
    f866:       32 d0 f0 00     bis     #240,   r2      ;#0x00f0
    f86a:       fd 3f           jmp     $-4             ;abs 0xf866

0000f86c <__ctors_end>:
    f86c:       30 40 88 f8     br      #0xf888

0000f870 <timer_interrupt>:
    f870:       92 53 00 02     inc     &0x0200
    f874:       b2 90 7d 00     cmp     #125,   &0x0200 ;#0x007d
    f878:       00 02 
    f87a:       05 20           jnz     $+12            ;abs 0xf886
    f87c:       f2 e0 40 00     xor.b   #64,    &0x0021 ;#0x0040
    f880:       21 00 
    f882:       82 43 00 02     mov     #0,     &0x0200 ;r3 As==00
    f886:       00 13           reti

0000f888 <_unexpected_>:
    f888:       00 13           reti

Disassembly of section .vectors:

0000ffe0 <__ivtbl_16>:
    ffe0:       6c f8 6c f8 6c f8 6c f8 6c f8 6c f8 6c f8 6c f8     l.l.l.l.l.l.l.l.
    fff0:       6c f8 70 f8 6c f8 6c f8 6c f8 6c f8 6c f8 00 f8     l.p.l.l.l.l.l...

#endif

static std::string binary(unsigned n) {
	std::string res;

	while (n)
	{
		res.push_back((n & 1) + '0');
		n >>= 1;
	}

	if (res.empty())
		res = "0";
	else
		std::reverse(res.begin(), res.end());

	return res;
}

class BlinkingLedTimerTest : public CPPUNIT_NS :: TestFixture {
	CPPUNIT_TEST_SUITE(BlinkingLedTimerTest);
	CPPUNIT_TEST(execute);
	CPPUNIT_TEST_SUITE_END();

	Memory *m;
	RegisterSet *r;
	InstructionDecoder *d;
	Instruction *i;
	Variant *v;
	InterruptManager *intManager;
	BasicClock *bc;
	TimerFactory *factory;
	PinManager *pinManager;


	public:
		void setUp (void) {
			m = new Memory(120000);
			r = new RegisterSet;
			r->addDefaultRegisters();
			d = new InstructionDecoder(r, m);
			i = new Instruction;

			v = getVariant("msp430x241x");
			intManager = new InterruptManager(r, m, v);
			factory = new DummyTimerFactory();
			pinManager = new PinManager(m, intManager, v);
			bc = new BasicClock(m, v, intManager, pinManager, factory);
		}

		void tearDown (void) {
			delete d;
			delete i;
			delete m;
			delete r;
			delete intManager;
			delete bc;
			delete factory;
			delete pinManager;
		}

	void execute() {
		std::string data = ""
			":10F80000314080021542200175F335D0085A3F403F\r\n"
			":10F8100000000F930724824520012F839F4F8AF811\r\n"
			":10F820000002F9233F4002000F9306248245200185\r\n"
			":10F830001F83CF430002FA23B240805A2001B2D086\r\n"
			":10F84000C4026001B240E8037201B24010006201DC\r\n"
			":10F85000B2D010006001F2D040002200F2D040008F\r\n"
			":10F86000210032D2FF3F32D0F000FD3F304088F817\r\n"
			":10F8700092530002B2907D0000020520F2E04000A9\r\n"
			":0AF880002100824300020013001370\r\n"
			":10FFE0006CF86CF86CF86CF86CF86CF86CF86CF8F1\r\n"
			":10FFF0006CF870F86CF86CF86CF86CF86CF800F849\r\n"
			":040000030000F80001\r\n"
			":00000001FF\r\n";

		m->loadA43(data, r);

		// f800:       31 40 80 02     mov     #640,   r1      ;#0x0280
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			CPPUNIT_ASSERT_EQUAL((uint16_t) 0x0280, r->getp(1)->getBigEndian());
		// f804:       15 42 20 01     mov     &0x0120,r5
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
		// f808:       75 f3           and.b   #-1,    r5      ;r3 As==11
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
		// f80a:       35 d0 08 5a     bis     #23048, r5      ;#0x5a08
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);

		// f80e:       3f 40 00 00     mov     #0,     r15     ;#0x0000
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			CPPUNIT_ASSERT_EQUAL((uint16_t) 0, r->getp(15)->getBigEndian());
		// f812:       0f 93           tst     r15
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			CPPUNIT_ASSERT_EQUAL(true, r->getp(2)->isBitSet(SR_Z));
		// f814:       07 24           jz      $+16            ;abs 0xf824
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			CPPUNIT_ASSERT_EQUAL((uint16_t) 0xf824, r->getp(0)->getBigEndian());
		// f824:       3f 40 02 00     mov     #2,     r15     ;#0x0002
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			CPPUNIT_ASSERT_EQUAL((int16_t) 2, (int16_t) r->getp(15)->getBigEndian());
		// f828:       0f 93           tst     r15
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			CPPUNIT_ASSERT_EQUAL(false, r->getp(2)->isBitSet(SR_Z));
		// f82a:       06 24           jz      $+14            ;abs 0xf838
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			CPPUNIT_ASSERT_EQUAL((uint16_t) 0xf82c, r->getp(0)->getBigEndian());

		// f82c:       82 45 20 01     mov     r5,     &0x0120
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
		// f830:       1f 83           dec     r15
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			CPPUNIT_ASSERT_EQUAL((int16_t) 1, (int16_t) r->getp(15)->getBigEndian());
		// f832:       cf 43 00 02     mov.b   #0,     512(r15);r3 As==00, 0x0200(r15)
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
		// f836:       fa 23           jnz     $-10            ;abs 0xf82c
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			CPPUNIT_ASSERT_EQUAL((uint16_t) 0xf82c, r->getp(0)->getBigEndian());
		// f82c:       82 45 20 01     mov     r5,     &0x0120
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
		// f830:       1f 83           dec     r15
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			CPPUNIT_ASSERT_EQUAL((int16_t) 0, (int16_t) r->getp(15)->getBigEndian());
		// f832:       cf 43 00 02     mov.b   #0,     512(r15);r3 As==00, 0x0200(r15)
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
		// f836:       fa 23           jnz     $-10            ;abs 0xf82c
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			CPPUNIT_ASSERT_EQUAL((uint16_t) 0xf838, r->getp(0)->getBigEndian());

		/// MAIN
		// f838:       b2 40 80 5a     mov     #23168, &0x0120 ;#0x5a80
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			CPPUNIT_ASSERT_EQUAL((uint16_t) 0x5a80, m->getBigEndian(0x0120));
		/// f83e:       b2 d0 c4 02     bis     #708,   &0x0160 ;#0x02c4
		// TACTL = 0000001011000100
		//	             | ||   |---- TACLR
		//	             | ||-------- Divider 8
		//	             |----------- SMCLK
			// Check that TAR gets cleared later after execution
			m->setBigEndian(0x0170, 500);
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			// TACLR is cleared automatically after execution, that's why 704
			CPPUNIT_ASSERT_EQUAL((uint16_t) 704, m->getBigEndian(0x0160));
			CPPUNIT_ASSERT_EQUAL((uint16_t) 0, m->getBigEndian(0x0170));
			
		/// f844:       b2 40 e8 03     mov     #1000,  &0x0172 ;#0x03e8
		// Move 1000 to TACCR0
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			CPPUNIT_ASSERT_EQUAL((uint16_t) 1000, m->getBigEndian(0x0172));
		/// f84a:       b2 40 10 00     mov     #16,    &0x0162 ;#0x0010
		// Enable TACCTL0 interrupts
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			CPPUNIT_ASSERT_EQUAL((uint16_t) 16, m->getBigEndian(0x0162));
		/// f850:       b2 d0 10 00     bis     #16,    &0x0160 ;#0x0010
		// Set UP Mode
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			CPPUNIT_ASSERT_EQUAL((uint16_t) (704 | 0x0010), m->getBigEndian(0x0160));
		// f856:       f2 d0 40 00     bis.b   #64,    &0x0022 ;#0x0040
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			CPPUNIT_ASSERT_EQUAL((uint8_t) 0x0040, m->getByte(0x0022));
		// f85c:       f2 d0 40 00     bis.b   #64,    &0x0021 ;#0x0040
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			CPPUNIT_ASSERT_EQUAL((uint8_t) 0x0040, m->getByte(0x0021));
		// f862:       32 d2           eint
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			// TODO: test this one and probably implement...

		for (int y = 0; y < 125; ++y) {
		/// LOOP
		// f864:       ff 3f           jmp     $+0             ;abs 0xf864
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			CPPUNIT_ASSERT_EQUAL((uint16_t) 0xf864, r->getp(0)->getBigEndian());

		/// Do timer interrupt
			// tick to 999
			for (int x = 0; x < 999; ++x) {
				for (int divider = 0; divider < 8; ++divider) {
					bc->getTimerA()->tick();
				}
				CPPUNIT_ASSERT_EQUAL((uint16_t) (x + 1), m->getBigEndian(0x0170));
			}

			// No queued interrupt yet...
			CPPUNIT_ASSERT_EQUAL(false, intManager->hasQueuedInterrupts());

			// one more tick and we have interrupt about tar == taccr0
			for (int divider = 0; divider < 8; ++divider) {
				bc->getTimerA()->tick();
			}
			CPPUNIT_ASSERT_EQUAL(true, intManager->hasQueuedInterrupts());
			CPPUNIT_ASSERT_EQUAL(true, m->isBitSet(0x162, 1));
			CPPUNIT_ASSERT_EQUAL(true, intManager->runQueuedInterrupts());
			CPPUNIT_ASSERT_EQUAL((uint16_t) 0xf870, r->getp(0)->getBigEndian());

		/// Timer interrupt handler
		// f870:       92 53 00 02     inc     &0x0200
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			CPPUNIT_ASSERT_EQUAL((uint16_t) (y + 1), m->getBigEndian(0x0200));
		// f874:       b2 90 7d 00     cmp     #125,   &0x0200 ;#0x007d
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			if (y != 124) {
				CPPUNIT_ASSERT_EQUAL(false, r->getp(2)->isBitSet(SR_Z));
			// f87a:       05 20           jnz     $+12            ;abs 0xf886
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((uint16_t) 0xf886, r->getp(0)->getBigEndian());
			}
			else {
				CPPUNIT_ASSERT_EQUAL(true, r->getp(2)->isBitSet(SR_Z));
			// f87a:       05 20           jnz     $+12            ;abs 0xf886
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((uint16_t) 0xf87c, r->getp(0)->getBigEndian());
			// f87c:       f2 e0 40 00     xor.b   #64,    &0x0021 ;#0x0040
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((uint16_t) 0x0000, (uint16_t) m->getByte(0x0021));
			// f882:       82 43 00 02     mov     #0,     &0x0200 ;r3 As==00
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((uint16_t) 0, m->getBigEndian(0x0200));
			}
		// f886:       00 13           reti
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			intManager->handleInstruction(i);
			// reti should clear TACCTL CCIFG bit
			CPPUNIT_ASSERT_EQUAL(false, m->isBitSet(0x162, 1));
			CPPUNIT_ASSERT_EQUAL(false, intManager->hasQueuedInterrupts());

			// one more tick and we overlow, no TAIV interrupt, because
			// TAIFG interrupts are disabled
			for (int divider = 0; divider < 8; ++divider) {
				bc->getTimerA()->tick();
			}
			CPPUNIT_ASSERT_EQUAL((uint16_t) 0, m->getBigEndian(0x0170));
			CPPUNIT_ASSERT_EQUAL(false, intManager->hasQueuedInterrupts());
// 			CPPUNIT_ASSERT_EQUAL(true, intManager->runQueuedInterrupts());
// 			CPPUNIT_ASSERT_EQUAL((uint16_t) 0xf86c, r->getp(0)->getBigEndian());
// 		// f86c:       30 40 88 f8     br      #0xf888
// 			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
// 			CPPUNIT_ASSERT_EQUAL((uint16_t) 0xf888, r->getp(0)->getBigEndian());
// 		// f888:       00 13           reti
// 			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
// 			intManager->handleInstruction(i);
		}
			
		
	}

};

CPPUNIT_TEST_SUITE_REGISTRATION (BlinkingLedTimerTest);

}
