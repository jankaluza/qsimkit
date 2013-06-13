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
#include "CPU/Pins/PinMultiplexer.h"

#include <algorithm>

namespace MSP430 {

class DummyTimerFactory : public TimerFactory {
	public:
		DCO *createDCO(Memory *mem, Variant *variant) { return new DCO(mem, variant); }
		LFXT1 *createLFXT1(Memory *mem, Variant *variant) { return new LFXT1(mem, variant); }
		VLO *createVLO() { return new VLO(); }
};

#if 0

Disassembly of section .text:

0000f800 <__init_stack>:
    f800:	31 40 80 02 	mov	#640,	r1	;#0x0280

0000f804 <__low_level_init>:
    f804:	15 42 20 01 	mov	&0x0120,r5	
    f808:	75 f3       	and.b	#-1,	r5	;r3 As==11
    f80a:	35 d0 08 5a 	bis	#23048,	r5	;#0x5a08

0000f80e <__do_copy_data>:
    f80e:	3f 40 00 00 	mov	#0,	r15	;#0x0000
    f812:	0f 93       	tst	r15		
    f814:	07 24       	jz	$+16     	;abs 0xf824
    f816:	82 45 20 01 	mov	r5,	&0x0120	
    f81a:	2f 83       	decd	r15		
    f81c:	9f 4f c2 f8 	mov	-1854(r15),512(r15);0xf8c2(r15), 0x0200(r15)
    f820:	00 02 
    f822:	f9 23       	jnz	$-12     	;abs 0xf816

0000f824 <__do_clear_bss>:
    f824:	3f 40 4a 00 	mov	#74,	r15	;#0x004a
    f828:	0f 93       	tst	r15		
    f82a:	06 24       	jz	$+14     	;abs 0xf838
    f82c:	82 45 20 01 	mov	r5,	&0x0120	
    f830:	1f 83       	dec	r15		
    f832:	cf 43 00 02 	mov.b	#0,	512(r15);r3 As==00, 0x0200(r15)
    f836:	fa 23       	jnz	$-10     	;abs 0xf82c

0000f838 <main>:
    f838:	b2 40 80 5a 	mov	#23168,	&0x0120	;#0x5a80
    f83c:	20 01 
    f83e:	d2 43 22 00 	mov.b	#1,	&0x0022	;r3 As==01
    f842:	e2 43 26 00 	mov.b	#2,	&0x0026	;r3 As==10
    f846:	b2 40 10 49 	mov	#18704,	&0x0162	;#0x4910
    f84a:	62 01 
    f84c:	b2 40 20 02 	mov	#544,	&0x0160	;#0x0220
    f850:	60 01 
    f852:	32 d0 18 00 	bis	#24,	r2	;#0x0018
    f856:	0f 43       	clr	r15		

0000f858 <__stop_progExec__>:
    f858:	32 d0 f0 00 	bis	#240,	r2	;#0x00f0
    f85c:	fd 3f       	jmp	$-4      	;abs 0xf858

0000f85e <__ctors_end>:
    f85e:	30 40 c0 f8 	br	#0xf8c0	

0000f862 <TimerA0>:
    f862:	0f 12       	push	r15		
    f864:	0e 12       	push	r14		
    f866:	0d 12       	push	r13		
    f868:	0c 12       	push	r12		
    f86a:	1e 42 72 01 	mov	&0x0172,r14	
    f86e:	82 4e 00 02 	mov	r14,	&0x0200	
    f872:	0d 4e       	mov	r14,	r13	
    f874:	1d 82 02 02 	sub	&0x0202,r13	
    f878:	82 4d 04 02 	mov	r13,	&0x0204	
    f87c:	5c 42 06 02 	mov.b	&0x0206,r12	
    f880:	4f 4c       	mov.b	r12,	r15	
    f882:	0f 5f       	rla	r15		
    f884:	8f 4d 0a 02 	mov	r13,	522(r15);0x020a(r15)
    f888:	8f 4e 2a 02 	mov	r14,	554(r15);0x022a(r15)
    f88c:	4f 4c       	mov.b	r12,	r15	
    f88e:	5f 53       	inc.b	r15		
    f890:	c2 4f 06 02 	mov.b	r15,	&0x0206	
    f894:	7f 90 10 00 	cmp.b	#16,	r15	;#0x0010
    f898:	02 20       	jnz	$+6      	;abs 0xf89e
    f89a:	c2 43 06 02 	mov.b	#0,	&0x0206	;r3 As==00
    f89e:	82 4e 02 02 	mov	r14,	&0x0202	
    f8a2:	5f 42 08 02 	mov.b	&0x0208,r15	
    f8a6:	5f 53       	inc.b	r15		
    f8a8:	c2 4f 08 02 	mov.b	r15,	&0x0208	
    f8ac:	7f 90 20 00 	cmp.b	#32,	r15	;#0x0020
    f8b0:	02 20       	jnz	$+6      	;abs 0xf8b6
    f8b2:	c2 43 08 02 	mov.b	#0,	&0x0208	;r3 As==00
    f8b6:	3c 41       	pop	r12		
    f8b8:	3d 41       	pop	r13		
    f8ba:	3e 41       	pop	r14		
    f8bc:	3f 41       	pop	r15		
    f8be:	00 13       	reti			

0000f8c0 <_unexpected_>:
    f8c0:	00 13       	reti			

Disassembly of section .vectors:

0000ffe0 <__ivtbl_16>:
    ffe0:	5e f8 5e f8 5e f8 5e f8 5e f8 5e f8 5e f8 5e f8     ^.^.^.^.^.^.^.^.
    fff0:	5e f8 62 f8 5e f8 5e f8 5e f8 5e f8 5e f8 00 f8     ^.b.^.^.^.^.^...

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

class CaptureTest : public CPPUNIT_NS :: TestFixture {
	CPPUNIT_TEST_SUITE(CaptureTest);
	CPPUNIT_TEST(execute);
	CPPUNIT_TEST_SUITE_END();

