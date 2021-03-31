# envoi par openMV mélange les exemples 
#   * openmv/arduino/arduino_uart.py
#   * openmv/april tags/find_april_tags_w_lens_zoom.py (champ de vision 
#     plus petit mais meilleure détection des marqueurs lointains)
#
#  openMV IDE 2.6.9 / openMV cam H7 capteur OV7725 / firmware 3.9.4
#  @ kirin, Debian Stretch 9.5 / 31 mars 2021
#  
#
# This example shows the power of the OpenMV Cam to detect April Tags
# on the OpenMV Cam M7. The M4 versions cannot detect April Tags.

import sensor, image, time, math
from pyb import UART

# UART 3, and baudrate.
uart = UART(3, 9600)

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.VGA) # we run out of memory if the resolution is much bigger...
sensor.set_windowing((160, 120)) # Look at center 160x120 pixels of the VGA resolution.
sensor.skip_frames(time = 2000)
sensor.set_auto_gain(False)  # must turn this off to prevent image washout...
sensor.set_auto_whitebal(False)  # must turn this off to prevent image washout...
clock = time.clock()

# Note! Unlike find_qrcodes the find_apriltags method does not need lens correction on the image to work.

# What's the difference between tag families? Well, for example, the TAG16H5 family is effectively
# a 4x4 square tag. So, this means it can be seen at a longer distance than a TAG36H11 tag which
# is a 6x6 square tag. However, the lower H value (H5 versus H11) means that the false positve
# rate for the 4x4 tag is much, much, much, higher than the 6x6 tag. So, unless you have a
# reason to use the other tags families just use TAG36H11 which is the default family.

while(True):
    clock.tick()
    img = sensor.snapshot()
    for tag in img.find_apriltags(): # defaults to TAG36H11
        img.draw_rectangle(tag.rect(), color = (255, 0, 0))
        img.draw_cross(tag.cx(), tag.cy(), color = (0, 255, 0))
        # print_args = (tag.id(), (180 * tag.rotation()) / math.pi)
        # print("Tag Family TAG36H11, Tag ID %d, rotation %f (degrees)" % print_args)

        # Afficher le message sur le terminal série d'openMV
        print_args = (tag.id(), tag.cx(), tag.cy())
        print("%d,%d,%d" % print_args)

        # Envoi du message à arduino
        uart.write("%d,"%tag.id())
        uart.write("%d,"%tag.cx())
        uart.write("%d\n"%tag.cy())
    #print(clock.fps())
