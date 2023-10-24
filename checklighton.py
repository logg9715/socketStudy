import RPi.GPIO as GPIO
import time
import requests
print("Get file remote:")

GPIO.setmode(GPIO.BCM)
GPIO.setup(18,GPIO.OUT)

#ulist = ["http://192.168.200.79:8000/DATA01", "http://192.168.200.79:8000/DATA02", "http://192.168.200.79:8000/DATA03"]
ulist = ["http://localhost:8000/DATA01", "http://localhost:8000/DATA02", "http://192.168.200.70:8000/DATA"]
for i in range(50):
	for url in ulist : 
		#print(url, ": \n\n")
		r =requests.get(url)
		#diff =time.time() - float(r.text)
		print(r.text)
		time.sleep(0.3)
	
	print("------------------------\n")
	time.sleep(2)
GPIO.cleanup()
