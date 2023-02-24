Installation
============
libirap is under development, and is only available in source form. When the
project matures, pre-compiled binaries will be provided.

Dependencies

Build time:

- C++11 compiler (tested on gcc, clang, and msvc)
- cmake https://cmake.org/

Documentation:

- Doxygen http://www.doxygen.nl/
- Sphinx http://www.sphinx-doc.org/en/master/
- Breathe https://breathe.readthedocs.io/en/latest/

From source with cmake
----------------------
libirap is a cookie-cutter cmake project:

.. code::

   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON
   cmake --build build --target install

To build the documentation, you need doxygen, sphinx, and breathe. To have it
built automatically, pass :code:`-DBUILD_DOCS=ON` to cmake. Sphinx is invoked
through python, you can help cmake find the correct python by invoking cmake
with :code:`-DPYTHON_EXECUTABLE=`which python3``

libirap also offers some handy command-line applications. These can be built by passing
:code:`-DBUILD_APPS=ON` to cmake.
