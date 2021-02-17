#!/usr/bin/python3 

import os
import sys
import re

MAGIC_NUMBER_1 = chr(0xBA) + chr(0x5E) + chr(0xBA) + chr(0x11)
MAGIC_NUMBER_2 = chr(0xBA) + chr(0x5E) + chr(0xBA) + chr(0x12)

class Files:
	"""A class defining the infile and outfile"""
	def __init__(self):
		self.infile=None
		self.outfile=None

	# For opening files with encoder
	def eopen(self, input_name, output_name):
		try:
			self.infile=open(input_name, 'r')
			self.outfile=open(output_name, encoding='latin-1', mode='w')
		except:
			print("Error. Exception was raised when opening the file.")
			sys.exit(1)

	# For opening files with decoder
	def dopen(self, input_name, output_name):
		try:
			self.infile=open(input_name, encoding='latin-1', mode='r', newline="")
			self.outfile=open(output_name, 'w')
		except:
			print("Error. Exception was raised when opening the file.")
			sys.exit(1)

	# Close all files
	def fclose(self):
		self.infile.close()
		self.outfile.close()

def encode(input_name):
	(base_name, _, _) = input_name.rpartition(".")
	output_name = base_name + "." + "mtf"

	# Creating instance of class Files
	files=Files()
	files.eopen(input_name, output_name)

	# Writes magic number sequence to outfile
	write_magic_numbers(files)

	# Initialzing variables
	dictionary=[]
	status=1

	# Using re to split entire file into a list of words and '\n' characters
	text=files.infile.read()
	regex=re.split(r'(\s?|\n)', text)
	for item in regex:
		if item==' ' or item=='':
			regex.pop(regex.index(item))

	# Cycle through words in the list returned by re
	for word in regex:
		# Newline handling (& empty line handling)
		if word=='\n':
			files.outfile.write('\n')
			continue

		# Cycle through words in dictionary to attempt to find word it has encountered before
		for d_word in dictionary:
			if word==d_word:
				# Old word handling
				status=0
				index=dictionary.index(d_word)
				pos=len(dictionary)-index
				write_code_to_outfile(pos, files)
				# Rearraging dictionary
				move_to_top(d_word, index, dictionary)
				break
		if status:
			# New word handling
			dictionary.append(word)
			write_code_to_outfile(len(dictionary), files)
			files.outfile.write(word)
		status=1

	# Closing files
	files.fclose()

# Writes the magic number sequence to the file
def write_magic_numbers(files):
	global MAGIC_NUMBER_2
	for code in MAGIC_NUMBER_2:
		files.outfile.write(code)

# Moves current d_word to the top of the dictionary
def move_to_top(d_word, index, dictionary):
	dictionary.pop(index)
	dictionary.append(d_word)

# Outputs the right chr based on the value of code
def write_code_to_outfile(code, files):
	if code>=1 and code<121:
		files.outfile.write(chr(code+128))
	elif code>120 and code<376:
		files.outfile.write(chr(121+128)+chr(code-121))
	elif code>375 and code<=65912:
		files.outfile.write(chr(122+128)+chr((code-376)//256)+chr((code-376)%256))

def decode(input_name):
	(base_name, _, _) = input_name.rpartition(".")
	output_name = base_name + "." + "txt"

	# Creating instance of class Files
	files=Files()
	files.dopen(input_name, output_name)

	# Checking for mtf sequences
	check_magic_numbers(files)

	# Initializing variables
	dictionary=[]
	word=""
	status=1
	byte=None

	# Loops through chars in the file
	while True:
		# Newline handling
		if byte=='\n':
			files.outfile.write(byte)
			status=1

		# Read next byte if last word was terminated due to \n
		if status:
			byte=files.infile.read(1)
			status=0

		# Break at the end of the file
		if not byte:
			break

		# Check if byte is not a character
		if ord(byte)>128:
			# num holds decoded int value of num byte
			num=findnum(files, byte)

			# Reads next byte (sorta looking ahead to check for \n)
			byte=files.infile.read(1)

			# Check if the number has been encountered before i.e. old word
			if num<=len(dictionary):
				pos=len(dictionary)-num
				write_pos(pos, dictionary, files, byte)
				# Rearrange dictionary
				d_move_to_top(dictionary, pos)
				continue

			# Loops through chars until a number or '\n' is encountered and stores it in word
			while ord(byte)<128:
				word+=byte
				byte=files.infile.read(1)
				if byte=='\n':
					status=1
					break

			# Adds new word to dictionary and prints it
			dictionary.append(word)
			write_pos(-1, dictionary, files, byte)
			word=""

	# Closing files
	files.fclose()

# Checks if the file has either of the 2 magic number sequences in it
def check_magic_numbers(files):
	global MAGIC_NUMBER_1, MAGIC_NUMBER_2
	MAGIC_NUMBER_1=list(MAGIC_NUMBER_1)
	MAGIC_NUMBER_2=list(MAGIC_NUMBER_2)
	m_nums=[]
	for i in range(4):
		m_nums.append(chr(ord(files.infile.read(1))))
	if not (m_nums==MAGIC_NUMBER_1 or MAGIC_NUMBER_2):
		print("Error. Magic numbers not detected.")
		sys.exit(1)

# Moves word at position, pos to the top of the dictionary
def d_move_to_top(dictionary, pos):
	temp=dictionary[pos]
	dictionary.pop(pos)
	dictionary.append(temp)

# Returns decoded int value of byte
def findnum(files, byte):
	num=ord(byte)-128
	if num==121:
		byte=files.infile.read(1)
		num=ord(byte)+121
	elif num==122:
		byte=files.infile.read(1)
		num=(ord(byte)*256)+376
		byte=files.infile.read(1)
		num+=(ord(byte)%256)
	return num

def write_pos(pos, dictionary, files, byte):
	if byte=='\n':
		files.outfile.write(dictionary[pos])
	else:
		files.outfile.write("%s " % dictionary[pos])
