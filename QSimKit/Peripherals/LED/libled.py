from PythonQt.QtCore import *
from PythonQt.QtGui import *

class Peripheral():
	def __init__(self):
		self.width = 36
		self.height = 36
		self.pins = []
		self.pins.append(QRect(24, 12, 12, 12))
		self.state = False
		self.color = QColor(Qt.green);

		self.options = []
		self.options.append("Set color");

	def executeOption(self, option):
		if (option == 0):
			self.color = QColorDialog.getColor(self.color)

	def save(self):
		return "<color>" + self.color.name() + "</color>"

	def simpleParser(self, xml, tag):
		start = xml.find("<" + tag + ">")
		end = xml.find("</" + tag + ">")
		if start == -1 or end == -1:
			return None

		return xml[start + len(tag) + 2 : end]

	def load(self, xml):
		c = self.simpleParser(xml, "color")
		if c != None:
			self.color.setNamedColor(c)

	def reset(self):
		self.state = False;

	def externalEvent(self, pin, value):
		if value > 512: # HIGH_IMPEDANCE
			return
		self.state = value >  1.5;
		self.screen.update();

	def timeAdvance(self):
		return 365

	def paint(self):
		p = QPainter();
		p.begin(self.screen);
		p.drawRect(self.x, self.y + 6, 24, 24)

		color = Qt.black
		if self.state:
			color = self.color

		p.setPen(QPen(Qt.black, 2, Qt.SolidLine))
		p.setBrush(QBrush(color))
		p.drawLine(self.x + 3, self.y + 18, self.x + 22, self.y + 18);
		p.drawLine(self.x + 17, self.y + 12, self.x + 17, self.y + 24);

		p.setPen(QPen(Qt.black, 1, Qt.SolidLine))
		pol = QPolygon()
		pol.append(QPoint(self.x + 8, self.y + 9))
		pol.append(QPoint(self.x + 17, self.y + 18))
		pol.append(QPoint(self.x + 8, self.y + 27))
		p.drawPolygon(pol)
		p.setBrush(QBrush())

		for rect in self.pins:
			if self.state:
				p.fillRect(rect, QBrush(QColor(0,255,0)))
			p.drawRect(rect)

