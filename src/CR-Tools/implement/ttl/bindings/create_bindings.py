#! /usr/bin/env python

import re
import xml.dom.minidom
from optparse import OptionParser

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

                name=line.split()[1]

                element.setAttribute('name', name)

                current_node = current_node.appendChild(element)

            elif re.match('template', line):
                next = True

            elif re.search(r'\\brief', line):

                comment = ""
                arg_comment = {}

                while not re.search(r'\*\/', line):
                    if re.search(r'\\param', line):
                        temp = line.split()

                        c = line[line.find(temp[1])+len(temp[1]):]
                        arg_comment[temp[1]]=c.strip()
                    else:
                        if re.search(r'\\brief', line):
                            comment += line[line.find('\brief')+7:].strip()+'\n'
                        else:
                            comment += line.rstrip()+'\n'

                    line=lines.pop(0)

            elif next:
                # Create new function element
                element=dom.createElement('function')

                fname=line.split('(')[0].split()[-1]

                element.setAttribute('name', fname)

                ftype=line[:line.find(fname)].strip()

                element.setAttribute('type', ftype)

                # Add docstring if found
                if comment:
                    element.setAttribute('docstring', comment)
                else:
                    print 'Warning: function '+fname+' has no docstring'

                function = current_node.appendChild(element)

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

                arguments = arguments[arguments.find('(')+1:arguments.find(')')].split(',')

                for arg in arguments:
                    arg=arg.strip()

                    if arg != "":
                        # Create new argument element
                        element=dom.createElement('argument')

                        name = arg[arg.rfind(' '):].strip()
                        element.setAttribute('name', name)

                        type = arg[:arg.rfind(' ')].strip()
                        element.setAttribute('type', type)

                        if name in arg_comment:
                            docstring = arg_comment[name]
                            element.setAttribute('docstring', docstring)
                        else:
                            print 'Warning: argument '+name+' of function '+fname+' has no docstring.'

                        function.appendChild(element)

        except IndexError:
            break

    return dom

def elementToCode(node, namespace='', indent='  ', addindent='  '):
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

        code+=indent+ftype+' '+fname+'('
        
        # Add function arguments
        if node.hasChildNodes():
            arguments = node.childNodes[:]

            skip = False
            iter_param = 0
            while True:
                try:
                    arg = arguments.pop(0)

                # Go out of loop if there are no more arguments
                except IndexError:
                    break

                type=arg.getAttribute('type')
                name=arg.getAttribute('name')

                nextarg=arg.nextSibling

                # First check if we have a begin/end itterator pair
                if nextarg and re.search(name,nextarg.getAttribute('name')) and re.search('Iter',nextarg.getAttribute('type')):
                    code+='boost::python::numeric::array pydata'+str(iter_param)
                    iter_param+=1

                    # Add ',' between parameters
                    if len(arguments)>1:
                        code+=', '

                    # Go to end itterator
                    try:
                        arg = arguments.pop(0)

                    # Go out of loop if there are no more arguments
                    except IndexError:
                        break

                # Then check if we have a single begin itterator
                elif re.search('Iter', type):
                    code+='boost::python::numeric::array pydata'+str(iter_param)
                    iter_param+=1

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
        code+=')'+'\n'+indent+'{\n'

        # Add TTL function call
        if ftype == 'void':
            code+=indent+addindent+namespace+fname+'('
        else:
            code+=indent+addindent+'return '+namespace+fname+'('

        # Add TTL function call arguments
        if node.hasChildNodes():
            arguments = node.childNodes[:]

            skip = False
            iter_param = 0
            while True:
                try:
                    arg = arguments.pop(0)

                # Go out of loop if there are no more arguments
                except IndexError:
                    break

                type=arg.getAttribute('type')
                name=arg.getAttribute('name')

                nextarg=arg.nextSibling

                # First check if we have a begin/end itterator pair
                if nextarg and re.search(name,nextarg.getAttribute('name')) and re.search('Iter',nextarg.getAttribute('type')):
                    code+='ttl::numpyBeginPtr<double>(pydata'+str(iter_param)+'), ttl::numpyEndPtr<double>(pydata'+str(iter_param)+')'
                    iter_param+=1

                    # Add ',' between parameters
                    if len(arguments)>1:
                        code+=', '

                    # Go to end itterator
                    try:
                        arg = arguments.pop(0)

                    # Go out of loop if there are no more arguments
                    except IndexError:
                        break

                # Then check if we have a single begin itterator
                elif re.search('Iter', type):
                    code+='ttl::numpyBeginPtr<double>(pydata'+str(iter_param)+')'
                    iter_param+=1

                    # Add ',' between parameters
                    if len(arguments)>=1:
                        code+=', '

                # Otherwise it is a normal parameter (may be templated)
                else:
                    # Correct for reference
                    name=re.sub('\&', '', name)

                    code+=name

                    # Add ',' between parameters
                    if len(arguments)>=1:
                        code+=', '
        
        # Add TTL function closing
        code+=')'+';\n'

        # Add function closing
        code+=indent+'}\n\n'

    # Recursively move through the list
    if node.hasChildNodes():
        for e in node.childNodes:
            temp, namespace = elementToCode(e, namespace)
            code += temp

    return code, namespace

def elementToBoostWrapper(node, module=''):
    """Recursively move through all child elements of *node* and output Boost Python wrapper wrap."""

    indent = '  '
    wrap = ""

    # Check for namespace
    if node.nodeName == 'module' and node.getAttribute('name') != 'ttl':
        # Add module name
        wrap += 'BOOST_PYTHON_MODULE('+node.getAttribute('name')+')\n{\n'
    
        # Add num_util directives
        wrap += indent+'import_array();\n'
        wrap += indent+'boost::python::numeric::array::set_module_and_type("numpy", "ndarray");\n\n'
    
        # Add Boost python namespace
        wrap += indent+'using namespace boost::python;\n\n'
  
    elif node.nodeName == 'function':
        # Add function name
        fname = node.getAttribute('name')

        wrap+=indent+'def("'+fname+'", ttl::bindings::'+fname

        # Get function docstring
        docstring = node.getAttribute('docstring').strip()+'\n\n'

        # Get argument docstrings
        if node.hasChildNodes():
            arguments = node.childNodes[:]

            for arg in arguments:
                docstring+=indent+'*'+arg.getAttribute('name')+'* '+arg.getAttribute('docstring').strip()+'\n'

        # Replace newline by litteral '\n' in code
        docstring = re.sub('\n',r'\\n', docstring)

        wrap+=', "'+docstring+'"'
        
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

# Open namespace
out.write("namespace ttl\n{\n  namespace bindings\n  {\n\n")

# Write code
out.write(elementToCode(dom, indent='    ')[0])

# Close namespace
out.write("  }// End bindings\n} // End ttl\n\n")

# Write wrappers
out.write(elementToBoostWrapper(dom)[0])

