from PythonQt.QtCore import *
from PythonQt.QtGui import *

class Peripheral():
	def __init__(self):
		self.width = 30
		self.height = 36
		self.pins = []
		self.pins.append(QRect(12, 24, 12, 12))
		self.state = False
		
	def reset(self):
		pass

	def externalEvent(self, pin, value):
		self.state = value;

	def timeAdvance(self):
		return 365

	def paint(self):
		p = QPainter();
		p.begin(screen);
		p.drawEllipse(self.x + 6, self.y, self.width - 6, self.height - 12);

		for rect in self.pins:
			if self.state:
				p.fillRect(rect, QBrush(QColor(0,255,0)))
			p.drawRect(rect)

