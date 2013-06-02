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

for header in os.listdir("../../3rdparty/msp430gcc"):
	if not header.startswith("msp430x"):
		continue

	output = ""

	child = subprocess.Popen("msp430-gcc -D__ASSEMBLER__=1 -E -dM -I../../3rdparty/msp430gcc/ ../../3rdparty/msp430gcc/" + header, shell=True, stdout=subprocess.PIPE)
	for line in child.communicate()[0].split('\n'):
		if len(line) == 0 or line.find("(") != -1:
			continue
		output += "#ifndef %s\n" % (line.split(' ')[1])
		output += line + "\n"
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
