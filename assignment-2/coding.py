#!/usr/bin/env python3

import os
import sys

# Code by Tarush Roy
# a.k.a Troy a.k.a Astrochicken827
# Netlink login: tarushroy
# Student Number: V00883469

def encode_main():
	# File handling
	if len(sys.argv) == 1:
		print("Error. No filename detected.")
		sys.exit(1)
	else:
		infilename = sys.argv[1]

	if ".mtf" in infilename:
		print("Error. Incorrect file type. Please use a .txt file.")
		sys.exit(1)

	# Constructing outfile name
	outfilename=(infilename.split('.'))[0]+".mtf"

	# Opening files
	try:
		infile = open(infilename, 'r')
		outfile = open(outfilename, encoding="latin-1", mode="w") # Code by Michael Zastre from mmost SENG265
	except FileNotFoundError:
		print("Error. File does not exist.")

	# Writes the *magical* numbers to outfile
	write_magic_numbers(outfile)

	# Initializing variables
	num_words=0
	status=1
	dictionary=[0]*200

	# Reading through each line in the file >
	# > Reading each word in the line >
	# > Determine if its a new word or old word and handles it
	for line in infile:
		# Empty line handling
		if line=='\n':
			outfile.write(chr(10))
			continue

		# Removes trailing \n character and splits line into the words list
		line=line.strip()
		words=line.split()

		# Cycle through words in the dictionary and then words in the line to find old and new words
		for word in words:
			for y in range(num_words):
				if word==dictionary[y]:
					# Old word handling
					status=0
					pos=num_words-y
					outfile.write(chr(pos + 128))

					# Rearranging dictionary
					temp=dictionary[y]
					for z in range(pos):
						dictionary[z+y], dictionary[z+y+1] = dictionary[z+y+1], dictionary[z+y]
					dictionary[num_words-1] = temp

					break
			if status:
				# New word handling
				dictionary[num_words]=word
				num_words+=1
				outfile.write(chr(num_words + 128))
				outfile.write(word)
			status=1
		outfile.write(chr(10)) # Writes newline character to outfile

	# Closing files
	infile.close()
	outfile.close()

def decode_main():
	# File handling
	if len(sys.argv) == 1:
		print("Error. No filename detected")
		sys.exit(1)
	else:
		infilename = sys.argv[1]
	if ".txt" in infilename:
		print("Error: Incorrect file type. Please use a .mtf file")
		sys.exit(1)

	# Constructing outfilename
	outfilename=(infilename.split('.'))[0]+".txt"

	# Opening files
	try:
		infile = open(infilename, encoding="latin-1", mode='r')
		outfile = open(outfilename, 'w')
	except FileNotFoundError:
		print("Error. File does not exist")
		sys.exit(1)

	# Checks if the file has the *magical* numbers
	check_magic_numbers(infile)
	
	# Initializing variables
	dictionary=[""]*200
	word=""
	num_words=0
	status=1
	byte=None
	
	# Reads .mtf file char by char
	while True:
		# Newline handling
		if byte=='\n':
			outfile.write(byte)
			status=1

		# Read next byte if last word was terminated due to \n
		if status:
			byte=infile.read(1)
			status=0

		# Break at the end of the file
		if not byte:
			break

		# Check if char is a number
		if ord(byte)>128:
			num=ord(byte)-128
			byte=infile.read(1)
			# Check if the number has been encountered before i.e. old word
			if num<=num_words:
				pos=num_words-num
				if byte=='\n':
					outfile.write(dictionary[pos])
				else:
					outfile.write("%s " % dictionary[pos])

				# Rearranging dictionary
				for i in range(num-1):
					dictionary[pos+i], dictionary[pos+i+1] = dictionary[pos+i+1], dictionary[pos+i]
				
				word=""
				continue

			# Loops through word characters until a number or '\n' is encountered and stores it in word
			while ord(byte)<128:
				word+=byte
				byte=infile.read(1)
				if byte=='\n':
					status=1
					break

			# Adds new word to dictionary and prints it
			dictionary[num_words]=word
			num_words+=1
			if byte=='\n':
				outfile.write(dictionary[num_words-1])
			else:
				outfile.write("%s " % dictionary[num_words-1])

			word=""

	# Closing files
	infile.close()
	outfile.close()

# Writing magic number sequence for mtf to outfile
# Taken from mmost.csc.uvic.ca SENG265 Course Central
# Code by Michael Zastre
def write_magic_numbers(outfile):
	magic_number = chr(0xba) + chr(0x5e) + chr(0xba) + chr(0x11)
	for code in magic_number:
		outfile.write(code)

# Checks if the given file has the magical numbers in the first 4 bytes by storing bytes in an array
def check_magic_numbers(infile):
	m_nums=[0]*4
	for i in range(4):
		m_nums[i]=ord(infile.read(1))
	if m_nums!=[186, 94, 186, 17]:
		print("Error. Magic numbers not detected.")
		sys.exit(1)

# Taken from a2_writeup.pdf on connex. Skeleton code for coding.py
command = os.path.basename(__file__)
if __name__ == "__main__" and command == "text2mtf.py":
	encode_main()
elif __name__ == "__main__" and command == "mtf2text.py":
	decode_main()
