#!/usr/bin/python

import math
import numpy as np

"""
This file is designed to study the Transpose function done by Spider.
Indeed, Spider is using a DOPF algorithm, that needs the data to be specifically
organized.
The goal of this python script is to highlight the different ways to compute the 
DOPF transpose, in order to port it on a dedicated hardware, such as EDMA. Expected
result is cherry-picking CPU cycles.
"""

#playable parameters
FFTSize = 65536 #Total size of the output FFT that will be produced by Spider
N2 = 4096  		#could be 2048, 4096 or 8192, this is the size of samples provided to the FFTc module
N1 = FFTSize/N2	#number of time a FFTc unit must be used in order to complete a FFTsize


data_in  = None #corresponds to the input memory data.

#Internal parameters
data_out = None #corresponds to the output memory data.
idx_in	 = None #corresponds to the index generated in input
idx_out	 = None #corresponds to the index generated in output

"""===== USEFUL FUNCTIONS ====="""
def data_reset():
	global data_out; 
	global idx_in;
	global idx_out
	data_out = np.zeros((N1*N2,), dtype=np.uint16) #stores the generated memory output
	idx_in	 = np.zeros((N1*N2,), dtype=np.uint16)
	idx_out  = np.zeros((N1*N2,), dtype=np.uint16)
	return

#V is the value while N is the MAXIMUM VALUE that v will take.
def bitReverse(v, N) :
	range = math.ceil(math.log(N, 2))
	b = '{:0{width}b}'.format(v, width=int(range))
	return int(b[::-1], 2)

# def bitReverse( x, num_bits ):
#     answer = 0
#     for i in range( num_bits ):                   # for each bit number
#         if (x & (1 << i)):                        # if it matches that bit
#             answer |= (1 << (num_bits - 1 - i))   # set the "opposite" bit in answer
#     return answer


def binary(num, length=16):
	return format(int(num), '#0{}b'.format(length + 2))


def display_info(display_input_data = True):
	print "{0:15} {1}".format("FFTSize ", FFTSize)
	print "{0:15} {1}".format("N1 ", N1)
	print "{0:15} {1}".format("N2 ", N2)

	if display_input_data:
		data_in = np.array(xrange(0,FFTSize), dtype=np.uint16)
		print "{0:15} {1}".format("data_in ", data_in)


g = lambda str, bool : str if bool else ""
#This display function support iterator, that is only certain index can be displayed.
def display_function_tupple(iterator_array=xrange(0, FFTSize), 
							ix_in=True, 
							bin_ix_in=False, 
							ix_out=True,
							bin_ix_out=True, 
							width=7,
							sorting_key=None):

	tupple = []

	for i in iterator_array:
		#create a new tupple that contains all datas:
		tupple.append((idx_in[i], binary(idx_in[i]), idx_out[i], binary(idx_out[i])))

	#sort if asked
	if(sorting_key != None):
		tupple = sorted(tupple, key = sorting_key)

	#display header:
	print "\n{0:{w}} {1:{w}} {2} {3:{w}} {4:{w}}\n".format(
		g("Src IDX", ix_in), 
		g("Src bin", bin_ix_in), 
		"=>", 
		g("Dst IDX", ix_out), 
		g("Dst bin", bin_ix_out), 
		w=width)

	#display the sorted array.
	for t in tupple:
		print "{0:{w}} {1:{w}} {2} {3:{w}} {4:{w}}".format(
			g(t[0], ix_in), 
			g(t[1], bin_ix_in), 
			"=>", 
			g(t[2], ix_out), 
			g(t[3], bin_ix_out), 
			w=width)

def csv_function_tupple(iterator_array=xrange(0, FFTSize), 
							ix_in=True, 
							ix_out=True,
							width=7,
							sorting_key=None):

	tupple = []
	tuppletmp = []

	for i in xrange(0,FFTSize):
		#create a new tupple that contains all datas:
		tuppletmp.append((idx_in[i], idx_out[i]))

	#sort if asked
	if(sorting_key != None):
		tuppletmp = sorted(tuppletmp, key = sorting_key)

	for i in iterator_array:
		tupple.append(tuppletmp[i])

	#display the sorted array.
	for t in tupple:
		print "{0:{w}},{1:{w}}".format(
			g(t[0], ix_in), 
			g(t[1], ix_out), w=width)

#print "{0} => {1}".format(i*N2 + j, j*N1 + idx)


"""===== TRANSPOSE FUNCTIONS ====="""

def T(N1, N2, data_in):				#original T function
	data_reset()
	k = int(0)

	for j in xrange(0, N2):
		for i in xrange(0, N1):
			idx = bitReverse (i, N1)
			data_out [i*N2 + j] = data_in [j*N1 + idx]	

			idx_in[k] = j*N1 + idx
			idx_out[k]  = i*N2 + j
			k+=1

def T_bis(N1, N2, data_in):				#original T function
	data_reset()
	k = int(0)

	for j in xrange(0, N2):
		for i in xrange(0, N1):
			idx = bitReverse (j, N2)
			data_out [j*N1 + i] = data_in [i*N2 + idx]	

			idx_in[k] = i*N2 + idx
			idx_out[k]  = j*N1 + i
			k+=1

def T2(N1, N2, data_in):			#loop inverted version
	data_reset()
	k = int(0)
	
	for i in xrange(0, N1):	
		for j in xrange(0, N2):
			idx = bitReverse (i, N1)
			data_out [i*N2 + j] = data_in [j*N1 + idx]	

			idx_in[k] = j*N1 + idx
			idx_out[k]  = i*N2 + j
			k+=1

def bitrev(N1):
	data_reset()

	for i in xrange(0, N1) :
		idx_out[i] = bitReverse(i, N1)
		idx_in[i] = i

def T_opt1(N1, N2, data_in):		#loop inverted version
	data_reset()
	
	for i in xrange(0, N1):  		
		for j in xrange(0, N2):
			idx = bitReverse (i, N1)
			print "{0} => {1}".format(i*N2 + j, j*N1 + idx)
			data_out [i*N2 + j] = data_in [j*N1 + idx]
		print ("   "),

def T_opt1_dis1(N1, N2, data_in):
	data_reset()
	
	for i in xrange(0, N1): 
		for j in xrange(0, N2):
			
			idx = bitReverse (i, N1)
			#print "{0} => {1}".format(i*N2 + j, j*N1 + idx),
			data_out [i*N2 + j] = data_in [j*N1 + idx]

			if j == 0:
				print "{0:5} => {1:5}".format(i*N2 + j, j*N1 + idx),
				print binary(j*N1 + idx)

		print ("   "),	


#create a representation of data
data_in = np.array(xrange(0,FFTSize), dtype=np.uint16)


#__MAIN__
display_info()

"""DISPLAY BITREVERSE"""
#bitrev(N1)
#csv_function_tupple(xrange(0, N1))

"""DISPLAY INTERLEAVING"""


T(N1, N2, data_in)
#T2(N1, N2, data_in)


#display_function_tupple(sorting_key=lambda s:s[0])

csv_function_tupple(xrange(0, FFTSize), sorting_key=lambda s:s[1])
#display_function_tupple(xrange(0, FFTSize, 16), bin_ix_in=True, sorting_key=lambda s:s[3])
#display_function_tupple(xrange(0, FFTSize), bin_ix_in=True)
#display_function_tupple()


#T_opt1(N1, N2, data_in)
