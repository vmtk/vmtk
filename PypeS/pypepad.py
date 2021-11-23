#!${PYTHON_SHEBANG}

## Program:   Pype Pad
## Module:    $RCSfile: pypepad.py,v $
## Language:  Python
## Date:      $Date: 2006/05/26 12:36:30 $
## Version:   $Revision: 1.6 $

##   Copyright (c) Luca Antiga, David Steinman. All rights reserved.
##   See LICENSE file for details.

##      This software is distributed WITHOUT ANY WARRANTY; without even
##      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
##      PURPOSE.  See the above copyright notices for more information.

from __future__ import absolute_import, unicode_literals #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import sys

from vmtk import pypeserver

from multiprocessing import Process, Manager
import importlib
from inspect import isclass, getmembers


class TkPadOutputStream(object):

    def __init__(self,tk_text_widget):
        self.text_widget = tk_text_widget
        self.output_to_file = False
        self.output_file = None

    def write(self,text):
        from tkinter import NORMAL, END, DISABLED
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
        from tkinter import END, DISABLED
        self.text = self.entry_widget.get()
        self.entry_widget.delete(0,END)
        self.entry_widget.quit()
        self.entry_widget["state"] = DISABLED
        self.input_stream.write(self.text+'\n')

    def readline(self):
        from tkinter import NORMAL
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

    def __init__(self, master, queue, pypeOutput):

        self.queue = queue
        self.pypeOutput = pypeOutput

        self.master = master
        self.master.title('PypePad')
        self.master.geometry("%dx%d%+d%+d" % (700, 500, 0, 0))
        self.master.minsize(300, 100)
        self.output_file_name = None

        self.BuildMainFrame()
        self.UpdateOutput()

    def NewCommand(self):
        self.ClearAllCommand()

    def OpenCommand(self):
        import tkinter.filedialog
        from tkinter import END
        openfile = tkinter.filedialog.askopenfile()
        if not openfile:
            return
        for line in openfile.readlines():
            self.text_input.insert(END,line)

    def SaveCommand(self):
        import tkinter.filedialog
        from tkinter import END
        saveasfile = tkinter.filedialog.asksaveasfile()
        if not saveasfile:
            return
        alltext = self.text_input.get("1.0",END)
        saveasfile.write(alltext)

    def QuitCommand(self):
        self.master.quit()

    def ClearInputCommand(self):
        from tkinter import END
        self.text_input.delete("1.0",END)

    def ClearOutputCommand(self):
        from tkinter import NORMAL, END, DISABLED
        self.text_output["state"] = NORMAL
        self.text_output.delete("1.0",END)
        self.text_output["state"] = DISABLED
        self.text_output.see(END)
        self.text_output.update()

    def ClearAllCommand(self):
        self.ClearInputCommand()
        self.ClearOutputCommand()

    def OutputFileCommand(self):
        import tkinter.filedialog
        outputfilename = tkinter.filedialog.asksaveasfilename()
        if sys.platform == 'win32' and len(outputfilename.split()) > 1:
            outputfilename = '"%s"' % outputfilename
        self.output_file_name = outputfilename

    def AboutCommand(self):
        self.OutputText('\n')
        self.OutputText('* PypePad, Copyright (c) Luca Antiga, David Steinman. *\n')
        self.OutputText('\n')

    def UpdateOutput(self):
        if self.pypeOutput:
            text = self.pypeOutput.pop(0)
            self.output_stream.write(text)
        self.master.after(10,self.UpdateOutput)

    def RunPype(self,arguments):
        if not arguments:
            return
        if self.output_to_file.get() is not 'n' and self.output_file_name:
            self.output_stream.output_to_file = True
            self.output_stream.output_file = open(self.output_file_name,self.output_to_file.get())
        else:
            self.output_stream.output_to_file = False

        self.queue.append(arguments)

    def GetWordUnderCursor(self):
        from tkinter import CURRENT
        splitindex = self.text_input.index(CURRENT).split('.')
        line = self.text_input.get(splitindex[0]+".0",splitindex[0]+".end")
        wordstart = line.rfind(' ',0,int(splitindex[1])-1)+1
        wordend = line.find(' ',int(splitindex[1]))
        if wordend == -1:
            wordend = len(line)
        word = line[wordstart:wordend]
        return word

    def GetWordIndex(self):
        startindex = self.text_input.index("insert-1c wordstart")
        endindex = self.text_input.index("insert-1c wordend")
        if self.text_input.get(startindex+'-1c') == '-' and self.text_input.get(startindex+'-2c') == '-':
           startindex = self.text_input.index("insert-1c wordstart -2c")
        elif self.text_input.get(startindex+'-1c') == '-' and self.text_input.get(startindex+'-2c') == ' ':
           startindex = self.text_input.index("insert-1c wordstart -1c")
        self.wordIndex[0] = startindex
        self.wordIndex[1] = endindex
        word = self.text_input.get(self.wordIndex[0],self.wordIndex[1])
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
        from tkinter import END
        # Python 2 hack to remove the u'...' prefix from unicode literal strings. does not change py3 behavior
        physicallines = [str(line) for line in self.text_input.get("1.0",END).split('\n')]
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
        from tkinter import INSERT
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
        from tkinter import TclError, SEL_FIRST, SEL_LAST
        try:
            firstlineid = int(self.text_input.index(SEL_FIRST).split('.')[0]) - 1
            lastlineid = int(self.text_input.index(SEL_LAST).split('.')[0]) - 1
            lines = self.GetLogicalLineRange(firstlineid,lastlineid)
            for line in lines:
                self.RunPype(line)
        except TclError:
            pass

    def GetSuggestionsList(self,word):
        list = []
        try:
            from vmtk import vmtkscripts
            from vmtk import pypes
        except ImportError:
            return None
        if word.startswith('--'):
            list = ['--pipe','--help']
        elif word.startswith('-'):
            optionlist = []
            scriptindex = self.text_input.search('vmtk',self.wordIndex[0],backwards=1)
            moduleName  = self.text_input.get( scriptindex,scriptindex+' wordend' )
            try:
                module = importlib.import_module('vmtk.'+moduleName)
                # Find the principle class to instantiate the requested action defined inside the requested writerModule script.
                # Returns a single member list (containing the principle class name) which satisfies the following criteria:
                #   1) is a class defined within the script
                #   2) the class is a subclass of pypes.pypescript
                scriptObjectClasses = [x for x in dir(module) if isclass(getattr(module, x)) and issubclass(getattr(module, x), pypes.pypeScript)]
                scriptObjectClassName = scriptObjectClasses[0]
                scriptObject = getattr(module, scriptObjectClassName)
                scriptObject = scriptObject()
                members = scriptObject.InputMembers + scriptObject.OutputMembers
                for member in members:
                    optionlist.append('-'+member.OptionName)
                list = [option for option in optionlist if option.count(word)]
            except:
                return list
        else:
            list = [scriptname for scriptname in vmtkscripts.__all__ if scriptname.count(word)]
            for index, item in enumerate(list):
                # check if scriptname contains starting prefix 'vmtk.' and remove it before returning list to the user.
                if 'vmtk.' == item[0:5]:
                    splitList = item.split('.')
                    list[index] = splitList[1]
                else:
                    continue
        return list

    def FillSuggestionsList(self,word):
        from tkinter import END
        self.suggestionslist.delete(0,END)
        suggestions = self.GetSuggestionsList(word)
        for suggestion in suggestions:
            self.suggestionslist.insert(END,suggestion)

    def ReplaceTextCommand(self,word):
        self.text_input.delete(self.wordIndex[0],self.wordIndex[1])
        self.text_input.insert(self.wordIndex[0],word)
        self.text_input.focus_set()

    def ShowHelpCommand(self):
        word = self.GetWordUnderCursor()
        self.OutputText(word)
        if word:
            self.RunPype(word+' --help')
        else:
            self.OutputText('Enter your vmtk Pype above and Run.\n')

    def AutoCompleteCommand(self):
        word = self.GetWordIndex()
        self.suggestionswindow.withdraw()
        if word:
            self.FillSuggestionsList(word)
            self.suggestionswindow.geometry("%dx%d%+d%+d" % (400, 150, self.text_output.winfo_rootx(),self.text_output.winfo_rooty()))
            self.suggestionswindow.deiconify()
            self.suggestionswindow.lift()

    def InsertScriptName(self,scriptname):
        from tkinter import INSERT
        self.text_input.insert(INSERT,scriptname+' ')

    def InsertFileName(self):
        from tkinter import INSERT
        import tkinter.filedialog
        openfilename = tkinter.filedialog.askopenfilename()
        if not openfilename:
            return
        if len(openfilename.split()) > 1:
            openfilename = '"%s"' % openfilename
        self.text_input.insert(INSERT,openfilename+' ')

    def KeyPressHandler(self,event):
        if event.keysym == "Tab" :
            self.AutoCompleteCommand()
            self.suggestionslist.focus_set()
            self.suggestionslist.selection_set(0)
            return "break"
        else:
            self.text_input.focus_set()

    def TopKeyPressHandler(self,event):
        from tkinter import ACTIVE, INSERT
        if event.keysym in ['Down','Up'] :
            self.suggestionslist.focus_set()
        elif event.keysym == "Return":
            word = self.suggestionslist.get(ACTIVE)
            self.ReplaceTextCommand(word)
            self.suggestionswindow.withdraw()
            self.text_input.focus_set()
        elif len(event.keysym) == 1 :
            self.suggestionswindow.withdraw()
            self.text_input.insert(INSERT,event.keysym)
            self.text_input.focus_set()
        else :
            self.suggestionswindow.withdraw()
            self.text_input.focus_set()

    def NewHandler(self,event):
        self.NewCommand()

    def OpenHandler(self,event):
        self.OpenCommand()

    def SaveHandler(self,event):
        self.SaveCommand()

    def InsertFileNameHandler(self,event):
        self.InsertFileName()
        return "break"

    def QuitHandler(self,event):
        self.QuitCommand()

    def ShowHelpHandler(self,event):
        self.ShowHelpCommand()

    def RunKeyboardHandler(self,event):
        from tkinter import SEL_FIRST, TclError
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
        from tkinter import NORMAL, END, DISABLED
        self.text_output["state"] = NORMAL
        self.text_output.insert(END,text)
        self.text_output["state"] = DISABLED

    def BuildScriptMenu(self,parentmenu,modulename):
        from tkinter import Menu
        menu = Menu(parentmenu,bd=1,activeborderwidth=0)
        try:
            module = importlib.import_module('vmtk.'+modulename)
        except ImportError:
            return None
        scriptnames = [scriptname for scriptname in getattr(module, '__all__')]
        for index, scriptname in enumerate(scriptnames):
            # check if scriptname contains starting prefix 'vmtk.' and remove it before returning list to the user.
            if 'vmtk.' == scriptname[0:5]:
                splitList = scriptname.split('.')
                scriptnames[index] = splitList[1]
            else:
                continue
        menulength = 20
        for i in range(len(scriptnames)//menulength+1):
            subscriptnames = scriptnames[i*menulength:(i+1)*menulength]
            if not subscriptnames:
                break
            submenu = Menu(menu,bd=1,activeborderwidth=0)
            menu.add_cascade(label=subscriptnames[0]+"...",menu=submenu)
            for scriptname in subscriptnames:
                callback = CallbackShim(self.InsertScriptName,scriptname)
                submenu.add_command(label=scriptname,command=callback)
        return menu

    def BuildMainFrame(self):
        from tkinter import Menu, IntVar, StringVar, Toplevel, Listbox, Frame, PanedWindow, Text, Scrollbar, Entry
        from tkinter import X, N, S, W, E, VERTICAL, TOP, END, DISABLED, RAISED

        menu = Menu(self.master,activeborderwidth=0,bd=0)
        self.master.config(menu=menu)

        filemenu = Menu(menu,tearoff=0,bd=1,activeborderwidth=0)
        menu.add_cascade(label="File", underline=0,  menu=filemenu)
        filemenu.add_command(label="New", accelerator='Ctrl+N',command=self.NewCommand)
        filemenu.add_command(label="Open...",accelerator='Ctrl+O', command=self.OpenCommand)
        filemenu.add_command(label="Save as...",accelerator='Ctrl+S', command=self.SaveCommand)
        filemenu.add_separator()
        filemenu.add_command(label="Quit",accelerator='Ctrl+Q', command=self.QuitCommand)

        self.log_on = IntVar()
        self.log_on.set(1)

        self.output_to_file = StringVar()
        self.output_to_file.set('n')

        scriptmenu = Menu(menu,tearoff=0,bd=1,activeborderwidth=0)
        modulenames = ['vmtkscripts']
        for modulename in modulenames:
            scriptsubmenu = self.BuildScriptMenu(menu,modulename)
            if scriptsubmenu:
                scriptmenu.add_cascade(label=modulename,menu=scriptsubmenu)

        editmenu = Menu(menu,tearoff=0,bd=1,activeborderwidth=0)
        menu.add_cascade(label="Edit",underline=0,  menu=editmenu)
        editmenu.add_cascade(label="Insert script",menu=scriptmenu)
        editmenu.add_command(label="Insert file name", accelerator='Ctrl+F',command=self.InsertFileName)
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

        runmenu = Menu(menu,tearoff=0,bd=1,activeborderwidth=0)
        menu.add_cascade(label="Run", underline=0, menu=runmenu)
        runmenu.add_command(label="Run all", command=self.RunAllCommand)
        runmenu.add_command(label="Run current line", command=self.RunLineCommand)
        runmenu.add_command(label="Run selection", command=self.RunSelectionCommand)

        helpmenu = Menu(menu,tearoff=0,bd=1,activeborderwidth=0)
        menu.add_cascade(label="Help", underline=0, menu=helpmenu)
        helpmenu.add_command(label="Help", underline=0, accelerator='F1',command=self.ShowHelpCommand)
        helpmenu.add_command(label="About", underline=0, command=self.AboutCommand)

        self.master.bind("<Control-KeyPress-q>", self.QuitHandler)
        self.master.bind("<Control-KeyPress-n>", self.NewHandler)
        self.master.bind("<Control-KeyPress-o>", self.OpenHandler)
        self.master.bind("<Control-KeyPress-s>", self.SaveHandler)
        self.master.bind("<Control-KeyPress-f>", self.InsertFileNameHandler)
        self.master.bind("<KeyPress-F1>", self.ShowHelpHandler)
        self.master.bind("<KeyPress>", self.KeyPressHandler)

        self.wordIndex = ['1.0','1.0']

        self.suggestionswindow = Toplevel(bg='#ffffff',bd=0,height=50,width=600,highlightthickness=0,takefocus=True)
        self.suggestionswindow.overrideredirect(1)
        self.suggestionslist = Listbox(self.suggestionswindow,bg='#ffffff',bd=1,fg='#336699',activestyle='none',highlightthickness=0,height=9)
        self.suggestionslist.insert(END,"foo")
        self.suggestionslist.pack(side=TOP,fill=X)
        self.suggestionswindow.bind("<KeyPress>", self.TopKeyPressHandler)
        self.suggestionswindow.withdraw()

        self.master.rowconfigure(0,weight=1)
        self.master.columnconfigure(0,weight=1)
        content = Frame(self.master,bd=0,padx=2,pady=2)
        content.grid(row=0,column=0,sticky=N+S+W+E)
        content.rowconfigure(0,weight=1,minsize=50)
        content.rowconfigure(1,weight=0)
        content.columnconfigure(0,weight=1)

        panes = PanedWindow(content,orient=VERTICAL,bd=1,sashwidth=8,sashpad=0,sashrelief=RAISED,showhandle=True)
        panes.grid(row=0,column=0,sticky=N+S+W+E)

        frame1 = Frame(panes,bd=0)
        frame1.grid(row=0,column=0,sticky=N+S+W+E)
        frame1.columnconfigure(0,weight=1)
        frame1.columnconfigure(1,weight=0)
        frame1.rowconfigure(0,weight=1)

        panes.add(frame1,height=300,minsize=20)

        frame2 = Frame(panes,bd=0)
        frame2.grid(row=1,column=0,sticky=N+S+W+E)
        frame2.columnconfigure(0,weight=1)
        frame2.columnconfigure(1,weight=0)
        frame2.rowconfigure(0,weight=1)

        panes.add(frame2,minsize=20)

        self.text_input = Text(frame1, bg='#ffffff',bd=1,highlightthickness=0)

        self.text_input.bind("<KeyPress>", self.KeyPressHandler)
        self.text_input.bind("<Button-3>", self.PopupHandler)
        self.text_input.bind("<Control-Return>", self.RunKeyboardHandler)

        self.input_scrollbar = Scrollbar(frame1,orient=VERTICAL,command=self.text_input.yview)
        self.text_input["yscrollcommand"] = self.input_scrollbar.set

        self.text_output = Text(frame2,state=DISABLED,bd=1,bg='#ffffff',highlightthickness=0)

        self.output_scrollbar = Scrollbar(frame2,orient=VERTICAL,command=self.text_output.yview)
        self.text_output["yscrollcommand"] = self.output_scrollbar.set

        self.text_entry = Entry(content,bd=1,bg='#ffffff',state=DISABLED,highlightthickness=0)

        self.text_input.focus_set()

        self.text_input.grid(row=0,column=0,sticky=N+S+W+E)
        self.input_scrollbar.grid(row=0,column=1,sticky=N+S+W+E)
        self.text_output.grid(row=0,column=0,sticky=N+S+W+E)
        self.output_scrollbar.grid(row=0,column=1,sticky=N+S+W+E)
        self.text_entry.grid(row=1,column=0,sticky=N+S+W+E)

        self.popupmenu = Menu(self.text_input, tearoff=1, bd=0)
        self.popupmenu.add_command(label="Context help", command=self.ShowHelpCommand)
        self.popupmenu.add_cascade(label="Insert script",menu=scriptmenu)
        self.popupmenu.add_command(label="Insert file name...", command=self.InsertFileName)
        self.popupmenu.add_separator()
        self.popupmenu.add_command(label="Run all", command=self.RunAllCommand)
        self.popupmenu.add_command(label="Run current line", command=self.RunLineCommand)
        self.popupmenu.add_command(label="Run selection", command=self.RunSelectionCommand)

        self.output_stream = TkPadOutputStream(self.text_output)
        self.input_stream = TkPadInputStream(self.text_entry,self.output_stream)


def RunPypeTkPad():

    manager = Manager()
    queue = manager.list()
    output = manager.list()

    pypeProcess = Process(target=pypeserver.PypeServer, args=(queue,output,output))
    pypeProcess.start()

    from tkinter import Tk

    root = Tk()
    app = PypeTkPad(root,queue,output)
    root.mainloop()

    pypeProcess.terminate()


if __name__=='__main__':

    RunPypeTkPad()
