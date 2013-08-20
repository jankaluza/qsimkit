from PythonQt.QtCore import *
from PythonQt.QtGui import *

CS = 0
MOSI = 1
SCK = 4
MISO = 6

class Peripheral():
	def __init__(self):
		self.width = 12*10
		self.height = 12*8
		self.pins = []
		self.states = []
		for i in range(1, 8):
			self.pins.append(QRect(self.width - 12, i * 12, 10, 10))
			self.states.append(False)

		self.pins_desc = []
		self.pins_desc.append("CS")
		self.pins_desc.append("MOSI")
		self.pins_desc.append("GND")
		self.pins_desc.append("VCC")
		self.pins_desc.append("SCK")
		self.pins_desc.append("GND")
		self.pins_desc.append("MISO")

		self.reset()

	def executeOption(self, option):
		return

	def save(self):
		return ""

	def simpleParser(self, xml, tag):
		start = xml.find("<" + tag + ">")
		end = xml.find("</" + tag + ">")
		if start == -1 or end == -1:
			return None

		return xml[start + len(tag) + 2 : end]

	def load(self, xml):
		return

	def reset(self):
		self.out = []
		self.options = []
		#self.dl = BIT8
		self.n = False
		self.secondCycle = False
		self.current = self.states
		self.addr = 0
		self.cursorInc = 1
		self.shift = False
		self.disp = ['']*16
		self.cursor = 0

	def output(self):
		if len(self.out) == 0:
			return ()
		return self.out.pop(0)

	def dumpStates(self):
		for i in range(D0, D7 + 1):
			print i - D0, self.current[i]

	def executeCommand(self):
		#self.dumpStates()
		if not self.current[RS] and not self.current[RW]:
			if self.current[D7]:
				print "Set DDRAM address"
			elif self.current[D6]:
				print "Set CGRAM address"
			elif self.current[D5]:
				self.dl = self.current[D4]
				self.n = self.current[D3]
				self.f = self.current[D2]
				print "Function set, DL=", self.dl, "F=", self.f
			elif self.current[D4]:
				print "Cursor/display shift"
			elif self.current[D3]:
				print "Display on/off control"
			elif self.current[D2]:
				print "Entry mode set"
				if self.current[D1]:
					self.cursorInc = 1
				else:
					self.cursorInc = -1
				self.shift = self.current[D0]
			elif self.current[D1]:
				print "Cursor home"
				self.cursor = 0
			elif self.current[D0]:
				print "Clear display"
				self.disp = ['']*16
		elif self.current[RS] and not self.current[RW]:
			data = 0
			for i in range(8):
				data |= self.current[D0 + i] << i
			self.disp[self.cursor] = str(unichr(data));
			self.cursor += self.cursorInc;
			self.screen.update()

	def externalEvent(self, pin, value):
		if pin != SCK:
			self.states[pin] = value >= 1.5
		else:
			en = self.states[pin]
			self.states[pin] = value >= 1.5
			if self.states[pin] == False and en:
				print "sample"

	def timeAdvance(self):
		if not len(self.out) == 0:
			return 0
		return 365

	def clicked(self, p):
		return

	def paint(self):
		p = QPainter();
		p.begin(self.screen);

		pol = QPolygon()
		pol.append(QPoint(self.x, self.y))
		pol.append(QPoint(self.x + self.width - 10, self.y))
		pol.append(QPoint(self.x + self.width, self.y + 10))
		pol.append(QPoint(self.x + self.width, self.y + self.height))
		pol.append(QPoint(self.x, self.y + self.height))
		p.setBrush(QBrush(QColor(226, 206, 255)))
		p.drawPolygon(pol);
		p.setBrush(QBrush(QColor(255, 255, 255)))

		i = 0
		for rect in self.pins:
			if self.states[i]:
				p.fillRect(rect, QBrush(QColor(0,255,0)))
			i += 1
			p.drawRect(rect)

