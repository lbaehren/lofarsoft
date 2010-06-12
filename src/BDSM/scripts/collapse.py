"""Module collapse

Defines operation Op_collapse which collapses 3D image.
"""

import numpy as N
from image import Op

class Op_collapse(Op):
    """Collapse 3D image"""
    def __call__(self, img):
        if len(img.image.shape) == 3:
            img.image = img.image[0]
