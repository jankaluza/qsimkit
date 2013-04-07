from PythonQt.QtCore import *
from PythonQt.QtGui import *

class Peripheral():
	def __init__(self):
		self.width = 36
		self.height = 36
		self.pins = []
		self.pins.append(QRect(24, 12, 12, 12))
		self.state = False
		
	def reset(self):
		self.state = False;

	def externalEvent(self, pin, value):
		self.state = value;
		self.screen.update();

	def timeAdvance(self):
		return 365

	def paint(self):
		p = QPainter();
		p.begin(self.screen);
		p.drawRect(self.x, self.y + 6, 24, 24)

		color = Qt.black
		if self.state:
			color = Qt.green

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

