#!/usr/bin/env python

import sys
import os
import re
from optparse import OptionParser

## _____________________________________________________________________________
##                                                             Global parameters

debugFlag = False

## =============================================================================
##
##  Class definitions
##
## =============================================================================

## _____________________________________________________________________________
##                                                                   Parse Error
class ParserError(Exception):
    """
    Parser exception class
    """

    def __init__(self, msg):
        """
        Initialisation
        """
        self.msg = msg


    def __str__(self):
        """
        Return value string
        """
        return repr(self.msg)


## _____________________________________________________________________________
##                                                                Iterator block
class IteratorBlock():
    """
    Iterator block
    """

    # ______________________________________________________________________
    #                                                         Initialisation
    def __init__(self, options):
        """
        Initialisation
        """
        self._iterator_var = ""
        self._functions = []
        self._blocks = []


    # ______________________________________________________________________
    #                                                  Set iterator variable
    def setIteratorVar(self,var=""):
        """
        Set iterator variable.
        """
        self._iterator_var = var


    # ______________________________________________________________________
    #                                                      Set function list
    def setFunctionList(self, functions=""):
        """
        Set function list.
        """
        self._functions = re.split(",",functions)


    # ______________________________________________________________________
    #                                                               AddBlock
    def addWrapperBlock(self, block):
        """
        Add block to iterator list
        """
        self._blocks.append(block)


    # ______________________________________________________________________
    #                                             Process the iterator block
    def parse(self):
        """
        Parse the iterator block and all its wrapper blocks.
        """
        for block in self._blocks:
            for function_name in self._functions:
                block.parse(iter_var=self._iterator_var, iter_val=function_name)


    # ______________________________________________________________________
    #                                                                Summary
    def summary(self):
        """
        Summary of the iterator block.
        """
        print "============================================================"
        print "  Summary of the iterator block"
        print "============================================================"
        print "  Iterator variable : %s" %(self._iterator_var)
        print "  Number of blocks  : %d" %(len(self._blocks))
        if (len(self._functions) > 0):
            print "  Functions :"
            for f_element in self._functions:
                print "    %s " %(f_element)
        print "============================================================"


