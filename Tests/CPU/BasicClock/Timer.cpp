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

using namespace MCU;

class DummyTimerFactory : public TimerFactory {
	public:
		Timer *createTimer(InterruptManager *intManager, Memory *mem,
						   Variant *variant, ACLK *aclk,
						   SMCLK *smclk, uint16_t tactl, uint16_t tar,
						   uint16_t taiv) {
			return new Timer(intManager, mem, variant, aclk, smclk, tactl, tar, taiv);
		}
};

class TimerTest : public CPPUNIT_NS :: TestFixture{
	CPPUNIT_TEST_SUITE(TimerTest);
	// TODO: Add tests for changing CCR0 in upMode and UpDownMode!
	CPPUNIT_TEST(upMode);
	CPPUNIT_TEST(continuousMode);
	CPPUNIT_TEST(upDownMode);
	CPPUNIT_TEST_SUITE_END();

	Memory *m;
	RegisterSet *r;
	Variant *v;
	InterruptManager *intManager;
	BasicClock *bc;
	TimerFactory *factory;

	public:
		void setUp (void) {
			m = new Memory(120000);
			r = new RegisterSet;
			r->addDefaultRegisters();
			v = getVariant("msp430x241x");
			intManager = new InterruptManager(r, m);
			factory = new DummyTimerFactory();
			bc = new BasicClock(m, v, intManager, factory);
		}

		void tearDown (void) {
			delete m;
			delete r;
			delete intManager;
			delete bc;
			delete factory;
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

};

CPPUNIT_TEST_SUITE_REGISTRATION (TimerTest);
