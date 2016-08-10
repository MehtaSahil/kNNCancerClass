import os
import sys

input_filename = "ktest.txt"
output_filename = "%s.csv" % os.path.splitext(input_filename)[0]
fout = open(output_filename, 'w');

line_trigger = 0
with open(input_filename) as f:
	for line in f:
		# exclude newline character
		temp = line[:-1]
		if line_trigger == 0:
			fout.write(temp + ",")
		elif line_trigger == 1:
			symbs = temp.split(" ")
			fout.write(symbs[2] + ",")
		elif line_trigger == 2:
			symbs = temp.split(" ")
			fout.write(symbs[2])
			fout.write("\n")

		if line_trigger == 3:
			line_trigger = 0
		else:
			line_trigger += 1
fout.close()
