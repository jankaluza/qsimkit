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

using namespace MCU;

class DummyTimerFactory : public TimerFactory {
	public:
		Timer *createTimer(PinManager *pinManager, InterruptManager *intManager, Memory *mem,
						   Variant *variant, ACLK *aclk,
						   SMCLK *smclk, uint16_t tactl, uint16_t tar,
						   uint16_t taiv, uint16_t intvec0, uint16_t intvec1) {
			return new Timer(pinManager, intManager, mem, variant, aclk, smclk, tactl, tar, taiv, intvec0, intvec1);
		}
};

class DummyPinWatcher : public PinWatcher {
	public:
		DummyPinWatcher() : id(-1), value(-1) {}

		void handlePinChanged(int i, double v) {
			id = i;
			value = v;
		}

		int id;
		double value;
};

class TimerTest : public CPPUNIT_NS :: TestFixture{
	CPPUNIT_TEST_SUITE(TimerTest);
	// TODO: Add tests for changing CCR0 in upMode and UpDownMode!
	CPPUNIT_TEST(upMode);
	CPPUNIT_TEST(continuousMode);
	CPPUNIT_TEST(upDownMode);
	CPPUNIT_TEST(captureRisingEdge);
	CPPUNIT_TEST(captureRisingEdgeAsync);
	CPPUNIT_TEST(captureGNDVCC);
	CPPUNIT_TEST(captureGNDCCIA);
	CPPUNIT_TEST(compareSetReset);
	CPPUNIT_TEST_SUITE_END();

	Memory *m;
	RegisterSet *r;
	Variant *v;
	InterruptManager *intManager;
	BasicClock *bc;
	TimerFactory *factory;
	PinManager *pinManager;
	DummyPinWatcher *watcher;

