#! /usr/bin/env python

##########################################################################
#  This file is part of the Transient Template Library.                  #
#  Copyright (C) 2010 Pim Schellart <P.Schellart@astro.ru.nl>            #
#                                                                        #
#  This library is free software: you can redistribute it and/or modify  #
#  it under the terms of the GNU General Public License as published by  #
#  the Free Software Foundation, either version 3 of the License, or     #
#  (at your option) any later version.                                   #
#                                                                        # 
#  This library is distributed in the hope that it will be useful,       #
#  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
#  GNU General Public License for more details.                          #
#                                                                        #
#  You should have received a copy of the GNU General Public License     #
#  along with this library. If not, see <http://www.gnu.org/licenses/>.  #
##########################################################################

import re
import xml.dom.minidom
from optparse import OptionParser

def indent(incode, indent='', addindent='  '):
    """Indent code (default 2 spaces)."""

    outcode=""

    level = 0
    for line in code.split('\n'):
        if line.strip()=='':
            outcode+='\n'
        elif line.lstrip()[0]=='{':
            outcode+=indent+level*addindent+line+'\n'
            level+=1
        elif level>0 and line.lstrip()[0]=='}':
            level-=1
            outcode+=indent+level*addindent+line+'\n'
        else:
            outcode+=indent+level*addindent+line+'\n'

    return outcode

def parse_file(f):
    """Parse header file with template functions to XML DOM.
    """

    next = False
    function = None

    # Get empty DOM implementation
    implement = xml.dom.minidom.getDOMImplementation()
    
    # Create root node
    dom = implement.createDocument(None, "file", None)
    
    # Set current node
    current_node = dom.documentElement

    # Loop over file
    lines = f.readlines()

    while True:

        try:
            # Get current line minus whitespace
            line = lines.pop(0).strip()

            # Skip commented lines
            if line.strip()[:2]=='//':
                continue

            # Add namespace
            if re.match('namespace', line):
                # Create new module element
                element=dom.createElement('module')

                # Get name of namespace and add as attribute
                name=line.split()[1]

                element.setAttribute('name', name)

                # Add module to dom
                current_node = current_node.appendChild(element)

            elif re.match('template', line):
                # Next line is a function definition
                next = True

            elif re.search(r'\\brief', line):
                # We are now reading a Doxygen formatted comment block
                comment = ""
                arg_comment = {}

                # Read until end of comment block
                while not re.search(r'\*\/', line):
                    if re.search(r'\\param', line):
                        # Read a function argument docstring
                        temp = line.split()

                        # Strip '\param ' from argument docstring
                        c = line[line.find(temp[1])+len(temp[1]):]
                        arg_comment[temp[1]]=c.strip()
                    else:
                        # Add line to docstring of function itself
                        if re.search(r'\\brief', line):
                            # Strip '\brief ' from function docstring
                            comment += line[line.find('\brief')+7:].strip()+'\n'
                        else:
                            comment += line.rstrip()+'\n'

                    # To next line in file
                    line=lines.pop(0)

            elif next:
                # Next line is a function so create a new function element
                element=dom.createElement('function')

                # Get name of function (e.g. first word before '(')
                fname=line.split('(')[0].split()[-1]

                # Add function name to dom element as attribute
                element.setAttribute('name', fname)

                # Get function type (e.g. everything before fname)
                ftype=line[:line.find(fname)].strip()

                # Add function type to dom element as attribute
                element.setAttribute('type', ftype)

                # Add docstring if found
                if comment:
                    element.setAttribute('docstring', comment)
                else:
                    print 'Warning: function '+fname+' has no docstring'

                # Add function element to dom
                function = current_node.appendChild(element)

                # Next line is no longer part of the function definition
                next = False

                # Get function arguments
                arguments = ''
                while True:
                    arguments += line

                    # Check for end of function
                    if re.search('\)', line):
                        break
                    else:
                        line = lines.pop(0).strip()

                # Get all arguments between '()' separated by ',' in a list
                arguments = arguments[arguments.find('(')+1:arguments.find(')')].split(',')

                # Loop over arguments in list and create argument dom nodes
                for arg in arguments:
                    arg=arg.strip()

                    if arg != "":
                        # Create new argument element
                        element=dom.createElement('argument')

                        # Get argument name e.g. 'a' from 'int a'
                        name = arg[arg.rfind(' '):].strip()
                        element.setAttribute('name', name)

                        # Get argument type e.g. 'int' from 'int a'
                        type = arg[:arg.rfind(' ')].strip()
                        element.setAttribute('type', type)

                        # If argument has a doctring add it to the node
                        if name in arg_comment:
                            docstring = arg_comment[name]
                            element.setAttribute('docstring', docstring)
                        else:
                            print 'Warning: argument '+name+' of function '+fname+' has no docstring.'

                        # Add argument node to dom as child of function node
                        function.appendChild(element)

        except IndexError:
            # Last line of file reached so break out of while loop
            break

    return dom

