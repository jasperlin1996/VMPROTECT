from tkinter import *
from tkinter import messagebox
from tkinter import filedialog

import subprocess
import binascii
import time
import threading
import os

class Editor:
  # Defining Constructor
  def __init__(self,root):
    # Assigning root
    self.root = root
    # Title of the window
    self.root.title("VMPROTECT EDITOR")
    # Window Geometry
    self.root.geometry("1200x700+200+150")
    # Initializing filename
    self.filename = None
    # Declaring Title variable
    self.title = StringVar()
    # Declaring Status variable
    self.status = StringVar()

    # Creating Titlebar
    self.titlebar = Label(self.root,textvariable=self.title,font=("times new roman",15,"bold"),bd=2,relief=GROOVE)
    # Packing Titlebar to root window
    self.titlebar.pack(side=TOP,fill=BOTH)
    # Calling Settitle Function
    self.settitle()

    # Creating Statusbar
    self.statusbar = Label(self.root,textvariable=self.status,font=("times new roman",15,"bold"),bd=2,relief=GROOVE)
    # Packing status bar to root window
    self.statusbar.pack(side=BOTTOM,fill=BOTH)
    # Initializing Status
    self.status.set("Welcome :)")

    # Creating Menubar
    self.menubar = Menu(self.root,font=("times new roman",15,"bold"),activebackground="skyblue")
    # Configuring menubar on root window
    self.root.config(menu=self.menubar)

    # Creating File Menu
    self.filemenu = Menu(self.menubar,font=("times new roman",12,"bold"),activebackground="skyblue",tearoff=0)
    # Adding New file Command
    self.filemenu.add_command(label="New",accelerator="Ctrl+N",command=self.newfile)
    # Adding Open file Command
    self.filemenu.add_command(label="Open",accelerator="Ctrl+O",command=self.openfile)
    # Adding Save File Command
    self.filemenu.add_command(label="Save",accelerator="Ctrl+S",command=self.savefile)
    # Adding Save As file Command
    self.filemenu.add_command(label="Save As",accelerator="Ctrl+A",command=self.saveasfile)
    # Adding Seprator
    self.filemenu.add_separator()
    # Adding Exit window Command
    self.filemenu.add_command(label="Exit",accelerator="Ctrl+E",command=self.exit)
    # Cascading filemenu to menubar
    self.menubar.add_cascade(label="File", menu=self.filemenu)

    # Creating Edit Menu
    self.editmenu = Menu(self.menubar,font=("times new roman",12,"bold"),activebackground="skyblue",tearoff=0)
    # Adding Cut text Command
    self.editmenu.add_command(label="Cut",accelerator="Ctrl+X",command=self.cut)
    # Adding Copy text Command
    self.editmenu.add_command(label="Copy",accelerator="Ctrl+C",command=self.copy)
    # Adding Paste text command
    self.editmenu.add_command(label="Paste",accelerator="Ctrl+V",command=self.paste)
    # Adding Seprator
    self.editmenu.add_separator()
    # Adding Undo text Command
    self.editmenu.add_command(label="Undo",accelerator="Ctrl+U",command=self.undo)
    # Cascading editmenu to menubar
    self.menubar.add_cascade(label="Edit", menu=self.editmenu)

    # Creating Build Menu
    self.buildmenu = Menu(self.menubar,font=("times new roman",12,"bold"),activebackground="red",tearoff=0)
    # Adding Build separately mode text Command
    self.buildmenu.add_command(label="Build code separately from VMCore",accelerator="Ctrl+B+S",command=self.buildS)
    # Adding Build integrated mode text Command
    self.buildmenu.add_command(label="Build code integrated to VMCore",accelerator="Ctrl+B+I",command=self.buildI)
    # Cascading buildmenu to menubar
    self.menubar.add_cascade(label="Build", menu=self.buildmenu)

    # Creating Help Menu
    self.helpmenu = Menu(self.menubar,font=("times new roman",12,"bold"),activebackground="skyblue",tearoff=0)
    # Adding About Command
    self.helpmenu.add_command(label="About",command=self.infoabout)
    # Cascading helpmenu to menubar
    self.menubar.add_cascade(label="Help", menu=self.helpmenu)

    # Creating Settings button if win32
    # ********************************** TODO: read a config file **********************************
    self.pathToCompiler = ""
    self.pathToNASM = ""
    if os.name == 'nt':
      self.menubar.add_command(label="Settings", command=self.showSettings)
    # ********************************** TODO: read a config file **********************************

    # Creating Scrollbar
    scrol_y = Scrollbar(self.root,orient=VERTICAL)
    # Creating Text Area
    self.txtarea = Text(self.root,yscrollcommand=scrol_y.set,font=("times new roman",15,"bold"),state="normal",relief=GROOVE)
    # Packing scrollbar to root window
    scrol_y.pack(side=RIGHT,fill=Y)
    # Adding Scrollbar to text area
    scrol_y.config(command=self.txtarea.yview)
    # Packing Text Area to root window
    self.txtarea.pack(fill=BOTH,side=TOP,expand=1)

    # Creating Outputbar
    self.outputbar = Label(self.root,text="Build Output",font=("times new roman",15,"bold"))
    # Packing Outputbar to root window
    self.outputbar.pack(side=TOP,fill=BOTH,expand=1)
    # Creating Scrollbar
    scrol_y_out = Scrollbar(self.root,orient=VERTICAL)
    # Creating Build Output Area
    self.buildOutputArea = Text(self.root,yscrollcommand=scrol_y_out.set,font=("times new roman",15,"bold"),state="normal",relief=GROOVE)
    # Packing scrollbar to root window
    scrol_y_out.pack(side=RIGHT,fill=Y)
    # Adding Scrollbar to text area
    scrol_y_out.config(command=self.buildOutputArea.yview)
    # Packing Build Output Area to root window
    self.buildOutputArea.pack(fill=BOTH,side=TOP,expand=1)
    # Set Build Output Area to non editable
    self.buildOutputArea.config(state=DISABLED)

    # Calling shortcuts funtion
    self.shortcuts()

  # Defining settitle function
  def settitle(self):
    # Checking if Filename is not None
    if self.filename:
      # Updating Title as filename
      self.title.set(self.filename)
    else:
      # Updating Title as Untitled
      self.title.set("Untitled")

  # Defining New file Function
  def newfile(self,*args):
    # Clearing the Text Area
    self.txtarea.delete("1.0",END)
    # Updating filename as None
    self.filename = None
    # Calling settitle funtion
    self.settitle()
    # updating status
    self.status.set("New File Created")

  # Defining Open File Funtion
  def openfile(self,*args):
    # Exception handling
    try:
      # Asking for file to open
      self.filename = filedialog.askopenfilename(title = "Select file",filetypes = (("All Files","*.*"),("Text Files","*.txt"),("Python Files","*.py")))
      # checking if filename not none
      if self.filename:
        # opening file in readmode
        infile = open(self.filename,"r")
        # Clearing text area
        self.txtarea.delete("1.0",END)
        # Inserting data Line by line into text area
        for line in infile:
          self.txtarea.insert(END,line)
        # Closing the file  
        infile.close()
        # Calling Set title
        self.settitle()
        # Updating Status
        self.status.set("Opened Successfully")
    except Exception as e:
      messagebox.showerror("Exception",e)

  # Defining Save File Funtion
  def savefile(self,*args):
    # Exception handling
    try:
      # checking if filename not none
      if self.filename:
        # Reading the data from text area
        data = self.txtarea.get("1.0",END)
        # opening File in write mode
        outfile = open(self.filename,"w")
        # Writing Data into file
        outfile.write(data)
        # Closing File
        outfile.close()
        # Calling Set title
        self.settitle()
        # Updating Status
        self.status.set("Saved Successfully")
      else:
        self.saveasfile()
    except Exception as e:
      messagebox.showerror("Exception",e)

  # Defining Save As File Funtion
  def saveasfile(self,*args):
    # Exception handling
    try:
      # Asking for file name and type to save
      untitledfile = filedialog.asksaveasfilename(title = "Save file As",defaultextension=".txt",initialfile = "Untitled.txt",filetypes = (("All Files","*.*"),("Text Files","*.txt"),("Python Files","*.py")))
      # Reading the data from text area
      data = self.txtarea.get("1.0",END)
      # opening File in write mode
      outfile = open(untitledfile,"w")
      # Writing Data into file
      outfile.write(data)
      # Closing File
      outfile.close()
      # Updating filename as Untitled
      self.filename = untitledfile
      # Calling Set title
      self.settitle()
      # Updating Status
      self.status.set("Saved Successfully")
    except Exception as e:
      messagebox.showerror("Exception",e)

  # Defining Exit Funtion
  def exit(self,*args):
    op = messagebox.askyesno("WARNING","Your Unsaved Data May be Lost!!")
    if op>0:
      self.root.destroy()
    else:
      return

  # Defining Cut Funtion
  def cut(self,*args):
    self.txtarea.event_generate("<<Cut>>")

  # Defining Copy Funtion
  def copy(self,*args):
          self.txtarea.event_generate("<<Copy>>")

  # Defining Paste Funtion
  def paste(self,*args):
    self.txtarea.event_generate("<<Paste>>")

  # Defining Undo Funtion
  def undo(self,*args):
    # Exception handling
    try:
      # checking if filename not none
      if self.filename:
        # Clearing Text Area
        self.txtarea.delete("1.0",END)
        # opening File in read mode
        infile = open(self.filename,"r")
        # Inserting data Line by line into text area
        for line in infile:
          self.txtarea.insert(END,line)
        # Closing File
        infile.close()
        # Calling Set title
        self.settitle()
        # Updating Status
        self.status.set("Undone Successfully")
      else:
        # Clearing Text Area
        self.txtarea.delete("1.0",END)
        # Updating filename as None
        self.filename = None
        # Calling Set title
        self.settitle()
        # Updating Status
        self.status.set("Undone Successfully")
    except Exception as e:
      messagebox.showerror("Exception",e)

  # Defining About Funtion
  def infoabout(self):
    messagebox.showinfo("About VMPROTECT","A code obfuscation method using virtual machines to protect a product.\nMore information at github.com/eaglx/VMPROTECT")

  # Settings
  def showSettings(self):
    tl = Toplevel(self.root)
    tl.geometry("500x100")
    tl.wm_title("Settings")
    Label(tl, text="Path to NASM").grid(row=0)
    Label(tl, text="Path to cl.exe").grid(row=1)
    textEntry1 = Entry(tl, width = 60) 
    textEntry1.grid(row=0, column=1)
    textEntry2 = Entry(tl, width = 60)
    textEntry2.grid(row=1, column=1)
    def getTextInput():
        self.pathToNASM = textEntry1.get()
        self.pathToCompiler = textEntry2.get()
    Button(tl, text='Save', command=getTextInput, width = 25).grid(row=3, column=1, pady=4) 

  # Defining shortcuts Funtion
  def shortcuts(self):
    # Binding Ctrl+n to newfile funtion
    self.txtarea.bind("<Control-n>",self.newfile)
    # Binding Ctrl+o to openfile funtion
    self.txtarea.bind("<Control-o>",self.openfile)
    # Binding Ctrl+s to savefile funtion
    self.txtarea.bind("<Control-s>",self.savefile)
    # Binding Ctrl+a to saveasfile funtion
    self.txtarea.bind("<Control-a>",self.saveasfile)
    # Binding Ctrl+e to exit funtion
    self.txtarea.bind("<Control-e>",self.exit)
    # Binding Ctrl+x to cut funtion
    self.txtarea.bind("<Control-x>",self.cut)
    # Binding Ctrl+c to copy funtion
    self.txtarea.bind("<Control-c>",self.copy)
    # Binding Ctrl+v to paste funtion
    self.txtarea.bind("<Control-v>",self.paste)
    # Binding Ctrl+u to undo funtion
    self.txtarea.bind("<Control-u>",self.undo)
    
    # Binding Ctrl+b+i to build separately funtion
    self.root.bind("<Control-b><s>",self.buildS)
    # Binding Ctrl+b+i to build integrated funtion
    self.root.bind("<Control-b><i>",self.buildI)

  # Build a program separately to VMCore
  def buildS(self, event = None):
    self.buildOutputArea.config(state=NORMAL)
    self.buildOutputArea.delete("1.0",END)
    self.status.set("building separately ...")
    self.buildStage1()
    codePart = "#ifndef _VM_PROTECTED_D\n#define _VM_PROTECTED_D\n#include \"main.hpp\"\nVBYTE ProtectedData[] = { 0xFF };\n#endif"
    thread = threading.Thread(target = self.buildStage2, args = (codePart,))
    thread.start()

  # Build a program integrated to VMCore
  def buildI(self, event = None):
    self.buildOutputArea.config(state=NORMAL)
    self.buildOutputArea.delete("1.0",END)
    self.status.set("building integrated ...")
    self.buildStage1()
    codePart1 = "#ifndef _VM_PROTECTED_D\n#define _VM_PROTECTED_D\n#include \"main.hpp\"\nVBYTE ProtectedData[] = { "
    codePart2 = ""
    fileNamePart1 = (self.filename.replace(self.filename.split("/")[-1],''))
    fileNamePart2 = (self.filename.split("/")[-1]).split('.')[0]
    with open(fileNamePart1 + fileNamePart2, mode='rb') as file:
      codePart2 = file.read()
    codePart2str = binascii.hexlify(VBYTEarray(codePart2)).decode("utf-8")
    codePart2final = ""
    for i in range(0, len(codePart2str), 2):
      s1,s2 = codePart2str[i:i+2]
      codePart2final += "0x" + s1 + s2 + ", "
    codePart2final = codePart2final[:-2]
    codePart3 = "};\n#endif"
    fullCode = codePart1 + codePart2final + codePart3
    thread = threading.Thread(target = self.buildStage2, args = (fullCode,))
    thread.start()
  
  def buildStage1(self):
    cmdProcess = subprocess.Popen(['nasm', self.filename], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = cmdProcess.communicate()
    self.buildOutputArea.insert(END, out)
    self.buildOutputArea.insert(END, err)
  
  def buildStage2(self, codeToWrite):
    f = open("./VMCore/include/protected.hpp", 'w')
    f.write(codeToWrite)
    f.close()
    if os.name == 'nt':
      codeToWrite = "#ifndef _GLOBAL_VARIABLES_HPP\n#define _GLOBAL_VARIABLES_HPP\n#define _WIN32_DEV_ENVIRONMENT\n#endif"
      f = open("./SharedCode/global.hpp.hpp", 'w')
      f.write(codeToWrite)
      f.close()
    else:
      codeToWrite = "#ifndef _GLOBAL_VARIABLES_HPP\n#define _GLOBAL_VARIABLES_HPP\n#define _LINUX_DEV_ENVIRONMENT\n#endif"
      f = open("./SharedCode/global.hpp.hpp", 'w')
      f.write(codeToWrite)
      f.close()
    time.sleep(3)
    self.buildStage3()
    self.buildOutputArea.config(state=DISABLED)
    self.status.set("build finished")

  def buildStage3(self):
    if os.name == 'nt':
      # TODO: NOT IMPLEMENTED YET!
      # cl /EHsc or msbuild project.sln/Flags 
      # self.pathToCompiler
      # self.pathToNASM
      pass
    else:
      cmdProcess = subprocess.Popen(['make', '-C', './VMCore', 'clean'], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
      out, err = cmdProcess.communicate()
      self.buildOutputArea.insert(END, out)
      self.buildOutputArea.insert(END, err)
      cmdProcess = subprocess.Popen(['make', '-C', './VMCore', 'build'], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
      out, err = cmdProcess.communicate()
      self.buildOutputArea.insert(END, out)
      self.buildOutputArea.insert(END, err)
      cmdProcess = subprocess.Popen(['make', '-C', './Debugger', 'build'], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
      out, err = cmdProcess.communicate()
      self.buildOutputArea.insert(END, out)
      self.buildOutputArea.insert(END, err)
      cmdProcess = subprocess.Popen(['cp', '-v', './VMCore/VMPROTECT.exe', (self.filename.replace(self.filename.split("/")[-1],'')) + 'VMPROTECT.exe'], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
      out, err = cmdProcess.communicate()
      self.buildOutputArea.insert(END, out)
      self.buildOutputArea.insert(END, err)
      cmdProcess = subprocess.Popen(['cp', '-v', './Debugger/VmprotDEBUGGER.exe', (self.filename.replace(self.filename.split("/")[-1],'')) + 'VmprotDEBUGGER.exe'], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
      out, err = cmdProcess.communicate()
      self.buildOutputArea.insert(END, out)
      self.buildOutputArea.insert(END, err)