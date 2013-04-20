#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "CPU/Memory/Memory.h"
#include "CPU/Memory/RegisterSet.h"
#include "CPU/Memory/Register.h"
#include "CPU/Instructions/InstructionDecoder.h"
#include "CPU/Instructions/InstructionManager.h"
#include "CPU/Instructions/Instruction.h"

#include <algorithm>

using namespace MCU;

#if 0

leds.elf:     file format elf32-msp430

SYMBOL TABLE:
0000f000 l    d  .text	00000000 .text
0000ffe0 l    d  .vectors	00000000 .vectors
00000000 l    d  .stab	00000000 .stab
00000000 l    d  .stabstr	00000000 .stabstr
00000056 l       *ABS*	00000000 DCOCTL
00000057 l       *ABS*	00000000 BCSCTL1
00000058 l       *ABS*	00000000 BCSCTL2
00000128 l       *ABS*	00000000 FCTL1
0000012a l       *ABS*	00000000 FCTL2
0000012c l       *ABS*	00000000 FCTL3
00000054 l       *ABS*	00000000 EPCTL
0000012e l       *ABS*	00000000 TA0IV
00000160 l       *ABS*	00000000 TA0CTL
00000170 l       *ABS*	00000000 TA0R
00000162 l       *ABS*	00000000 TA0CCTL0
00000164 l       *ABS*	00000000 TA0CCTL1
00000172 l       *ABS*	00000000 TA0CCR0
00000174 l       *ABS*	00000000 TA0CCR1
00000166 l       *ABS*	00000000 TA0CCTL2
00000176 l       *ABS*	00000000 TA0CCR2
00000020 l       *ABS*	00000000 P1IN
00000021 l       *ABS*	00000000 P1OUT
00000022 l       *ABS*	00000000 P1DIR
00000023 l       *ABS*	00000000 P1IFG
00000024 l       *ABS*	00000000 P1IES
00000025 l       *ABS*	00000000 P1IE
00000026 l       *ABS*	00000000 P1SEL
00000028 l       *ABS*	00000000 P2IN
00000029 l       *ABS*	00000000 P2OUT
0000002a l       *ABS*	00000000 P2DIR
0000002b l       *ABS*	00000000 P2IFG
0000002c l       *ABS*	00000000 P2IES
0000002d l       *ABS*	00000000 P2IE
0000002e l       *ABS*	00000000 P2SEL
00000120 l       *ABS*	00000000 WDTCTL
00000000 l       *ABS*	00000000 IE1
00000002 l       *ABS*	00000000 IFG1
00000000 l    df *ABS*	00000000 main.c
00000130 l       *ABS*	00000000 __MPY
00000132 l       *ABS*	00000000 __MPYS
00000134 l       *ABS*	00000000 __MAC
00000136 l       *ABS*	00000000 __MACS
00000138 l       *ABS*	00000000 __OP2
0000013a l       *ABS*	00000000 __RESLO
0000013c l       *ABS*	00000000 __RESHI
0000013e l       *ABS*	00000000 __SUMEXT
00000000 g       *ABS*	00000000 __data_size
0000f0b0 g       .text	00000000 _etext
00000000 g       *ABS*	00000000 __bss_size
0000f0ae  w      .text	00000000 __stop_progExec__
0000f030 g       .text	00000000 _unexpected_1_
0000f030  w      .text	00000000 vector_ffe0
0000f030  w      .text	00000000 vector_ffec
0000f030  w      .text	00000000 vector_fff0
0000f0b0 g       *ABS*	00000000 __data_load_start
0000f030 g       .text	00000000 __dtors_end
0000f030  w      .text	00000000 vector_fffc
0000f030  w      .text	00000000 vector_ffe4
0000ffe0 g     O .vectors	00000020 InterruptVectors
0000f01c  w      .text	00000000 __do_clear_bss
0000f030  w      .text	00000000 vector_ffe2
0000f030  w      .text	00000000 vector_ffe8
0000f034  w      .text	00000000 _unexpected_
0000f030  w      .text	00000000 vector_fffa
0000f000  w      .text	00000000 _reset_vector__
0000f030 g       .text	00000000 __ctors_start
0000f00a  w      .text	00000000 __do_copy_data
00000200 g       .text	00000000 __bss_start
0000f030  w      .text	00000000 vector_ffee
0000f030  w      .text	00000000 vector_fff4
0000f04e g     F .text	00000060 main
0000f030  w      .text	00000000 vector_fff8
0000f030  w      .text	00000000 vector_fff2
00010000 g       .vectors	00000000 _vectors_end
0000f030  w      .text	00000000 vector_ffe6
0000f036 g     F .text	00000010 delay
0000f000  w      .text	00000000 __init_stack
0000f030 g       .text	00000000 __dtors_start
0000f030 g       .text	00000000 __ctors_end
00000300 g       *ABS*	00000000 __stack
00000200 g       .text	00000000 _edata
00000200 g       .text	00000000 _end
0000f030  w      .text	00000000 vector_fff6
0000f004  w      .text	00000000 __low_level_init
0000f02c  w      .text	00000000 __jump_to_main
00000200 g       .text	00000000 __data_start
00000120 g       *ABS*	00000000 __WDTCTL
0000f030  w      .text	00000000 vector_ffea



