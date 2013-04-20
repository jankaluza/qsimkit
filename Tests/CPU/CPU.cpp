#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "CPU/CPU.h"
#include "CPU/Memory/Memory.h"
#include "CPU/Memory/RegisterSet.h"
#include "CPU/Memory/Register.h"

#include "adevs.h"

using namespace MCU;

class CPUTest : public CPPUNIT_NS :: TestFixture{
	CPPUNIT_TEST_SUITE(CPUTest);
	CPPUNIT_TEST(simulation);
	CPPUNIT_TEST_SUITE_END();

	adevs::SimpleDigraph<Event *> *dig;
	CPU *c;
	adevs::Simulator<Event *> *sim;

	public:
		void setUp (void) {
			dig = new adevs::SimpleDigraph<Event *>();
			c = new CPU();
			dig->add(c);
			sim = new adevs::Simulator<Event *>(dig);
		}

		void tearDown (void) {
			delete sim;
			delete dig;
		}

		void simulation() {
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

			c->loadA43(data);

			while (sim->nextEventTime() < 200.0) {
				sim->execNextEvent();
			}
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION (CPUTest);
