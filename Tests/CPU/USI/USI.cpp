#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "CPU/Memory/Memory.h"
#include "CPU/Memory/RegisterSet.h"
#include "CPU/Interrupts/InterruptManager.h"
#include "CPU/BasicClock/Timer.h"
#include "CPU/BasicClock/TimerFactory.h"
#include "CPU/BasicClock/BasicClock.h"
#include "CPU/BasicClock/ACLK.h"
#include "CPU/BasicClock/SMCLK.h"
#include "CPU/Variants/Variant.h"
#include "CPU/Variants/VariantManager.h"
#include "CPU/Pins/PinManager.h"
#include "CPU/Pins/PinMultiplexer.h"
#include "CPU/BasicClock/VLO.h"
#include "CPU/BasicClock/DCO.h"
#include "CPU/BasicClock/LFXT1.h"

#include "CPU/USI/USI.h"

namespace MSP430 {

class DummyTimerFactory : public TimerFactory {
	public:
		DCO *createDCO(Memory *mem, Variant *variant) { return new DCO(mem, variant); }
		VLO *createVLO() { return new VLO(); }
};

class DummyPinWatcher1 : public PinWatcher {
	public:
		DummyPinWatcher1() : sclk(-1), sdo(0) {
		}

		void handlePinChanged(int i, double v) {
			switch (i) {
				case 0: sclk = v; break;
				case 1: sdo = v; break;
				default: break;
			}
		}

		double sclk;
		double sdo;
};

class USITest : public CPPUNIT_NS :: TestFixture{
	CPPUNIT_TEST_SUITE(USITest);
	CPPUNIT_TEST(spiMaster);
	CPPUNIT_TEST(spiMasterTA0);
	CPPUNIT_TEST_SUITE_END();

	Memory *m;
	RegisterSet *r;
	Variant *v;
	InterruptManager *intManager;
	BasicClock *bc;
	TimerFactory *factory;
	PinManager *pinManager;
	DummyPinWatcher1 *watcher;
	USI *usi;

	public:
		void setUp (void) {
			m = new Memory(120000);
			r = new RegisterSet;
			r->addDefaultRegisters();
			v = getVariant("msp430x20x3");
			intManager = new InterruptManager(r, m, v);
			factory = new DummyTimerFactory();
			pinManager = new PinManager(m, intManager, v);

			PinMultiplexer *mpx = pinManager->addPin(P1, 5);

			{
				PinMultiplexer::Condition c;
				c["usip"] = 1;
				mpx->addMultiplexing(c, "SCLK");
			}

			mpx = pinManager->addPin(P1, 6);

			{
				PinMultiplexer::Condition c;
				c["usip"] = 1;
				mpx->addMultiplexing(c, "SDO");
			}

			mpx = pinManager->addPin(P1, 7);

			{
				PinMultiplexer::Condition c;
				c["usip"] = 1;
				mpx->addMultiplexing(c, "SDI");
			}
			
			bc = new BasicClock(m, v, intManager, pinManager, factory);
			watcher = new DummyPinWatcher1();
			pinManager->setWatcher(watcher);

			usi = new USI(pinManager, intManager, m, v, bc->getACLK(), bc->getSMCLK());
		}

		void tearDown (void) {
			delete m;
			delete r;
			delete intManager;
			delete bc;
			delete factory;
			delete pinManager;
			delete watcher;
		}