	public:
		void setUp (void) {
			m = new Memory(120000);
			r = new RegisterSet;
			r->addDefaultRegisters();
			v = getVariant("msp430x241x");
			intManager = new InterruptManager(r, m);
			factory = new DummyTimerFactory();
			pinManager = new PinManager(m, intManager, v);

			PinMultiplexer *mpx = pinManager->addPin(P1, 0);

			{
				PinMultiplexer::Condition c;
				c["sel"] = 0;
				mpx->addMultiplexing(c, "GP");
			}

			{
				PinMultiplexer::Condition c;
				c["dir"] = 0;
				c["sel"] = 1;
				mpx->addMultiplexing(c, "TA0.CCI0A");
			}

			mpx = pinManager->addPin(P1, 1);

			{
				PinMultiplexer::Condition c;
				c["sel"] = 0;
				mpx->addMultiplexing(c, "GP");
			}

			{
				PinMultiplexer::Condition c;
				c["dir"] = 0;
				c["sel"] = 1;
				mpx->addMultiplexing(c, "TA0.CCI1A");
			}

			{
				PinMultiplexer::Condition c;
				c["dir"] = 1;
				c["sel"] = 1;
				mpx->addMultiplexing(c, "TA0.1");
			}
			
			bc = new BasicClock(m, v, intManager, pinManager, factory);
			watcher = new DummyPinWatcher();
			pinManager->setWatcher(watcher);
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

		void upMode() {
			// timer stopped now - tick() has no effect
			CPPUNIT_ASSERT_EQUAL((uint16_t) 0, m->getBigEndian(v->getTA0R()));
			bc->getTimerA()->tick();
			CPPUNIT_ASSERT_EQUAL((uint16_t) 0, m->getBigEndian(v->getTA0R()));

			// set UP mode and CCR0 to 2
			m->setBigEndian(v->getTA0CTL(), 16);
			m->setBigEndian(v->getTA0CCR0(), 2);
			m->setBigEndian(v->getTA0CCTL0(), 16);
			// set CCR1 to 1, this should trigger interrupt later
			m->setBigEndian(v->getTA0CCR1(), 1);
			m->setBigEndian(v->getTA0CCTL1(), 16);

			bc->getTimerA()->tick();
			CPPUNIT_ASSERT_EQUAL((uint16_t) 1, m->getBigEndian(v->getTA0R()));
			CPPUNIT_ASSERT_EQUAL(true, intManager->hasQueuedInterrupts());
			CPPUNIT_ASSERT_EQUAL(false, m->isBitSet(v->getTA0CCTL0(), 1)); // CCIFG 0
			CPPUNIT_ASSERT_EQUAL(true, m->isBitSet(v->getTA0CCTL1(), 1)); // CCIFG 1
			CPPUNIT_ASSERT_EQUAL((uint16_t) 2, m->getBigEndian(v->getTA0IV()));
			// We have read TAIV, so CCIFG 1 is reset
			CPPUNIT_ASSERT_EQUAL(false, m->isBitSet(v->getTA0CCTL1(), 1)); // CCIFG 1
			CPPUNIT_ASSERT_EQUAL(false, m->isBitSet(v->getTA0IV(), 4)); // TAIV

			bc->getTimerA()->tick();
			CPPUNIT_ASSERT_EQUAL((uint16_t) 2, m->getBigEndian(v->getTA0R()));
			CPPUNIT_ASSERT_EQUAL(true, intManager->hasQueuedInterrupts());
			CPPUNIT_ASSERT_EQUAL(true, m->isBitSet(v->getTA0CCTL0(), 1)); // CCIFG
			CPPUNIT_ASSERT_EQUAL(false, m->isBitSet(v->getTA0IV(), 4)); // TAIV

			bc->getTimerA()->tick();
			CPPUNIT_ASSERT_EQUAL((uint16_t) 0, m->getBigEndian(v->getTA0R()));
			CPPUNIT_ASSERT_EQUAL(true, intManager->hasQueuedInterrupts());
			CPPUNIT_ASSERT_EQUAL(true, m->isBitSet(v->getTA0CCTL0(), 1)); // CCIFG
			CPPUNIT_ASSERT_EQUAL((uint16_t) 0, m->getBigEndian(v->getTA0IV())); // TAIV

			// and now again with TAIFG interrupt enabled
			m->setBigEndian(v->getTA0CTL(), 18);
			bc->getTimerA()->tick();
			bc->getTimerA()->tick();
			bc->getTimerA()->tick();
			CPPUNIT_ASSERT_EQUAL((uint16_t) 0, m->getBigEndian(v->getTA0R()));
			CPPUNIT_ASSERT_EQUAL(true, intManager->hasQueuedInterrupts());
			CPPUNIT_ASSERT_EQUAL(true, m->isBitSet(v->getTA0CCTL0(), 1)); // CCIFG
			// Interrupt with biggest priority is CCIFG 1 => 2
			CPPUNIT_ASSERT_EQUAL((uint16_t) 2, m->getBigEndian(v->getTA0IV()));
			// Next - with lower priority - is TAIFG
			CPPUNIT_ASSERT_EQUAL((uint16_t) 10, m->getBigEndian(v->getTA0IV()));
		}

		void continuousMode() {
			// timer stopped now - tick() has no effect
			CPPUNIT_ASSERT_EQUAL((uint16_t) 0, m->getBigEndian(v->getTA0R()));
			bc->getTimerA()->tick();
			CPPUNIT_ASSERT_EQUAL((uint16_t) 0, m->getBigEndian(v->getTA0R()));

			// set continuous mode and CCR0 to 2
			m->setBigEndian(v->getTA0CTL(), 32);
			m->setBigEndian(v->getTA0CCR0(), 2);
			m->setBigEndian(v->getTA0CCTL0(), 16);
			// set CCR1 to 1, this should trigger interrupt later
			m->setBigEndian(v->getTA0CCR1(), 1);
			m->setBigEndian(v->getTA0CCTL1(), 16);

			bc->getTimerA()->tick();
			CPPUNIT_ASSERT_EQUAL((uint16_t) 1, m->getBigEndian(v->getTA0R()));
			CPPUNIT_ASSERT_EQUAL(true, intManager->hasQueuedInterrupts());
			CPPUNIT_ASSERT_EQUAL(false, m->isBitSet(v->getTA0CCTL0(), 1)); // CCIFG 0
			CPPUNIT_ASSERT_EQUAL(true, m->isBitSet(v->getTA0CCTL1(), 1)); // CCIFG 1
			CPPUNIT_ASSERT_EQUAL((uint16_t) 2, m->getBigEndian(v->getTA0IV()));
			// We have read TAIV, so CCIFG 1 is reset
			CPPUNIT_ASSERT_EQUAL(false, m->isBitSet(v->getTA0CCTL1(), 1)); // CCIFG 1
			CPPUNIT_ASSERT_EQUAL(false, m->isBitSet(v->getTA0IV(), 4)); // TAIV

			bc->getTimerA()->tick();
			CPPUNIT_ASSERT_EQUAL((uint16_t) 2, m->getBigEndian(v->getTA0R()));
			CPPUNIT_ASSERT_EQUAL(true, intManager->hasQueuedInterrupts());
			CPPUNIT_ASSERT_EQUAL(true, m->isBitSet(v->getTA0CCTL0(), 1)); // CCIFG
			CPPUNIT_ASSERT_EQUAL(false, m->isBitSet(v->getTA0IV(), 4)); // TAIV

			bc->getTimerA()->tick();
			CPPUNIT_ASSERT_EQUAL((uint16_t) 3, m->getBigEndian(v->getTA0R()));
			CPPUNIT_ASSERT_EQUAL(true, intManager->hasQueuedInterrupts());
			CPPUNIT_ASSERT_EQUAL(true, m->isBitSet(v->getTA0CCTL0(), 1)); // CCIFG
			CPPUNIT_ASSERT_EQUAL((uint16_t) 0, m->getBigEndian(v->getTA0IV())); // TAIV

			for (int i = 3; i != 0xffff; ++i) {
				bc->getTimerA()->tick();
				CPPUNIT_ASSERT_EQUAL((uint16_t) (i + 1), m->getBigEndian(v->getTA0R()));
			}
			bc->getTimerA()->tick();
			CPPUNIT_ASSERT_EQUAL((uint16_t) 0, m->getBigEndian(v->getTA0R()));
			CPPUNIT_ASSERT_EQUAL((uint16_t) 0, m->getBigEndian(v->getTA0IV())); // TAIV

			// and now again with TAIFG interrupt enabled
			m->setBigEndian(v->getTA0CTL(), 34);
			bc->getTimerA()->tick();
			bc->getTimerA()->tick();
			bc->getTimerA()->tick();
			CPPUNIT_ASSERT_EQUAL((uint16_t) 3, m->getBigEndian(v->getTA0R()));

			for (int i = 3; i != 0xffff; ++i) {
				bc->getTimerA()->tick();
				CPPUNIT_ASSERT_EQUAL((uint16_t) (i + 1), m->getBigEndian(v->getTA0R()));
			}
			bc->getTimerA()->tick();
			CPPUNIT_ASSERT_EQUAL((uint16_t) 0, m->getBigEndian(v->getTA0R()));

			CPPUNIT_ASSERT_EQUAL(true, intManager->hasQueuedInterrupts());
			CPPUNIT_ASSERT_EQUAL(true, m->isBitSet(v->getTA0CCTL0(), 1)); // CCIFG
			// Interrupt with biggest priority is CCIFG 1 => 2
			CPPUNIT_ASSERT_EQUAL((uint16_t) 2, m->getBigEndian(v->getTA0IV()));
			// Next - with lower priority - is TAIFG
			CPPUNIT_ASSERT_EQUAL((uint16_t) 10, m->getBigEndian(v->getTA0IV()));
		}

		void upDownMode() {
			// timer stopped now - tick() has no effect
			CPPUNIT_ASSERT_EQUAL((uint16_t) 0, m->getBigEndian(v->getTA0R()));
			bc->getTimerA()->tick();
			CPPUNIT_ASSERT_EQUAL((uint16_t) 0, m->getBigEndian(v->getTA0R()));

			// set UP DOWN mode and CCR0 to 2
			m->setBigEndian(v->getTA0CTL(), 48);
			m->setBigEndian(v->getTA0CCR0(), 2);
			m->setBigEndian(v->getTA0CCTL0(), 16);
			// set CCR1 to 1, this should trigger interrupt later
			m->setBigEndian(v->getTA0CCR1(), 1);
			m->setBigEndian(v->getTA0CCTL1(), 16);

			bc->getTimerA()->tick();
			CPPUNIT_ASSERT_EQUAL((uint16_t) 1, m->getBigEndian(v->getTA0R()));
			CPPUNIT_ASSERT_EQUAL(true, intManager->hasQueuedInterrupts());
			CPPUNIT_ASSERT_EQUAL(false, m->isBitSet(v->getTA0CCTL0(), 1)); // CCIFG 0
			CPPUNIT_ASSERT_EQUAL(true, m->isBitSet(v->getTA0CCTL1(), 1)); // CCIFG 1
			CPPUNIT_ASSERT_EQUAL((uint16_t) 2, m->getBigEndian(v->getTA0IV()));
			// We have read TAIV, so CCIFG 1 is reset
			CPPUNIT_ASSERT_EQUAL(false, m->isBitSet(v->getTA0CCTL1(), 1)); // CCIFG 1
			CPPUNIT_ASSERT_EQUAL(false, m->isBitSet(v->getTA0IV(), 4)); // TAIV

			bc->getTimerA()->tick();
			CPPUNIT_ASSERT_EQUAL((uint16_t) 2, m->getBigEndian(v->getTA0R()));
			CPPUNIT_ASSERT_EQUAL(true, intManager->hasQueuedInterrupts());
			CPPUNIT_ASSERT_EQUAL(true, m->isBitSet(v->getTA0CCTL0(), 1)); // CCIFG
			m->setBit(v->getTA0CCTL0(), 1, false);
			CPPUNIT_ASSERT_EQUAL((uint16_t) 0, m->getBigEndian(v->getTA0IV()));

			bc->getTimerA()->tick();
			CPPUNIT_ASSERT_EQUAL((uint16_t) 1, m->getBigEndian(v->getTA0R()));
			CPPUNIT_ASSERT_EQUAL(true, intManager->hasQueuedInterrupts());
			CPPUNIT_ASSERT_EQUAL(false, m->isBitSet(v->getTA0CCTL1(), 1)); // CCIFG 1
			CPPUNIT_ASSERT_EQUAL((uint16_t) 0, m->getBigEndian(v->getTA0IV())); // TAIV

			bc->getTimerA()->tick();
			CPPUNIT_ASSERT_EQUAL((uint16_t) 0, m->getBigEndian(v->getTA0R()));
			bc->getTimerA()->tick();
			CPPUNIT_ASSERT_EQUAL((uint16_t) 1, m->getBigEndian(v->getTA0R()));
			CPPUNIT_ASSERT_EQUAL(true, intManager->hasQueuedInterrupts());
			CPPUNIT_ASSERT_EQUAL(false, m->isBitSet(v->getTA0CCTL0(), 1)); // CCIFG 0
			CPPUNIT_ASSERT_EQUAL(true, m->isBitSet(v->getTA0CCTL1(), 1)); // CCIFG 1
			CPPUNIT_ASSERT_EQUAL((uint16_t) 2, m->getBigEndian(v->getTA0IV()));
			// We have read TAIV, so CCIFG 1 is reset
			CPPUNIT_ASSERT_EQUAL(false, m->isBitSet(v->getTA0CCTL1(), 1)); // CCIFG 1
			CPPUNIT_ASSERT_EQUAL(false, m->isBitSet(v->getTA0IV(), 4)); // TAIV
		}

		void captureRisingEdge() {
			// Start timer in continuous mode
			m->setBigEndian(v->getTA0CTL(), 32);
			// Rising edge, SCS, CCIA0, Capture mode, Interrupt
			m->setBigEndian(v->getTA0CCTL0(), 0x4910);
			// Set P1.0 to be handled by Timer
			m->setBitWatcher(v->getP1SEL(), 1, true);

			CPPUNIT_ASSERT_EQUAL(false, intManager->hasQueuedInterrupts());
			// Change from 0 to 0 should not trigger interrupt
			pinManager->handlePinInput(0, 0.0);
			bc->getTimerA()->tick();
			CPPUNIT_ASSERT_EQUAL(false, intManager->hasQueuedInterrupts());

			pinManager->handlePinInput(0, 1.0);
			// We should not have interrupt until we ->tick(), because we are
			// in SCS mode
			CPPUNIT_ASSERT_EQUAL(false, intManager->hasQueuedInterrupts());
			// Until tick, TA0CCR0 should be unchanged
			CPPUNIT_ASSERT_EQUAL((uint16_t) 0, m->getBigEndian(v->getTA0CCR0(), false));
			bc->getTimerA()->tick();
			CPPUNIT_ASSERT_EQUAL(true, intManager->hasQueuedInterrupts());
			CPPUNIT_ASSERT_EQUAL((uint16_t) 2, m->getBigEndian(v->getTA0CCR0(), false));

			intManager->clearQueuedInterrupts();

			// Another tick should not reraise the interrupt
			bc->getTimerA()->tick();
			CPPUNIT_ASSERT_EQUAL(false, intManager->hasQueuedInterrupts());

			// Change from 1 to 1 should not raise the interrupt
			pinManager->handlePinInput(0, 1.0);
			bc->getTimerA()->tick();
			CPPUNIT_ASSERT_EQUAL(false, intManager->hasQueuedInterrupts());

			// Change from 1 to 0 should not raise the interrupt
			pinManager->handlePinInput(0, 0.0);
			bc->getTimerA()->tick();
			CPPUNIT_ASSERT_EQUAL(false, intManager->hasQueuedInterrupts());

			// We have not really read TA0CCR0 yet, so another capture should
			// set COV bit
			pinManager->handlePinInput(0, 1.0);
			CPPUNIT_ASSERT_EQUAL(false, m->isBitSet(v->getTA0CCTL0(), 2)); // COV
			bc->getTimerA()->tick();
			CPPUNIT_ASSERT_EQUAL(false, intManager->hasQueuedInterrupts());
			CPPUNIT_ASSERT_EQUAL(true, m->isBitSet(v->getTA0CCTL0(), 2)); // COV
			CPPUNIT_ASSERT_EQUAL((uint16_t) 2, m->getBigEndian(v->getTA0CCR0(), false));

			// Rest COV
			m->setBit(v->getTA0CCTL0(), 2, false);

			// Read the TA0CCR0 and check that interrupt is not raised
			CPPUNIT_ASSERT_EQUAL((uint16_t) 2, m->getBigEndian(v->getTA0CCR0()));
			bc->getTimerA()->tick();
			CPPUNIT_ASSERT_EQUAL(false, intManager->hasQueuedInterrupts());

			// Next capture should raise an interrupt
			pinManager->handlePinInput(0, 0.0);
			pinManager->handlePinInput(0, 1.0);
			bc->getTimerA()->tick();
			CPPUNIT_ASSERT_EQUAL(true, intManager->hasQueuedInterrupts());
			CPPUNIT_ASSERT_EQUAL((uint16_t) 8, m->getBigEndian(v->getTA0CCR0(), false));
		}

		void captureRisingEdgeAsync() {
			m->setBigEndian(v->getTA0CCR0(), 1000);
			// Rising edge, CCIA0, Capture mode, Interrupt
			m->setBigEndian(v->getTA0CCTL0(), 0x4110);
			// Set P1.0 to be handled by Timer
			m->setBitWatcher(v->getP1SEL(), 1, true);

			CPPUNIT_ASSERT_EQUAL(false, intManager->hasQueuedInterrupts());
			// Change from 0 to 0 should not trigger interrupt
			pinManager->handlePinInput(0, 0.0);
			CPPUNIT_ASSERT_EQUAL(false, intManager->hasQueuedInterrupts());

			pinManager->handlePinInput(0, 1.0);
			CPPUNIT_ASSERT_EQUAL(true, intManager->hasQueuedInterrupts());
			CPPUNIT_ASSERT_EQUAL((uint16_t) 0, m->getBigEndian(v->getTA0CCR0(), false));

			intManager->clearQueuedInterrupts();

			// Change from 1 to 1 should not raise the interrupt
			pinManager->handlePinInput(0, 1.0);
			CPPUNIT_ASSERT_EQUAL(false, intManager->hasQueuedInterrupts());

			// Change from 1 to 0 should not raise the interrupt
			pinManager->handlePinInput(0, 0.0);
			CPPUNIT_ASSERT_EQUAL(false, intManager->hasQueuedInterrupts());

			// We have not really read TA0CCR0 yet, so another capture should
			// set COV bit
			CPPUNIT_ASSERT_EQUAL(false, m->isBitSet(v->getTA0CCTL0(), 2)); // COV
			pinManager->handlePinInput(0, 1.0);
			CPPUNIT_ASSERT_EQUAL(false, intManager->hasQueuedInterrupts());
			CPPUNIT_ASSERT_EQUAL(true, m->isBitSet(v->getTA0CCTL0(), 2)); // COV
			CPPUNIT_ASSERT_EQUAL((uint16_t) 0, m->getBigEndian(v->getTA0CCR0(), false));

			// Rest COV
			m->setBit(v->getTA0CCTL0(), 2, false);

			// Read the TA0CCR0 and check that interrupt is not raised
			CPPUNIT_ASSERT_EQUAL((uint16_t) 0, m->getBigEndian(v->getTA0CCR0()));
			m->setBigEndian(v->getTA0CCR0(), 1000);
			CPPUNIT_ASSERT_EQUAL(false, intManager->hasQueuedInterrupts());

			// Next capture should raise an interrupt
			pinManager->handlePinInput(0, 0.0);
			CPPUNIT_ASSERT_EQUAL(false, intManager->hasQueuedInterrupts());
			pinManager->handlePinInput(0, 1.0);
			CPPUNIT_ASSERT_EQUAL(true, intManager->hasQueuedInterrupts());
			CPPUNIT_ASSERT_EQUAL((uint16_t) 0, m->getBigEndian(v->getTA0CCR0(), false));
		}

		void compareSetReset() {
			// set UP mode and CCR0 to 4
			m->setBigEndian(v->getTA0CTL(), 16);
			m->setBigEndian(v->getTA0CCR0(), 4);
			// set CCR1 to 2, and Set/Reset mode, CCI1A input and interrupts
			m->setBigEndian(v->getTA0CCR1(), 2);
			m->setBigEndian(v->getTA0CCTL1(), 112);
			// Set P1.1 to be handled by Timer
			m->setBitWatcher(v->getP1SEL(), 2, true);

			// intput should be handled by tiemr and CCI bit should be set,
			// but it should not be sampled yet, so SCCI should be 0
			CPPUNIT_ASSERT_EQUAL(false, m->isBitSet(v->getTA0CCTL1(), 8));
			CPPUNIT_ASSERT_EQUAL(false, m->isBitSet(v->getTA0CCTL1(), 1 << 10));
			pinManager->handlePinInput(1, 1.0);
			CPPUNIT_ASSERT_EQUAL(true, m->isBitSet(v->getTA0CCTL1(), 8));
			CPPUNIT_ASSERT_EQUAL(false, m->isBitSet(v->getTA0CCTL1(), 1 << 10));

			// First tick, TAR == 1, no interrupt yet
			bc->getTimerA()->tick();
			CPPUNIT_ASSERT_EQUAL(false, intManager->hasQueuedInterrupts());

			// Second tick, TAR == 2 == CCR1, CCR1 interrupt raised, SCCI latched,
			// Set output generated
			CPPUNIT_ASSERT_EQUAL(false, m->isBitSet(v->getTA0CCTL1(), 4));
			bc->getTimerA()->tick();
			CPPUNIT_ASSERT_EQUAL(true, intManager->hasQueuedInterrupts());
			CPPUNIT_ASSERT_EQUAL(true, m->isBitSet(v->getTA0CCTL1(), 4));
			CPPUNIT_ASSERT_EQUAL(true, m->isBitSet(v->getTA0CCTL1(), 1 << 10));
			CPPUNIT_ASSERT_EQUAL(1, watcher->id);
			CPPUNIT_ASSERT_EQUAL(1.0, watcher->value);
			intManager->clearQueuedInterrupts();

			// TAR == 3, no interrupt yet
			bc->getTimerA()->tick();
			CPPUNIT_ASSERT_EQUAL(false, intManager->hasQueuedInterrupts());

			// TAR == 4 == CCR0, CCR0 interrupts disabled, Reset output generated
			bc->getTimerA()->tick();
			CPPUNIT_ASSERT_EQUAL(false, intManager->hasQueuedInterrupts());
			CPPUNIT_ASSERT_EQUAL(1, watcher->id);
			CPPUNIT_ASSERT_EQUAL(0.0, watcher->value);
			CPPUNIT_ASSERT_EQUAL(false, m->isBitSet(v->getTA0CCTL1(), 4));
		}

		void captureGNDVCC() {
			// Start timer in continuous mode
			m->setBigEndian(v->getTA0CTL(), 32);
			// Rising edge, SCS, GND, Capture mode, Interrupt
			m->setBigEndian(v->getTA0CCTL0(), 0x6910);
			// Set P1.0 to be handled by Timer
			m->setBitWatcher(v->getP1SEL(), 1, true);

			CPPUNIT_ASSERT_EQUAL(false, intManager->hasQueuedInterrupts());
			// Change SCCI to VCC - rising edge
			m->setBigEndian(v->getTA0CCTL0(), 0x7910);
			bc->getTimerA()->tick();
			CPPUNIT_ASSERT_EQUAL(true, intManager->hasQueuedInterrupts());
		}

		void captureGNDCCIA() {
			// TODO
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION (TimerTest);