## _____________________________________________________________________________
##                                                                 Wrapper block
class WrapperBlock():
    """
    Wrapper block
    """

    # ______________________________________________________________________
    #                                                         Initialisation
    def __init__(self, file_ptr, deffile_ptr, pydocfile_ptr, options):
        """
        Initialisation of the wrapper block.

        *file_ptr* File object for output file
        *file_ptr* File object for wrapper definition file
        *file_ptr* File object for python function documentation file
        *options*  Processing options.
        """
        self._file = file_ptr
        self._deffile = deffile_ptr
        self._pydocfile = pydocfile_ptr
        self._lines = []

        self.doc = DocumentationBlock(file_ptr)
        self.inDoc = False

        self._options = options


    # ______________________________________________________________________
    #                                             Set file pointer for block
    def setFile(self, file_ptr=None):
        """
        Set the file
        """
        self._file = file_ptr


    # ______________________________________________________________________
    #                                        Add a line of code to the block
    def addLine(self, line=""):
        """
        Process line and add it to the block.
        """

        line_org = line # TODO: Fix shallow copy -> deep copy

        # ______________________________________________________________
        #                                                  Substitutions

        # Substitute DOCSTRING
        if (self.doc.summary() != ""):
            line = re.sub("\$DOCSTRING", self.doc.summary(), line)

        # Substitute HFPP_FUNC_NAME
        if (self.doc.name() != ""):
            line = re.sub("HFPP_FUNC_NAME", self.doc.name(), line)

        # ______________________________________________________________
        #                                                       Comments
        m = re.match("^\/\/\#.*", line)
        if  (m is not None):
            return

        # ______________________________________________________________
        #                                          Extract documentation

        # Check for function name
        m = re.match("^\#define HFPP_FUNC_NAME (.*)", line_org)
        if (m is not None):
            self.doc.setName(m.group(1))
            return

        # Check for parameters
        m = re.match("^\#define HFPP_PARDEF_\d+ \(\w*\)\((\w*)\)\(\)\(\"(.*)\"\)\(.*\)\(.*\)\(.*\)", line)
        if (m is not None):
            self.doc.addParameter(m.group(1),m.group(2))

        # Check for Extra documentation
        m = re.match("^\/\*!", line)
        if (m is not None):
            self.inDoc = True
            return

        if (self.inDoc):
            # Check if end of comment block is reached
            m = re.match("^[\s]*\*\/", line)
            if (m is not None):
                self.inDoc = False
                return

            # Check for redundant (superfluous) comments
            if (re.match("\s*.brief.*", line) is not None):
                #print "Brief comment found."
                return
            if (re.match(".*\$PARDOCSTRING.*", line) is not None):
                return

            #print line.strip("\n")
            # Add comment line to examples
            self.doc.addExample(line.strip())
            return

        # ______________________________________________________________
        #                                              Add line to block

        self._lines.append(line)

        return


    # ______________________________________________________________________
    #                                                      Process the block
    def parse(self, iter_var=None, iter_val=None):
        """
        Parse pp
        the wrapper block for output to file
        """
        inDoc = False

        pyDocString = self.doc.getPyDocString()
        pythonDoc = self.doc.getPythonDoc()
        doxDoc = self.doc.getDoxygenDoc()

        # Write block content to output file
        if ((self._file is not None) and
            (self._deffile is not None) and
            (self._pydocfile is not None)):
            for line in self._lines:
                # In iterator blocks: replace iterator variable
                if (iter_var is not None):
                    line = iter_subst(iter_var, iter_val, line)
                    pyDocString = iter_subst(iter_var, iter_val, pyDocString)
                    pythonDoc = iter_subst(iter_var, iter_val, pythonDoc)
                    doxDoc = iter_subst(iter_var, iter_val, doxDoc)

                # Header block start
                m = re.match("\/\/\$COPY_TO HFILE START",line)
                if (m is not None):
                    if (self._deffile is not None):
                        inHeader = True
                        continue
                    else:
                        raise ParserError("No open definition file.")

                if (inHeader):
                    m = re.match("^\/\/", line)
                    if (m is None):
                        self._deffile.write(line)

                # Header block end
                m = re.match(".*\/\/\$COPY_TO END",line)
                if (m is not None):
                    inHeader = False
                    # Dump all extra (auto generated) header information, e.g. PYDOCSTRING
                    self._file.write("#define PYDOCSTRING \"" + pyDocString + "\"\n")
                    # Dump documentation
                    self._file.write("/*!\n" + doxDoc + "\n*/\n")
                    self._pydocfile.write(pythonDoc + "\n\n")
                    # Add separator in definition file
                    self._deffile.write("#include \"hfppnew-generatewrappers.def\"\n\n")
                    continue

                self._file.write(line)

            self._file.write("\n\n")
        else:
            raise ParserError("No open output file.")

        return


    # ______________________________________________________________________
    #                                                                Summary
    def summary(self):
        """
        Summary of the wrapper block.
        """
        print "============================================================"
        print "  Summary of the wrapper block"
        print "============================================================"
        print "  Name            : %s" %(self.doc.name())
        print "  Number of lines : %d" %(len(self._lines))
        print "============================================================"


