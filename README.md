# CLL ![https://img.shields.io/github/license/PercentEquals/CLL](https://img.shields.io/github/license/PercentEquals/CLL) ![https://ci.appveyor.com/api/projects/status/github/PercentEquals/CLL?branch=master&svg=true](https://ci.appveyor.com/api/projects/status/github/PercentEquals/CLL?branch=master&svg=true)  
  
CLL is my attempt at creating a 'simple' programming language with c-like syntax and ideas  
such as operator precedence or data types.  
  
It also borrows some ideas from others interpreted languages like Python or batch/bash  
such as triple equals sign and arguments passing.  
  
CLL can be used to create scripts or can be embedded into a c++ project.  
It is also worth pointing out that CLL is [turing complete](https://en.wikipedia.org/wiki/Turing_completeness), but it is **not** [object-oriented](https://en.wikipedia.org/wiki/Object-oriented_programming).

## Useful links  

* [Wiki](https://github.com/PercentEquals/CLL/wiki)  
* [Releases](https://github.com/PercentEquals/CLL/releases)  
* [Examples](https://github.com/PercentEquals/CLL/tree/master/Examples)  
* [Visual Studio Code Extension](https://github.com/PercentEquals/CLL/tree/master/VisualStudioCode)  
  
## Features
  
* Dynamic variables  
* Multidimensional and irregular arrays (vectors)
* Flow control  
* Functions  
* User defined functions  
* Recursion  

## Installation

### Windows

You can download latest interpreter from [here](https://github.com/PercentEquals/CLL/releases) or You can compile it yourself  
preferably using Visual Studio with v142 toolset on 10.0.18362.0 Windows SDK.  

### Linux

You need to compile it Yourself with g++. I prepared CMakeLists.txt to make it easier.  
First install cmake: `sudo apt-get install cmake`, then You need to download or clone the   
whole project and lastly from the project directory You need to execute the following commands:

```batch
mkdir build && cd build
cmake ..
make
sudo make install
```

## Build requirements

CLL uses c++14 features, so it must be compiled with c++14 flag.  
`CLL` directory must be built as a static library.
No external libraries needed.

## Contributing

Every contribution is welcome, whether it is typo or bug fix or some new feature.  
You can also create example scripts and embedded c++ programs. More info [here](https://github.com/PercentEquals/CLL/tree/master/Examples).  
