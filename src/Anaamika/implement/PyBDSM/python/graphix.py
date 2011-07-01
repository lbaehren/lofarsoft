
from Tkinter import *
import tkFileDialog
import mylogger

def do_gui(paralist):
    """ Gets the list of lists from opts and parses it to send to the gui """

    class dostuff:
  
      def __init__(self, root, paralist):
  
          mylog = mylogger.logging.getLogger("PyBDSM."+"Gui       ")
          self.canvas=Canvas(root, borderwidth=0,  width=1000, height=600)
          self.frame=Frame(self.canvas)
          self.vsb = Scrollbar(root, orient='vertical', command=self.canvas.yview)
          self.canvas.configure(yscrollcommand=self.vsb.set)

          self.vsb.pack(side='right', fill='y')
          self.canvas.pack(side='right', fill='y', expand=True)
          self.canvas.create_window((0,0), window=self.frame, anchor="nw", tags="self.frame")
          self.frame.bind("<Configure>", self.OnFrameConfigure)
  
          self.put_intro()                      # Intro message
          self.put_dir_file()                   # Get dir and file/dir name 
          self.rownum = 2

          exclude = ['indir', 'fitsname']
          #ptypes = {'Bool' : self.pbool, 'String' : self.pstring, 'Int' : self.pint, 'Float' : self.pfloat, \
          #          'Option' : self.poption, 'Enum' : self.penum, 'List' : self.plist}
          ptypes = {'Bool' : self.pbool, 'String' : self.pbool, 'Int' : self.pbool, 'Float' : self.pbool, \
                    'Option' : self.pbool, 'Enum' : self.pbool, 'List' : self.pbool}
          i = 0
          for para in paralist:
            if para not in exclude:
              pname, ptype, pdefault, pdoc = para
              pdoc = pdoc[1:-1]
              if ptype not in ptypes.keys():
                mylog.debug(ptype+' is not a recognised type for the GUI. Fail')
                mylog.info(ptype+' is not a recognised type for the GUI. Fail')
                return
              else:
                #if ptype == 'Bool': 
                  #i += 1
                  #if i < 10: 
                   ptypes[ptype](pname, pdefault, pdoc)
                
  
          # How to get fits file or MS dir input ? tkfiledialog doesnt have that. check.
          # askopenfilename is there but u want a dir or file 
          #self.quitButton = Button(self.frame, text="Quit", fg="red", command=self.frame.quit)
          #self.quitButton.grid(row=self.rownum, column=1, columnspan=2)
  
      def OnFrameConfigure(self, event):
        '''Reset the scroll region to encompass the inner frame'''
        self.canvas.configure(scrollregion=self.canvas.bbox("all"))


      def pbool(self, pname, pdefault, pdoc):
          import functools
          
          self.plabel = Label(self.frame, text=pname)
          self.plabel.grid(row = self.rownum, column = 0, sticky = W)

          self.pentry = Entry(self.frame)
          self.pentry.insert(0, pdefault.split(',')[0])
          self.pentry.grid(row = self.rownum, column = 1)

          self.setdef = Button(self.frame, text='Default', command = functools.partial(self.setdefault, pdefault, self.rownum))
          #var = StringVar(root)
          #self.setdef = OptionMenu(self.frame, var, "True", "False")
          self.setdef.grid(row = self.rownum, column = 2, sticky = W)

          self.pdoc = Message(self.frame, text=pdoc, width=500)
          self.pdoc.grid(row = self.rownum, column = 3, sticky = W)

          self.rownum += 1

      def setdefault(self, pdef, rnum):

          self.pentry = Entry(self.frame)
          self.pentry.grid(row = rnum, column = 1)
          self.pentry.delete(0, END)
          self.pentry.insert(0, pdef.split(',')[0])

      def put_dir_file(self):

          self.indir=Label(self.frame, text='Directory')
          self.indir.grid(row=1, column=0, sticky = W)
          self.indirEntry=Entry(self.frame)
          self.indirEntry.grid(row=1, column=1)
          self.indirBrowse=Button(self.frame, text='Browse', command=self.browse)
          self.indirBrowse.grid(row=1, column=2)
          self.indirDoc=Label(self.frame, text='Dir for image file')
          self.indirDoc.grid(row=1, column=3, sticky = W)
  
      def browse(self):
          dirname = tkFileDialog.askdirectory(initialdir='.',title='Please select the MS to plot')
          #dirname = tkFileDialog.askopenfilename(initialdir='.',title='Please select the MS to plot')
          self.indirEntry.delete(0, END)
          self.indirEntry.insert(0, dirname)

      def put_intro(self):

            text='Intro you dont specify a size, the label is made just large enough to hold its contents. You can also use the height and width options to explicitly set the size. If you display text in the la'
            self.notes=Message(self.frame, text=text, bg='tan', fg='red', width=500)
            self.notes.grid(columnspan=4)

  
    root = Tk()
    root.title("PyBDSM : Input Parameters")
    a=dostuff(root, paralist)
    root.mainloop()

    return 0





