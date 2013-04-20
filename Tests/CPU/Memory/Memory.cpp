#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "CPU/Memory/Memory.h"
#include "CPU/Memory/RegisterSet.h"
#include "CPU/Memory/Register.h"

using namespace MCU;

class MemoryTest : public CPPUNIT_NS :: TestFixture{
	CPPUNIT_TEST_SUITE(MemoryTest);
	CPPUNIT_TEST(loadA43);
	CPPUNIT_TEST(setget);
	CPPUNIT_TEST_SUITE_END();

	public:
		void setUp (void) {
			
		}

		void tearDown (void) {

		}

		void setget() {
			Memory m(120000);
			m.setByte(1, 0xff);
			m.setByte(2, 0x00);
			CPPUNIT_ASSERT_EQUAL((uint8_t) 0xff, m.getByte(1));
			CPPUNIT_ASSERT_EQUAL((uint8_t) 0x00, m.getByte(2));

			CPPUNIT_ASSERT_EQUAL((uint16_t) 0x00ff, m.getBigEndian(1));
			CPPUNIT_ASSERT_EQUAL((uint16_t) 0xff00, m.get(1));

			m.set(1, 0xf000);
			CPPUNIT_ASSERT_EQUAL((uint16_t) 0xf000, m.get(1));

			m.setBigEndian(1, 0xf000);
			CPPUNIT_ASSERT_EQUAL((uint16_t) 0xf000, m.getBigEndian(1));
		}

		void loadA43() {
			Memory m(120000);
			RegisterSet r;
			r.addRegister("PC", 0);

			std::string data =
				":10F0000031400003B240805A20013F4000000F937E\r\n"
				":10F0100005242F839F4FB0F00002FB233F400000E8\r\n"
				":10F020000F9304241F83CF430002FC2330404EF093\r\n"
				":10F03000304034F000130E430E9F042C03431E5344\r\n"
				":10F040000E9FFC2B30410000010040004100314088\r\n"
				":10F05000F802B240805A2001F2432200D24321003C\r\n"
				":10F060000B433F4046F0B14F0000B14F0200B14F9B\r\n"
				":10F070000400B14F06000F4B0F5F0F51E24F21000C\r\n"
				":10F080003F40FF4FB01236F03F40FF4FB01236F016\r\n"
				":10F090003F40FF4FB01236F03F40FF4FB01236F006\r\n"
				":10F0A0001B532B92DE3BDC3F31523040AEF0FF3F32\r\n"
				":10FFE00030F030F030F030F030F030F030F030F011\r\n"
				":10FFF00030F030F030F030F030F030F030F000F031\r\n"
				":040000030000F00009\r\n"
				":00000001FF\r\n";


			bool ret = m.loadA43(data, &r);
			CPPUNIT_ASSERT_EQUAL(true, ret);
			CPPUNIT_ASSERT_EQUAL((uint8_t) 4, m.getByte(61552)); // 0xF070
			// little endian...
			CPPUNIT_ASSERT_EQUAL((uint16_t) 1024, m.get(61552)); // 0xF070
			// ... big endian
			CPPUNIT_ASSERT_EQUAL((uint16_t) 4, m.getBigEndian(61552)); // 0xF070
			// check if PC is set properly
			CPPUNIT_ASSERT_EQUAL((uint16_t) 61440, r[0]->getBigEndian()); // 0xF000
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION (MemoryTest);
