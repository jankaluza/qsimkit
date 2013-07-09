from PythonQt.QtCore import *
from PythonQt.QtGui import *

class Peripheral():
	def __init__(self):
		self.width = 36
		self.height = 48
		self.pins = []
		self.pins.append(QRect(24, 12, 10, 10))
		self.pins.append(QRect(24, 24, 10, 10))
		self.state = False
		self.out = []
		self.options = []
		self.options.append("Set frequency")
		self.frequency = 1000000
		self.step = 1.0 / self.frequency / 2

	def executeOption(self, option):
		self.frequency = QInputDialog.getInt(None, "Set frequency", "Frequency (Hz):", self.frequency)

	def save(self):
		return "<frequency>" + str(self.frequency) + "</frequency>"

	def simpleParser(self, xml, tag):
		start = xml.find("<" + tag + ">")
		end = xml.find("</" + tag + ">")
		if start == -1 or end == -1:
			return None

		return xml[start + len(tag) + 2 : end]

	def load(self, xml):
		c = self.simpleParser(xml, "frequency")
		if c != None:
			self.frequency = int(c)
			self.step = 1.0 / self.frequency / 2

	def reset(self):
		self.state = False;
		self.out.append((0, 0.0))
		self.out.append((1, 0.0))

	def output(self):
		if len(self.out) == 0:
			return ()
		return self.out.pop(0)

	def externalEvent(self, pin, value):
		pass

	def internalTransition(self):
		self.state = not self.state
		self.out.append((0, self.state * 3))
		self.out.append((1, self.state * 3))

	def timeAdvance(self):
		return self.step

	def paint(self):
		p = QPainter();
		p.begin(self.screen);
		p.drawRect(self.x, self.y, self.width - 12, self.height)

		#color = Qt.black
		#if self.state:
			#color = Qt.green

		#p.setPen(QPen(Qt.black, 2, Qt.SolidLine))
		#p.setBrush(QBrush(color))
		#p.drawLine(self.x + 3, self.y + 18, self.x + 3, self.y + 18);
		#p.drawLine(self.x + 22, self.y + 18, self.x + 22, self.y + 18);
		#if self.state:
			#p.drawLine(self.x + 8, self.y + 18, self.x + 18, self.y + 18);
		#else:
			#p.drawLine(self.x + 8, self.y + 18, self.x + 18, self.y + 12);

		p.drawRect(self.x + 8, self.y + 12, 9, 24);
		

		#p.setPen(QPen(Qt.black, 1, Qt.SolidLine))
		#p.setBrush(QBrush())
		#p.drawEllipse(self.x + 3, self.y + 16, 4, 4);
		#p.drawEllipse(self.x + 17, self.y + 16, 4, 4);
		#pol = QPolygon()
		#pol.append(QPoint(self.x + 8, self.y + 9))
		#pol.append(QPoint(self.x + 17, self.y + 18))
		#pol.append(QPoint(self.x + 8, self.y + 27))
		#p.drawPolygon(pol)
		

		for rect in self.pins:
			if self.state:
				p.fillRect(rect, QBrush(QColor(0,255,0)))
			p.drawRect(rect)

