#                                                       LOFAR PIPELINE FRAMEWORK
#
#                                                                    Ingredients
#                                                         John Swinbank, 2009-10
#                                                      swinbank@transientskp.org
# ------------------------------------------------------------------------------

import os
from optparse import make_option
from UserDict import DictMixin

from lofarpipe.cuisine.ingredient import WSRTingredient
from lofarpipe.support.utilities import string_to_list, is_iterable

#       These are currently only used by lofarrecipe.run_task to provide default
#              input and output dicts based on copying metadata from the parent.
# ------------------------------------------------------------------------------
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

    These are defined in the RecipeIngredients class; all options (but not
    arguments) defined there are required.
    """
    def __init__(self, defaults):
        super(LOFARinput, self).__init__(self)
        for param in RecipeIngredients.inputs.iterkeys():
            if param != "args" and defaults.has_key(param):
                self[param] = defaults[param]

class LOFARoutput(WSRTingredient):
    """
    LOFARoutput makes no changes to WSRTingredient.
    """
    pass

class Field(object):
    """
    Fields provided validation and type checking of input/output.

    Unlimited user-defined fields are possible; they should all derive from
    this class.
    """
    def __init__(self, *opts, **attrs):
        self.optionstrings = opts
        if attrs.has_key("help"):
            self.help = attrs['help']
        else:
            self.help = ""
        if attrs.has_key("default"):
            self.default = attrs['default']
        elif attrs.has_key("optional") and attrs["optional"]:
            self.optional = True

    def is_valid(self, value):
        """
        Check whether ``value`` is a valid value for this field.

        This must be defined in subclasses.

        :param value: value to be checked
        :rtype: bool
        """
        raise NotImplementedError

    def coerce(self, value):
        """
        Try to convert value into the appropriate type for this sort of field.
        Results should be checked with ``is_valid()``.

        :param value: value to be coerced
        :rtype: coerced value
        """
        return value

    def generate_option(self, name):
        """
        Generated an :mod:`optparse` option.

        :param name: Destination
        :rtype: :class:`optparse.Option`
        """
        if hasattr(self, "default"):
            help = self.help + " [Default: %s]" % str(self.default)
        elif hasattr(self, "optional"):
            help = self.help + " [Optional]"
        else:
            help = self.help
        return make_option(help=help, dest=name, *self.optionstrings)

class StringField(Field):
    """
    A Field which accepts any string as its value.
    """
    def is_valid(self, value):
        return isinstance(value, str)

class IntField(Field):
    """
    A Field which accepts any int as its value.
    """
    def is_valid(self, value):
        return isinstance(value, int)

    def coerce(self, value):
        try:
            return int(value)
        except:
            return value

class FloatField(Field):
    """
    A Field which accepts any float as its value.
    """
    def is_valid(self, value):
        return isinstance(value, float)

    def coerce(self, value):
        try:
            return float(value)
        except:
            return value

class FileField(Field):
    """
    A Field which accepts the name of an extant file.
    """
    def is_valid(self, value):
        return os.path.exists(str(value))

class ExecField(Field):
    """
    A Field which accepts the name of an executable file.
    """
    def is_valid(self, value):
        return os.access(value, os.X_OK)

class DirectoryField(Field):
    """
    A Field which accepts the name of an extant directory.
    """
    def is_valid(self, value):
        return os.path.isdir(str(value))

    def coerce(self, value):
        try:
            os.makedirs(str(value))
        except:
            pass
        finally:
            return value

class BoolField(Field):
    """
    A Field which accepts a bool.
    """
    def is_valid(self, value):
        return isinstance(value, bool)

    def coerce(self, value):
        if value == "False" or value == "None" or value == "":
            return False
        elif value == "True":
            return True
        else:
            return value

class ListField(Field):
    """
    A Field which accepts a non-string iterable (ie, list or tuple).
    """
    def is_valid(self, value):
        return not isinstance(value, str) and is_iterable(value)

    def coerce(self, value):
        if isinstance(value, str):
            return string_to_list(value)
        else:
            return value

class DictField(Field):
    """
    A Field which accepts a dict.
    """
    def is_valid(self, value):
        return isinstance(value, dict)

class FileList(ListField):
    """
    A Field which accepts a list of extant filenames.
    """
    def is_valid(self, value):
        if super(FileList, self).is_valid(value) and \
        not False in [os.path.exists(file) for file in value]:
            return True
        else:
            return False

#                                            The magic that makes it all work...
#                      RecipeIngredients should be mixed in to any recipes which
#             need to deal with input or output. BaseRecipe already includes it,
#                                so that will almost always the case by default.
# ------------------------------------------------------------------------------

class LOFARingredient(DictMixin):
    """
    LOFARingredient provides dict-like access to a group of instances of
    :class:`Field`.  If a field is defined which does not have a value set,
    but which does have a default, that is returned.
    """
    def __init__(self, fields):
        self._fields = fields
        self._values = {}

    def __getitem__(self, key):
        # If we don't have the value for this key, but we do have a field with
        # a valid default, return that.
        if (
            not self._values.has_key(key) and
            self._fields.has_key(key) and
            hasattr(self._fields[key], "default")
        ):
            field = self._fields[key]
            value = field.coerce(field.default)
            if not field.is_valid(value):
                raise TypeError(
                    "%s is an invalid value for %s %s" %
                    (str(value), type(field).__name__, key)
                )
        elif self._values.has_key(key):
            value = self._values[key]
        else:
            raise KeyError(key)
        return value

    def __setitem__(self, key, value):
        if key in self._fields:
            field = self._fields[key]
            converted_value = field.coerce(value)
            if not field.is_valid(converted_value):
                raise TypeError(
                    "%s is an invalid value for %s %s" %
                    (str(value), type(field).__name__, key)
                )
            self._values[key] = converted_value
        else:
            raise TypeError("Ingredient %s not defined" % key)

    def keys(self):
        # We want to return a list of everything we have a value for. That's
        # everything in _values, plus things in _fields which have a default.
        return list(
            set(self._values.keys()).union(
                set(k for k, v in self._fields.items() if hasattr(v, "default"))
            )
        )

    def make_options(self):
        return [value.generate_option(key) for key, value in self._fields.iteritems()]

    def missing(self):
        return [
            key for key in self._fields.iterkeys()
            if not self._values.has_key(key)
            and not hasattr(self._fields[key], "optional")
            and not hasattr(self._fields[key], "default")
        ]

    def complete(self):
        return False if self.missing() else True

    def update(self, args, **kwargs):
        for key, value in args.iteritems():
            self._values[key] = value
        for key, value in kwargs.iteritems():
            self._values[key] = value

class RecipeIngredientsMeta(type):
    """
    This metaclass ensures that the appropriate instances of :class:`Field`
    are available in the inputs of every LOFAR recipe.
    """
    def __init__(cls, name, bases, ns):
        # Inputs are inherited from the superclass.
        # Need to do some gymnastics here, as we don't want to update the
        # superclass's _infields -- thus we replace it and copy the contents.
        new_inputs = {}
        if hasattr(cls, "_infields"):
            new_inputs.update(cls._infields)
        if ns.has_key("inputs"):
            new_inputs.update(ns["inputs"])
        cls._infields = new_inputs

        # Outputs are not inherited.
        if ns.has_key('outputs'):
            cls._outfields = ns['outputs']

class RecipeIngredients(object):
    """
    All LOFAR recipes ultimately inherit from this. It provides the basic
    ingredient structure, as well as the default fields which are available in
    every recipe.
    """
    __metaclass__ = RecipeIngredientsMeta

    inputs = {
        'job_name': StringField(
            '-j', '--job-name',
            help="Job name"
        ),
        'runtime_directory': FileField(
            '-r', '--runtime-directory',
            help="Runtime directory"
        ),
        'config': FileField(
            '-c', '--config',
            help="Configuration file"
        ),
        'task_files': FileList(
            '--task-file',
            help="Task definition file"
        ),
        'start_time': StringField(
            '--start-time',
            help="[Expert use] Pipeline start time"
        ),
        'dry_run': BoolField(
            '-n', '--dry-run',
            help="Dry run",
            default=False
        ),
        'args': ListField(
            '--args', help="Args", default=[]
        )
    }

    outputs = {}

    def __init__(self):
        super(RecipeIngredients, self).__init__()
        #                  Must run the following *after* WSRTrecipe.__init__().
        # ----------------------------------------------------------------------
        self.inputs = LOFARingredient(self._infields)
        self.outputs = LOFARingredient(self._outfields)
        self.optionparser.add_options(self.inputs.make_options())
