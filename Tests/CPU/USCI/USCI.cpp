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

#include "CPU/USCI/USCI.h"

namespace MSP430 {

class DummyTimerFactory : public TimerFactory {
	public:
		DCO *createDCO(Memory *mem, Variant *variant) { return new DCO(mem, variant); }
		VLO *createVLO() { return new VLO(); }
};

class DummyPinWatcher2 : public PinWatcher {
	public:
		DummyPinWatcher2() : sclk(-1), sdo(0) {
		}

		void handlePinChanged(int i, double v) {
// 			std::cout << "PIN CHANGED " << i << " " << v << "\n";
			switch (i) {
				case 2: sclk = v; break;
				case 1: sdo = v; break;
				default: break;
			}
		}

		double sclk;
		double sdo;
};

class USCITest : public CPPUNIT_NS :: TestFixture{
	CPPUNIT_TEST_SUITE(USCITest);
	CPPUNIT_TEST(spiMaster);
	CPPUNIT_TEST_SUITE_END();

	Memory *m;
	RegisterSet *r;
	Variant *v;
	InterruptManager *intManager;
	BasicClock *bc;
	TimerFactory *factory;
	PinManager *pinManager;
	DummyPinWatcher2 *watcher;
	USCI *usci;

	public:
		void setUp (void) {
			m = new Memory(120000);
			r = new RegisterSet;
			r->addDefaultRegisters();
			v = getVariant("msp430x241x");
			intManager = new InterruptManager(r, m, v);
			factory = new DummyTimerFactory();
			pinManager = new PinManager(m, intManager, v);

			PinMultiplexer *mpx = pinManager->addPin(P1, 0);

			{
				PinMultiplexer::Condition c;
				c["sel"] = 1;
				mpx->addMultiplexing(c, "UCA0SOMI");
			}

			mpx = pinManager->addPin(P1, 4);

			{
				PinMultiplexer::Condition c;
				c["sel"] = 1;
				mpx->addMultiplexing(c, "UCA0SIMO");
			}

			mpx = pinManager->addPin(P1, 5);

			{
				PinMultiplexer::Condition c;
				c["sel"] = 1;
				mpx->addMultiplexing(c, "UCA0CLK");
			}
			
			bc = new BasicClock(m, v, intManager, pinManager, factory);
			watcher = new DummyPinWatcher2();
			pinManager->setWatcher(watcher);

			usci = new USCI(pinManager, intManager, m, v, USCI::USCI_A, 0, bc->getACLK(), bc->getSMCLK());
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
			m->setByte(v->getP1SEL(), 0x31);
			m->setByte(v->getUCA0CTL1(), 0); // UCSWRST
			// UCA0CTL0 |= UCSYNC+UCMSB;
			m->setByte(v->getUCA0CTL0(), 41);
			// UCA0CTL1 &= ~UCSWRST;
			m->setByte(v->getUCA0CTL1(), 0);
			// UCA0IE |= UCRXIE | UCTXIE;
			m->setByte(v->getUC0IE(), 255);

			// nothing should happen until we send character
			CPPUNIT_ASSERT_EQUAL(-1.0, watcher->sclk);
			usci->tickRising();
			usci->tickFalling();
			CPPUNIT_ASSERT_EQUAL(-1.0, watcher->sclk);
			// UCBUSY is 0 too
			CPPUNIT_ASSERT_EQUAL(false, m->isBitSet(v->getUCA0STAT(), 1));

			// Set TX IFG just to test it gets cleared by write to TXBUF later
			m->setBit(v->getUC0IFG(), 2, true);
			CPPUNIT_ASSERT_EQUAL(true, m->isBitSet(v->getUC0IFG(), 2));

			// start transmitting - we will transfer 8 bits
			m->setByte(v->getUCA0TXBUF(), 55); // 00110111
			CPPUNIT_ASSERT_EQUAL(-1.0, watcher->sclk);

			// Write to TXBUF should clear TX IFG
			CPPUNIT_ASSERT_EQUAL(false, m->isBitSet(v->getUC0IFG(), 2));
			// We are transmitting now, so UCBUSY should be 1
			CPPUNIT_ASSERT_EQUAL(true, m->isBitSet(v->getUCA0STAT(), 1));

		/// BIT 1
			// First (MSB) bit should be sent and rising clock generated
			usci->tickRising();
			CPPUNIT_ASSERT_EQUAL(3.0, watcher->sclk);
			CPPUNIT_ASSERT_EQUAL(0.0, watcher->sdo);

			// prepare '1' to be captured
			pinManager->handlePinInput(0, 3.0);

			//  bit captured from SOMI
			usci->tickFalling();
			CPPUNIT_ASSERT_EQUAL(0.0, watcher->sclk);

		/// BIT 2
			// First (MSB) bit should be sent and rising clock generated
			usci->tickRising();
			CPPUNIT_ASSERT_EQUAL(3.0, watcher->sclk);
			CPPUNIT_ASSERT_EQUAL(0.0, watcher->sdo);

			// prepare '0' to be captured
			pinManager->handlePinInput(0, 0.0);

			// cnt--, bit captured from SDI
			usci->tickFalling();
			CPPUNIT_ASSERT_EQUAL(0.0, watcher->sclk);

		/// BIT 3
			// First (MSB) bit should be sent and rising clock generated
			usci->tickRising();
			CPPUNIT_ASSERT_EQUAL(3.0, watcher->sclk);
			CPPUNIT_ASSERT_EQUAL(3.0, watcher->sdo);

			// prepare '0' to be captured
			pinManager->handlePinInput(0, 3.0);

			// cnt--, bit captured from SDI
			usci->tickFalling();
			CPPUNIT_ASSERT_EQUAL(0.0, watcher->sclk);

		/// BIT 4
			// First (MSB) bit should be sent and rising clock generated
			usci->tickRising();
			CPPUNIT_ASSERT_EQUAL(3.0, watcher->sclk);
			CPPUNIT_ASSERT_EQUAL(3.0, watcher->sdo);

			// prepare '0' to be captured
			pinManager->handlePinInput(0, 0.0);

			// cnt--, bit captured from SDI
			usci->tickFalling();
			CPPUNIT_ASSERT_EQUAL(0.0, watcher->sclk);

		/// BIT 5
			// First (MSB) bit should be sent and rising clock generated
			usci->tickRising();
			CPPUNIT_ASSERT_EQUAL(3.0, watcher->sclk);
			CPPUNIT_ASSERT_EQUAL(0.0, watcher->sdo);

			// prepare '0' to be captured
			pinManager->handlePinInput(0, 3.0);

			// cnt--, bit captured from SDI
			usci->tickFalling();
			CPPUNIT_ASSERT_EQUAL(0.0, watcher->sclk);

		/// BIT 6
			// First (MSB) bit should be sent and rising clock generated
			usci->tickRising();
			CPPUNIT_ASSERT_EQUAL(3.0, watcher->sclk);
			CPPUNIT_ASSERT_EQUAL(3.0, watcher->sdo);

			// prepare '0' to be captured
			pinManager->handlePinInput(0, 0.0);

			// cnt--, bit captured from SDI
			usci->tickFalling();
			CPPUNIT_ASSERT_EQUAL(0.0, watcher->sclk);

		/// BIT 7
			// First (MSB) bit should be sent and rising clock generated
			usci->tickRising();
			CPPUNIT_ASSERT_EQUAL(3.0, watcher->sclk);
			CPPUNIT_ASSERT_EQUAL(3.0, watcher->sdo);

			// prepare '0' to be captured
			pinManager->handlePinInput(0, 3.0);

			// cnt--, bit captured from SDI
			usci->tickFalling();
			CPPUNIT_ASSERT_EQUAL(0.0, watcher->sclk);
			CPPUNIT_ASSERT_EQUAL(false, intManager->hasQueuedInterrupts());

		/// BIT 8
			// First (MSB) bit should be sent and rising clock generated
			usci->tickRising();
			CPPUNIT_ASSERT_EQUAL(3.0, watcher->sclk);
			CPPUNIT_ASSERT_EQUAL(3.0, watcher->sdo);

			// prepare '0' to be captured
			pinManager->handlePinInput(0, 0.0);

			CPPUNIT_ASSERT_EQUAL(true, intManager->hasQueuedInterrupts());
			CPPUNIT_ASSERT_EQUAL(false, m->isBitSet(v->getUC0IFG(), 1));
			CPPUNIT_ASSERT_EQUAL(true, m->isBitSet(v->getUC0IFG(), 2));

			// cnt--, bit captured from SDI
			usci->tickFalling();
			CPPUNIT_ASSERT_EQUAL(0.0, watcher->sclk);

			// Transmission stopped, UCBUSY is 0 again
			CPPUNIT_ASSERT_EQUAL(false, m->isBitSet(v->getUCA0STAT(), 1));

			// Check RX IFG flag now, because after reading the value, it should be cleared
			CPPUNIT_ASSERT_EQUAL(true, m->isBitSet(v->getUC0IFG(), 1));
			CPPUNIT_ASSERT_EQUAL(170, (int) m->getByte(v->getUCA0RXBUF()));

		/// INTERRUPT
			// RX IFG is cleared by reading RX BUF
			CPPUNIT_ASSERT_EQUAL(false, m->isBitSet(v->getUC0IFG(), 1));
			CPPUNIT_ASSERT_EQUAL(true, m->isBitSet(v->getUC0IFG(), 2));
			CPPUNIT_ASSERT_EQUAL(true, intManager->hasQueuedInterrupts());
// 
// 		/// Writing to USCICNT will clear the USCIIFG bit
// 			m->setByte(v->getUSCICCTL() + 1, 8);
// 			CPPUNIT_ASSERT_EQUAL(false, m->isBitSet(v->getUSCICTL() + 1, 1));
// 
// 			// writing 0 will raise USCIIFG bit again
// 			m->setByte(v->getUSCICCTL() + 1, 0);
// 			CPPUNIT_ASSERT_EQUAL(true, m->isBitSet(v->getUSCICTL() + 1, 1));
		}

};

CPPUNIT_TEST_SUITE_REGISTRATION (USCITest);

}
