from PythonQt.QtCore import *
from PythonQt.QtGui import *

GND = 0
VCC = 1
VO = 2
RS = 3
RW = 4
EN = 5
D0 = 6
D1 = 7
D2 = 8
D3 = 9
D4 = 10
D5 = 11
D6 = 12
D7 = 13

BIT4 = 0
BIT8 = 1

class Peripheral():
	def __init__(self):
		self.width = 200
		self.height = 36
		self.pins = []
		self.states = []
		for i in range(0, 16):
			self.pins.append(QRect(i*12, 0, 10, 10))
			self.states.append(False)

		self.pins_desc = []
		self.pins_desc.append("GND")
		self.pins_desc.append("VCC")
		self.pins_desc.append("VO")
		self.pins_desc.append("RS")
		self.pins_desc.append("RW")
		self.pins_desc.append("EN")
		self.pins_desc.append("D0")
		self.pins_desc.append("D1")
		self.pins_desc.append("D2")
		self.pins_desc.append("D3")
		self.pins_desc.append("D4")
		self.pins_desc.append("D5")
		self.pins_desc.append("D6")
		self.pins_desc.append("D7")
		self.pins_desc.append("BL+")
		self.pins_desc.append("BL-")

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
		self.dl = BIT8
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
			try:
				self.screen.update()
			except:
				pass

	def receiveCommand(self):
		if self.dl == BIT4 and not self.secondCycle:
			self.current = list(self.states)
			self.secondCycle = True
			return
		elif self.dl == BIT8:
			self.current = self.states
			self.executeCommand()
			return

		if self.secondCycle:
			self.secondCycle = False
			self.current[D0:D3+1] = self.states[D4:D7+1]
			self.executeCommand()

	def externalEvent(self, pin, value):
		if pin != EN:
			self.states[pin] = value >= 1.5
		else:
			en = self.states[pin]
			self.states[pin] = value >= 1.5
			if self.states[pin] == False and en:
				self.receiveCommand()

	def timeAdvance(self):
		if not len(self.out) == 0:
			return 0
		return 365

	def clicked(self, p):
		return

	def paint(self):
		p = QPainter();
		p.begin(self.screen);
		p.fillRect(self.x, self.y + 10, self.width, self.height - 10, QColor(226, 206, 255));
		p.drawRect(self.x, self.y + 10, self.width, self.height - 10);

		p.fillRect(self.x + 3, self.y + 13, self.width - 6, self.height - 16, QColor(68, 255, 83))
		p.drawRect(self.x + 3, self.y + 13, self.width - 6, self.height - 16);
		i = 0
		for c in self.disp:
			if len(c):
				p.drawText(self.x + 10 + i*10, self.y + 16, 10, self.height - 16, 0, c);
			i += 1

		i = 0
		for rect in self.pins:
			if self.states[i]:
				p.fillRect(rect, QBrush(QColor(0,255,0)))
			i += 1
			p.drawRect(rect)

