#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "CPU/Memory/Register.h"

class RegisterTest : public CPPUNIT_NS :: TestFixture{
	CPPUNIT_TEST_SUITE(RegisterTest);
	CPPUNIT_TEST(setget);
	CPPUNIT_TEST(setByte);
	CPPUNIT_TEST_SUITE_END();

	public:
		void setUp (void) {
			
		}

		void tearDown (void) {

		}

		void setget() {
			Register r("R0", 55, "desc");
			CPPUNIT_ASSERT_EQUAL((uint16_t) 55, r.get());
			r.set(66);
			CPPUNIT_ASSERT_EQUAL((uint16_t) 66, r.get());
		}

		void setByte() {
			Register r("R0", 0x0203, "desc");
			CPPUNIT_ASSERT_EQUAL((int) 3, (int) r.getByte());
			r.setByte(0x04);
			CPPUNIT_ASSERT_EQUAL((int) 4, (int) r.getByte());
			CPPUNIT_ASSERT_EQUAL((uint16_t) 0x0204, r.get());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION (RegisterTest);
