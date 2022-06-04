#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include <string.h>
#include "engine.h"
int main()
{
	/* open matlab engine */
	Engine *m_pEngine;
	m_pEngine = engOpen(NULL);
	if (m_pEngine == NULL)
	{
		std::cout << "Error" << std::endl;
		exit(1);
	}
	
	engEvalString(m_pEngine, "clc;");
	engEvalString(m_pEngine, "close all;");
	engEvalString(m_pEngine, "clear all;");
	engEvalString(m_pEngine, "figure"); 
	engEvalString(m_pEngine, "img = imread('test.jpg')");
	engEvalString(m_pEngine, "min_x = -0.040; max_x = 0.040; min_y = -4.0; max_y = 4.0");
	engEvalString(m_pEngine, "imagesc([min_x max_x], [min_y max_y], flipud(img));");
	system("pause");
	//engEvalString(m_pEngine, "close;"); // Closes the matlab engine
	return 0;
}