Disassembly of section .text:

0000f000 <__init_stack>:
    f000:	31 40 00 03 	mov	#768,	r1	;#0x0300

0000f004 <__low_level_init>:
    f004:	b2 40 80 5a 	mov	#23168,	&0x0120	;#0x5a80
    f008:	20 01

0000f00a <__do_copy_data>:
    f00a:	3f 40 00 00 	mov	#0,	r15	;#0x0000
    f00e:	0f 93       	tst	r15
    f010:	05 24       	jz	$+12     	;abs 0xf01c
    f012:	2f 83       	decd	r15
    f014:	9f 4f b0 f0 	mov	-3920(r15),512(r15);0xf0b0(r15), 0x0200(r15)
    f018:	00 02
    f01a:	fb 23       	jnz	$-8      	;abs 0xf012

0000f01c <__do_clear_bss>:
    f01c:	3f 40 00 00 	mov	#0,	r15	;#0x0000
    f020:	0f 93       	tst	r15
    f022:	04 24       	jz	$+10     	;abs 0xf02c
    f024:	1f 83       	dec	r15
    f026:	cf 43 00 02 	mov.b	#0,	512(r15);r3 As==00, 0x0200(r15)
    f02a:	fc 23       	jnz	$-6      	;abs 0xf024

0000f02c <__jump_to_main>:
    f02c:	30 40 4e f0 	br	#0xf04e

0000f030 <__ctors_end>:
    f030:	30 40 34 f0 	br	#0xf034

0000f034 <_unexpected_>:
    f034:	00 13       	reti

0000f036 <delay>:
    f036:	0e 43       	clr	r14
    f038:	0e 9f       	cmp	r15,	r14	;
    f03a:	04 2c       	jc	$+10     	;abs 0xf044
    f03c:	03 43       	nop
    f03e:	1e 53       	inc	r14
    f040:	0e 9f       	cmp	r15,	r14
    f042:	fc 2b       	jnc	$-6      	;abs 0xf03c
    f044:	30 41       	ret
    f046:	00 00       	.word	0x0000;	????
    f048:	01 00       	.word	0x0001;	????
    f04a:	40 00       	.word	0x0040;	????
    f04c:	41 00       	.word	0x0041;	????

