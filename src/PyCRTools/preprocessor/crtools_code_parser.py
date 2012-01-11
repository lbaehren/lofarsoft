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
    def __init__(self, file_ptr, deffile_ptr, pydocfile_ptr, ifdef_list, options):
        """
        Initialisation of the wrapper block.

        ============= ===================================================
        *file_ptr*    File object for output file
        *file_ptr*    File object for wrapper definition file
        *file_ptr*    File object for python function documentation file
        *ifdef_list*  Ifdef preprocessor list
        *options*     Processing options.
        ============= ===================================================
        """
        self._file = file_ptr
        self._deffile = deffile_ptr
        self._pydocfile = pydocfile_ptr
        self._lines = []
        self._ifdef_list = ifdef_list

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

        line_org = line

        # ______________________________________________________________
        #                                                  Substitutions

        # Substitute DOCSTRING
        if (self.doc.getSummary() != ""):
            line = re.sub("\$DOCSTRING", self.doc.getSummary(), line)

        # Substitute HFPP_FUNC_NAME
        if (self.doc.getName() != ""):
            line = re.sub("HFPP_FUNC_NAME", self.doc.getName(), line)

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
            if (re.match("\s*.brief.*", line)):
                return
            if (re.match(".*\$PARDOCSTRING.*", line)):
                return

            # Add line to documentation
            self.doc.parseDocLine(line)

            # Add comment line to examples
            # self.doc.addExample(line.strip())
            return

        # ______________________________________________________________
        #                                              Add line to block

        self._lines.append(line)

        return


    # ______________________________________________________________________
    #                                                      Process the block
    def parse(self, iter_var=None, iter_val=None):
        """
        Parse the wrapper block for output to file.
        """
        inDoc = False
        inHeader = False

        sphinxDoc = "" # Disabled writing of sphinxDoc in cc files
        pythonDocStatements = self.doc.getPythonDocStatements()
        doxDoc = self.doc.getDoxygenDoc()

        # Write block content to output file
        if ((self._file is not None) and
            (self._deffile is not None) and
            (self._pydocfile is not None)):
            for line in self._lines:
                # In iterator blocks: replace iterator variable
                if (iter_var is not None):
                    line = iter_subst(iter_var, iter_val, line)
                    sphinxDoc = iter_subst(iter_var, iter_val, sphinxDoc)
                    pythonDocStatements = iter_subst(iter_var, iter_val, pythonDocStatements)
                    doxDoc = iter_subst(iter_var, iter_val, doxDoc)

                # Header block start
                m = re.match("\/\/\$COPY_TO HFILE START",line)
                if (m is not None):
                    if (self._deffile is not None):
                        inHeader = True
                        # Start ifdef-preproc statements
                        if (self._ifdef_list):
                            for ifdef in self._ifdef_list:
                                ifdef_statement = "#ifdef " + ifdef + "\n"
                                self._deffile.write(ifdef_statement)
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
                    self._file.write("#define PYDOCSTRING \"" + sphinxDoc + "\"\n")
                    self._deffile.write("#define PYDOCSTRING \"\"\n")
                    # Dump documentation
                    self._file.write("/*!\n" + doxDoc + "\n*/\n")
                    self._pydocfile.write(pythonDocStatements + "\n\n")
                    # Add separator in definition file
                    self._deffile.write("#include \"hfppnew-generatewrappers.def\"\n")
                    # End ifdef-preproc statements
                    if (self._ifdef_list):
                        self._ifdef_list.reverse()
                        for ifdef in self._ifdef_list:
                            endif_statement = "#endif /* " + ifdef + " */\n"
                            self._deffile.write(endif_statement)
                        self._ifdef_list.reverse()
                    self._deffile.write("\n")
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
        self._sectionType = ""
        self._docIndent = 0
        self._parameters = []
        self._description_lines = []
        self._usage_lines = []
        self._reference_list = []
        self._example_lines = []
        self._plotcode_lines = []
        self._dump_lines = []


    # ______________________________________________________________________
    #                                                    Check string syntax
    def checkSyntax(self, line_str):
        """
        Check and correct the syntax of a string in order not to
        conflict with python syntax.
        """
        result = line_str.rstrip()

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
    #                                                  Documentation parsing

    def parseDocLine(self,line):
        """
        Add a documentation line to the documentation section of the
        current sectionType.
        """
        if (not self.setSection(line)):
            sectionType = self.getSection()

            if (sectionType == "Description"):
                self.addDescription(line)
                return
            elif (sectionType == "Usage"):
                self.addUsage(line)
                return
            elif (sectionType == "Reference"):
                self.addReference(line)
                return
            elif (sectionType == "Example"):
                self.addExample(line)
                return
            elif (sectionType == "Plot"):
                self.addPlot(line)
                return


    def setSection(self, line):
        """
        Set the sectiontype within the documentation.
        """
        line_content = line.strip()

        if (line_content == "Description:"):
            self._sectionType = "Description"
            self._docIndent = len(line.rstrip()) - len(line_content)
            return True
        elif (line_content == "Usage:"):
            self._sectionType = "Usage"
            self._docIndent = len(line.rstrip()) - len(line_content)
            return True
        elif ((line_content == "See also:") or
              (line_content == "Reference:") or
              (line_content == "References:")):
            self._sectionType = "Reference"
            self._docIndent = len(line.rstrip()) - len(line_content)
            return True
        elif ((line_content == "Example:") or
              (line_content == "Examples:")):
            self._sectionType = "Example"
            self._docIndent = len(line.rstrip()) - len(line_content)
            return True
        elif ((line_content == "Plot:")):
            self._sectionType = "Plot"
            self._docIndent = len(line.rstrip()) - len(line_content)
            return True

        return False


    def formatSectionTitle(self, title):
        """
        Get the documentation section title in a Sphinx parsable format.
        """
        result = ""

        result += "**" + title.strip() + "**" + r"\n"

        return result


    def getSection(self):
        """
        Return the current documentation sectionType.
        """
        return self._sectionType


    # ______________________________________________________________________
    #                                                   Name of the function
    def setName(self, name="[empty]"):
        """
        Set the name of the block.
        """

        name_checked = self.checkSyntax(name)

        self._name = name_checked


    def getName(self):
        """
        Return the name attribute.
        """
        return self._name


    def formatSyntax(self):
        """
        Return the syntax of the command in a Sphinx parsable format.
        """
        result = "**"

        result += self.getName()

        parameters = self.getParameters()
        result += "("
        if parameters:
            for i in range(len(parameters)):
                par_name = parameters[i][0]
                result += par_name
                if (i < len(parameters)-1):
                    result += ", "
        result += ")**"

        return result


    # ______________________________________________________________________
    #                                             Parameters of the function
    def addParameter(self, name, description):
        """
        Add a parameter description.
        """
        name_checked = self.checkSyntax(name)
        description_checked = self.checkSyntax(description)

        self._parameters.append([name_checked, description_checked])


    def formatParameters(self):
        """
        Put the parameter list in a parsable Sphinx table format.
        """
        result = ""
        indent = 2
        name_width = 0
        desc_width = 0

        for par in self._parameters:
            name_width = max(len(par[0]), name_width)
            desc_width = max(len(par[1]), desc_width)

        table_hline = " " * indent + "=" * (name_width+3) + " " + "=" * (desc_width+2) + r"\n"

        result += table_hline

        for par in self._parameters:
            result += " " * indent + "*" + par[0] + "*" + " " * (name_width - len(par[0]) + 2) + par[1] + r"\n"

        result += table_hline

        return result


    def getParameters(self):
        """
        Return the parameter description.
        """
        return self._parameters


    # ______________________________________________________________________
    #                                                Summary of the function
    def setSummary(self, summary="[empty]"):
        """
        Set the summary of the block.
        """
        summary_checked = self.checkSyntax(summary)

        self._summary = summary_checked


    def formatSummary(self):
        """
        Put the function summary in a Sphinx parsable format.
        """
        return self.getSummary()


    def getSummary(self):
        """
        Return the summary attribute.
        """
        return self._summary


    # ______________________________________________________________________
    #                                            Description of the function
    def addDescription(self, description):
        """
        Add a description of the function.
        """
        line_content = description.rstrip()

        if (len(line_content) >= self._docIndent):
            line_content = line_content[self._docIndent:]

        self._description_lines.append(line_content)


    def formatDescription(self):
        """
        Put the function description in a Sphinx parsable format
        """
        result = ""

        for line in self.getDescription():
            result +=  line + r" \n"

        return result


    def getDescription(self):
        """
        Return the function description.
        """
        return self._description_lines


    # ______________________________________________________________________
    #                                                  Usage of the function
    def addUsage(self, usage):
        """
        Add usage description of the function.
        """

        if (len(usage.strip()) > 0):
            line_content = usage.rstrip()

            if (len(line_content) >= self._docIndent):
                line_content = line_content[self._docIndent:]

            self._usage_lines.append(line_content)


    def formatUsage(self):
        """
        Put the function usage in a Sphinx parsable format.
        """
        result = ""

        for line in self.getUsage():
            result += r"``" + line + r"``\n\n"

        return result


    def getUsage(self):
        """
        Return the function usage.
        """
        return self._usage_lines


    # ______________________________________________________________________
    #                                          References to other functions
    def addReference(self, reference):
        """
        Add a reference to another function.
        """
        reference_line = reference.strip()
        reference_line = reference_line.replace(".", " ")
        reference_line = reference_line.replace(",", " ")
        reference_functions = reference_line.strip().split(' ')

        for reference_function in reference_functions:
            if (len(reference_function) > 0):
                self._reference_list.append(reference_function)


    def formatReference(self):
        """
        Put the references in a Sphinx parsable format.
        """
        result = ""

        nReferences = len(self.getReference())
        for i in range(nReferences):
            result += ":func:`" + self._reference_list[i] + "`"
            if (i < nReferences - 1):
                result += ", "
            result += r"\n"

        return result


    def getReference(self):
        """
        Return the refernces to other functions.
        """
        return self._reference_list


    # ______________________________________________________________________
    #                                  Example documentation of the function
    def addExample(self, example):
        """
        Add example.
        """
        example_checked = self.checkSyntax(example)

        self._example_lines.append(example_checked)


    def formatExample(self):
        """
        Put example documentation in a Sphinx parsable format.
        """
        result = ""
        indent = "    "

        for line in self.getExample():
            result += indent + line + r"\n"

        result += r"\n"

        return result


    def getExample(self):
        """
        Return example.
        """
        return self._example_lines


    # ______________________________________________________________________
    #                                           Example plot of the function
    def addPlot(self, plotcode):
        """
        Add plot given a code generating the plot.
        """