def elementToCode(node, namespace=''):
    """Recursively move through all child elements of *node* and output C++ wrapper code."""

    code = ""

    # Check for namespace
    if node.nodeName == 'module':
        namespace+=node.getAttribute('name')+'::'
    elif node.nodeName == 'function':
        # Add function name and type
        fname = node.getAttribute('name')
        ftype = node.getAttribute('type')

        # Correct for template type T
        ftype=re.sub('T', 'double', ftype)

        # Add start of function definition (e.g. '  void f(')
        code+=ftype+' '+fname+'('
        
        # Add function arguments
        if node.hasChildNodes():
            # Make copy of list to prevent removing elements from the DOM
            arguments = node.childNodes[:]

            while True:
                try:
                    # Get next argument
                    arg = arguments.pop(0)

                # Go out of loop if there are no more arguments
                except IndexError:
                    break

                # Get argument name and type
                type=arg.getAttribute('type')
                name=arg.getAttribute('name')

                # Check the next argument in the list if it exists
                nextarg=arg.nextSibling

                # First check if we have a begin/end iterator pair
                if nextarg and re.search(name,nextarg.getAttribute('name')) and re.search('Iter',nextarg.getAttribute('type')):
                    code+='boost::python::numeric::array '+name.strip()

                    # Add ',' between parameters
                    if len(arguments)>1:
                        code+=', '

                    # Go to end iterator
                    try:
                        arg = arguments.pop(0)

                    # Go out of loop if there are no more arguments
                    except IndexError:
                        break

                # Then check if we have a single begin iterator
                elif re.search('Iter', type):
                    code+='boost::python::numeric::array '+name.strip()

                    # Add ',' between parameters
                    if len(arguments)>=1:
                        code+=', '

                # Otherwise it is a normal parameter (may be templated)
                else:
                    # Correct for template type T
                    type=re.sub('T', 'double', type)

                    code+=type+' '+name

                    # Add ',' between parameters
                    if len(arguments)>=1:
                        code+=', '

        # Add function opening
        code+=')'+'\n'+'{\n'

        # Add TTL function call
        if ftype == 'void':
            code+=namespace+fname+'('
        else:
            code+='return '+namespace+fname+'('

        # Add TTL function call arguments
        if node.hasChildNodes():
            # Make copy of list to prevent removing elements from the DOM
            arguments = node.childNodes[:]

            while True:
                try:
                    # Get next argument in list
                    arg = arguments.pop(0)

                # Go out of loop if there are no more arguments
                except IndexError:
                    break

                # Get argument name and type
                type=arg.getAttribute('type')
                name=arg.getAttribute('name')

                # Check next argument if it exists
                nextarg=arg.nextSibling

                # First check if we have a begin/end iterator pair
                if nextarg and re.search(name,nextarg.getAttribute('name')) and re.search('Iter',nextarg.getAttribute('type')):
                    # Wrap input numpy ndarray with functions to get
                    # the begin and end iterators
                    if re.search('CIter', type):
                        code+='ttl::numpyBeginPtr< std::complex<double> >('+name.strip()+'), ttl::numpyEndPtr< std::complex<double> >('+name.strip()+')'
                    else:
                        code+='ttl::numpyBeginPtr<double>('+name.strip()+'), ttl::numpyEndPtr<double>('+name.strip()+')'

                    # Add ',' between parameters
                    if len(arguments)>1:
                        code+=', '

                    # Go to end iterator
                    try:
                        arg = arguments.pop(0)

                    # Go out of loop if there are no more arguments
                    except IndexError:
                        break

                # Then check if we have a single begin iterator
                elif re.search('Iter', type):
                    # Wrap input numpy ndarray with functions to get
                    # the begin iterator
                    if re.search('CIter', type):
                        code+='ttl::numpyBeginPtr< std::complex<double> >('+name.strip()+')'
                    else:
                        code+='ttl::numpyBeginPtr<double>('+name.strip()+')'

                    # Add ',' between parameters
                    if len(arguments)>=1:
                        code+=', '

                # Otherwise it is a normal parameter (may be templated)
                else:
                    # Correct for reference
                    name=re.sub('\&', '', name)

                    # Add function name
                    code+=name

                    # Add ',' between parameters
                    if len(arguments)>=1:
                        code+=', '
        
        # Add TTL function closing
        code+=')'+';\n'

        # Add function closing
        code+='}\n\n'

    # Recursively move through the list
    if node.hasChildNodes():
        for e in node.childNodes:
            temp, namespace = elementToCode(e, namespace)
            code += temp

    return code, namespace

