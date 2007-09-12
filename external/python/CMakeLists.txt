
add_subdirectory (matplotlib)
add_subdirectory (numarray)

## ------------------------------------------------------------------------------
## Installation option for Python packages

# [1] python setup.py build install --root=`pwd`/../install_with_root
#
# install_with_root
# `-- usr
#     |-- include
#     |   `-- python2.4
#     |       `-- numarray
#     `-- lib
#         `-- python2.4
#             `-- site-packages
#                 `-- numarray
#                     |-- codegenerator
#                     |-- convolve
#                     |-- fft
#                     |-- image
#                     |-- linear_algebra
#                     |-- ma
#                     |-- matrix
#                     |-- mlab
#                     |-- nd_image
#                     `-- random_array
#
# [2] python setup.py build install --home=`pwd`/../install_with_home
# 
# install_with_home
# |-- include
# |   `-- python
# |       `-- numarray
# `-- lib
#     `-- python
#         `-- numarray
#             |-- codegenerator
#             |-- convolve
#             |-- fft
#             |-- image
#             |-- linear_algebra
#             |-- ma
#             |-- matrix
#             |-- mlab
#             |-- nd_image
#             `-- random_array
#
# [3] python setup.py build install --prefix=`pwd`/../install_with_prefix
#
# install_with_prefix
# |-- include
# |   `-- python2.4
# |       `-- numarray
# `-- lib
#     `-- python2.4
#         `-- site-packages
#             `-- numarray
#                 |-- codegenerator
#                 |-- convolve
#                 |-- fft
#                 |-- image
#                 |-- linear_algebra
#                 |-- ma
#                 |-- matrix
#                 |-- mlab
#                 |-- nd_image
#                 `-- random_array