#        plotcode_checked = self.checkSyntax(plotcode)
        line_content = plotcode.rstrip()

        if (line_content >= self._docIndent):
            line_content = line_content[self._docIndent:]

        self._plotcode_lines.append(line_content)


    def formatPlot(self):
        """
        Put code to generate a plot in a Sphinx parsable format.
        """
        result = ""

        result += ".. plot::" + r"\n"
        result += "   :include-source:" + r"\n\n"

        for line in self.getPlot():
            result += "   " + line + r"\n"

        result += r"\n"

        return result


    def getPlot(self):
        """
        Return plot code.
        """
        return self._plotcode_lines



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
    #                                                           getPythonDoc
    def getPythonDocStatements(self):
        """
        Get Python documentation statement.
        """
        result = ""

        name = self.getName()
        doc = self.getSphinxDoc()
        newline = "\n"

        result += "try:" + newline
        result += "    from _hftools import " + name + newline
        result += "    docstring = \"\"\"" + doc + "\"\"\"" + newline
        result += "    if (" + name + ".__doc__):" + newline
        result += "        " + name + ".__doc__ += docstring" + newline
        result += "    else:" + newline
        result += "        " + name + ".__doc__ = docstring" + newline
        result += "    __all__.append(\'" + name + "\')" + newline
        result += "except ImportError:" + newline
        result += "    pass" + newline

        return result


    # ______________________________________________________________________
    #                                                 getSphinxDocumentation
    def getSphinxDoc(self):
        """
        Get a python formatted docstring.
        """
        result = ""

        # Function name + parameters
        result += self.formatSyntax() + r"\n"

        # Brief function description (summary)
        result += r"\n" + self.formatSummary() + r"\n"

        # Parameter description
        if self.getParameters():
            result += r"\n" + self.formatSectionTitle("Parameters") + r"\n"
            result += self.formatParameters() + r"\n\n"

        # Description
        if self.getDescription():
            result += r"\n" + self.formatSectionTitle("Description") + r"\n"
            result += self.formatDescription() + r"\n"

        # Usage
        if self.getUsage():
            result += r"\n" + self.formatSectionTitle("Usage") + r"\n"
            result += self.formatUsage() + r"\n"

        # References
        if self.getReference():
            result += r"\n" + self.formatSectionTitle("See also") + r"\n"
            result += self.formatReference() + r"\n"

        # Examples
        if self.getExample():
            result += r"\n" + self.formatSectionTitle("Example") + r"::\n\n"
            result += self.formatExample() + r"\n"

        # Plot
        if self.getPlot():
            result += r"\n" + self.formatSectionTitle("Example") + r"\n"
            result += self.formatPlot() + r"\n"

        return result


    # ______________________________________________________________________
    #                                                getDoxygenDocumentation
    def getDoxygenDoc(self):
        """
        Get a Doxygen parsable string.
        """
        result = ""

        # Function name
        result += self.getName() + "("
        parameters = self.getParameters()
        if parameters:
            for i in range(len(parameters)):
                par_name = parameters[i][0]
                result += par_name
                if (i < len(parameters)-1):
                    result += ", "
        result += ")\n\n"

        # Brief summary
        result += "\\brief %s\n\n" %(self.getSummary())

        # Parameter description
        for par in self.getParameters():
            result += "\\param %s -- %s\n" %(par[0], par[1])

        # Examples
        result += "\n"
        if self.getExample():
            for line in self.getExample():
                result += line + "\n"

        return result



