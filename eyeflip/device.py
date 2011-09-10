import usb
import usb.util
import usb.core
import sys
	
class Device:
	VENDOR	= 0x1bae
	PRODUCT = 0x0002

	eye = 0

	def __init__(self):
		dev = usb.core.find(idVendor = self.VENDOR, idProduct = self.PRODUCT)
		if dev is None:
			raise ValueError('Device not found')

		for i in range(4):
			try:
				dev.detach_kernel_driver(i)
				dev.detach_kernel_head(i)
			except:
				pass

		epx = None
		dev.set_configuration(1)
		for cfg in dev:
			for intf in cfg:
				for ep in intf:
					if ep.bEndpointAddress is 0x05 :
						epx = ep

		if epx is None:
			raise ValueError('ep not found')

		self.epo = epx
		self.epi = epx

		dev.ctrl_transfer(0x40, 0x9, 0x302, 0x3, "\x02\x07\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00")
		dev.ctrl_transfer(0x40, 0x9, 0x302, 0x3, "\x02\x07\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00")
		ep.write("\x00")
		dev.ctrl_transfer(0x40, 0x9, 0x302, 0x3, "\x02\x07\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00")


	def xwrite(self, what):
		self.epo.write(what)

	def xread(self):
		return self.epo.read(1);

	def flip(self):
		self.eye ^= 1
		if self.eye is 1:
			self.xwrite("\x01")
		else:
			self.xwrite("\x00")
		