def elementToBoostWrapper(node, module=''):
    """Recursively move through all child elements of *node* and output Boost Python wrapper wrap."""

    wrap = ""

    # Check for namespace
    if node.nodeName == 'module' and node.getAttribute('name') != 'ttl':
        # Add module name
        wrap += 'BOOST_PYTHON_MODULE('+node.getAttribute('name')+')\n{\n'
    
        # Add num_util directives
        wrap += 'import_array();\n'
        wrap += 'boost::python::numeric::array::set_module_and_type("numpy", "ndarray");\n\n'
    
        # Add Boost python namespace
        wrap += 'using namespace boost::python;\n\n'
  
    elif node.nodeName == 'function':
        # Add function wrapper with correct namespace
        fname = node.getAttribute('name')

        wrap+='def("'+fname+'", ttl::bindings::'+fname

        # Get function docstring
        docstring = node.getAttribute('docstring').strip()+'\n\n'

        # Get argument docstrings
        if node.hasChildNodes():
            # Make copy of list to prevent removing elements from the DOM
            arguments = node.childNodes[:]

            for arg in arguments:
                # Construct docstring for argument
                docstring+='*'+arg.getAttribute('name')+'* '+arg.getAttribute('docstring').strip()+'\n'

        # Replace newline by literal '\n' in code
        docstring = re.sub('\n',r'\\n', docstring)

        # Add docstring to wrapper function
        wrap+=', "'+docstring+'"'
        
        # Add closing
        wrap+=');\n'
    
    # Recursively move through the list
    if node.hasChildNodes():
        for e in node.childNodes:
            temp, module = elementToBoostWrapper(e, module)
            wrap += temp

    if node.nodeName == 'module' and node.getAttribute('name') != 'ttl':
        # Add module closing
        wrap += '}\n'

    return wrap, module

def getModule(node):
    """Recursively move through all child elements of *node* and output module name."""

    # Check for namespace
    if node.nodeName == 'module' and node.getAttribute('name') != 'ttl':
        return node.getAttribute('name')
    elif node.hasChildNodes():
        for e in node.childNodes:
            module = getModule(e)
            if module:
                return module
    else:
        return None

# Parse commandline arguments
parser=OptionParser(usage="usage: %prog [options] template.h", version="%prog 1.0")
parser.add_option("-v", "--verbose",
                  action="store_true", dest="verbose", default=False,
                  help="verbose output")
parser.add_option("-x", "--output-xml",
                  action="store_true", dest="xml", default=False,
                  help="save xml output for analysis")
parser.add_option("-d", "--output-directory",
                  action="store", dest="outdir", default="",
                  help="output directory")
(options, args)=parser.parse_args()

if not args:
    parser.print_help()
    sys.exit(0)

# Open input file
f = open(args[0])

# Parse input file into DOM tree
dom = parse_file(f)

# Write output file
out = open(options.outdir+'/'+args[0].split('/')[-1].rstrip('h')+'cc', 'w')

# Write license
license = """/**************************************************************************
 *  This file is part of the Transient Template Library.                  *
 *  Copyright (C) 2010 Pim Schellart <P.Schellart@astro.ru.nl>            *
 *                                                                        *
 *  This library is free software: you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation, either version 3 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        * 
 *  This library is distributed in the hope that it will be useful,       *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *  GNU General Public License for more details.                          *
 *                                                                        *
 *  You should have received a copy of the GNU General Public License     *
 *  along with this library. If not, see <http://www.gnu.org/licenses/>.  *
 **************************************************************************/

"""

out.write(license)

# Write includes
include =  ""

include += "// SYSTEM INCLUDES\n"
include += "//\n\n"

include += "// PROJECT INCLUDES\n"
include += "#include <ttl/"+getModule(dom)+".h>\n\n"

include += "// LOCAL INCLUDES\n"
include += "#define PY_ARRAY_UNIQUE_SYMBOL PyArrayHandle\n"
include += '#include "num_util.h"\n'

include += '#include "numpy_ptr.h"\n\n'

include += "// FORWARD REFERENCES\n"
include += "//\n\n"

out.write(include)

if options.xml:
    # Write DOM to file
    xml = open(options.outdir+'/'+args[0].split('/')[-1].rstrip('h')+'xml', 'w')
    dom.writexml(xml, indent="  ", addindent="  ", newl="\n")

# Add code
code=""

# Open namespace
code+='namespace ttl\n{\n'+'namespace bindings\n{\n\n'

# Write code
code+=elementToCode(dom)[0]

# Close namespace
code+='} // End bindings\n} // End ttl\n\n'

# Write wrappers
code+=elementToBoostWrapper(dom)[0]

# Indent code and write to file
out.write(indent(code))