		void spiMaster() {
			// USICTL0 |= USIPE7 +  USIPE6 + USIPE5 + USIMST + USIOE; // Port, SPI master
			m->setByte(v->getUSICTL(), 234);
			// USICTL1 |= USIIE;                     // Counter interrupt, flag remains set
			m->setByte(v->getUSICTL() + 1, 16);
			m->setByte(v->getUSISR(), 55); // 00110111

			// nothing should happen until we set usicnt
			CPPUNIT_ASSERT_EQUAL(-1.0, watcher->sclk);
			usi->tickRising();
			usi->tickFalling();
			CPPUNIT_ASSERT_EQUAL(-1.0, watcher->sclk);

			// Set usicnt - we will transfer 8 bits
			m->setByte(v->getUSICCTL() + 1, 8);
			CPPUNIT_ASSERT_EQUAL(-1.0, watcher->sclk);

		/// BIT 1
			// First (MSB) bit should be sent and rising clock generated
			usi->tickRising();
			CPPUNIT_ASSERT_EQUAL(3.0, watcher->sclk);
			CPPUNIT_ASSERT_EQUAL(0.0, watcher->sdo);

			// prepare '1' to be captured
			pinManager->handlePinInput(2, 3.0);

			// cnt--, bit captured from SDI
			usi->tickFalling();
			CPPUNIT_ASSERT_EQUAL(7, (int) m->getByte(v->getUSICCTL() + 1));
			CPPUNIT_ASSERT_EQUAL(0.0, watcher->sclk);
			CPPUNIT_ASSERT_EQUAL(111, (int) m->getByte(v->getUSISR())); // 00110111 (after shift) -> 0110111|1

		/// BIT 2
			// First (MSB) bit should be sent and rising clock generated
			usi->tickRising();
			CPPUNIT_ASSERT_EQUAL(3.0, watcher->sclk);
			CPPUNIT_ASSERT_EQUAL(0.0, watcher->sdo);

			// prepare '0' to be captured
			pinManager->handlePinInput(2, 0.0);

			// cnt--, bit captured from SDI
			usi->tickFalling();
			CPPUNIT_ASSERT_EQUAL(6, (int) m->getByte(v->getUSICCTL() + 1));
			CPPUNIT_ASSERT_EQUAL(0.0, watcher->sclk);
			CPPUNIT_ASSERT_EQUAL(222, (int) m->getByte(v->getUSISR())); // 0110111|1 (after shift) -> 110111|10

		/// BIT 3
			// First (MSB) bit should be sent and rising clock generated
			usi->tickRising();
			CPPUNIT_ASSERT_EQUAL(3.0, watcher->sclk);
			CPPUNIT_ASSERT_EQUAL(3.0, watcher->sdo);

			// prepare '0' to be captured
			pinManager->handlePinInput(2, 3.0);

			// cnt--, bit captured from SDI
			usi->tickFalling();
			CPPUNIT_ASSERT_EQUAL(5, (int) m->getByte(v->getUSICCTL() + 1));
			CPPUNIT_ASSERT_EQUAL(0.0, watcher->sclk);
			CPPUNIT_ASSERT_EQUAL(189, (int) m->getByte(v->getUSISR())); // 110111|10 (after shift) -> 10111|101

		/// BIT 4
			// First (MSB) bit should be sent and rising clock generated
			usi->tickRising();
			CPPUNIT_ASSERT_EQUAL(3.0, watcher->sclk);
			CPPUNIT_ASSERT_EQUAL(3.0, watcher->sdo);

			// prepare '0' to be captured
			pinManager->handlePinInput(2, 0.0);

			// cnt--, bit captured from SDI
			usi->tickFalling();
			CPPUNIT_ASSERT_EQUAL(4, (int) m->getByte(v->getUSICCTL() + 1));
			CPPUNIT_ASSERT_EQUAL(0.0, watcher->sclk);
			CPPUNIT_ASSERT_EQUAL(122, (int) m->getByte(v->getUSISR())); // 10111|101 (after shift) -> 0111|1010

		/// BIT 5
			// First (MSB) bit should be sent and rising clock generated
			usi->tickRising();
			CPPUNIT_ASSERT_EQUAL(3.0, watcher->sclk);
			CPPUNIT_ASSERT_EQUAL(0.0, watcher->sdo);

			// prepare '0' to be captured
			pinManager->handlePinInput(2, 3.0);

			// cnt--, bit captured from SDI
			usi->tickFalling();
			CPPUNIT_ASSERT_EQUAL(3, (int) m->getByte(v->getUSICCTL() + 1));
			CPPUNIT_ASSERT_EQUAL(0.0, watcher->sclk);
			CPPUNIT_ASSERT_EQUAL(245, (int) m->getByte(v->getUSISR())); // 0111|1010 (after shift) -> 111|10101

		/// BIT 6
			// First (MSB) bit should be sent and rising clock generated
			usi->tickRising();
			CPPUNIT_ASSERT_EQUAL(3.0, watcher->sclk);
			CPPUNIT_ASSERT_EQUAL(3.0, watcher->sdo);

			// prepare '0' to be captured
			pinManager->handlePinInput(2, 0.0);

			// cnt--, bit captured from SDI
			usi->tickFalling();
			CPPUNIT_ASSERT_EQUAL(2, (int) m->getByte(v->getUSICCTL() + 1));
			CPPUNIT_ASSERT_EQUAL(0.0, watcher->sclk);
			CPPUNIT_ASSERT_EQUAL(234, (int) m->getByte(v->getUSISR())); // 111|10101 (after shift) -> 11|101010

		/// BIT 7
			// First (MSB) bit should be sent and rising clock generated
			usi->tickRising();
			CPPUNIT_ASSERT_EQUAL(3.0, watcher->sclk);
			CPPUNIT_ASSERT_EQUAL(3.0, watcher->sdo);

			// prepare '0' to be captured
			pinManager->handlePinInput(2, 3.0);

			// cnt--, bit captured from SDI
			usi->tickFalling();
			CPPUNIT_ASSERT_EQUAL(1, (int) m->getByte(v->getUSICCTL() + 1));
			CPPUNIT_ASSERT_EQUAL(0.0, watcher->sclk);
			CPPUNIT_ASSERT_EQUAL(213, (int) m->getByte(v->getUSISR())); // 11|101010 (after shift) -> 1|1010101

		/// BIT 8
			// First (MSB) bit should be sent and rising clock generated
			usi->tickRising();
			CPPUNIT_ASSERT_EQUAL(3.0, watcher->sclk);
			CPPUNIT_ASSERT_EQUAL(3.0, watcher->sdo);

			// prepare '0' to be captured
			pinManager->handlePinInput(2, 0.0);

			CPPUNIT_ASSERT_EQUAL(false, intManager->hasQueuedInterrupts());

			// cnt--, bit captured from SDI
			usi->tickFalling();
			CPPUNIT_ASSERT_EQUAL(0, (int) m->getByte(v->getUSICCTL() + 1));
			CPPUNIT_ASSERT_EQUAL(0.0, watcher->sclk);
			CPPUNIT_ASSERT_EQUAL(170, (int) m->getByte(v->getUSISR())); // 1|1010101 (after shift) -> |10101010

		/// INTERRUPT
			CPPUNIT_ASSERT_EQUAL(true, intManager->hasQueuedInterrupts());
		}

