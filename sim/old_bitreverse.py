#!/usr/bin/python

import math
import numpy as np

def bitReverse(v, N) :
	range = math.log(N ,2)
	b = '{:0{width}b}'.format(v, width=int(range))
	return int(b[::-1], 2)

def binary(num, length=13):
    return format(num, '#0{}b'.format(length + 2))

"""
for i in xrange(0,32):
		r = bitReverse(i,32)
		print ("{0:4} => {1:4}    {2}").format(i ,r, binary(r))
"""

def T(N1, N2, data_in):

	#create arrays of size N1*N2, to store
	#the output
	data_out = np.zeros((N1*N2,), dtype=np.uint16)

	#the index in
	idx_in = np.zeros((N1*N2,), dtype=np.uint16)

	#the index out
	Idx_out = np.zeros((N1*N2,), dtype=np.uint16)

	#do the copy
	for j in xrange(0, N2):
		for i in xrange(0, N1):
			idx = bitReverse (i, N1)
			print "{0} => {1}".format(i*N2 + j, j*N1 + idx)
			data_out [i*N2 + j] = data_in [j*N1 + idx]	

def T_opt1(N1, N2, data_in):

	#create an array of size N1*N2
	data_out = np.zeros((N1*N2,), dtype=np.uint16)

	#do the copy
	
	for i in xrange(0, N1):  #loop inversion in order to enlight the pattern
		for j in xrange(0, N2):
			idx = bitReverse (i, N1)
			print "{0} => {1}".format(i*N2 + j, j*N1 + idx)
			data_out [i*N2 + j] = data_in [j*N1 + idx]
		print ("   "),	

def T_opt1_excel(N1, N2, data_in):

	#create an array of size N1*N2
	data_out = np.zeros((N1*N2,), dtype=np.uint16)

	#do the copy
	
	for i in xrange(0, N1):  #loop inversion in order to enlight the pattern
		for j in xrange(0, N2):
			idx = bitReverse (i, N1)
			print "{0} => {1}".format(i*N2 + j, j*N1 + idx)
			data_out [i*N2 + j] = data_in [j*N1 + idx]
		print ("   "),	

def T_opt1_dis1(N1, N2, data_in):

	#create an array of size N1*N2
	data_out = np.zeros((N1*N2,), dtype=np.uint16)

	#do the copy
	
	for i in xrange(0, N1):  #loop inversion in order to enlight the pattern
		for j in xrange(0, N2):
			
			idx = bitReverse (i, N1)
			#print "{0} => {1}".format(i*N2 + j, j*N1 + idx),
			data_out [i*N2 + j] = data_in [j*N1 + idx]

			if j == 0:
				print "{0:5} => {1:5}".format(i*N2 + j, j*N1 + idx),
				print binary(j*N1 + idx)

		print ("   "),	

#Transpose modelization

FFTSize = 65536 #Total size of the output FFT that will be produced by Spider
N1 = 4096 		#could be 2048, 4096 or 8192, this is the size of samples provided to the FFTc module
N2 = FFTSize/N1 #number of time a FFTc unit must be used in order to complete a FFTsize

print "{0:15} {1}".format("FFTSize ", FFTSize)
print "{0:15} {1}".format("N1 ", N1)
print "{0:15} {1}".format("N2 ", N2)

#create a representation of datas
data_in = np.array(xrange(0,FFTSize), dtype=np.uint16)
print "{0:15} {1}".format("data_in ", data_in)
T_opt1(N1, N2, data_in)

#Modelization of the transpose function