## =============================================================================
##
##  Function definitions
##
## =============================================================================

## _____________________________________________________________________________
##                                                      Parser support functions

## ________________________________________________________________________
##                                             Substitute iterator variable
def iter_subst(var, subst, input_string):
    """
    Replace a variable by a name

    ============== ========================================
    *var*          Variable to be substituted.
    *repl*         Substitution string.
    *input_string* Source string in which to substitute.
    ============== ========================================

    """

    result = input_string
    subst_caps = subst[0].capitalize() + subst[1:]
    subst_low = subst.lower()

    result = re.sub("\{\$" + var + "\!CAPS\}|\$" + var + "\!CAPS", subst_caps, result)
    result = re.sub("\{\$" + var + "\!LOW\}|\$" + var + "\!LOW", subst_low, result)
    result = re.sub("\{\$" + var +"\}|\$" + var, subst, result)

    return result


## _____________________________________________________________________________
##                                                                        Parser

## ________________________________________________________________________
##                                                           Parser options
def parserOptions():
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
def parseFile(input_filename, output_filename, options):
    """
    Parse a code file.
    """

    iterator_block = None
    wrapper_block = None
    ifdef_list = []

    header_rule = "="*80
    header_text = "ATTENTION: DON'T EDIT THIS FILE!!! IT IS GENERATED AUTOMATICALLY BY crtools_code_parser.py"

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
    if (output_file):
        output_file.write("// " + header_text + "\n")
        output_file.write("// " + header_rule + "\n")
    if (options.verbose):
        print "Output file : %s" %(os.path.relpath(output_filename))

    # Definitions file
    def_file = None

    # PyDoc file
    pydoc_filename = output_path + "/" + output_basename + ".py"
    pydoc_file = open(pydoc_filename, 'w')
    if (pydoc_file):
        pydoc_file.write("# " + header_text + "\n")
        pydoc_file.write("# " + header_rule + "\n\n")
        pydoc_file.write("__all__ = []\n\n")
    if (options.verbose):
        print "Python doc file  : %s" %(os.path.relpath(pydoc_filename))

    # Parse input file
    for line in input_file:
        # Check definition file (open if filename is set)
        m = re.match("^\/\/\$FILENAME: HFILE=(.*)", line)
        if (m and (not def_file)):
            def_filename = m.group(1) + ".latest"
            def_file = open(def_filename, 'w')
            if (options.verbose):
                print "Definition file: %s" %(def_filename)
            if (def_file):
                def_file.write("// " + header_text + "\n")
                def_file.write("// " + header_rule + "\n\n")
            continue

        # Check ifdef
        m = re.match("^#if(.*)", line)
        if (m):
            ifdef_list.append(m.group(1))

        # Check endif
        m = re.match("^#endif", line)
        if (m):
            ifdef_list.pop()

        # Check docstring
        m = re.match('^\/\/\$DOCSTRING: (.*)',line)
        if (m):
            docstring = m.group(1)
            continue

        # Check iterator block (start)
        m = re.match("^\/\/\$ITERATE (\w*) ([A-Za-z0-9,]*)", line)
        if (m):
            iterator_block = IteratorBlock(options)         # Create iterator block
            iterator_block.setIteratorVar(m.group(1))       # Set iterator variable name
            iterator_block.setFunctionList(m.group(2))      # Fill list of functions to iterate
            continue

        # Check iterator block (end)
        m = re.match("^\/\/\$ENDITERATE", line)
        if (m):
            if (iterator_block):
                iterator_block.parse()                      # Process iterator block
                iterator_block = None                       # Reset iterator_block
            else:
                raise ParserError("End iterate without begin iterate.")
            continue

        # Check wrapper block (start)
        m = re.match("^\/\/\$COPY_TO HFILE START", line)
        if (m):
            wrapper_block = WrapperBlock(output_file, def_file, pydoc_file, ifdef_list, options)  # Create block
            wrapper_block.addLine(line)
            wrapper_block.doc.setSummary(docstring)         # Set document summary
            continue

        # Check wrapper block (end)
        m = re.match("^\/\/\$COPY_TO HFILE: \#include \"hfppnew-generatewrappers\.def\"", line)
        if (m):
            if (wrapper_block):
                wrapper_block.addLine("#include \"hfppnew-generatewrappers.def\"")
                if (iterator_block):
                    iterator_block.addWrapperBlock(wrapper_block)  # Add wrapper block to iterator
                else:
                    wrapper_block.parse()                   # Process wrapper block
            else:
                raise ParserError("End wrapper block without begin.")

            wrapper_block = None                            # Reset wrapper block
            continue

        if (wrapper_block):
            wrapper_block.addLine(line)
        else:
            output_file.write(line)

    # Check if all blocks are closed
    if (wrapper_block):
        raise ParserError("Wrapper block is not closed at end of file.")
    if (iterator_block):
        raise ParserError("Iterator block is not closed at end of file.")

    # Close definition file
    if (def_file):
        def_file.close()

    # Close python documentation file
    if (pydoc_file):
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
    (options, args) = parserOptions()

    input_filename  = args[0]
    output_filename = args[1]

    parseFile(input_filename, output_filename, options)


if __name__ == '__main__':
    main()

