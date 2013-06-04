#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "CPU/Memory/Register.h"

using namespace MSP430;

class RegisterTest : public CPPUNIT_NS :: TestFixture{
	CPPUNIT_TEST_SUITE(RegisterTest);
	CPPUNIT_TEST(setget);
	CPPUNIT_TEST(setByte);
	CPPUNIT_TEST(setBit);
	CPPUNIT_TEST_SUITE_END();

	public:
		void setUp (void) {
			
		}

		void tearDown (void) {

		}

		void setget() {
			Register r(0, "R0", 55, "desc");
			CPPUNIT_ASSERT_EQUAL((uint16_t) 55, r.getBigEndian());
			r.setBigEndian(0xff00);
			CPPUNIT_ASSERT_EQUAL((uint16_t) 0xff00, r.getBigEndian());

			r.set(0xfffc); // -4
			CPPUNIT_ASSERT_EQUAL((int16_t) -4, (int16_t) r.get());
			CPPUNIT_ASSERT_EQUAL((int16_t) 0, (int16_t) ((uint32_t) r.get() + 4));
		}

		void setByte() {
			Register r(0, "R0", 0x0203, "desc");
			CPPUNIT_ASSERT_EQUAL((int) 3, (int) r.getByte());
			r.setByte(0x04);
			CPPUNIT_ASSERT_EQUAL((int) 4, (int) r.getByte());
			CPPUNIT_ASSERT_EQUAL((uint16_t) 0x0204, r.getBigEndian());
		}

		void setBit() {
			Register r(0, "R0", 0x0203, "desc");
			r.setBit(SR_V, false);
			CPPUNIT_ASSERT_EQUAL(false, r.isBitSet(SR_V));
			r.setBit(SR_V, true);
			CPPUNIT_ASSERT_EQUAL(true, r.isBitSet(SR_V));
			r.setBit(SR_V, false);
			CPPUNIT_ASSERT_EQUAL(false, r.isBitSet(SR_V));
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION (RegisterTest);
