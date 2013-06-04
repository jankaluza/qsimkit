#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include "CPU/Memory/Memory.h"
#include "CPU/Memory/RegisterSet.h"
#include "CPU/Interrupts/InterruptManager.h"
#include "CPU/Pins/PinMultiplexer.h"
#include "CPU/Pins/PinHandler.h"
#include "CPU/Pins/PinManager.h"
#include "CPU/Variants/Variant.h"
#include "CPU/Variants/VariantManager.h"

namespace MSP430 {

class DummyPinHandler : public PinHandler {
	public:
		DummyPinHandler() : active(false), value(0) {}

		void handlePinInput(const std::string &name, double v) {
			value = v;
		}

		void handlePinActivated(const std::string &name) {
			active = true;
		}

		void handlePinDeactivated(const std::string &name) {
			active = false;
		}

		bool active;
		double value;
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

class PinMultiplexerTest : public CPPUNIT_NS :: TestFixture{
	CPPUNIT_TEST_SUITE(PinMultiplexerTest);
	CPPUNIT_TEST(switchHandlers);
	CPPUNIT_TEST(generateOutput);
	CPPUNIT_TEST_SUITE_END();

	Memory *m;
	RegisterSet *r;
	Variant *v;
	InterruptManager *intManager;
	DummyPinHandler *cci0a;
	PinManager *pinManager;
	PinMultiplexer *mpx;
	DummyPinWatcher *watcher;
	
	public:
		void setUp (void) {
			m = new Memory(120000);
			r = new RegisterSet;
			r->addDefaultRegisters();
			v = getVariant("msp430x241x");
			intManager = new InterruptManager(r, m);
			pinManager = new PinManager(m, intManager, v);
			watcher = new DummyPinWatcher();
			pinManager->setWatcher(watcher);

			mpx = pinManager->addPin(P1, 0);

			{
				PinMultiplexer::Condition c;
				c["sel"] = 0;
				mpx->addMultiplexing(c, "GP");
			}

			{
				PinMultiplexer::Condition c;
				c["dir"] = 0;
				c["sel"] = 1;
				mpx->addMultiplexing(c, "CCI0A");
			}

			cci0a = new DummyPinHandler();
			pinManager->addPinHandler("CCI0A", cci0a);
		}

		void tearDown (void) {
			delete m;
			delete r;
			delete intManager;
			delete pinManager;
			delete cci0a;
		}

		void switchHandlers() {
			m->setBitWatcher(v->getP1OUT(), 1, true);
			// No output, because pin is in input mode
			CPPUNIT_ASSERT_EQUAL(-1, watcher->id);
			m->setBitWatcher(v->getP1DIR(), 1, true);
			CPPUNIT_ASSERT_EQUAL(0, watcher->id);
			watcher->id = -1;

			// this pin input is forwarded to GP, not CCIOA
			pinManager->handlePinInput(0, 1.0);
			CPPUNIT_ASSERT_EQUAL(0.0, cci0a->value);
			CPPUNIT_ASSERT_EQUAL(false, cci0a->active);

			// switch to CCI0A, the value on PIN is still 1.0
			m->setBitWatcher(v->getP1SEL(), 1, true);
			// cci0a not set yet, because DIR is 1
			CPPUNIT_ASSERT_EQUAL(false, cci0a->active);
			m->setBitWatcher(v->getP1DIR(), 1, false);
			CPPUNIT_ASSERT_EQUAL(true, cci0a->active);
			// this pin input is forwarded CCIOA
			CPPUNIT_ASSERT_EQUAL(1.0, cci0a->value);

			m->setBitWatcher(v->getP1SEL(), 1, false);
			CPPUNIT_ASSERT_EQUAL(false, cci0a->active);
		}

		void generateOutput() {
			mpx->generateOutput(cci0a, 1.0);
			CPPUNIT_ASSERT_EQUAL(-1.0, watcher->value);
			m->setBitWatcher(v->getP1SEL(), 1, true);
			m->setBitWatcher(v->getP1DIR(), 1, true);
			CPPUNIT_ASSERT_EQUAL(1.0, watcher->value);
		}

};

CPPUNIT_TEST_SUITE_REGISTRATION (PinMultiplexerTest);

}