## _____________________________________________________________________________
##                                                           Documentation block
class DocumentationBlock():
    """
    Documentation of a wrapper block function.
    """



    # ______________________________________________________________________
    #                                                         Initialisation
    def __init__(self, options):
        """
        Initialisation
        """
        self._name = ""
        self._summary = ""
        self._parameters = []
        self._example_lines = []


    # ______________________________________________________________________
    #                                                    Check string syntax
    def checkSyntax(self, line_str):
        """
        Check and correct the syntax of a string in order not to
        conflict with python syntax.
        """
        result = line_str

        # Check for quotes: " -> '
        result = re.sub("\"", "\'", result)

        # Remove \f$
        result = re.sub(".f\$", "$", result)

        # Exponent (** -> ^)
        result = re.sub("\*\*", "^", result)

        # Vector
        result = re.sub("\\vec", "vec", result)

        return result


    # ______________________________________________________________________
    #                                                   Name of the function
    def setName(self, name="[empty]"):
        """
        Set the name of the block.
        """

        name_checked = self.checkSyntax(name)

        self._name = name_checked


    def name(self):
        """
        Return the name attribute.
        """
        return self._name


    # ______________________________________________________________________
    #                                                Summary of the function
    def setSummary(self, summary="[empty]"):
        """
        Set the summary of the block.
        """
        summary_checked = self.checkSyntax(summary)

        self._summary = summary_checked


    def summary(self):
        """
        Return the summary attribute.
        """
        return self._summary


    # ______________________________________________________________________
    #                                                    Function parameters
    def addParameter(self, name, description):
        """
        Add a parameter description.
        """
        name_checked = self.checkSyntax(name)
        description_checked = self.checkSyntax(description)

        self._parameters.append([name_checked, description_checked])


    def parameters(self):
        """
        Return the parameter description.
        """
        return self._parameters


    # ______________________________________________________________________
    #                                                      Function examples
    def addExample(self, example):
        """
        Add example.
        """
        example_checked = self.checkSyntax(example)

        self._example_lines.append(example_checked)


    def examples(self):
        """
        Return example.
        """
        return self._example_lines


    # ______________________________________________________________________
    #                                                          Add attribute
    def addAttribute(self, attribute_name, attribute_description):
        """
        Add attribute name and description.
        """
        attribute_name_checked = self.checkSyntax(attribute_name)
        attribute_description_checked = self.checkSyntax(attribute_description)

        self._attributes.append([attribute_name_checked, attribute_description_checked])


    # ______________________________________________________________________
    #                                                         getPyDocString
    def getPyDocString(self):
        """
        Get a compact version of the Python docstring
        """
        result = ""

        # Function name
        result += self.name() + "("
        parameters = self.parameters()
        if (len(parameters) > 0):
            for i in range(len(parameters)):
                par_name = parameters[i][0]
                result += par_name
                if (i < len(parameters)-1):
                    result += ", "
        result += ")\\n\\n"

        # Brief function description
        result += self.summary() + "\\n\\n"

        # Parameter description
        if (len(self.parameters()) > 0):
            result += "Parameters:\\n"
            for par in self.parameters():
                result += "*" + par[0] + "* " + par[1] + "\\n"

        # Examples
        if (len(self.examples()) > 0):
            result += "\\nExample:\\n"
            for example in self.examples():
                result += "  " + example.strip() + "\\n"

        return result


    # ______________________________________________________________________
    #                                                           getPythonDoc
    def getPythonDoc(self):
        """
        Get Python documentation statement.
        """
        result = ""

        result += "%s.__doc__ = \"\"\"%s\"\"\"" %(self.name(), self.getPyDocString())

        return result


    # ______________________________________________________________________
    #                                                 getSphinxDocumentation
    def getSphinxDoc(self):
        """
        Get a python formatted docstring.
        """
        result = ""

        # Function name
        result += self.name() + "("
        parameters = self.parameters()
        if (len(parameters) > 0):
            for i in range(len(parameters)):
                par_name = parameters[i][0]
                result += par_name
                if (i < len(parameters)-1):
                    result += ", "
        result += ")\n\n"

        # Brief function description
        result += self.summary() + "\n"

        # Parameter description
        if (len(self.parameters()) > 0):
            result += "Parameters:\n"
            for par in parameters:
                result += "*" + par[0] + "* " + par[1] + "\n"

        # Examples
        if (len(self.example()) > 0):
            result += "\nExamples::\n"
            for example in self.examples():
                result += "  " + example.strip() + "\n"

        return result


    # ______________________________________________________________________
    #                                                getDoxygenDocumentation
    def getDoxygenDoc(self):
        """
        Get a Doxygen parsable string.
        """
        result = ""

        # Function name
        result += self.name() + "("
        parameters = self.parameters()
        if (len(parameters) > 0):
            for i in range(len(parameters)):
                par_name = parameters[i][0]
                result += par_name
                if (i < len(parameters)-1):
                    result += ", "
        result += ")\n\n"

        # Brief summary
        result += "\\brief %s\n\n" %(self.summary())

        # Parameter description
        for par in self.parameters():
            result += "\\param %s -- %s\n" %(par[0], par[1])

        # Examples
        result += "\n"
        for line in self._example_lines:
            result += line

        return result


## =============================================================================
##
##  Function definitions
##
## =============================================================================

## _____________________________________________________________________________
##                                                  Substitute iterator variable
def iter_subst(var, subst, input_string):
    """
    Replace a variable by a name

    *var*          Variable to be substituted.
    *repl*         Substitution string.
    *input_string* Source string in which to substitute.
    """

    result = input_string
    subst_caps = subst[0].capitalize() + subst[1:]
    subst_low = subst.lower()

    result = re.sub("\{\$" + var + "\!CAPS\}|\$" + var + "\!CAPS", subst_caps, result)
    result = re.sub("\{\$" + var + "\!LOW\}|\$" + var + "\!LOW", subst_low, result)
    result = re.sub("\{\$" + var +"\}|\$" + var, subst, result)

    return result


## _____________________________________________________________________________
##                                                                       Parsers

## ________________________________________________________________________
##                                                            Parse options
def parseOptions():
    """
    Parse the options of the script
    """
    if debugFlag:
        print "Executing parseOptions..."

    usage = "usage: %prog [options] inputfile outputfile"
    parser = OptionParser(usage)

    # Verbose option
    parser.add_option("-v", "--verbose",
                      action="store_true", dest="verbose", default=True,
                      help="show verbose processing information [default]")

    # Quiet option
    parser.add_option("-q", "--quiet",
                      action="store_false", dest="verbose",
                      help="hide processing information")

    (options, args) = parser.parse_args()

    if len(args) != 2:
        parser.error("Incorrect number of arguments")

    return (options, args)


