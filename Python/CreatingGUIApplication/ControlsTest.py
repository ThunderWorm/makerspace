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

import tkMessageBox
from Tkinter import *

root = Tk()
root.geometry("250x150+300+300")
root.title("Testing controls")

# Frames
frame = Frame(root)
frame.pack()

inputframe = Frame(root)
inputframe.pack()

bottomFrame = Frame(root)
bottomFrame.pack(side = BOTTOM)

# Some labels and buttons
w = Label(frame, text="Hello Tkinter!")
w.pack()

def helloCallBack():
   tkMessageBox.showinfo( "Hello Python", "Hello World")
B = Button(frame, text ="Hello", command = helloCallBack)

B.pack(side = LEFT)


def byeCallBack():
   tkMessageBox.showinfo( "Bye Python", "Bye World")
C = Button(frame, text ="Bye", command = byeCallBack)
C.pack(side = LEFT )


# Entries
Label(inputframe, text="First Name").grid(row=0)
Label(inputframe, text="Last Name").grid(row=1)

e1 = Entry(inputframe)
e2 = Entry(inputframe)

e1.grid(row=0, column=1)
e2.grid(row=1, column=1)

#Close dialog
def closeApp():
   if tkMessageBox.askokcancel("Quit", "You want to quit now? *sniff*"):
        root.destroy()
Close = Button(bottomFrame, text ="Exit", command = closeApp)
Close.pack(side = BOTTOM )

root.protocol("WM_DELETE_WINDOW", closeApp)
root.mainloop()