0000f04e <main>:
    f04e:	31 40 f8 02 	mov	#760,	r1	;#0x02f8
    f052:	b2 40 80 5a 	mov	#23168,	&0x0120	;#0x5a80
    f056:	20 01
    f058:	f2 43 22 00 	mov.b	#-1,	&0x0022	;r3 As==11
    f05c:	d2 43 21 00 	mov.b	#1,	&0x0021	;r3 As==01
    f060:	0b 43       	clr	r11
    f062:	3f 40 46 f0 	mov	#-4026,	r15	;#0xf046
    f066:	b1 4f 00 00 	mov	@r15+,	0(r1)	;0x0000(r1)
    f06a:	b1 4f 02 00 	mov	@r15+,	2(r1)	;0x0002(r1)
    f06e:	b1 4f 04 00 	mov	@r15+,	4(r1)	;0x0004(r1)
    f072:	b1 4f 06 00 	mov	@r15+,	6(r1)	;0x0006(r1)
    f076:	0f 4b       	mov	r11,	r15
    f078:	0f 5f       	rla	r15
    f07a:	0f 51       	add	r1,	r15
    f07c:	e2 4f 21 00 	mov.b	@r15,	&0x0021
    f080:	3f 40 30 00 	mov	#48,	r15	;#0x0030
    f084:	b0 12 36 f0 	call	#0xf036
    f088:	3f 40 30 00 	mov	#48,	r15	;#0x0030
    f08c:	b0 12 36 f0 	call	#0xf036
    f090:	3f 40 30 00 	mov	#48,	r15	;#0x0030
    f094:	b0 12 36 f0 	call	#0xf036
    f098:	3f 40 30 00 	mov	#48,	r15	;#0x0030
    f09c:	b0 12 36 f0 	call	#0xf036
    f0a0:	1b 53       	inc	r11
    f0a2:	2b 92       	cmp	#4,	r11	;r2 As==10
    f0a4:	de 3b       	jl	$-66     	;abs 0xf062
    f0a6:	dc 3f       	jmp	$-70     	;abs 0xf060
    f0a8:	31 52       	add	#8,	r1	;r2 As==11
    f0aa:	30 40 ae f0 	br	#0xf0ae

0000f0ae <__stop_progExec__>:
    f0ae:	ff 3f       	jmp	$+0      	;abs 0xf0ae

Disassembly of section .vectors:

0000ffe0 <InterruptVectors>:
    ffe0:	30 f0 30 f0 30 f0 30 f0 30 f0 30 f0 30 f0 30 f0     0.0.0.0.0.0.0.0.
    fff0:	30 f0 30 f0 30 f0 30 f0 30 f0 30 f0 30 f0 00 f0     0.0.0.0.0.0.0...

#endif

