#from setuptools import setup, find_packages
from distutils.core import setup

setup(
    name="Pipeline",
    version="0.1.dev",
#    packages=find_packages(),
    packages=['lofarpipe', 'lofarrecipe', 'qcheck'],
    scripts=['sip.py', 'mac-sip.py'],
#    description="LOFAR Pipeline System",
#    author="John Swinbank",
#    author_email="swinbank@transientskp.org",
#    url="http://www.transientskp.org/",
#    package_dir={'cuisine': 'framework/cuisine'},
#    packages=['cuisine', 'pipeline', 'pipeline.nodes', 'pipeline.master', 'pipeline.support'],
)
