import time
import random
from Tkinter import *
from device import Device

class App:
	def __init__(self, root):
		self.fm = Frame(root, width=800, height=600, bg="red")
		self.fm.pack(side=TOP, expand=NO, fill=NONE)

	def setdev(self, dev):
		self.dev = dev

	def setbgs(self, bg1, bg2):
		self.bg1 = bg1
		self.bg2 = bg2

	def update(self):
		self.bg1 = self.bg1 + 0x000010
		self.bg2 = self.bg2 + 0x000100
		self.bg1 = self.bg1 - 0x000100
		self.bg2 = self.bg2 - 0x000010
		self.bg1 &= 0xffffff
		self.bg2 &= 0xffffff
	
		if dev.eye^1 is 1:
			display.fm.config(bg="#%.6x" % self.bg1)
		else:
			display.fm.config(bg="#%.6x" % self.bg2)
		
		dev.flip()
	
		root.after(1000/30, self.update)
	

dev = Device()
root = Tk()
display = App(root)

display.setdev(dev)
display.setbgs(0xff0000, 0x0000ff)

root.after(1000, display.update)
root.mainloop()

