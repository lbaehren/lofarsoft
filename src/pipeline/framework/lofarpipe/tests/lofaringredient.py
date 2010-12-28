import unittest
import os

class StringFieldTest(unittest.TestCase):
    def setUp(self):
        from lofarpipe.support.lofaringredient import StringField
        self.stringfield = StringField(default="a string")

    def test_validator(self):
        self.assertFalse(self.stringfield.is_valid(1))
        self.assertTrue(self.stringfield.is_valid("1"))

    def test_default(self):
        self.assertEqual(self.stringfield.default, "a string")

class IntFieldTest(unittest.TestCase):
    def setUp(self):
        from lofarpipe.support.lofaringredient import IntField
        self.intfield = IntField(default=1)

    def test_validator(self):
        self.assertFalse(self.intfield.is_valid("1"))
        self.assertTrue(self.intfield.is_valid(1))

    def test_default(self):
        self.assertEqual(self.intfield.default, 1)

    def test_coerce(self):
        self.assertEqual(self.intfield.coerce("1"), 1)

class FloatFieldTest(unittest.TestCase):
    def setUp(self):
        from lofarpipe.support.lofaringredient import FloatField
        self.floatfield = FloatField(default=1.0)

    def test_validator(self):
        self.assertFalse(self.floatfield.is_valid("1"))
        self.assertTrue(self.floatfield.is_valid(1.0))

    def test_default(self):
        self.assertEqual(self.floatfield.default, 1.0)

    def test_coerce(self):
        self.assertEqual(self.floatfield.coerce("1"), 1.0)

class FileFieldTest(unittest.TestCase):
    def setUp(self):
        from lofarpipe.support.lofaringredient import FileField
        self.filefield = FileField(default='/')

    def test_validator(self):
        self.assertFalse(self.filefield.is_valid(1))
        self.assertTrue(self.filefield.is_valid("/"))

    def test_default(self):
        self.assertEqual(self.filefield.default, "/")

class ExecFieldTest(unittest.TestCase):
    def setUp(self):
        from lofarpipe.support.lofaringredient import ExecField
        self.execfield = ExecField(default='/bin/ls')

    def test_validator(self):
        self.assertFalse(self.execfield.is_valid("/etc/passwd"))
        self.assertTrue(self.execfield.is_valid("/bin/ls"))

    def test_default(self):
        self.assertEqual(self.execfield.default, "/bin/ls")

class DirectoryFieldTest(unittest.TestCase):
    def setUp(self):
        from lofarpipe.support.lofaringredient import DirectoryField
        self.directoryfield = DirectoryField(default='/tmp')

    def test_validator(self):
        self.assertFalse(self.directoryfield.is_valid(1))
        self.assertTrue(self.directoryfield.is_valid("/tmp"))

    def test_default(self):
        self.assertEqual(self.directoryfield.default, "/tmp")

    def test_coerce(self):
        self.directoryfield.coerce("/tmp/foo")
        self.assertTrue(os.path.exists("/tmp/foo"))

class LOFARIngredientTest(unittest.TestCase):
    def setUp(self):
        from lofarpipe.support.lofaringredient import StringField
        from lofarpipe.support.lofaringredient import LOFARingredient
        f = StringField(default="foo")
        g = StringField()
        h = StringField(default=1)
        self.lofaringredient = LOFARingredient({"f": f, "g": g, "h": h})

    def test_keys(self):
        self.assertEqual(len(self.lofaringredient.keys()), 2)
        self.assertRaises(KeyError, lambda: self.lofaringredient['g'])

    def test_values(self):
        self.assertEqual(self.lofaringredient['f'], "foo")

    def test_set(self):
        self.lofaringredient['g'] = "bar"
        self.assertEqual(self.lofaringredient['g'], "bar")

    def test_bad_values(self):
        self.assertRaises(TypeError, lambda: self.lofaringredient['h'])
        self.lofaringredient['h'] = "bar"
        self.assertEqual(self.lofaringredient['h'], "bar")

if __name__ == "__main__":
    unittest.main()