		void spiMasterTA0() {
			// USICTL0 |= USIPE7 +  USIPE6 + USIPE5 + USIMST + USIOE; // Port, SPI master
			m->setByte(v->getUSICTL(), 234);
			// USICTL1 |= USIIE;                     // Counter interrupt, flag remains set
			m->setByte(v->getUSICTL() + 1, 16);
			m->setByte(v->getUSISR(), 55); // 00110111
			m->setByte(v->getUSICCTL(), 20); // 00010100

			// nothing should happen until we set usicnt
			CPPUNIT_ASSERT_EQUAL(-1.0, watcher->sclk);
			usi->tickRising();
			usi->tickFalling();
			CPPUNIT_ASSERT_EQUAL(-1.0, watcher->sclk);

			// Set usicnt - we will transfer 8 bits
			m->setByte(v->getUSICCTL() + 1, 8);
			CPPUNIT_ASSERT_EQUAL(-1.0, watcher->sclk);

		/// BIT 1
			// First (MSB) bit should be sent and rising clock generated
			pinManager->generateSignal("TA0.0", 3);
			CPPUNIT_ASSERT_EQUAL(3.0, watcher->sclk);
			CPPUNIT_ASSERT_EQUAL(0.0, watcher->sdo);

			// prepare '1' to be captured
			pinManager->handlePinInput(2, 3.0);

			// cnt--, bit captured from SDI
			pinManager->generateSignal("TA0.0", 0);
			CPPUNIT_ASSERT_EQUAL(7, (int) m->getByte(v->getUSICCTL() + 1));
			CPPUNIT_ASSERT_EQUAL(0.0, watcher->sclk);
			CPPUNIT_ASSERT_EQUAL(111, (int) m->getByte(v->getUSISR())); // 00110111 (after shift) -> 0110111|1

		/// BIT 2
			// First (MSB) bit should be sent and rising clock generated
			pinManager->generateSignal("TA0.0", 3);
			CPPUNIT_ASSERT_EQUAL(3.0, watcher->sclk);
			CPPUNIT_ASSERT_EQUAL(0.0, watcher->sdo);

			// prepare '0' to be captured
			pinManager->handlePinInput(2, 0.0);

			// cnt--, bit captured from SDI
			pinManager->generateSignal("TA0.0", 0);
			CPPUNIT_ASSERT_EQUAL(6, (int) m->getByte(v->getUSICCTL() + 1));
			CPPUNIT_ASSERT_EQUAL(0.0, watcher->sclk);
			CPPUNIT_ASSERT_EQUAL(222, (int) m->getByte(v->getUSISR())); // 0110111|1 (after shift) -> 110111|10
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION (USITest);

}