## ________________________________________________________________________
##                                                              Parse files
def parseCode(input_filename, output_filename, options):
    """
    Parse a code file.
    """

    iterator_block = None
    wrapper_block = None

    header_rule  = "//" + "="*80 + "\n"
    header = header_rule + "// ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY code_parser.py\n" + header_rule + "\n"

    if (options.verbose | debugFlag):
        print "Executing parseFile..."

    # Open input file
    input_file = open(input_filename, 'r')
    if (options.verbose):
        print "Input file  : %s" %(input_filename)

    # Open output file
    output_path = os.path.dirname(os.path.abspath(output_filename))
    output_basename = os.path.basename(output_filename).split(".")[0]

    output_filename = output_path + "/" + output_basename + ".pp.cc"
    output_file = open(output_filename, 'w')
    if (output_file is not None):
        output_file.write(header)
    if (options.verbose):
        print "Output file : %s" %(os.path.relpath(output_filename))


    # Definitions file
    def_file = None

    # PyDoc file
    pydoc_filename = output_path + "/" + output_basename + ".py"
    pydoc_file = open(pydoc_filename, 'w')
    pydoc_file.write("from _hftools import *\n\n")
    if (options.verbose):
        print "Python doc file  : %s" %(os.path.relpath(pydoc_filename))

    # Parse input file
    for line in input_file:

        # Check definition file (open if filename is set)
        m = re.match("^\/\/\$FILENAME: HFILE=(.*)", line)
        if ((m is not None) & (def_file is None)):
            def_filename = m.group(1) + ".latest"
            def_file = open(def_filename, 'w')
            if (options.verbose):
                print "Definition file: %s" %(def_filename)
            if (def_file is not None):
                def_file.write(header)
            continue

        # Check docstring
        m = re.match('^\/\/\$DOCSTRING: (.*)',line)
        if (m is not None):
            docstring = m.group(1)
            continue

        # Check iterator block (start)
        m = re.match("^\/\/\$ITERATE (\w*) ([A-Za-z0-9,]*)", line)
        if (m is not None):
            iterator_block = IteratorBlock(options)         # Create iterator block
            iterator_block.setIteratorVar(m.group(1))       # Set iterator variable name
            iterator_block.setFunctionList(m.group(2))      # Fill list of functions to iterate
            continue

        # Check iterator block (end)
        m = re.match("^\/\/\$ENDITERATE", line)
        if (m is not None):
            if (iterator_block is not None):
                iterator_block.parse()                      # Process iterator block
                iterator_block = None                       # Reset iterator_block
            else:
                raise ParserError("End iterate without begin iterate.")
            continue

        # Check wrapper block (start)
        m = re.match("^\/\/\$COPY_TO HFILE START", line)
        if (m is not None):
            wrapper_block = WrapperBlock(output_file, def_file, pydoc_file, options)  # Create block
            wrapper_block.addLine(line)
            wrapper_block.doc.setSummary(docstring)         # Set document summary
            continue

        # Check wrapper block (end)
        m = re.match("^\/\/\$COPY_TO HFILE: \#include \"hfppnew-generatewrappers\.def\"", line)
        if (m is not None):
            if (wrapper_block is not None):
                wrapper_block.addLine("#include \"hfppnew-generatewrappers.def\"")
                if (iterator_block is not None):
                    iterator_block.addWrapperBlock(wrapper_block)  # Add wrapper block to iterator
                else:
                    wrapper_block.parse()                   # Process wrapper block
            else:
                raise ParserError("End wrapper block without begin.")

            wrapper_block = None                            # Reset wrapper block
            continue

        if (wrapper_block is not None):
            wrapper_block.addLine(line)
        else:
            output_file.write(line)

    # Check if all blocks are closed
    if (wrapper_block is not None):
        raise ParserError("Wrapper block is not closed at end of file.")
    if (iterator_block is not None):
        raise ParserError("Iterator block is not closed at end of file.")

    # Close definition file
    if (def_file is not None):
        def_file.close()

    # Close python documentation file
    if (pydoc_file is not None):
        pydoc_file.close()

    # Close output file
    output_file.close()

    # Close input file
    input_file.close()


## =============================================================================
##
##  Main
##
## =============================================================================

def main():
    """
    Main function
    """
    (options, args) = parseOptions()

    input_filename  = args[0]
    output_filename = args[1]

    parseCode(input_filename, output_filename, options)


if __name__ == '__main__':
    main()


## =============================================================================
##
##  TODO list
##
## =============================================================================

# TODO: Add Sphinx doxcumentation for file x.cc to x.py

