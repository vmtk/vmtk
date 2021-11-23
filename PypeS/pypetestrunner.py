#!${PYTHON_SHEBANG}

from __future__ import absolute_import #NEEDS TO STAY AS TOP LEVEL MODULE FOR Py2-3 COMPATIBILITY
import vtk
import sys

from vmtk import pypes

import unittest
import datetime
import traceback

pypetestrunner = 'PypeTestRunner'


class PypeTestCase(unittest.TestCase):
    def __init__(self,line,lineno):
        unittest.TestCase.__init__(self)
        self.Line = line
        self.LineNo = lineno
        self.Name = 'unavailable'

    def setUp(self):
        self.Pipe = pypes.Pype()
        self.Pipe.SetArgumentsString(self.Line)
        self.Pipe.LogOn = 0

    def runTest(self):
        self.Pipe.ParseArguments()
        if self.Pipe.Arguments:
            self.Name = self.Pipe.Arguments[self.Pipe.Arguments.index('-name') + 1]
        self.Pipe.Execute()
        self.Log = self.Pipe.GetScriptObject('pypetest','0').PypeTestLog
        splitLog = self.Log.split('.')
        self.Result = splitLog[1]
        self.assertEqual(self.Result,'passed')


class PypeTestResult(unittest.TestResult):
    def appendLogLine(self,logline):
        try:
            self.ResultList.append(logline)
        except:
            self.ResultList = []
            self.ResultList.append(logline)

    def startTest(self,test):
        self.testLog = {'id':str(test.LineNo),'error':'None'}

    def stopTest(self,test):
        self.testLog['name'] = test.Name
        self.testLog['pype'] = test.Line
        self.testLog['date'] = datetime.datetime.now().strftime('%d-%m-%Y')
        self.testLog['time'] = datetime.datetime.now().strftime('%H:%M:%S')
        self.appendLogLine(self.testLog)
        self.testsRun += 1

    def addError(self,test,err):
        self.testLog['result'] = 'error'
        self.testLog['error'] = err
        self.errors.append([test,err])

    def addFailure(self,test,err):
        self.testLog['result'] = 'failed'
        self.testLog['error'] = err
        self.failures.append([test,err])

    def addSuccess(self,test):
        self.testLog['result'] = 'passed'


class PypeTestRunner(pypes.pypeScript):

    def __init__(self):

        pypes.pypeScript.__init__(self)

        self.SuiteName = None
        self.TestSuiteFileName = None
        self.LogFileName = ''
        self.TestSuiteLog = ''
        self.Format = ''
        self.GuessFormat = 1

        self.SetScriptName('pypetestrunner')
        self.SetScriptDoc('Run a set of tests from a given list')
        self.SetInputMembers([
            ['SuiteName','name','str',1,'','name of the test suite'],
            ['TestSuiteFileName','testsuitefile','str',1,'','test suitefilename'],
            ['LogFileName','logfile','str',1,'','Log filename'],
            ['Format','f','str',1,'["text","xml"]','file format'],
            ['GuessFormat','guessformat','bool',1,'','guess file format from extension'],
            ])
        self.SetOutputMembers([
            ['TestSuiteLog','log','str',1,'','log']
            ])

    def WriteXMLLogFile(self):
        self.PrintLog('Writing XML Test Suite Log')
        from xml.dom import minidom
        xmlDocument = minidom.Document()
        xmlTestSuite = xmlDocument.appendChild(xmlDocument.createElement('TestSuite'))
        xmlTestSuite.setAttribute('name',self.SuiteName)
        xmlTestSuite.setAttribute('tests_run',str(self.Result.testsRun))
        xmlTestSuite.setAttribute('tests_errors',str(len(self.Result.errors)))
        xmlTestSuite.setAttribute('tests_failed',str(len(self.Result.failures)))
        xmlTestSuite.setAttribute('date',self.Date)
        xmlTestSuite.setAttribute('time',self.Time)
        xmlTestSuite.setAttribute('outcome',self.Success)

        for case in self.Result.ResultList:
            xmlCase = xmlTestSuite.appendChild(xmlDocument.createElement('TestCase'))
            if case['error'] != 'None':
                caseError = case['error']
                editedError = ''
                caseException = traceback.format_exception(caseError[0],caseError[1],caseError[2])
                for line in caseException:
                    editedError += line
                xmlCaseError = xmlCase.appendChild(xmlDocument.createElement('CaseError'))
                errorText = xmlDocument.createTextNode(editedError)
                xmlCaseError.appendChild(errorText)
                del case['error']
            xmlCasePype = xmlCase.appendChild(xmlDocument.createElement('CasePype'))
            xmlCasePype.appendChild(xmlDocument.createTextNode(case['pype']))
            del case['pype']
            for k,v in case.items():
                xmlCase.setAttribute(k,v)

        xmlFile = open(self.LogFileName,'w')
        xmlFile.write(xmlDocument.toprettyxml())
        xmlFile.close()

    def WriteTEXTLogFile(self):
        self.PrintLog('Writing TEXT Test Suite Log')
        txtDocument = open(self.LogFileName, 'w')
        txtDocument.write(self.TestSuiteLog)

    def Execute(self):

        self.PrintLog('Testing')

        if not self.TestSuiteFileName:
            self.PrintError('Error: No Test list.')
        if not self.SuiteName:
            self.PrintError('Error: No test name.')

        extensionFormats = {'txt':'text',
                            'xml':'xml'}

        self.Suite = unittest.TestSuite()
        self.Success = 'FAILED'
        lineno = 1
        self.TestSuiteLog = ''

        with open(self.TestSuiteFileName) as suitefile:
            for line in suitefile.readlines():
                if line.strip():
                    self.Suite.addTest(PypeTestCase(line,lineno))
                    lineno += 1

        self.Result = PypeTestResult()
        self.Suite.run(self.Result)
        if self.Result.wasSuccessful():
            self.Success = 'SUCCESS'

        self.TestSuiteLog += '\nID NAME RESULT'
        for case in self.Result.ResultList:
            resultline = '\n'+ case['id'] +' '+ case['name'] +' '+ case['result']
            self.TestSuiteLog += resultline

        self.Date = datetime.datetime.now().strftime('%d-%m-%Y')
        self.Time = datetime.datetime.now().strftime('%H:%M:%S')

        self.TestSuiteLog += '\n\nTEST SUITE NAME: ' + self.SuiteName
        self.TestSuiteLog += '\nDATE: ' + self.Date
        self.TestSuiteLog += '\nTIME: ' + self.Time
        self.TestSuiteLog += '\nTOTAL TESTS RUN: ' + str(self.Result.testsRun)
        self.TestSuiteLog += '\nTOTAL TEST ERRORS: ' + str(len(self.Result.errors))
        self.TestSuiteLog += '\nTOTAL TEST FAILURES: ' + str(len(self.Result.failures))
        self.TestSuiteLog += '\nTEST SUITE OUTCOME: ' + self.Success

        if self.GuessFormat and self.LogFileName and not self.Format:
            import os.path
            extension = os.path.splitext(self.LogFileName)[1]
            if extension:
                extension = extension[1:]
                if extension in list(extensionFormats.keys()):
                    self.Format = extensionFormats[extension]

        if self.LogFileName:
            if (self.Format == 'text'):
                self.WriteTEXTLogFile()
            elif (self.Format == 'xml'):
                self.WriteXMLLogFile()
            else:
                self.PrintError('Error: unsupported format '+ self.Format + '.')


if __name__=='__main__':
    main = pypes.pypeMain()
    main.Arguments = sys.argv
    main.Execute()
