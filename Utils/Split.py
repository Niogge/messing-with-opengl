#!/usr/bin/env python

# Will split a png cubemap/skymap image produced by blender into 6 separated image files for use in a skybox within unity
# Requires Python Imaging Library > http://www.pythonware.com/products/pil/

# The author takes no responsibility for any damage this script might cause,
# feel free to use, change and or share this script.
# 2013-07, CanOfColliders, m@canofcolliders.com

# Forked by Niogge, Cross layout version

from PIL import Image
import sys, os

path = os.path.abspath("") + "/";
processed = False
flipped = False;

def processImage(path, name):
	img = Image.open(os.path.join(path, name))
	size = img.size[0] / 4 
	
	if flipped:
		splitAndSave(img, size, size, size, addToFilename(name, "_left"))
		splitAndSave(img, size*2, size, size, addToFilename(name, "_front"))
		splitAndSave(img, size * 3, size, size, addToFilename(name, "_right"))
		splitAndSave(img, 0, size, size, addToFilename(name, "_back"))
		splitAndSave(img, size*2, size*2, size, addToFilename(name, "_down"))
		splitAndSave(img, size*2, 0, size, addToFilename(name, "_up"))
	else:
		splitAndSave(img, size, size*2, size, addToFilename(name, "_down"))
		splitAndSave(img, size, 0, size, addToFilename(name, "_up"))
		splitAndSave(img, 0, size, size, addToFilename(name, "_left"))
		splitAndSave(img, size, size, size, addToFilename(name, "_front"))
		splitAndSave(img, size * 2, size, size, addToFilename(name, "_right"))
		splitAndSave(img, size * 3, size, size, addToFilename(name, "_back"))

def addToFilename(name, add):
	name = name.split('.')
	return name[0] + add + "." + name[1]

def splitAndSave(img, startX, startY, size, name):
	area = (startX, startY, startX + size, startY + size)
	saveImage(img.crop(area), path, name)

def saveImage(img, path, name):
	try:
		img.save(os.path.join(path, name))
	except:
		print ("*   ERROR: Could not convert image.")
		pass

for arg in sys.argv:
	if "flip" in arg:
		flipped = True;

for arg in sys.argv:
	if ".png" in arg or ".jpg" in arg:
		processImage(path, arg)
		processed = True
if not processed:
	print ("*  ERROR: No Image")
	print ("   usage: 'python script.py image-name.png  flip(optional, if top and bottom are on the right side)'")
	