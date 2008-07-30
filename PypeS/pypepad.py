#!/usr/bin/env python

## Program:   Pype Pad
## Module:    $RCSfile: pypepad.py,v $
## Language:  Python
## Date:      $Date: 2006/05/26 12:36:30 $
## Version:   $Revision: 1.6 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENCE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even 
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
##      PURPOSE.  See the above copyright notices for more information.


import sys
import pypes
from Tkinter import *
import tkFileDialog


class TkPadOutputStream(object):

    def __init__(self,tk_text_widget):
        self.text_widget = tk_text_widget
        self.output_to_file = False
        self.output_file = None
  
    def write(self,text):
        self.text_widget["state"] = NORMAL
        if text[0] == '\r':
            endline = str(int(self.text_widget.index(END).split('.')[0])-1)
            self.text_widget.delete(endline+".0",endline+".end")
            self.text_widget.update()
            text = text[1:]
        self.text_widget.insert(END,text)
        self.text_widget["state"] = DISABLED
        self.text_widget.see(END)
        self.text_widget.update()
        if self.output_to_file and self.output_file:
            self.output_file.write(text)
            self.output_file.flush()
      
    def flush(self):
        if self.output_to_file and self.output_file:
            self.output_file.flush()


class TkPadInputStream(object):

    def __init__(self,tk_entry_widget,input_stream):
        self.entry_widget = tk_entry_widget
        self.entry_widget.bind("<KeyPress-Return>",self.EntryReturnHandler)
        self.text = ''
        self.input_stream = input_stream
 
    def EntryReturnHandler(self,event):
        self.text = self.entry_widget.get()
        self.entry_widget.delete(0,END)
        self.entry_widget.quit()
        self.entry_widget["state"] = DISABLED
        self.input_stream.write(self.text+'\n')
       
    def readline(self):
        self.entry_widget["state"] = NORMAL
        self.entry_widget.focus_set()
        self.entry_widget.grab_set()
        self.entry_widget.mainloop()
        self.entry_widget.grab_release()
        return self.text

class CallbackShim:
    """Create a callback shim. Based on code by Scott David Daniels
    (which also handles keyword arguments).
    """
    def __init__(self, callback, *firstArgs):
        self.__callback = callback
        self.__firstArgs = firstArgs
  
    def __call__(self, *args):
        return self.__callback (*(self.__firstArgs + args))     