std::string binary(unsigned n) {
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

class BlinkingLedTest : public CPPUNIT_NS :: TestFixture {
	CPPUNIT_TEST_SUITE(BlinkingLedTest);
	CPPUNIT_TEST(execute);
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

	void execute() {
			std::string data = ""
				":10F0000031400003B240805A20013F4000000F937E\r\n"
				":10F0100005242F839F4FB0F00002FB233F400000E8\r\n"
				":10F020000F9304241F83CF430002FC2330404EF093\r\n"
				":10F03000304034F000130E430E9F042C03431E5344\r\n"
				":10F040000E9FFC2B30410000010040004100314088\r\n"
				":10F05000F802B240805A2001F2432200D24321003C\r\n"
				":10F060000B433F4046F0B14F0000B14F0200B14F9B\r\n"
				":10F070000400B14F06000F4B0F5F0F51E24F21000C\r\n"
				":10F080003F403000B01236F03F403000B01236F052\r\n"
				":10F090003F403000B01236F03F403000B01236F042\r\n"
				":10F0A0001B532B92DE3BDC3F31523040AEF0FF3F32\r\n"
				":10FFE00030F030F030F030F030F030F030F030F011\r\n"
				":10FFF00030F030F030F030F030F030F030F000F031\r\n"
				":040000030000F00009\r\n"
				":00000001FF\r\n";

			m->loadA43(data, r);

// 			f000:	31 40 00 03 	mov	#768,	r1	;#0x0300
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((uint16_t) 0x0300, r->get(1)->getBigEndian());
// 			f004:	b2 40 80 5a 	mov	#23168,	&0x0120	;#0x5a80
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((uint16_t) 0x5a80, m->getBigEndian(0x0120));
// 			f00a:	3f 40 00 00 	mov	#0,	r15	;#0x0000
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((uint16_t) 0, r->get(15)->getBigEndian());
// 			f00e:	0f 93       	tst	r15
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL(true, r->get(2)->isBitSet(SR_Z));
// 			f010:	05 24       	jz	$+12     	;abs 0xf01c
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((uint16_t) 0xf01c, r->get(0)->getBigEndian());
// 			f01c:	3f 40 00 00 	mov	#0,	r15	;#0x0000
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((uint16_t) 0, r->get(15)->getBigEndian());
// 			f020:	0f 93       	tst	r15
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL(true, r->get(1)->isBitSet(SR_Z));
// 			f022:	04 24       	jz	$+10     	;abs 0xf02c
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((uint16_t) 0xf02c, r->get(0)->getBigEndian());
// 			f02c:	30 40 4e f0 	br	#0xf04e
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((uint16_t) 0xf04e, r->get(0)->getBigEndian());
// 			f04e:	31 40 f8 02 	mov	#760,	r1	;#0x02f8
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((uint16_t) 0x02f8, r->get(1)->getBigEndian());
// 			f052:	b2 40 80 5a 	mov	#23168,	&0x0120	;#0x5a80
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((uint16_t) 0x5a80, m->getBigEndian(0x0120));
// 			f058:	f2 43 22 00 	mov.b	#-1,	&0x0022	;r3 As==11
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((int8_t) -1, (int8_t) m->getByte(0x0022));
				CPPUNIT_ASSERT_EQUAL(std::string("11111111"), binary(m->getByte(0x0022))); // I know, but to be really sure...
// ON		f05c:	d2 43 21 00 	mov.b	#1,	&0x0021	;r3 As==01
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((int) 1, (int) m->getByte(0x0021));
// 			f060:	0b 43       	clr	r11
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((uint16_t) 0, r->get(11)->getBigEndian());
		for (int cycles4 = 0; cycles4 < 4; cycles4++) {
// 			f062:	3f 40 46 f0 	mov	#-4026,	r15	;#0xf046
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((int16_t) -4026, (int16_t) r->get(15)->getBigEndian());
// 			f066:	b1 4f 00 00 	mov	@r15+,	0(r1)	;0x0000(r1)
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((uint16_t) 0x0000, m->getBigEndian(0x02f8));
// 			f06a:	b1 4f 02 00 	mov	@r15+,	2(r1)	;0x0002(r1)
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((uint16_t) 0x0001, m->getBigEndian(0x02f8 + 2));
// 			f06e:	b1 4f 04 00 	mov	@r15+,	4(r1)	;0x0004(r1)
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((uint16_t) 0x0040, m->getBigEndian(0x02f8 + 4));
// 			f072:	b1 4f 06 00 	mov	@r15+,	6(r1)	;0x0006(r1)
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((uint16_t) 0x0041, m->getBigEndian(0x02f8 + 6));
// 			f076:	0f 4b       	mov	r11,	r15
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((uint16_t) cycles4, r->get(15)->getBigEndian());
// 			f078:	0f 5f       	rla	r15
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((uint16_t) (cycles4 + cycles4), r->get(15)->getBigEndian());
// 			f07a:	0f 51       	add	r1,	r15
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((uint16_t) (0x02f8 + (cycles4 + cycles4)), r->get(15)->getBigEndian());
// OFF		f07c:	e2 4f 21 00 	mov.b	@r15,	&0x0021
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				if (cycles4 == 0) {
					CPPUNIT_ASSERT_EQUAL((int) 0x00, (int) m->getByte(0x0021));
				}
				else if (cycles4 == 1) {
					CPPUNIT_ASSERT_EQUAL((int) 0x01, (int) m->getByte(0x0021));
				}
				else if (cycles4 == 2) {
					CPPUNIT_ASSERT_EQUAL((int) 64, (int) m->getByte(0x0021));
				}
				else if (cycles4 == 3) {
					CPPUNIT_ASSERT_EQUAL((int) 65, (int) m->getByte(0x0021));
				}
// DELAY1	f080:	3f 40 30 00 	mov	#48,	r15	;#0x0030
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((uint16_t) 48, r->get(15)->getBigEndian());
// 			f084:	b0 12 36 f0 	call	#0xf036
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((uint16_t) 0xf036, r->get(0)->getBigEndian());
// 			f036:	0e 43       	clr	r14
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((uint16_t) 0, r->get(14)->getBigEndian());
// 			f038:	0e 9f       	cmp	r15,	r14	;
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL(false, r->get(2)->isBitSet(SR_C));
// 			f03a:	04 2c       	jc	$+10     	;abs 0xf044
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((uint16_t) 0xf03c, r->get(0)->getBigEndian());
			for (int x = 1; x < 49; x++) {
// 			f03c:	03 43       	nop
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
// 			f03e:	1e 53       	inc	r14
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((uint16_t) x, r->get(14)->getBigEndian());
// 			f040:	0e 9f       	cmp	r15,	r14
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				if (x == 48) {
					CPPUNIT_ASSERT_EQUAL(true, r->get(2)->isBitSet(SR_C));
				}
				else {
					CPPUNIT_ASSERT_EQUAL(false, r->get(2)->isBitSet(SR_C));
				}
// 			f042:	fc 2b       	jnc	$-6      	;abs 0xf03c
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				if (x == 48) {
					CPPUNIT_ASSERT_EQUAL((uint16_t) 0xf044, r->get(0)->getBigEndian());
				}
				else {
					CPPUNIT_ASSERT_EQUAL((uint16_t) 0xf03c, r->get(0)->getBigEndian());
				}
			}
// 			f044:	30 41       	ret
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((uint16_t) 0xf088, r->get(0)->getBigEndian());
// DELAY2	f088:	3f 40 30 00 	mov	#48,	r15	;#0x0030
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((uint16_t) 48, r->get(15)->getBigEndian());
//			f08c:	b0 12 36 f0 	call	#0xf036
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((uint16_t) 0xf036, r->get(0)->getBigEndian());
// 			f036:	0e 43       	clr	r14
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((uint16_t) 0, r->get(14)->getBigEndian());
// 			f038:	0e 9f       	cmp	r15,	r14	;
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL(false, r->get(2)->isBitSet(SR_C));
// 			f03a:	04 2c       	jc	$+10     	;abs 0xf044
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((uint16_t) 0xf03c, r->get(0)->getBigEndian());
			for (int x = 1; x < 49; x++) {
// 			f03c:	03 43       	nop
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
// 			f03e:	1e 53       	inc	r14
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((uint16_t) x, r->get(14)->getBigEndian());
// 			f040:	0e 9f       	cmp	r15,	r14
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				if (x == 48) {
					CPPUNIT_ASSERT_EQUAL(true, r->get(2)->isBitSet(SR_C));
				}
				else {
					CPPUNIT_ASSERT_EQUAL(false, r->get(2)->isBitSet(SR_C));
				}
// 			f042:	fc 2b       	jnc	$-6      	;abs 0xf03c
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				if (x == 48) {
					CPPUNIT_ASSERT_EQUAL((uint16_t) 0xf044, r->get(0)->getBigEndian());
				}
				else {
					CPPUNIT_ASSERT_EQUAL((uint16_t) 0xf03c, r->get(0)->getBigEndian());
				}
			}
// 			f044:	30 41       	ret
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((uint16_t) 0xf090, r->get(0)->getBigEndian());
// DELAY3	f090:	3f 40 30 00 	mov	#48,	r15	;#0x0030
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((uint16_t) 48, r->get(15)->getBigEndian());
//			f08c:	b0 12 36 f0 	call	#0xf036
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((uint16_t) 0xf036, r->get(0)->getBigEndian());
// 			f036:	0e 43       	clr	r14
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((uint16_t) 0, r->get(14)->getBigEndian());
// 			f038:	0e 9f       	cmp	r15,	r14	;
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL(false, r->get(2)->isBitSet(SR_C));
// 			f03a:	04 2c       	jc	$+10     	;abs 0xf044
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((uint16_t) 0xf03c, r->get(0)->getBigEndian());
			for (int x = 1; x < 49; x++) {
// 			f03c:	03 43       	nop
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
// 			f03e:	1e 53       	inc	r14
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((uint16_t) x, r->get(14)->getBigEndian());
// 			f040:	0e 9f       	cmp	r15,	r14
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				if (x == 48) {
					CPPUNIT_ASSERT_EQUAL(true, r->get(2)->isBitSet(SR_C));
				}
				else {
					CPPUNIT_ASSERT_EQUAL(false, r->get(2)->isBitSet(SR_C));
				}
// 			f042:	fc 2b       	jnc	$-6      	;abs 0xf03c
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				if (x == 48) {
					CPPUNIT_ASSERT_EQUAL((uint16_t) 0xf044, r->get(0)->getBigEndian());
				}
				else {
					CPPUNIT_ASSERT_EQUAL((uint16_t) 0xf03c, r->get(0)->getBigEndian());
				}
			}
// 			f044:	30 41       	ret
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((uint16_t) 0xf098, r->get(0)->getBigEndian());
// DELAY4	f098:	3f 40 30 00 	mov	#48,	r15	;#0x0030
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((uint16_t) 48, r->get(15)->getBigEndian());
//			f08c:	b0 12 36 f0 	call	#0xf036
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((uint16_t) 0xf036, r->get(0)->getBigEndian());
// 			f036:	0e 43       	clr	r14
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((uint16_t) 0, r->get(14)->getBigEndian());
// 			f038:	0e 9f       	cmp	r15,	r14	;
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL(false, r->get(2)->isBitSet(SR_C));
// 			f03a:	04 2c       	jc	$+10     	;abs 0xf044
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((uint16_t) 0xf03c, r->get(0)->getBigEndian());
			for (int x = 1; x < 49; x++) {
// 			f03c:	03 43       	nop
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
// 			f03e:	1e 53       	inc	r14
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((uint16_t) x, r->get(14)->getBigEndian());
// 			f040:	0e 9f       	cmp	r15,	r14
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				if (x == 48) {
					CPPUNIT_ASSERT_EQUAL(true, r->get(2)->isBitSet(SR_C));
				}
				else {
					CPPUNIT_ASSERT_EQUAL(false, r->get(2)->isBitSet(SR_C));
				}
// 			f042:	fc 2b       	jnc	$-6      	;abs 0xf03c
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				if (x == 48) {
					CPPUNIT_ASSERT_EQUAL((uint16_t) 0xf044, r->get(0)->getBigEndian());
				}
				else {
					CPPUNIT_ASSERT_EQUAL((uint16_t) 0xf03c, r->get(0)->getBigEndian());
				}
			}
// 			f044:	30 41       	ret
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((uint16_t) 0xf0a0, r->get(0)->getBigEndian());
// AFTER	f0a0:	1b 53       	inc	r11
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				CPPUNIT_ASSERT_EQUAL((uint16_t) (cycles4 + 1), r->get(11)->getBigEndian());
// 			f0a2:	2b 92       	cmp	#4,	r11	;r2 As==10
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
// 			f0a4:	de 3b       	jl	$-66     	;abs 0xf062
				d->decodeCurrentInstruction(i); executeInstruction(r, m, i);
				if (cycles4 == 3) {
					CPPUNIT_ASSERT_EQUAL((uint16_t) 0xf0a6, r->get(0)->getBigEndian());
				}
				else {
					CPPUNIT_ASSERT_EQUAL((uint16_t) 0xf062, r->get(0)->getBigEndian());
				}
		}
	}

};

CPPUNIT_TEST_SUITE_REGISTRATION (BlinkingLedTest);
