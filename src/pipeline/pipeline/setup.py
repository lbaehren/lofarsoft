from distutils.core import setup

setup(
    name="Pipeline",
    version="0.1.dev",
    packages=[
        'lofarpipe', 'lofarpipe.cuisine', 'lofarpipe.support',
        'qcheck'
    ],
    description="LOFAR Pipeline System",
    author="John Swinbank",
    author_email="swinbank@transientskp.org",
    url="http://www.transientskp.org/",
)
