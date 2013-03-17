import os
import sys
import subprocess

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

output = """#pragma once

class Variant {
	public:
"""

for define in defines:
	output += "		virtual unsigned int get%s() = 0;\n" % (define[:-1])

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

	child = subprocess.Popen("msp430-gcc -D__ASSEMBLER__=1 -E -dM ../../3rdparty/msp430gcc/" + header, shell=True, stdout=subprocess.PIPE)
	for line in child.communicate()[0].split('\n'):
		if len(line) == 0 or line.find("(") != -1:
			continue
		output += "#ifndef %s\n" % (line.split(' ')[1])
		output += line + "\n"
		output += "#endif\n"

	name = header[:-2]
	output += """#include "CPU/Variants/Variant.h"
#include "CPU/Variants/VariantManager.h"

class Variant_%s : public Variant {
	public:
		Variant_%s() {}
		virtual ~Variant_%s() {}
""" % (name, name, name)

	for define in defines:
		output += """
		unsigned int get%s() {
#ifdef %s
			return %s;
#else
			return 0;
#endif
		}

""" % (define[:-1], define, define)

	output += """
};

MSP430_VARIANT("%s", %s);

""" %(name, name)

	f = open(name + ".cpp", "w")
	f.write(output)
	f.close()