
from Tkinter import *
import tkFileDialog
import mylogger

def do_gui():

    class dostuff:
  
      def __init__(self, root):
  
          self.frame=Frame(root)
          self.frame.grid()
  
          self.plabel = Label(self.frame, text='Normal')
          self.plabel.grid(row = 1, column = 0, sticky = W)
          self.pentry = Entry(self.frame)
          self.pentry.insert(0, 'default 1')
          self.pentry.grid(row = 1, column = 1)
          self.setdef = Button(self.frame, text='Default', command = self.blah)
          self.setdef.grid(row = 1, column = 2, sticky = W)

          self.label = Label(self.frame, text='Weird')
          self.label.grid(row = 2, column = 0, sticky = W)
          self.entry = Entry(self.frame)
          self.entry.insert(0, 'default 1')
          self.entry.grid(row = 2, column = 1)
          dum = 1
          self.etdef = Button(self.frame, text='Default', command = self.blah1(dum))
          self.etdef.grid(row = 2, column = 2, sticky = W)

      def blah(self):
              self.pentry.delete(0, END)
              self.pentry.insert(0, 'qwerty')

      def blah1(self, dum):
              self.entry.delete(0, END)
              self.entry.insert(0, 'asdfgh')

  
    root = Tk()
    root.title("PyBDSM : Input Parameters")
    a=dostuff(root)
    root.mainloop()

    return 0





