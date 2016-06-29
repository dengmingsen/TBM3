/*-----------------------------------------------------------|
| Copyright (C) 2016 Yuan-Yen Tai, Hongchul Choi,            |
|                    Jian-Xin Zhu                            |
|                                                            |
| This file is distributed under the terms of the GNU        |
| General Public License. See the file `LICENSE' in          |
| the root directory of the present distribution, or         |
| http://www.gnu.org/copyleft/gpl.txt .                      |
|                                                            |
|-----------------------------------------------------------*/

//
//  header.h
//  TBM^3
//
//  Created by Yuan Yen Tai on 9/10/15.
//


#ifndef _header_h
#define _header_h

#include <vector>
#include <deque>
#include <map>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cmath>
using namespace std;

#include "../Gramat/include/gramat.h"

namespace lift{
	
using namespace gmt;
	
	//typedef		dvar	r_var;
	//typedef		dmat	r_mat;
	//typedef		zvar	x_var;
	//typedef		zmat	x_mat;

	typedef		svar	r_var;
	typedef		smat	r_mat;
	typedef		cvar	x_var;
	typedef		cmat	x_mat;

#include "string_func.h"
#include "math_func.h"
	
#include "atom.cpp"
#include "lattice.cpp"
#include "order_parameter.cpp"
#include "tbm_operation_element.cpp"
#include "hopping_constructor.cpp"
#include "tbm.cpp"

}

#endif