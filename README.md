<p align="center">
  <img src="share/SKAAntennas.png" alt="SKA Dish logo"/>
</p>

# **SKA DSH.LMC**
[![Build Status](http://jenkins.oact.inaf.it:8080/job/SKA/job/SKA.DSH.LMC-prod/badge/icon)](http://jenkins.oact.inaf.it:8080/job/SKA/job/SKA.DSH.LMC-prod/)

## **About**  
DSH.LMC is the prototype monitoring and control (M&C) system for the dish antennas of the Square Kilometer Array (SKA).

## **Installation**  

### **Pre-requisites**
Install the project dependencies:  
* omniORB [http://omniorb.sourceforge.net/]  
* zmq (>=4.*) [http://zeromq.org/]  
* Tango Control System (>=9) [http://www.tango-controls.org/]: NB: Enable APPENDERS_HAVE_LEVEL_THRESHOLD in compilation
* yat (Rev>=29577) [http://svn.code.sf.net/p/tango-cs/code/share/yat/trunk]
* yat4tango (Rev>=29574) [http://svn.code.sf.net/p/tango-cs/code/share/yat4tango/trunk]: NB: Enable \_USING_TANGO_8_OR_ABOVE_ flag in compilation
* jsoncpp [https://github.com/open-source-parsers/jsoncpp]  
* log4cxx [https://logging.apache.org/log4cxx/]
* boost [http://www.boost.org/]
* pugixml (>=1.8) [http://pugixml.org/]: NB: Enable PUGIXML_HAS_LONG_LONG flag in compilation
* cmake (>=2.8) [https://cmake.org]  
  
Make sure you have set the following environment variables to the external library installation dirs 
* OMNIORB_ROOT  
* ZMQ_ROOT  
* TANGO_ROOT
* YAT_ROOT
* YAT4TANGO_ROOT
* JSONCPP_ROOT
* LOG4CXX_ROOT
* BOOST_ROOT
* PUGIXML_ROOT


Add also the following paths to the PKG_CONFIG_PATH environment var: 
* $TANGO_ROOT/lib/pkgconfig  
* $OMNIORB_ROOT/lib/pkgconfig  
* $ZMQ_ROOT/lib/pkgconfig  
* $LOG4CXX_ROOT/lib/pkgconfig
* $PUGIXML_ROOT/lib/pkgconfig

To generate the documentation you must install:

* Doxygen [www.doxygen.org/]
* Sphinx [sphinx-doc.org/] + Breathe [http://breathe.readthedocs.org/]

To run the unit tests you must enable the `ENABLE_TEST` flag in cmake build (`cmake ... -DENABLE_TEST=ON`) and install:

* GoogleTest [https://github.com/google/googletest]    
    
Make sure you define the `GTEST_ROOT` environment variable to your Google Test installation directory and specify the correct library path in `$LD_LIBRARY_PATH`.    

To run the integration tests (but also to run deploy and configure scripts) you must have installed:

* PyTango (>=9.2.*) [https://github.com/tango-controls/pytango]  
* behave [https://github.com/behave/behave]
* nose [https://nose.readthedocs.io/en/latest/]

### **Build and install**
The project makes use of the cmake build system. To build and install the project follow these steps:

* Clone this repository into your local $SOURCE_DIR  
  ```git clone git@gitlab.com:ska_dish/dsh.lmc.git $SOURCE_DIR```
* Create the build and install directories: $BUILD_DIR, $INSTALL_DIR  
   **NB: Don't build the project in the same source directory! Keep build and install files separated!**  
* In the build directory:  
  ```cmake -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR -DENABLE_TEST=ON $SOURCE_DIR```  
  ```make```  
  ```make install```  

If external libraries are correctly installed and configured cmake should find all needed include dirs and libraries used to build the project.  
If the project build and install step are performed successfully the following directories are generated in your installation directory:

* *include*: headers files
* *lib*: shared libraries 
* *bin*: binary files (e.g. device servers)
* *share*: API documentation
* *config*: control system configuration files

### **Documentation generation**
To generate the documentation enter the build directory and type:  
* ```make doc```  
* ```make doc-sphinx```  (currently under test)  
* ```make install``` (to install doc files in the install directory share/doc)  

### **Running unit tests**
To run unit tests:  
* ```make test``` (tests to be added...)  

### **Running integration tests**
TO BE FILLED
