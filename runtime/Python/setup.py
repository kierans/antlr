import os
import sys
from distutils.errors import *
from distutils.command.clean import clean as _clean
from distutils.cmd import Command
from distutils.core import setup
from distutils import log


from distutils.core import setup

class clean(_clean):
    """Also cleanup local temp files."""

    def run(self):
        _clean.run(self)

        import fnmatch
        
        # kill temporary files
        patterns = [
            # generic tempfiles
            '*~', '*.bak', '*.pyc',

            # tempfiles generated by ANTLR runs
            't[0-9]*Lexer.py', 't[0-9]*Parser.py',
            '*.tokens', '*__.g',
            ]
            
        for path in ('antlr3', 'unittests', 'tests'):
            path = os.path.join(os.path.dirname(__file__), path)
            if os.path.isdir(path):
                for root, dirs, files in os.walk(path, topdown=True):
                    graveyard = []                    
                    for pat in patterns:
                        graveyard.extend(fnmatch.filter(files, pat))

                    for name in graveyard:
                        filePath = os.path.join(root, name)

                        try:
                            log.info("removing '%s'", filePath)
                            os.unlink(filePath)
                        except OSError, exc:
                            log.warn(
                                "Failed to delete '%s': %s",
                                filePath, exc
                                )

            
class TestError(DistutilsError):
    pass


# grml.. the class name appears in the --help output:
# ...
# Options for 'CmdUnitTest' command
# ...
# so I have to use a rather ugly name...
class unittest(Command):
    """Run unit tests for package"""

    description = "run unit tests for package"

    user_options = [
        ]
    boolean_options = []

    def initialize_options(self):
        pass
    
    def finalize_options(self):
        pass
    
    def run(self):
        testDir = os.path.join(os.path.dirname(__file__), 'unittests')
        if not os.path.isdir(testDir):
            raise DistutilsFileError(
                "There is not 'unittests' directory. Did you fetch the development version?",
                )

        import glob
        import imp
        import unittest
        import traceback
        import StringIO
        
        suite = unittest.TestSuite()
        loadFailures = []
        
        # collect tests from all unittests/test*.py files
        testFiles = []
        for testPath in glob.glob(os.path.join(testDir, 'test*.py')):
            testFiles.append(testPath)

        testFiles.sort()
        for testPath in testFiles:
            testID = os.path.basename(testPath)[:-3]

            try:
                modFile, modPathname, modDescription \
                         = imp.find_module(testID, [testDir])

                testMod = imp.load_module(
                    testID, modFile, modPathname, modDescription
                    )
                
                suite.addTests(
                    unittest.defaultTestLoader.loadTestsFromModule(testMod)
                    )
                
            except Exception:
                buf = StringIO.StringIO()
                traceback.print_exc(file=buf)
                
                loadFailures.append(
                    (os.path.basename(testPath), buf.getvalue())
                    )              
                
            
        runner = unittest.TextTestRunner(verbosity=2)
        result = runner.run(suite)

        for testName, error in loadFailures:
            sys.stderr.write('\n' + '='*70 + '\n')
            sys.stderr.write(
                "Failed to load test module %s\n" % testName
                )
            sys.stderr.write(error)
            sys.stderr.write('\n')
            
        if not result.wasSuccessful() or loadFailures:
            raise TestError(
                "Unit test suite failed!",
                )
            

class functest(Command):
    """Run functional tests for package"""

    description = "run functional tests for package"

    user_options = [
        ('testcase=', None,
         "testcase to run [default: run all]"),
        ]
    
    boolean_options = []

    def initialize_options(self):
        self.testcase = None
        
    
    def finalize_options(self):
        pass

    
    def run(self):
        testDir = os.path.join(os.path.dirname(__file__), 'tests')
        if not os.path.isdir(testDir):
            raise DistutilsFileError(
                "There is not 'tests' directory. Did you fetch the development version?",
                )

        # make sure, relative imports from testcases work
        sys.path.insert(0, testDir)

        import glob
        import imp
        import unittest
        import traceback
        import StringIO
        
        suite = unittest.TestSuite()
        loadFailures = []
        
        # collect tests from all tests/t*.py files
        testFiles = []
        for testPath in glob.glob(os.path.join(testDir, 't*.py')):
            if (testPath.endswith('Lexer.py')
                or testPath.endswith('Parser.py')
                ):
                continue

            # if a single testcase has been selected, filter out all other
            # tests
            if (self.testcase is not None
                and os.path.basename(testPath)[:-3] != self.testcase
                ):
                continue
            
            testFiles.append(testPath)

        testFiles.sort()
        for testPath in testFiles:
            testID = os.path.basename(testPath)[:-3]

            try:
                modFile, modPathname, modDescription \
                         = imp.find_module(testID, [testDir])

                testMod = imp.load_module(
                    testID, modFile, modPathname, modDescription
                    )
                
                suite.addTests(
                    unittest.defaultTestLoader.loadTestsFromModule(testMod)
                    )
                
            except Exception:
                buf = StringIO.StringIO()
                traceback.print_exc(file=buf)
                
                loadFailures.append(
                    (os.path.basename(testPath), buf.getvalue())
                    )              
                
            
        runner = unittest.TextTestRunner(verbosity=2)
        result = runner.run(suite)

        for testName, error in loadFailures:
            sys.stderr.write('\n' + '='*70 + '\n')
            sys.stderr.write(
                "Failed to load test module %s\n" % testName
                )
            sys.stderr.write(error)
            sys.stderr.write('\n')
            
        if not result.wasSuccessful() or loadFailures:
            raise TestError(
                "Functional test suite failed!",
                )
            

setup(name='antlr3',
      version='1.0b1',
      packages=['antlr3'],

      cmdclass={'unittest': unittest,
                'functest': functest,
                'clean': clean
                },
      )