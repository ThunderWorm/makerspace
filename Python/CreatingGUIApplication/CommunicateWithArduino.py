#-------------------------------------------------------------------------------
# Name:        module1
# Purpose:
#
# Author:      ThunderEvolverWormVK
#
# Created:     07/08/2016
# Copyright:   (c) ThunderEvolverWormVK 2016
# Licence:     <your licence>
#-------------------------------------------------------------------------------

import serial
import tkMessageBox
from Tkinter import *
from threading import Thread

root = Tk()
root.geometry("250x150+300+300")
root.title("Testing controls")

# Frames, containers for controls
frame = Frame(root)
frame.pack()

inputframe = Frame(root)
inputframe.pack()

bottomFrame = Frame(root)
bottomFrame.pack(side = BOTTOM)

# Some labels and buttons
w = Label(frame, text="Communicate with Arduino!")
w.pack()


def displayAnswerFromArduino():
   ind = 0
   while ind < 10 :
       time.sleep(100);
       a = ser.read()
       e2.delete(0, END)
       e2.insert(0, a)

def sendNumberToArduino():
   ser = serial.Serial('/dev/' + e3.get(), 9600)
   ser.write(e1.get())
   a = ser.read()

   t = Thread(target=displayAnswerFromArduino)
   t.start()

B = Button(frame, text ="Send number to Arduino", command = sendNumberToArduino)

B.pack(side = LEFT)


def byeCallBack():
   tkMessageBox.showinfo( "Bye Python", "Bye World")
C = Button(frame, text ="Bye", command = byeCallBack)
C.pack(side = LEFT )


# Entries (Textboxes)
Label(inputframe, text="Times to run").grid(row=0)
Label(inputframe, text="Response from Arduino").grid(row=1)
Label(inputframe, text="Port").grid(row=2)

e1 = Entry(inputframe)
e2 = Entry(inputframe)
e3 = Entry(inputframe)

e1.grid(row=0, column=1)
e2.grid(row=1, column=1)
e3.grid(row=2, column=1)

#Close dialog
def closeApp():
    if tkMessageBox.askokcancel("Quit", "You want to quit now? *sniff*"):
        root.destroy()
Close = Button(bottomFrame, text ="Exit", command = closeApp)
Close.pack(side = BOTTOM )

root.protocol("WM_DELETE_WINDOW", closeApp)

root.mainloop()
