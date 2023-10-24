import RPi.GPIO as GP
import time
import os

GP.setmode(GP.BCM)
GP.setup(4, GP.IN)

'''
with open("DATA", "wt") as d:
	# 현재 시간
	#d.write(str(time.time()))
	#d.write("\n")
	# 조도값
	d.write("true" if GP.input(4) else "fasle")
'''

fname = ["DATA01", "DATA02", "DATA03"]
fip = ["IP : \n192.168.200.60\n", "IP : \n192.168.200.56\n", "IP : \n192.168.200.70\n"]

#start for
for i in range(50) :
	# start for
	for k in range(3) :
		print("Update data.")
	
		#start fopen
		with open(fname[k], "wt") as d :
			#시간
			d.write("Time : \n")
			d.write(str(time.time()))
			d.write("\n");
			
			# 조도값
			d.write("Bright : \n")
			d.write("true" if (GP.input(4)) else "false")
			d.write("\n")
		
			#ip 
			d.write(fip[k])
		#end fopen
		
		time.sleep(2)
	#end for
#end for

GP.cleanup()
	