	Memory *m;
	RegisterSet *r;
	Variant *v;
	InterruptManager *intManager;
	BasicClock *bc;
	TimerFactory *factory;
	PinManager *pinManager;
	InstructionDecoder *d;
	Instruction *i;

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

			PinMultiplexer *mpx = pinManager->addPin(P1, 1);

			{
				PinMultiplexer::Condition c;
				c["sel"] = 0;
				mpx->addMultiplexing(c, "GP");
			}

			{
				PinMultiplexer::Condition c;
				c["dir"] = 0;
				c["sel"] = 1;
				mpx->addMultiplexing(c, "TA0.CCI0A");
			}
			
			bc = new BasicClock(m, v, intManager, pinManager, factory);
		}

		void tearDown (void) {
			delete m;
			delete r;
			delete intManager;
			delete bc;
			delete factory;
			delete pinManager;
			delete d;
			delete i;
		}

	void execute() {
		std::string data = ""
			":10F80000314080021542200175F335D0085A3F403F\r\n"
			":10F8100000000F930724824520012F839F4FC2F8D9\r\n"
			":10F820000002F9233F404A000F930624824520013D\r\n"
			":10F830001F83CF430002FA23B240805A2001D243F3\r\n"
			":10F840002200E2432600B24010496201B240200289\r\n"
			":10F85000600132D018000F4332D0F000FD3F30403D\r\n"
			":10F86000C0F80F120E120D120C121E427201824EBF\r\n"
			":10F8700000020D4E1D820202824D04025C4206020D\r\n"
			":10F880004F4C0F5F8F4D0A028F4E2A024F4C5F5331\r\n"
			":10F89000C24F06027F9010000220C2430602824E31\r\n"
			":10F8A00002025F4208025F53C24F08027F902000AD\r\n"
			":10F8B0000220C24308023C413D413E413F4100130A\r\n"
			":02F8C000001333\r\n"
			":10FFE0005EF85EF85EF85EF85EF85EF85EF85EF861\r\n"
			":10FFF0005EF862F85EF85EF85EF85EF85EF800F8AB\r\n"
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
		// f824:       3f 40 02 00     mov     #74,     r15     ;#0x0002
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			CPPUNIT_ASSERT_EQUAL((int16_t) 74, (int16_t) r->getp(15)->getBigEndian());
		// f828:       0f 93           tst     r15
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			CPPUNIT_ASSERT_EQUAL(false, r->getp(2)->isBitSet(SR_Z));
		// f82a:       06 24           jz      $+14            ;abs 0xf838
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			CPPUNIT_ASSERT_EQUAL((uint16_t) 0xf82c, r->getp(0)->getBigEndian());

		for (int x = 0; x < 74; ++x) {
		// f82c:       82 45 20 01     mov     r5,     &0x0120
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
		// f830:       1f 83           dec     r15
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			CPPUNIT_ASSERT_EQUAL((int16_t) (73 - x), (int16_t) r->getp(15)->getBigEndian());
		// f832:       cf 43 00 02     mov.b   #0,     512(r15);r3 As==00, 0x0200(r15)
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
		// f836:       fa 23           jnz     $-10            ;abs 0xf82c
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
		}
		CPPUNIT_ASSERT_EQUAL((uint16_t) 0xf838, r->getp(0)->getBigEndian());

		/// MAIN
		// f838:       b2 40 80 5a     mov     #23168, &0x0120 ;#0x5a80
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			CPPUNIT_ASSERT_EQUAL((uint16_t) 0x5a80, m->getBigEndian(0x0120));
		// f83e:	d2 43 22 00 	mov.b	#1,	&0x0022	;r3 As==01
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			CPPUNIT_ASSERT_EQUAL((uint8_t) 1, m->getByte(0x0022));
		// f842:	e2 43 26 00 	mov.b	#2,	&0x0026	;r3 As==10
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			CPPUNIT_ASSERT_EQUAL((uint8_t) 2, m->getByte(0x0026));
		// f846:	b2 40 10 49 	mov	#18704,	&0x0162	;#0x4910
		//   CCTL0 = CM_1 + SCS + CCIS_0 + CAP + CCIE;
		// Rising edge + CCI0A (P1.1) + Capture Mode + Interrupt
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i); 
			CPPUNIT_ASSERT_EQUAL((uint16_t) 0x4910, m->getBigEndian(0x0162));
		// f84c:	b2 40 20 02 	mov	#544,	&0x0160	;#0x0220
		// TACTL = TASSEL_2 + MC_2; // SMCLK + Continuous Mode
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			CPPUNIT_ASSERT_EQUAL((uint16_t) 0x0220, m->getBigEndian(0x0160));
		// f852:	32 d0 18 00 	bis	#24,	r2	;#0x0018
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			CPPUNIT_ASSERT_EQUAL(true, r->getp(2)->isBitSet(SR_GIE));

