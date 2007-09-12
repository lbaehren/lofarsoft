"""Module: template

Substrings of the form <identifier> are replaced by %(identifier)s
during calls to "dirty()".  Calls to "clean()" do the reverse, replacing
%(var)s with <var>.

sugar_dict is typically called on the dictionary of a module
(globals()) and mutates the module's global strings and dictionaries.

Template declares a string template which does iterative variable
substitution when str() is called.  In a Template, both the initial string,
and the variable values can contain variables.  Prior to instantiation of
a template, all variable values must be defined in the templates dictionary.

"""

import re

def clean(f):
    """replaces instances of %(var)s with <var>"""
    s = open(f).read()
    return re.sub(r"%\((\w+)\)s", r"<\1>", s)

def dirty(s):
    """replaces instances of <var> with %(var)s"""
    return re.sub(r"<(\w+)>", r"%(\1)s", s)

def sugar_dict(d):
    """sugar_dict does <var> to %(var)s translation on all values
    in a dictionary.
    """
    for k in d.keys():
        if type(d[k]) == type(""):  # handle strings
            d[k] = dirty(d[k])
        elif type(d[k]) == type(d): # handle dicts
            sugar_dict(d[k])

class Template:
    def __init__(self, pattern, **values):
        self._pattern = pattern
        self.__dict__.update(values)

    def __str__(self):
        """
        Instantiation of a template iterates until the resulting
        string no longer changes.

        Prior to instantiation, all template variables must be defined.
        """
        sugar_dict(self.__dict__)
        s = self._pattern
        i = 0
        while 1:
            old_s = s
            s = s % self.__dict__
            if old_s == s:
                break
        s = re.sub("@mod@", "%", s) 
        return s



