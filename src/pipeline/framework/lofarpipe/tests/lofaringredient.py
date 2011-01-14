#                                                       LOFAR PIPELINE FRAMEWORK
#
#                                                             Tests: ingredients
#                                                            John Swinbank, 2010
#                                                      swinbank@transientskp.org
# ------------------------------------------------------------------------------
import unittest
import os

class StringFieldTest(unittest.TestCase):
    """
    Tests for :class:`lofarpipe.support.lofaringredient.StringField`
    """
    def setUp(self):
        from lofarpipe.support.lofaringredient import StringField
        self.stringfield = StringField(default="a string")

    def test_validator(self):
        """
        Check that strings are correctly regarded as valid, and integers
        aren't.
        """
        self.assertFalse(self.stringfield.is_valid(1))
        self.assertTrue(self.stringfield.is_valid("1"))

    def test_default(self):
        """
        Check that default is correctly set.
        """
        self.assertEqual(self.stringfield.default, "a string")

class IntFieldTest(unittest.TestCase):
    """
    Tests for :class:`lofarpipe.support.lofaringredient.IntField`
    """
    def setUp(self):
        from lofarpipe.support.lofaringredient import IntField
        self.intfield = IntField(default=1)

    def test_validator(self):
        """
        Check that integers are correctly regarded as valid, and strings
        aren't.
        """
        self.assertFalse(self.intfield.is_valid("1"))
        self.assertTrue(self.intfield.is_valid(1))

    def test_default(self):
        """
        Check that default is correctly set.
        """
        self.assertEqual(self.intfield.default, 1)

    def test_coerce(self):
        """
        Check that a string is correctly coerced to an integer.
        """
        self.assertEqual(self.intfield.coerce("1"), 1)

class FloatFieldTest(unittest.TestCase):
    """
    Tests for :class:`lofarpipe.support.lofaringredient.FloatField`
    """
    def setUp(self):
        from lofarpipe.support.lofaringredient import FloatField
        self.floatfield = FloatField(default=1.0)

    def test_validator(self):
        """
        Check that floats are correctly regarded as valid, and strings
        aren't.
        """
        self.assertFalse(self.floatfield.is_valid("1"))
        self.assertTrue(self.floatfield.is_valid(1.0))

    def test_default(self):
        """
        Check that default is correctly set.
        """
        self.assertEqual(self.floatfield.default, 1.0)

    def test_coerce(self):
        """
        Check that a string is correctly coerced to an float.
        """
        self.assertEqual(self.floatfield.coerce("1"), 1.0)

class FileFieldTest(unittest.TestCase):
    """
    Tests for :class:`lofarpipe.support.lofaringredient.FileField`
    """
    def setUp(self):
        from lofarpipe.support.lofaringredient import FileField
        self.filefield = FileField(default='/')

    def test_validator(self):
        """
        Integers are not valid as filenames, and certainly don't exist on
        disk.

        ``/`` should, though.
        """
        self.assertFalse(self.filefield.is_valid(1))
        self.assertTrue(self.filefield.is_valid("/"))

    def test_default(self):
        """
        Check that default is correctly set.
        """
        self.assertEqual(self.filefield.default, "/")

class ExecFieldTest(unittest.TestCase):
    """
    Tests for :class:`lofarpipe.support.lofaringredient.ExecField`
    """
    def setUp(self):
        from lofarpipe.support.lofaringredient import ExecField
        self.execfield = ExecField(default='/bin/ls')

    def test_validator(self):
        """
        ``/etc/passwd`` should always exist as a file on disk, but not be
        executable.

        ``/bin/ls`` should always exist, and must be executable.
        """
        self.assertFalse(self.execfield.is_valid("/etc/passwd"))
        self.assertTrue(self.execfield.is_valid("/bin/ls"))

    def test_default(self):
        """
        Check that default is correctly set.
        """
        self.assertEqual(self.execfield.default, "/bin/ls")

class DirectoryFieldTest(unittest.TestCase):
    """
    Tests for :class:`lofarpipe.support.lofaringredient.DirectoryField`
    """
    def setUp(self):
        from lofarpipe.support.lofaringredient import DirectoryField
        self.directoryfield = DirectoryField(default='/tmp')

    def test_validator(self):
        """
        An integer is not a valid directory.

        ``/tmp`` should always be valid.
        """
        self.assertFalse(self.directoryfield.is_valid(1))
        self.assertTrue(self.directoryfield.is_valid("/tmp"))

    def test_default(self):
        """
        Check that default is correctly set.
        """
        self.assertEqual(self.directoryfield.default, "/tmp")

    def test_coerce(self):
        """
        Coercing a create-able directory name should cause it to exist. We
        should always be able to write in ``/tmp``.
        """
        self.directoryfield.coerce("/tmp/foo")
        self.assertTrue(os.path.exists("/tmp/foo"))

class LOFARIngredientTest(unittest.TestCase):
    """
    Tests for :class:`lofarpipe.support.lofaringredient.LOFARingredient`
    """
    def setUp(self):
        """
        An instance of
        :class:`~lofarpipe.support.lofaringredient.LOFARingredient` is defined
        which contains three instances of
        :class:`~lofarpipe.support.lofaringredient.StringField`.
        """
        from lofarpipe.support.lofaringredient import StringField
        from lofarpipe.support.lofaringredient import LOFARingredient
        f = StringField(default="foo")
        g = StringField()
        h = StringField(default=1)
        self.lofaringredient = LOFARingredient({"f": f, "g": g, "h": h})

    def test_keys(self):
        """
        ``self.lofaringredient`` should contain keys for the two fields
        which have default parameters, but not for the one which is unset.
        """
        self.assertEqual(len(self.lofaringredient.keys()), 2)
        self.assertRaises(KeyError, lambda: self.lofaringredient['g'])

    def test_values(self):
        """
        Prior to setting, the value of the fields should be equal to
        the default value.
        """
        self.assertEqual(self.lofaringredient['f'], "foo")

    def test_set(self):
        """
        When set, the value of the fields should be equal to the new value.
        """
        self.lofaringredient['g'] = "bar"
        self.assertEqual(self.lofaringredient['g'], "bar")

    def test_bad_values(self):
        """
        Unacceptable values should raise an exception.
        """
        self.assertRaises(TypeError, lambda: self.lofaringredient['h'])
        self.lofaringredient['h'] = "bar"
        self.assertEqual(self.lofaringredient['h'], "bar")

if __name__ == "__main__":
    unittest.main()