			// do some ticks
			for (int t = 0; t < 10; ++t) { bc->getTimerA()->tick(); }
			// No queued interrupt yet
			CPPUNIT_ASSERT_EQUAL(false, intManager->hasQueuedInterrupts());

		for (int x = 0; x < 16; ++x) {
			// Generate rising edge
			pinManager->handlePinInput(0, 0.0);
			pinManager->handlePinInput(0, 3.0);
			for (int t = 0; t < 10; ++t) { bc->getTimerA()->tick(); }
			CPPUNIT_ASSERT_EQUAL(true, intManager->hasQueuedInterrupts());

			// Run the interrupt
			CPPUNIT_ASSERT_EQUAL(true, intManager->runQueuedInterrupts());
			CPPUNIT_ASSERT_EQUAL((uint16_t) 0xf862, r->getp(0)->getBigEndian());

		/// TimerA0
		// f862:	0f 12       	push	r15
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
		// f864:	0e 12       	push	r14
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
		// f866:	0d 12       	push	r13
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
		// f868:	0c 12       	push	r12
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
		// f86a:	1e 42 72 01 	mov	&0x0172,r14	
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			CPPUNIT_ASSERT_EQUAL(m->getBigEndian(0x0172), r->getp(14)->getBigEndian());
		// f86e:	82 4e 00 02 	mov	r14,	&0x0200	
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			CPPUNIT_ASSERT_EQUAL(m->getBigEndian(0x0200), r->getp(14)->getBigEndian());
		// f872:	0d 4e       	mov	r14,	r13	
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			CPPUNIT_ASSERT_EQUAL((uint16_t) (11 + x*10), r->getp(14)->getBigEndian());
		// f874:	1d 82 02 02 	sub	&0x0202,r13	
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			// cap_diff is 10 everytime except of first cap
			CPPUNIT_ASSERT_EQUAL((uint16_t) (x == 0 ? 11 : 10), r->getp(13)->getBigEndian());
		// f878:	82 4d 04 02 	mov	r13,	&0x0204	
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			CPPUNIT_ASSERT_EQUAL((uint16_t) (x == 0 ? 11 : 10), m->getBigEndian(0x0204));
		// f87c:	5c 42 06 02 	mov.b	&0x0206,r12	
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			CPPUNIT_ASSERT_EQUAL((uint16_t) x, (uint16_t) r->getp(12)->getByte());
		// f880:	4f 4c       	mov.b	r12,	r15	
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			CPPUNIT_ASSERT_EQUAL((uint16_t) x, (uint16_t) r->getp(15)->getByte());
		// f882:	0f 5f       	rla	r15		
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			CPPUNIT_ASSERT_EQUAL((uint16_t) (x*2), (uint16_t) r->getp(15)->getByte());
		// f884:	8f 4d 0a 02 	mov	r13,	522(r15);0x020a(r15)
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			CPPUNIT_ASSERT_EQUAL((uint16_t) (x == 0 ? 11 : 10), m->getBigEndian(r->getp(15)->getBigEndian() + 522));
		// f888:	8f 4e 2a 02 	mov	r14,	554(r15);0x022a(r15)
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			CPPUNIT_ASSERT_EQUAL((uint16_t) (11 + x*10), m->getBigEndian(r->getp(15)->getBigEndian() + 554));
		// f88c:	4f 4c       	mov.b	r12,	r15	
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			CPPUNIT_ASSERT_EQUAL((uint16_t) x, (uint16_t) r->getp(15)->getByte());
		// f88e:	5f 53       	inc.b	r15
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			CPPUNIT_ASSERT_EQUAL((uint8_t) (x + 1), r->getp(15)->getByte());
		// f890:	c2 4f 06 02 	mov.b	r15,	&0x0206	
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			CPPUNIT_ASSERT_EQUAL((uint8_t) (x + 1), m->getByte(0x0206));
		// f894:	7f 90 10 00 	cmp.b	#16,	r15	;#0x0010
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			if (x == 15) {
				CPPUNIT_ASSERT_EQUAL(true, r->getp(2)->isBitSet(SR_Z));
			// f898:	02 20       	jnz	$+6      	;abs 0xf89e
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((uint16_t) 0xf89a, r->getp(0)->getBigEndian());
			// f89a:	c2 43 06 02 	mov.b	#0,	&0x0206	;r3 As==00
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((uint8_t) 0, m->getByte(0x0206));
			}
			else {
				CPPUNIT_ASSERT_EQUAL(false, r->getp(2)->isBitSet(SR_Z));
			// f898:	02 20       	jnz	$+6      	;abs 0xf89e
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((uint16_t) 0xf89e, r->getp(0)->getBigEndian());
			}
		// f89e:	82 4e 02 02 	mov	r14,	&0x0202
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			CPPUNIT_ASSERT_EQUAL((uint16_t) (11 + x*10), m->getBigEndian(0x0202));
		// f8a2:	5f 42 08 02 	mov.b	&0x0208,r15
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			CPPUNIT_ASSERT_EQUAL((uint8_t) x, r->getp(15)->getByte());
		// f8a6:	5f 53       	inc.b	r15
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			CPPUNIT_ASSERT_EQUAL((uint8_t) (x + 1), r->getp(15)->getByte());
		// f8a8:	c2 4f 08 02 	mov.b	r15,	&0x0208
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			CPPUNIT_ASSERT_EQUAL((uint8_t) (x + 1), m->getByte(0x0208));
		// f8ac:	7f 90 20 00 	cmp.b	#32,	r15	;#0x0020
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			CPPUNIT_ASSERT_EQUAL(false, r->getp(2)->isBitSet(SR_Z));
		// f8b0:	02 20       	jnz	$+6      	;abs 0xf8b6
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			CPPUNIT_ASSERT_EQUAL((uint16_t) 0xf8b6, r->getp(0)->getBigEndian());
		// f8b6:	3c 41       	pop	r12
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
		// f8b8:	3d 41       	pop	r13
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
		// f8ba:	3e 41       	pop	r14
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
		// f8bc:	3f 41       	pop	r15
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
		// f8be:	00 13       	reti
			d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
			CPPUNIT_ASSERT_EQUAL((uint16_t) 0xf856, r->getp(0)->getBigEndian());
			intManager->handleInstruction(i);
		}

		CPPUNIT_ASSERT_EQUAL((uint16_t) 11, m->getBigEndian(0x20a));
		for (int x = 1; x < 16; ++x) {
			CPPUNIT_ASSERT_EQUAL((uint16_t) 10, m->getBigEndian(0x20a + 2*x));
		}
		for (int x = 0; x < 16; ++x) {
			CPPUNIT_ASSERT_EQUAL((uint16_t) (11 + (x*10)), m->getBigEndian(0x20a + 32 + 2*x));
		}

	}

};

CPPUNIT_TEST_SUITE_REGISTRATION (CaptureTest);

}
