from setuptools import setup

setup(
    name="Pipeline",
    version="0.0",
    description="LOFAR Pipeline System",
    author="John Swinbank",
    author_email="swinbank@transientskp.org",
    url="http://www.transientskp.org/",
    package_dir={'cuisine': 'framework/cuisine'},
    packages=['cuisine', 'pipeline', 'pipeline.nodes', 'pipeline.master', 'pipeline.support'],
)
