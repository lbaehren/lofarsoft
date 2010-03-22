from distutils.core import setup

setup(
    name="Pipeline",
    version="0.1.dev",
    packages=[
        'lofarpipe', 'lofarpipe.cuisine', 'lofarpipe.support',
        'lofarrecipe', 'lofarrecipe.master', 'lofarrecipe.nodes',
        'qcheck'
    ],
    description="LOFAR Pipeline System",
    author="John Swinbank",
    author_email="swinbank@transientskp.org",
    url="http://www.transientskp.org/",
)
