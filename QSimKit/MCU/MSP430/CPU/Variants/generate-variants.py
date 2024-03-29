import os
import sys
import subprocess
from configobj import ConfigObj

double_defines = []

defines = ["P1DIR_","P1IE_","P1IES_","P1IFG_","P1IN_","P1OUT_","P1REN_","P1SEL_"]
defines += ["P2DIR_","P2IE_","P2IES_","P2IFG_","P2IN_","P2OUT_","P2REN_","P2SEL_"]
defines += ["P3DIR_", "P3IN_","P3OUT_","P3REN_","P3SEL_"]
defines += ["P4DIR_", "P4IN_","P4OUT_","P4REN_","P4SEL_"]
defines += ["P5DIR_", "P5IN_","P5OUT_","P5REN_","P5SEL_"]
defines += ["P6DIR_", "P6IN_","P6OUT_","P6REN_","P6SEL_"]
defines += ["P7DIR_", "P7IN_","P7OUT_","P7REN_","P7SEL_"]
defines += ["P8DIR_", "P8IN_","P8OUT_","P8REN_","P8SEL_"]
defines += ["P9DIR_", "P9IN_","P9OUT_","P9REN_","P9SEL_"]
defines += ["P10DIR_", "P10IN_","P10OUT_","P10REN_","P10SEL_"]

defines += ["PORT1_VECTOR", "PORT2_VECTOR"]

# Clock
defines += ["DCOCTL_", "BCSCTL1_", "BCSCTL2_", "BCSCTL3_"]

# Timer A
defines += ["TA0R_", "TA1R_", "TACCR0_"]
defines += ["TA0CTL_", "TA1CTL_"]
defines += ["TA0CCR0_", "TA0CCR1_", "TA0CCR2_", "TA0CCR3_", "TA0CCR4_"]
defines += ["TA1CCR0_", "TA1CCR1_", "TA1CCR2_", "TA1CCR3_", "TA1CCR4_"]
defines += ["TA0CCTL0_", "TA0CCTL1_", "TA0CCTL2_", "TA0CCTL3_", "TA0CCTL4_"]
defines += ["TA1CCTL0_", "TA1CCTL1_", "TA1CCTL2_", "TA1CCTL3_", "TA1CCTL4_"]
defines += ["TA0IV_", "TA1IV_"]

defines += ["TIMERA0_VECTOR", "TIMERA1_VECTOR"]
defines += ["TIMER0_A0_VECTOR", "TIMER0_A1_VECTOR", "TIMER1_A0_VECTOR", "TIMER1_A1_VECTOR"]

# Timer B
defines += ["TBR_", "TBCTL_"]
defines += ["TBCCR0_", "TBCCR1_", "TBCCR2_", "TBCCR3_", "TBCCR4_", "TBCCR5_", "TBCCR6_"]
defines += ["TBCCTL0_", "TBCCTL1_", "TBCCTL2_", "TBCCTL3_", "TBCCTL4_", "TBCCTL5_", "TBCCTL6_"]
defines += ["TBIV_"]

defines += ["TIMERB0_VECTOR", "TIMERB1_VECTOR"]

defines += ["INTVECT"]

# USI
defines += ["USISR_", "USICTL_", "USICCTL_", "USI_VECTOR"]

# USCI
defines += ["UCA0CTL0_", "UCA0CTL1_", "UCA0BR0_", "UCA0BR1_", "UCA0MCTL_", "UCA0STAT_"]
defines += ["UCA0RXBUF_", "UCA0TXBUF_", "UCA0ABCTL_", "UCA0IRTCTL_", "UCA0IRRCTL_"]
defines += ["UCB0CTL0_", "UCB0CTL1_", "UCB0BR0_", "UCB0BR1_", "UCB0MCTL_", "UCB0I2CIE_", "UCB0STAT_"]
defines += ["UCB0RXBUF_", "UCB0TXBUF_", "UCB0I2COA_", "UCB0I2CSA_"]
defines += ["USCIAB0TX_VECTOR", "USCIAB0RX_VECTOR"]

