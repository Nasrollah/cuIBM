/*
*  Copyright (C) 2012 by Anush Krishnan, Simon Layton, Lorena Barba
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy
*  of this software and associated documentation files (the "Software"), to deal
*  in the Software without restriction, including without limitation the rights
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*  copies of the Software, and to permit persons to whom the Software is
*  furnished to do so, subject to the following conditions:
*
*  The above copyright notice and this permission notice shall be included in
*  all copies or substantial portions of the Software.
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
*  THE SOFTWARE.
*/

/**
* @file NavierStokesSolver.h
* @brief Solves the Navier-Stokes equations in a rectangular domain.
*/

#pragma once

#include <types.h>
#include <domain.h>
#include <integrationScheme.h>
#include <io/io.h>
#include <parameterDB.h>
//#include <cusp/precond/smoothed_aggregation.h>
//#include <cusp/precond/diagonal.h>

/**
* Navier-Stokes solver for a rectangular domain.
*/
template <typename memoryType>
class NavierStokesSolver
{
protected:
	parameterDB *paramDB;
	domain      *domInfo;
	integrationScheme intgSchm;

	real QCoeff;
	
	cusp::coo_matrix<int, real, memoryType>
	     M,
	     Minv, 
	     L,    ///< Discrete Laplacian 
	     A, 
	     QT,   ///< 
	     Q, 
	     BN, 
	     C;

	cusp::array1d<real, memoryType>
	     q, qStar, lambda, rn, H, rhs1, rhs2, bc1, bc2, temp2, temp1, bc[4], qOld;

	size_t  timeStep, subStep, iterationCount1, iterationCount2;
	
	real forceX, forceY, force1;
	
	Logger logger;
	
	std::ofstream forceFile, iterationsFile;
	
	/**
	* Initialises stuff common to all IBM solvers
	*/
	void initialiseCommon();
	void initialiseArrays(int numQ, int numLambda);
	void initialiseFluxes();
	void initialiseBoundaryArrays();
	void assembleMatrices(); // contains subfunctions to calculate A, QT, BN, QTBNQ

	// functions to generate matrices
	
	void generateM();
	
	// Methods are defined as virtual when they are redefined in a derived class with the same name.
	
	virtual void generateL();
	virtual void generateA(real alpha);
	void generateBN();
	
	virtual void generateQT(int *QTRows, int *QTCols, real *QTVals){}
	virtual void generateQT();
	void updateQ(real gamma);
	
	void generateC();

	// generate explicit terms
	virtual void generateRN();
	void calculateExplicitQTerms();
	void calculateExplicitLambdaTerms();
	void generateRNFull();
	
	virtual void generateBC1();
	void generateBC1Full(real alpha);
	virtual void generateBC2();

	void assembleRHS1();
	void assembleRHS2();

	void solveIntermediateVelocity();
	void solvePoisson();
	void projectionStep();

	void updateBoundaryConditions();
	virtual void updateSolverState();
	
	virtual void calculateForce();

public:
	/**
	* \brief Initialise stuff required for the simulation
	*/
	virtual void initialise();
	
	/**
	* \brief Calculate all the variables at the next time step
	*/
	void stepTime();
	
	/**
	* \brief Write the data to files
	*/
	void writeData();
	
	/**
	* \brief Condition required to bring the simulation to a halt.
	* \return True if the simulation is over. False if it must continue.
	*/
	bool finished();
	
	/**
	* \brief Perform necessary actions to end the simulation
	*/
	void shutDown();
	
	// Factory methods are static (not entirely sure why)
	static NavierStokesSolver<memoryType>* createSolver(parameterDB &paramDB, domain &dom_info);
	
	/**
	* \brief Give the name of the current solver 
	* \return String that describes the type of solver
	*/
	virtual std::string name()
	{
		return "Navier-Stokes";
	}
};
