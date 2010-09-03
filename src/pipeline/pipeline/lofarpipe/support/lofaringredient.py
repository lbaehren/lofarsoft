#                                                       LOFAR PIPELINE FRAMEWORK
#
#                                                                    Ingredients
#                                                         John Swinbank, 2009-10
#                                                      swinbank@transientskp.org
# ------------------------------------------------------------------------------

import os
from optparse import make_option

from lofarpipe.cuisine.ingredient import WSRTingredient
from lofarpipe.support.utilities import string_to_list, is_iterable

class LOFARinput(WSRTingredient):
    """
    All LOFAR pipeline ingredients are required to provide a few basic
    parameters:

    * job_name
    * runtime_directory
    * config
    * task_files
    * dry_run
    * start_time
    """
    def __init__(self, defaults):
        super(LOFARinput, self).__init__(self)
        for param in (
            "job_name", "runtime_directory", "config", "task_files", "dry_run", "start_time"
        ):
            if defaults.has_key(param):
                self[param] = defaults[param]
            else:
                self[param] = None

class LOFARoutput(WSRTingredient):
    """
    LOFARoutput makes no changes to WSRTingredient.
    """
    pass

class Field(object):
    def __init__(self, *opts, **attrs):
        self.optionstrings = opts
        if attrs.has_key("help"):
            self.help = attrs['help']
        else:
            self.help = ""
        if attrs.has_key("default"):
            self.default = attrs['default']

    def is_valid(self, value):
        raise NotImplementedError

    def convert(self, value):
        """
        Try to convert value into the appropriate type for this sort of field.
        """
        return value

    def generate_option(self, name):
        return make_option(help=self.help, dest=name, *self.optionstrings)

class StringField(Field):
    def is_valid(self, value):
        return isinstance(value, str)

class IntField(Field):
    def is_valid(self, value):
        return isinstance(value, int)

class FileField(Field):
    def is_valid(self, value):
        return os.path.exists(value)

class BoolField(Field):
    def is_valid(self, value):
        return isinstance(value, bool)

    def convert(self, value):
        if value == "False" or value == "None" or value == "":
            return False
        elif value == "True":
            return True
        else:
            return value

class ListField(Field):
    def is_valid(self, value):
        return not isinstance(value, str) and is_iterable(obj)

    def convert(self, value):
        if isinstance(value, str):
            return string_to_list(value)
        else:
            return value

class FileList(ListField):
    def is_valid(self, value):
        if super(FileList, self).convert(value) and \
        not False in [os.path.exists(file) for file in value]:
            return True
        else:
            return False

class LOFARingredient(dict):
    def __init__(self, fields):
        self._fields = fields

        for key, value in self._fields.iteritems():
            if hasattr(value, "default"):
                self[key] = value.default

    def __setitem__(self, key, value):
        if key in self._fields:
            field = self._fields[key]
            converted_value = field.convert(value)
            if not field.is_valid(converted_value):
                raise TypeError("%s has invalid type for %s" % (str(value), key))
        else:
            raise TypeError("Input %s not defined" % key)
        super(LOFARingredient, self).__setitem__(key, converted_value)

    def make_options(self):
        return [value.generate_option(key) for key, value in self._fields.iteritems()]

    def complete(self):
        return not False in [self.has_key(key) for key in self._fields.iterkeys()]

class RecipeMeta(type):
    def __init__(cls, name, bases, ns):
        if not hasattr(cls, "_fields"):
            cls._fields = {}

        for key, value in ns.iteritems():
            if issubclass(type(value), Field):
                cls._fields[key] = value

class BaseIngredients(object):
    __metaclass__ = RecipeMeta

    job_name = StringField('-j', '--job-name', help="Job name")
    runtime_directory = FileField('-r', '--runtime-directory', help="Runtime directory")
    config = FileField('-c', '--config', help="Configuration file")
    task_files = FileList('--task-file', help="Task definition file")
    start_time = StringField('--start-time', help="[Expert use] Pipeline start time")
    dry_run = BoolField('-n', '--dry-run', help="Dry run", default=False)
    args = ListField('--args', help="Args", default=[])

    def __init__(self):
        super(BaseIngredients, self).__init__()
        self.inputs = LOFARingredient(self._fields) # Must run *after* WSRTrecipe.__init__()