defines += ["UCA1CTL0_", "UCA1CTL1_", "UCA1BR0_", "UCA1BR1_", "UCA1MCTL_", "UCA1STAT_"]
defines += ["UCA1RXBUF_", "UCA1TXBUF_", "UCA1ABCTL_", "UCA1IRTCTL_", "UCA1IRRCTL_"]
defines += ["UCB1CTL0_", "UCB1CTL1_", "UCB1BR0_", "UCB1BR1_", "UCB1I2CIE_", "UCB1STAT_"]
defines += ["UCB1RXBUF_", "UCB1TXBUF_", "UCB1I2COA_", "UCB1I2CSA_"]
defines += ["USCIAB1TX_VECTOR", "USCIAB1RX_VECTOR"]

defines += ["UC0IE", "UC0IFG"]
defines += ["UC1IE", "UC1IFG"]

# USART
defines += ["U0CTL", "U0TCTL_", "U0RCTL_", "U0MCTL_", "U0BR0_", "U0BR1_", "U0RXBUF_", "U0TXBUF_"]
defines += ["U0ME", "U0IE", "U0IFG"]
defines += ["USART0TX_VECTOR", "USART0RX_VECTOR"]
defines += ["UTXIE0", "URXIE0", "UTXIFG0", "URXIFG0", "USPIE0"];

defines += ["U1CTL", "U1TCTL_", "U1RCTL_", "U1MCTL_", "U1BR0_", "U1BR1_", "U1RXBUF_", "U1TXBUF_"]
defines += ["U1ME", "U1IE", "U1IFG"]
defines += ["USART1TX_VECTOR", "USART1RX_VECTOR"]
defines += ["UTXIE1", "URXIE1", "UTXIFG1", "URXIFG1", "USPIE1"];

# Config files
double_defines += ["SRSEL", "SDCO", "DCOZERO"]


def pdef(define):
	if (define[-1] == "_"):
		return define[:-1]
	return define

output = """#pragma once

class Variant {
	public:
		virtual const char *getName() = 0;
"""

for define in defines:
	output += "		virtual unsigned int get%s() = 0;\n" % (pdef(define))
for define in double_defines:
	output += "		virtual double get%s() = 0;\n" % (pdef(define))

output += """
};
"""

f = open("Variant.h", "w")
f.write(output)
f.close()

for header in os.listdir("../../../../../3rdparty/msp430gcc"):
	if not header.startswith("msp430x"):
		continue

	output = ""

	child = subprocess.Popen("msp430-gcc -D__ASSEMBLER__=1 -E -dM -I../../../../../3rdparty/msp430gcc/ ../../../../../3rdparty/msp430gcc/" + header, shell=True, stdout=subprocess.PIPE)
	for line in child.communicate()[0].split('\n'):
		if len(line) == 0 or (line.find("(") != -1 and line.find("<<") == -1):
			continue
		output += "#ifndef %s\n" % (line.split(' ')[1])
		output += line + "\n"
		output += "#endif\n"
		if (line.split(' ')[1].endswith("_")):
			output += "#ifndef %s\n" % (line.split(' ')[1][:-1])
			output += line.replace(line.split(' ')[1], line.split(' ')[1][:-1]) + "\n"
			output += "#endif\n"

	name = header[:-2]

	if os.path.exists(name + ".cfg"):
		config = ConfigObj(name + ".cfg")
		for k, v in config.iteritems():
			output += "#define " + k + " " + v + "\n";

	output += """#include "CPU/Variants/Variant.h"
#include "CPU/Variants/VariantManager.h"

class Variant_%s : public Variant {
	public:
		Variant_%s() {}
		virtual ~Variant_%s() {}
		const char *getName() { return "%s"; }
""" % (name, name, name, name)

	for define in defines:
		output += """
		unsigned int get%s() {
#ifdef %s
			return %s;
#else
			return 0;
#endif
		}

""" % (pdef(define), define, define)

	for define in double_defines:
		output += """
		double get%s() {
#ifdef %s
			return %s;
#else
			return 0;
#endif
		}

""" % (pdef(define), define, define)

	output += """
};

MSP430_VARIANT("%s", %s);

""" %(name, name)

	f = open(name + ".cpp", "w")
	f.write(output)
	f.close()
