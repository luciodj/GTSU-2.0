#
# graph.py
#
# reads "potentiometer" value from  HID Simple demo
#
import hid
import time

# clear screen
print "\x1b[2J\x1b[1;1H"   

try:
    h = hid.device(0x4d8, 0x3f)
   
    while True:
        # test the button status
	    h.write( [0x37])
        d = h.read(3)
        if d:
            print "*".rjust( (d[1]+d[2]*256) >> 4 )
            time.sleep(0.1)

    print "Closing device"
    h.close()

except IOError, ex:
    print ex
    print "You probably don't have the hard coded test hid. Update the hid.device line"
    print "in this script with one from the enumeration list output above and try again."

print "Done"
