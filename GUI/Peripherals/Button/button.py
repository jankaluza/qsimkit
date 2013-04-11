from PythonQt.QtCore import *
from PythonQt.QtGui import *

class Peripheral():
	def __init__(self):
		self.width = 36
		self.height = 36
		self.pins = []
		self.pins.append(QRect(24, 12, 12, 12))
		self.state = False
		self.highWhenPushed = True
		self.out = []
		self.options = []
		self.options.append("+High when pushed")

	def executeOption(self, option):
		if (option == 0):
			if self.options[0][0] == "-":
				self.options[0] = "+" + self.options[0][1:]
				self.highWhenPushed = True
			else:
				self.options[0] = "-" + self.options[0][1:]
				self.highWhenPushed = False

	def save(self):
		return "<highWhenPushed>" + str(self.highWhenPushed) + "</highWhenPushed>"

	def simpleParser(self, xml, tag):
		start = xml.find("<" + tag + ">")
		end = xml.find("</" + tag + ">")
		if start == -1 or end == -1:
			return None

		return xml[start + len(tag) + 2 : end]

	def load(self, xml):
		c = self.simpleParser(xml, "highWhenPushed")
		if c != None and c == "False":
			self.highWhenPushed = False

	def reset(self):
		self.state = False;

		if self.highWhenPushed:
			self.out.append((0, 0.0))
		else:
			self.out.append((0, 1.0))

	def output(self):
		if len(self.out) == 0:
			return ()
		return self.out.pop()

	def externalEvent(self, pin, value):
		pass

	def timeAdvance(self):
		if not len(self.out) == 0:
			return 0
		return 365

	def clicked(self, p):
		self.state = not self.state
		self.screen.update()
		if self.state:
			if self.highWhenPushed:
				self.out.append((0, 1.0))
			else:
				self.out.append((0, 0.0))
		else:
			if self.highWhenPushed:
				self.out.append((0, 0.0))
			else:
				self.out.append((0, 1.0))
		self.hasNewOutput = True

	def paint(self):
		p = QPainter();
		p.begin(self.screen);
		p.drawRect(self.x, self.y + 6, 24, 24)

		color = Qt.black
		if self.state:
			color = Qt.green

		p.setPen(QPen(Qt.black, 2, Qt.SolidLine))
		p.setBrush(QBrush(color))
		p.drawLine(self.x + 3, self.y + 18, self.x + 3, self.y + 18);
		p.drawLine(self.x + 22, self.y + 18, self.x + 22, self.y + 18);
		if self.state:
			p.drawLine(self.x + 8, self.y + 18, self.x + 18, self.y + 18);
		else:
			p.drawLine(self.x + 8, self.y + 18, self.x + 18, self.y + 12);
		

		p.setPen(QPen(Qt.black, 1, Qt.SolidLine))
		p.setBrush(QBrush())
		p.drawEllipse(self.x + 3, self.y + 16, 4, 4);
		p.drawEllipse(self.x + 17, self.y + 16, 4, 4);
		#pol = QPolygon()
		#pol.append(QPoint(self.x + 8, self.y + 9))
		#pol.append(QPoint(self.x + 17, self.y + 18))
		#pol.append(QPoint(self.x + 8, self.y + 27))
		#p.drawPolygon(pol)
		

		for rect in self.pins:
			if self.state == self.highWhenPushed:
				p.fillRect(rect, QBrush(QColor(0,255,0)))
			p.drawRect(rect)

