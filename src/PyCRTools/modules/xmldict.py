"""Module to dump and load Python dictionaries to and from XML.
"""

import pickle
import xml.dom.minidom

def flatten_result(f):
    """If return value is iterable and has only one element, just return
    this element. Otherwise return the iterable.

    Intended to be used as a decorator.
    Example::

        @flatten_result
        def f(i):
            return range(i)

    returns only a single value if range has one number or list otherwise.

    """

    def inner_func(*args, **kwargs):
        res = f(*args, **kwargs)

        if hasattr(res, '__iter__') and len(res) == 1:
            res = res[0]

        return res

    return inner_func

def dump(filename, *args, **kwargs):
    """Save a series of Python dictionary objects to XML file.

    Dictionaries provided as keyword arguments have their key stored as
    table header in the resulting XML file.
    """

    # Generate a empty DOM
    implement=xml.dom.minidom.getDOMImplementation()

    dom=implement.createDocument(None, "shelve", None)

    def add_dict(obj, dom, name = None):
        """Helper function to add dict to DOM.
        """
        
        dct = dom.createElement("dict")

        if name:
            n = dom.createElement("name")

            content = dom.createTextNode(name)

            n.appendChild(content)

            dct.appendChild(n)

        # Loop over items in dictionary and create human readable key, value
        # representation
        for k, v in obj.iteritems():
            
            # Create item
            parent=dom.createElement("item")

            # Add key
            element=dom.createElement("key")

            content=dom.createTextNode(k)

            element.appendChild(content)

            parent.appendChild(element)

            # Add value
            element=dom.createElement("value")

            content=dom.createTextNode(str(v))

            element.appendChild(content)

            parent.appendChild(element)

            # Add item to tree
            dct.appendChild(parent)

        # Add the same dict in binary format for pickling
        binary = dom.createElement("binary")

        comment = dom.createComment("Binary representation of dictionary, can be easily unpickled back into Python.")

        binary.appendChild(comment)

        cdata = dom.createCDATASection(pickle.dumps(obj))

        binary.appendChild(cdata)

        dct.appendChild(binary)

        dom.documentElement.appendChild(dct)


    # Loop over dictionaries and create <dict> element for each one
    for obj in args:

        add_dict(obj, dom)

    # Loop over named dictionaries and create <dict> element for each one
    for name, obj in kwargs.iteritems():

        add_dict(obj, dom, name)

    # Add stylesheet information
    pi = dom.createProcessingInstruction('xml-stylesheet', 'type="text/css" href="shelve.css"')

    dom.insertBefore(pi, dom.firstChild)

    # Store to file
    if not filename.endswith(".xml"):
        filename += ".xml"

    with open(filename, 'w') as f:
        f.write(dom.toprettyxml(indent="    "))
    
@flatten_result
def load(filename):
    """Parses XML file containing Python dictionaries in binary
    representation and return any dictionaries found.

    Either returns single dictionary or list of dictionaries if multiple
    are found.
    """

    # Read DOM from file
    if not filename.endswith(".xml"):
        filename += ".xml"

    dom = xml.dom.minidom.parse(filename)

    # Find all dictionaries
    data = dom.getElementsByTagName("binary")

    # Find CDATA sections storing binary representation of dict and append
    # pickle parsed dict to output list
    results = []
    for d in data:
        node = d.firstChild
        while node.nodeType != node.CDATA_SECTION_NODE and node is not None:
            node = node.nextSibling

        if node is not None:
            results.append(pickle.loads(node.data.encode('ascii','ignore')))

    return results

