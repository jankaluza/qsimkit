from PythonQt.QtCore import *
from PythonQt.QtGui import *

# Pins
CS = 0
MOSI = 1 # Data In
SCK = 4
MISO = 6 # Data Out

# Tokens
MMC_START_DATA_BLOCK_TOKEN          = 0xfe   # Data token start byte, Start Single Block Read
MMC_START_DATA_MULTIPLE_BLOCK_READ  = 0xfe   # Data token start byte, Start Multiple Block Read
MMC_START_DATA_BLOCK_WRITE          = 0xfe   # Data token start byte, Start Single Block Write
MMC_START_DATA_MULTIPLE_BLOCK_WRITE = 0xfc   # Data token start byte, Start Multiple Block Write
MMC_STOP_DATA_MULTIPLE_BLOCK_WRITE  = 0xfd   # Data token stop byte, Stop Multiple Block Write

# commands: first bit 0 (start bit), second 1 (transmission bit); CMD-number + 0ffsett = 0x40
MMC_GO_IDLE_STATE          = 0x40     #CMD0
MMC_SEND_OP_COND           = 0x41     #CMD1
MMC_READ_CSD               = 0x49     #CMD9
MMC_SEND_CID               = 0x4a     #CMD10
MMC_STOP_TRANSMISSION      = 0x4c     #CMD12
MMC_SEND_STATUS            = 0x4d     #CMD13
MMC_SET_BLOCKLEN           = 0x50     #CMD16 Set block length for next read/write
MMC_READ_SINGLE_BLOCK      = 0x51     #CMD17 Read block from memory
MMC_READ_MULTIPLE_BLOCK    = 0x52     #CMD18
MMC_CMD_WRITEBLOCK         = 0x54     #CMD20 Write block to memory
MMC_WRITE_BLOCK            = 0x58     #CMD24
MMC_WRITE_MULTIPLE_BLOCK   = 0x59     #CMD25
MMC_WRITE_CSD              = 0x5b     #CMD27 PROGRAM_CSD
MMC_SET_WRITE_PROT         = 0x5c     #CMD28
MMC_CLR_WRITE_PROT         = 0x5d     #CMD29
MMC_SEND_WRITE_PROT        = 0x5e     #CMD30
MMC_TAG_SECTOR_START       = 0x60     #CMD32
MMC_TAG_SECTOR_END         = 0x61     #CMD33
MMC_UNTAG_SECTOR           = 0x62     #CMD34
MMC_TAG_EREASE_GROUP_START = 0x63     #CMD35
MMC_TAG_EREASE_GROUP_END   = 0x64     #CMD36
MMC_UNTAG_EREASE_GROUP     = 0x65     #CMD37
MMC_EREASE                 = 0x66     #CMD38
MMC_READ_OCR               = 0x67     #CMD39
MMC_CRC_ON_OFF             = 0x68     #CMD40

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
		self.buf = 0xff
		self.out_buf = []
		self.to_recv = 8
		self.frame = []
		self.frames_to_skip = 0
		self.blocklen = 0
		self.address = 0
		self.waiting_for_data = False
		self.cmd = 0
		self.total_size = 2097152
		self.mem = [0] * self.total_size

	def output(self):
		if len(self.out) == 0:
			return ()
		return self.out.pop(0)

	def handleFrameReceived(self):
		print "frame received", self.frame
		self.cmd = self.frame[0]

		if self.cmd == MMC_GO_IDLE_STATE:
			self.out_buf += [0xff, 0x01]
		elif self.cmd == MMC_SEND_OP_COND:
			# TODO: Add delay between switch from 0x01 to 0x00
			self.out_buf += [0xff, 0x00]
		elif self.cmd == MMC_READ_CSD:
			# dummy byte, R1 response, token
			self.out_buf += [0xff, 0x00, 0xfe]
			# CSD + reserved, TAAC, NSAC, TRAN_SPEED
			self.out_buf += [0x00, 0x26, 0x00, 0x32]

			read_bl_len = 9
			size = 1023
			size_mult = 0
			# total bytes:
			# (2**read_bl_len)*(2**(size_mult+2))*(size + 1)

			# CCC, READ_BL_LEN
			self.out_buf += [0x1f, 0x50 + read_bl_len]
			self.out_buf += [0x80 + ((size & 0xc00) >> 10), (size & 0x3fc) >> 2, 0x3e + ((size & 0x3) << 6)]
			self.out_buf += [0xf8 + size_mult & 0x6, 0x4f + ((size_mult & 0x1) << 7)]
			self.out_buf += [0xff, 0x92, 0x40, 0x40]
			self.out_buf += [0x1] # TODO: crc
		elif self.cmd == MMC_SET_BLOCKLEN:
			self.blocklen = 0
			self.blocklen = (self.frame[1] << 24) | (self.frame[2] << 16) | (self.frame[3] << 8) | self.frame[4]
			self.out_buf += [0xff, 0x00]
		elif self.cmd == MMC_WRITE_BLOCK:
			self.out_buf += [0xff, 0x00]
			self.waiting_for_data = True
			self.address = (self.frame[1] << 24) | (self.frame[2] << 16) | (self.frame[3] << 8) | self.frame[4]
		elif self.cmd == MMC_READ_SINGLE_BLOCK:
			print "READ SINGLE BLOCK"
			self.out_buf += [0xff, 0x00, 0xff, 0xfe]
			self.address = (self.frame[1] << 24) | (self.frame[2] << 16) | (self.frame[3] << 8) | self.frame[4]
			self.out_buf += self.mem[self.address:self.address + self.blocklen]

		self.frames_to_skip = len(self.out_buf)

	def handleDataFrameReceived(self):
		if self.cmd == MMC_WRITE_BLOCK:
			self.out_buf += [0xff, 0xff, 0xff, 0x05] # success
			self.waiting_for_data = False

			for i in range(len(self.frame) - 1):
				self.mem[self.address + i] = self.frame[i + 1]
			print self.mem[self.address:self.address + 20]

	def handleByteReceived(self):
		if self.waiting_for_data:
			if len(self.frame) == 0 and self.buf != 0xfe:
				return
			self.frame.append(self.buf)
			if len(self.frame) == self.blocklen:
				self.handleDataFrameReceived()
				self.frame = []
		else:
			self.frames_to_skip -= 1
			if self.frames_to_skip >= 0:
				return

			if len(self.frame) == 0 and (not self.buf & 0x40 or self.buf & 0x80):
				return

			self.frame.append(self.buf)
			if len(self.frame) == 6:
				self.handleFrameReceived()
				self.frame = []

	def externalEvent(self, pin, value):
		if pin != SCK:
			self.states[pin] = value >= 1.5
		else:
			if self.states[CS]:
				return
			en = self.states[pin]
			self.states[pin] = value >= 1.5
			if self.states[pin] == False and en:
				# shift left (MSB mode)
				self.buf = (self.buf << 1) & 0xff
				# sample -> store to buffer
				if self.states[MOSI]:
					self.buf |= (1)
				else:
					self.buf &= ~(1)
				self.to_recv -= 1
				#print self.to_recv, self.states[MOSI], bin(self.buf)
				if self.to_recv == 0:
					self.handleByteReceived()
					self.to_recv = 8
					if len(self.out_buf) == 0:
						self.buf = 0xff
					else:
						self.buf = self.out_buf.pop(0)
					print "next output is", hex(self.buf)
			elif self.states[pin] == True and not en:
				# Change output
				if self.buf & (1 << 7):
					self.out.append((MISO, 3.0))
				else:
					self.out.append((MISO, 0.0))

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