class PypeTkPad(object):

    def __init__(self, master):
      
        self.master = master
        self.master.title('PypePad')

        self.output_file_name = None
        
        self.BuildMainFrame()

    def NewCommand(self):
        self.ClearAllCommand()
 
    def OpenCommand(self):
        openfile = tkFileDialog.askopenfile()
        if not openfile:
            return
        for line in openfile.readlines():
            self.text_input.insert(END,line)
 
    def SaveCommand(self):
        saveasfile = tkFileDialog.asksaveasfile()
        if not saveasfile:
            return
        alltext = self.text_input.get("1.0",END)
        saveasfile.write(alltext)
 
    def QuitCommand(self):
        self.master.quit()

    def ClearInputCommand(self):
        self.text_input.delete("1.0",END)
        
    def ClearOutputCommand(self):
        self.text_output["state"] = NORMAL
        self.text_output.delete("1.0",END)
        self.text_output["state"] = DISABLED
        self.text_output.see(END)
        self.text_output.update()
        
    def ClearAllCommand(self):
        self.ClearInputCommand()
        self.ClearOutputCommand()

    def OutputFileCommand(self):
        self.output_file_name = tkFileDialog.asksaveasfilename()

    def AboutCommand(self):
        self.OutputText('\n')
        self.OutputText('* PypePad, Copyright (c) Luca Antiga, David Steinman. *\n')
        self.OutputText('\n')

    def RunPype(self,arguments):
        if not arguments:
            return
        if self.output_to_file.get() is not 'n' and self.output_file_name:
            self.output_stream.output_to_file = True
            self.output_stream.output_file = open(self.output_file_name,self.output_to_file.get())
        else:
            self.output_stream.output_to_file = False
        pipe = pypes.Pype()
        pipe.ExitOnError = 0
        pipe.Arguments = arguments.split()
        pipe.InputStream = self.input_stream
        pipe.OutputStream = self.output_stream
        pipe.LogOn = self.log_on.get()
        pipe.ParseArguments()
        try: 
            pipe.Execute() 
        except Exception:
            return
 
    def GetWordUnderCursor(self):
        splitindex = self.text_input.index(CURRENT).split('.')
        line = self.text_input.get(splitindex[0]+".0",splitindex[0]+".end")
        wordstart = line.rfind(' ',0,int(splitindex[1])-1)+1
        wordend = line.find(' ',int(splitindex[1]))
        if wordend == -1:
            wordend = len(line)
        word = line[wordstart:wordend]
        return word

    def GetLogicalLine(self,physicallineid):
        indexes, lines = self.GetLogicalLines()
        return lines[indexes[physicallineid]]
 
    def GetLogicalLineRange(self,physicallinefirstid,physicallinelastid):
        indexes, lines = self.GetLogicalLines()
        return lines[indexes[physicallinefirstid]:indexes[physicallinelastid]+1]
   
    def GetAllLogicalLines(self):
        return self.GetLogicalLines()[1]
   
    def GetLogicalLines(self):
        physicallines = self.text_input.get("1.0",END).split('\n')
        lines = []
        indexes = [0] * len(physicallines)
        lineid = 0
        previousline = ""
        join = 0
        for line in physicallines:
            if line.startswith('#'):
                if join:
                    indexes[lineid] = indexes[lineid-1]
            elif join:
                if line.endswith('\\'):
                    lines[-1] = lines[-1] + " " + line[:-1]
                    join = 1
                else:
                    lines[-1] = lines[-1] + " " + line
                    join = 0
                indexes[lineid] = indexes[lineid-1]
            else:
                if line.endswith('\\'):
                    join = 1
                    lines.append(line[:-1])
                else:
                    lines.append(line)
                    join = 0
                if lineid > 0:
                    indexes[lineid] = indexes[lineid-1]+1
            lineid += 1
        return indexes, lines

    def GetLineUnderCursor(self):
        currentlineid = int(self.text_input.index(INSERT).split('.')[0]) - 1
        return self.GetLogicalLine(currentlineid)

    def RunAllCommand(self):
        lines = self.GetAllLogicalLines()
        for line in lines:
            if line and line.strip():
                self.RunPype(line)

    def RunLineCommand(self):
        line = self.GetLineUnderCursor()
        if line and line.strip():
            self.RunPype(line)
      
    def RunSelectionCommand(self):
        try:
            firstlineid = int(self.text_input.index(SEL_FIRST).split('.')[0]) - 1
            lastlineid = int(self.text_input.index(SEL_LAST).split('.')[0]) - 1
            lines = self.GetLogicalLineRange(firstlineid,lastlineid)
            for line in lines:
                self.RunPype(line)
        except TclError:
            pass

    def ShowHelpCommand(self):
        word = self.GetWordUnderCursor()
        self.OutputText(word)
        if word:
            self.RunPype(word+' --help')
        else: 
            self.OutputText('Enter your vmtk Pype above and Run.\n')
        
    def InsertScriptName(self,scriptname):
        self.text_input.insert(INSERT,scriptname+' ')
        
    def InsertFileName(self):
        openfilename = tkFileDialog.askopenfilename()
        if not openfilename:
            return
        self.text_input.insert(INSERT,openfilename+' ')
    
    def QuitHandler(self,event):
        self.QuitCommand()
 
    def RunKeyboardHandler(self,event):
        try: 
            self.text_input.index(SEL_FIRST)
            self.RunSelectionCommand()
        except TclError:
            self.RunLineCommand()
        return "break"
         
    def RunAllHandler(self,event):
        self.RunAllCommand()
      
    def PopupHandler(self,event):
        try:
            self.popupmenu.tk_popup(event.x_root, event.y_root, 0)
        finally:
            self.popupmenu.grab_release()

    def OutputText(self,text):
        self.text_output["state"] = NORMAL
        self.text_output.insert(END,text)
        self.text_output["state"] = DISABLED
 
    def BuildScriptMenu(self,parentmenu,modulename):
        menu = Menu(parentmenu)
        try:
            exec('import '+ modulename)
        except ImportError:
            return None
        scriptnames = []
        exec ('scriptnames = [scriptname for scriptname in '+modulename+'.__all__]') 
        for scriptname in scriptnames:
            callback = CallbackShim(self.InsertScriptName,scriptname)
            menu.add_command(label=scriptname,command=callback)
        return menu
        
    def BuildMainFrame(self): 
      
        menu = Menu(self.master)
        self.master.config(menu=menu)
  
        filemenu = Menu(menu)
        menu.add_cascade(label="File", menu=filemenu)
        filemenu.add_command(label="New", command=self.NewCommand)
        filemenu.add_command(label="Open...", command=self.OpenCommand)
        filemenu.add_command(label="Save as...", command=self.SaveCommand)
        filemenu.add_separator()
        filemenu.add_command(label="Quit", command=self.QuitCommand)

        self.log_on = IntVar()
        self.log_on.set(1)
  
        self.output_to_file = StringVar()
        self.output_to_file.set('n')
 
        scriptmenu = Menu(menu)
        modulenames = ['vmtkscripts']
        for modulename in modulenames:
            scriptsubmenu = self.BuildScriptMenu(menu,modulename)
            if scriptsubmenu:
                scriptmenu.add_cascade(label=modulename,menu=scriptsubmenu)
 
        editmenu = Menu(menu)
        menu.add_cascade(label="Edit", menu=editmenu)
        editmenu.add_cascade(label="Insert script",menu=scriptmenu)
        editmenu.add_command(label="Insert file name...", command=self.InsertFileName)
        editmenu.add_separator()
        editmenu.add_command(label="Clear input", command=self.ClearInputCommand)
        editmenu.add_command(label="Clear output", command=self.ClearOutputCommand)
        editmenu.add_command(label="Clear all", command=self.ClearAllCommand)
        editmenu.add_separator()
        editmenu.add_checkbutton(label="Log", variable=self.log_on)
        editmenu.add_separator()
        editmenu.add_radiobutton(label="No output to file", variable=self.output_to_file,value='n')
        editmenu.add_radiobutton(label="Write output to file", variable=self.output_to_file,value='w')
        editmenu.add_radiobutton(label="Append output to file", variable=self.output_to_file,value='a')
        editmenu.add_command(label="Output file...", command=self.OutputFileCommand)

        runmenu = Menu(menu)
        menu.add_cascade(label="Run", menu=runmenu)
        runmenu.add_command(label="Run all", command=self.RunAllCommand)
        runmenu.add_command(label="Run current line", command=self.RunLineCommand)
        runmenu.add_command(label="Run selection", command=self.RunSelectionCommand)
       
        helpmenu = Menu(menu)
        menu.add_cascade(label="Help", menu=helpmenu)
        helpmenu.add_command(label="Help", command=self.ShowHelpCommand)
        helpmenu.add_command(label="About", command=self.AboutCommand)

        self.master.bind("<Control-KeyPress-q>", self.QuitHandler)
       
        self.panes = PanedWindow(self.master,orient=VERTICAL)
        self.panes.pack(fill=BOTH,expand=1)
        
        self.frame1 = Frame(self.panes)
        self.panes.add(self.frame1)

        self.text_input = Text(self.frame1,bg='#fff')
        self.text_input.pack(side=LEFT,fill='both',expand=1)

        self.popupmenu = Menu(self.text_input, tearoff=1, title="PypePad")
        self.popupmenu.add_command(label="Context help", command=self.ShowHelpCommand)
        self.popupmenu.add_cascade(label="Insert script",menu=scriptmenu)
        self.popupmenu.add_command(label="Insert file name...", command=self.InsertFileName)
        self.popupmenu.add_separator()
        self.popupmenu.add_command(label="Run all", command=self.RunAllCommand)
        self.popupmenu.add_command(label="Run current line", command=self.RunLineCommand)
        self.popupmenu.add_command(label="Run selection", command=self.RunSelectionCommand)
                       
        self.text_input.bind("<Button-3>", self.PopupHandler)
        self.text_input.bind("<Control-Return>", self.RunKeyboardHandler)
 
        self.input_scrollbar = Scrollbar(self.frame1,orient=VERTICAL,command=self.text_input.yview)
        self.input_scrollbar.pack(side=RIGHT,fill=Y)
        self.text_input["yscrollcommand"] = self.input_scrollbar.set    
  
        self.frame2 = Frame(self.panes)
        self.panes.add(self.frame2)

        self.text_output = Text(self.frame2,state=DISABLED,relief=GROOVE)
        self.text_output.pack(side=LEFT,fill='both',expand=1)
        
        self.output_scrollbar = Scrollbar(self.frame2,orient=VERTICAL,command=self.text_output.yview)
        self.output_scrollbar.pack(side=RIGHT,fill=Y)
        self.text_output["yscrollcommand"] = self.output_scrollbar.set    
        
        self.frame3 = Frame(self.panes)
        self.panes.add(self.frame3)
      
        self.text_entry = Entry(self.frame3,bg='#fff',state=DISABLED)
        self.text_entry.pack(side=TOP,fill='both',expand=1)

        self.text_input.focus_set()

        self.output_stream = TkPadOutputStream(self.text_output)
        self.input_stream = TkPadInputStream(self.text_entry,self.output_stream)

  
def RunPypeTkPad():

    root = Tk()
    app = PypeTkPad(root)
    root.mainloop()
  
if __name__=='__main__':

    RunPypeTkPad()
