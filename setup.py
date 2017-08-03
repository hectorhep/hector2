from setuptools import setup
from distutils.sysconfig import get_python_lib
import glob

setup(
    name = "pyhector",
    package_dir = {'': 'Hector'},
    data_files = [
        (get_python_lib(), glob.glob('pyhector*.so')),
        ('bin', ['pyhector.py'])
    ],
    author = 'Laurent Forthomme',
    description = 'The Hector beamline transport algorithm.',
    license = 'GNU',
    keywords = 'proton transport beamline physics CERN LHC',
    url = 'https://github.com/hectorhep/hector2',
    #test_require = ['nose'],
    zip_safe = False,
)