/*
  Author:       Benjamin G. Friedman
  Date:         01/02/2022
  File:         Poly.c
  Description:  Implementation file for the polynomial opaque object interface.
*/


#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Poly.h"


typedef struct polyTerm {
	int exp;
	double coeff;
} PolyTerm;

typedef struct poly {
	PolyTerm* terms;
	int cap;
	int size;
} Poly;

const char validCompChars[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'x', 'X', '+', '-', '.', '^' };
const int validCompCharsSize = sizeof(validCompChars) / sizeof(*validCompChars);




/*********** Declarations for helper functions defined in this file **********/
/*
FUNCTION
  - Name:     allCompCharsAreValid
  - Purpose:  Check if all the characters in a polynomial component are valid.
PRECONDITION
  - comp
      Purpose:      String to check if it contains all valid polynomial component characters.
      Restrctions:  None.
POSTCONDITION
Success
  - Reason:        All cases.
  - Summary:       The correct value is returned accordingly.
  - Return value:  TRUE if all the characters are valid for a polynomial component.
                   FALSE if otherwise.
Failure
  - N/A
*/
static Boolean allCompCharsAreValid(const char* comp);


/*
FUNCTION
  - Name:     calcXValue
  - Purpose:  Calculates a polynomial with a given x-value.
PRECONDITION
  - pPoly
      Purpose:       Polynomial to to calculate with the x-value.
      Restrictions:  Pointer to a valid polynomial object with at least one term.
	             If x is 0, it has no terms with negative exponents.
  - x
      Purpose:       x-value used in the calculation.
      Restrictions:  If the polynomial has at least one term with a negative exponent, it isn't 0.
POSTCONDITION
Success
  - Reason:        All cases.
  - Summary:       The polynomial is calculated with the x-value and the result is returned.
  - Return value:  The calculation of the polynomial with the x-value.
Failure
  - N/A
*/
static double calcXValue(const Poly* pPoly, double x);


/*
FUNCTION
  - Name:     defIntegralDivByZeroError
  - Purpose:  Checks if a division by zero error would happen when calculating the definite ingral of a polynomial with the given bounds.
              This happens if the the polynomial has at least one term with a negative exponent that isn't -1, and the range of the lower and upper bound includes 0.
              During the definite integral calculation, this results in division by zero which is undefined.
PRECONDITION
  - pPoly
      Purpose:       Polynomial to check if division by zero would happen when calculating the definite integral with the given bounds.
      Restrictions:  Pointer to a valid polynomial object.
  - LB
      Purpose:       Lower bound used in the definite integral.
      Restrictions:  None.
  - UB
      Purpose:       Upper bound used in the definite integral.
      Restrictions:  None.
POSTCONDITION
Success
  - Reason:        All cases.
  - Summary:       The correct value is returned accordingly.
  - Return value:  TRUE if division by zero would happen.
		   FALSE if otherwise.
Failure
  - N/A
*/
static Boolean defIntegralDivByZeroError(const Poly* pPoly, double LB, double UB);


/*
FUNCTION
  - Name:     defIntegralNatLogError
  - Purpose:  Checks if a natural logarithm error would happen when calculating the definite integral of a polynomial with the given bounds.
              This happens if the polynomial has a term with an exponent of -1, and the range of the lower and upper bound includes 0.
              During the definite integral calculation, this results in taking the natural logarithm of zero which is undefined.
PRECONDITION
  - pPoly
      Purpose:       Polynomial to check if the natural logarithm error would happen when calculating the definite integral with the given bounds.
      Restrictions:  Pointer to a valid polynomial object.
  - LB
      Purpose:       Lower bound used in the definite integral.
      Restrictions:  None.
  - UB
      Purpose:       Upper bound used in the definite integral.
      Restrictions:  None.
POSTCONDITION
Success
  - Reason:        All cases.
  - Summary:       The correct value is returned accordingly.
  - Return value:  TRUE if the natural logarithm error would happen.
		   FALSE if otherwise.
Failure
  - N/A
*/
static Boolean defIntegralNatLogError(const Poly* pPoly, double LB, double UB);


/*
FUNCTION
  - Name:     diffPoly
  - Purpose:  Differentiates a polynomial.
PRECONDITION
  - pPoly
      Purpose:       Polynomial to differentiate.
      Restrictions:  Pointer to a valid polynomial object.
POSTCONDITION
Success
  - Reason:        The polynomial has terms before differentiating.
  - Summary:       The polynomial is differentiated.
  - Return value:  SUCCESS
  - pPoly:         The derivative of the polynomial is stored in the polynomial.
Failure
  - Reason:        The polynomial has no terms before differentiating.
  - Summary:       The polynomial isn't differentiated and nothing of significance happens.
  - Return value:  FAILURE
  - pPoly:         The state of the polynomial before the function call is preserved.
*/
static Status diffPoly(Poly* pPoly);


/*
FUNCTION
  - Name:     diffTerm
  - Purpose:  Differentiates a polynomial term.
PRECONDITION
  - term
      Purpose:       Term to differentiate.
      Restrictions:  Valid polynomial term.
POSTCONDITION
Success
  - Reason:        All cases.
  - Summary:       Differentiates the term and returns the derivative.
  - Return value:  The correct value is returned accordingly.
                     - If term isn't a constant, applies the derivative formula: d/dx of (k)(x^n) = (nk)(x^(n-1))
                     - If otherwise, sets both the coefficient and exponent to a default state of 0.
		       Since a valid polynomial term can't have a coefficient of 0, it can be used to identify this case.
Failure
  - N/A
*/
static PolyTerm diffTerm(PolyTerm term);


/*
FUNCTION
  - Name:     getCoeffOfTerm
  - Purpose:  Get a coefficient in the form of a number from a term in the form of a string.
PRECONDTION
  - term
      Purpose:       Term string for which to get the coefficient of.
      Restrictions:  Valid polynomial term string.
POSTCONDITION
Success
  - Reason:        All cases.
  - Summary:       Returns the coefficient of the term.
  - Return value:  Coefficient of the term.
Failure
  - N/A
*/
static double getCoeffOfTerm(const char* term);


/*
FUNCTION
  - Name:     getExpOfTerm
  - Purpose:  Get an exponent in the form of a number from a term in the form of a string.
PRECONDTION
  - term
      Purpose:       Term string for which to get the exponent of.
      Restrictions:  Valid polynomial term string.
POSTCONDITION
Success
  - Reason:        All cases.
  - Summary:       Returns the exponent of the term.
  - Return value:  Exponent of the term.
Failure
  - N/A
*/
static int getExpOfTerm(const char* term);


/*
FUNCTION
  - Name:     getIndexOfTermWithExp
  - Purpose:  Get the index of a term with a given exponent in the array of terms.
PRECONDITION
  - pPoly
      Purpose:       Polynomial for which to get the index of a term with the exponent.
      Restrictions:  Pointer to a valid polynomial object.
  - exp
      Purpose:       Exponent of the term whose index should be returned.
      Restrictions:  None.
POSTCONDITION
Success
  - Reason:        The polynomial contains a term with the exponent.
  - Summary:       Returns the index of the term.
  - Return value:  The index of the term with the exponent.
Failure
  - Reason:        The polynomial doesn't contain a term with the exponent.
  - Summary:       Returns a special value to indicate an error.
  - Return value:  -1
*/
static int getIndexOfTermWithExp(const Poly* pPoly, int exp);


/*
FUNCTION
  - Name:     getMaxNumOfTerms
  - Purpose:  Get the maximum number of possible terms in a polynomial string.
PRECONDITION
  - polyStr
      Purpose:       Polynomial string to count the maximum number of terms.
      Restrictions:  Valid polynomial string.
POSTCONDITION
Success
  - Reason:        All cases.
  - Summary:       Returns the maximum possible number of terms in the polynomial string.
  - Return value:  Maximum possible number of terms in the polynomial string.
                     - Note: Maximum possible number of terms means terms with the same exponent are treated as separate terms.
Failure
  - N/A
EXAMPLES
    polynomial: x^2 + x + 1      returned value: 3    polynomial object: x^2 + x + 1
    polynomial: x^2 + x^2 + 1    returned value: 3    polynomial object: 2x^2 + 1
*/
static int getMaxNumOfTerms(const char* polyStr);


/*
FUNCTION
  - Name:     getNumOfNegExps
  - Purpose:  Get the number of terms with negative exponents.
PRECONDITION
  - pPoly
      Purpose:       Polynomial for which to count how many terms contain negative exponents.
      Restrictions:  Pointer to a valid polynomial object.
POSTCONDITION
Success
  - Reason:        All cases.
  - Summary:       Returns the number of terms with negative exponents
  - Return value:  Number of terms with negative exponents.
Failure
  - N/A
*/
static int getNumOfNegExps(const Poly* pPoly);


/*
FUNCTION
  - Name:     isValidComp
  - Purpose:  Checks if a component string is valid.
PRECONDITION
  - comp
      Purpose:      String to check if its a valid component string.
      Restrctions:  Doesn't contain whitespace.
POSTCONDITION
Success
  - Reason:        All cases.
  - Summary:       The correct value is returned accordingly.
  - Return value:  TRUE if the component is a valid component string.
                   FALSE if otherwise.
Failure
  - N/A
*/
static Boolean isValidComp(const char* comp);


/*
FUNCTION
  - Name:     inputsAreValidDoubles
  - Purpose:  Checks if a string contains a specific amount of valid doubles.
PRECONDITION
  - input
      Purpose:       String to check if it contains only a specific amount of valid doubles.
      Restrictions:  None.
  - expectedNums
      Purpose:       The expected amount of doubles to be found in the input string.
      Restrictions:  Any integer > 0.
POSTCONDITION
Success
  - Reason:        All cases.
  - Summary:       The correct value is returned accordingly.
  - Return value:  TRUE if input only contains valid doubles and the amount of doubles equals expectedNums.
                   FALSE if otherwise.
Failure
  - N/A
*/
Boolean inputsAreValidDoubles(const char* input, int expectedNums);


/*
FUNCTION
  - Name:     inputsAreValidInts
  - Purpose:  Checks if a string contains a specific amount of valid integers.
PRECONDITION
  - input
      Purpose:       String to check if it contains only a specific amount of valid integers.
      Restrictions:  None.
  - expectedNums
      Purpose:       The expected amount of integers to be found in the input string.
      Restrictions:  Any integer > 0.
POSTCONDITION
Success
  - Reason:        All cases.
  - Summary:       The correct value is returned accordingly.
  - Return value:  TRUE if input only contains valid integers and the amount of integers equals expectedNums.
                   FALSE if otherwise.
Failure
  - N/A
*/
Boolean inputsAreValidInts(const char* input, int expectedNums);


/*
FUNCTION
  - Name:     integratePoly
  - Purpose:  Calculates the integral of a polynomial.
PRECONDITION
  - pPoly
      Purpose:       Polynomial to integrate.
      Restrictions:  Pointer to a valid polynomial object.
  - pExpNegOneIntegrated
      Purpose:       Indicate if a term with an exponent of -1 gets integrated.
      Restrictions:  Not NULL.
  - pCoeffExpNegOne
      Purpose:       Store the coefficient of the term with an exponent of -1 that gets integrated if it exists.
      Restrictions:  Not NULL.
POSTCONDITION
Success
  - Reason:                The polynomial has terms before integrating.
  - Summary:               The integral is calculated.
  - Return value:          SUCCESS
  - pPoly:                 The integral is stored in the polynomial with the exception of the constant C because it can't be stored as a valid polynomial term.
                             - If there's no term with an exponent of -1 before integrating, then all terms excluding the constant C are in the polynomial.
                             - If otherwise, then all terms excluding the integral of that term and the constant C are in the polynomial.
                               This is because the integral of a term with an exponent of -1 includes the natural logarithm which can't stored as a valid polynomial term.
  - pExpNegOneIntegrated:  The Boolean it points to is set accordingly.
                             - FALSE if there's no term with an exponent of -1 that gets integrated.
                             - TRUE if otherwise.
  - pCoeffExpNegOne:       The double it points to is set accordingly.
                             - 0 if there's no term with an exponent of -1 that gets integrated.
                             - The term's coefficient if otherwise.
Failure
  - Reason:                The polynomial has no terms before integrating.
  - Summary:               The integral isn't calculated.
  - Return value:          FAILURE
  - pPoly:                 The state of the polynomial before the function call is preserved.
  - pExpNegOneIntegrated:  The Boolean it points to is set to FALSE.
  - pCoeffExpNegOne:       The double it points to is set to 0.
*/
static Status integratePoly(Poly* pPoly, Boolean* pExpNegOneIntegrated, double* pCoeffExpNegOne);


/*
FUNCTION
  - Name:     integrateTerm
  - Purpose:  Calculates the integral of a term.
PRECONDITION
  - term
      Purpose:       Term to integrate.
      Restrictions:  Valid polynomial term.
  - pExpNegOneIntegrated
      Purpose:       Indicate if a term with an exponent of -1 gets integrated.
      Restrictions:  Not NULL.
  - pCoeffExpNegOne
      Purpose:       Store the coefficient of the term with an exponent of -1 that gets integrated if it exists.
      Restrictions:  Not NULL.
POSTCONDITION
Success
  - Reason:                All cases.
  - Summary:               The term is integrated and returned, and the Boolean and double for the term with an exponent of -1 are set accordingly.
  - Return value:          The integral of the term.
		             - If the integrated term didn't have an exponent of -1, applies the integral formula: integral of (k)(x^n) = (k)((x)^(n+1)) / (n+1).
		             - If the integrated term does have an exponent of -1, leaves the coefficient and integral as is.
		               In this case, the formula is integral of (k)(x^-1) = (k)(ln|x|) so its accurate to leave the coefficient as is
			       but the exponent can't be accurately represented in a valid polynomial term.
  - pExpNegOneIntegrated:  The Boolean it points to is set accordingly.
                             - FALSE if the term that gets integrated doesn't have an exponent of -1.
                             - TRUE if otherwise.
  - pCoeffExpNegOne:       The double it points to is set accordingly.
                             - 0 if the term that gets integrated doesn't have an exponent of -1.
                             - The term's coefficient if otherwise.
Failure
  - N/A
*/
static PolyTerm integrateTerm(PolyTerm term, Boolean* pExpNegOneIntegrated, double* pCoeffExpNegOne);


/*
FUNCTION
  - Name:     newPoly
  - Purpose:  Creates a new polynomial with an existing polynomial object using a polynomial string.
PRECONDITION
  - pPoly
      Purpose:       Polynomial to create a new polynomial with.
      Restrictions:  Pointer to a valid polynomial object.
  - polyStr
      Purpose:       Polynomial string to create a new polynomial with.
      Restrictions:  Valid polynomial string.
POSTCONDITION
Success
  - Reason:        No memory allocation failure.
  - Summary:       Creates a new polynomial in the existing polynomial object based on the polynomial string.
  - Return value:  SUCCESS
  - pPoly:         The new polynomial from the polynomial string is stored in the polynomial object.
Failure
  - Reason:        Memory allocation failure.
  - Summary:       Doesn't create a new polynomial with the polynomial string and nothing of significance happens.
  - Return value:  FAILURE
  - pPoly:         The state of the polynomial before the function call is not guaranteed to be preserved.
*/
static Status newPoly(Poly* pPoly, const char* polyStr);


/*
FUNCTION
  - Name:     resize
  - Purpose:  Resizes a polynomial's array of terms to be able to hold more terms and preserves the existng terms.
PRECONDITION
  - pPoly
      Purpose:       Polynomial to resize.
      Restrictions:  Pointer to a valid polynomial object.
POSTCONDITION
Success
  - Reason:        No memory allocation failure.
  - Summary:       Resizes the polynomial.
  - Return value:  SUCCESS
  - pPoly:         The array of terms is resized and all existing terms are preserved.
Failure
  - Reason:        Memory allocation failure.
  - Summary:       Doesn't resize the polynomial and nothing of significance happens.
  - Return value:  FAILURE
  - pPoly:         The state of the polynomial before the function call is preserved.
*/
static Status resize(Poly* pPoly);


/*
FUNCTION
  - Name:     swap
  - Purpose:  Swaps the coefficient and exponent of two terms.
PRECONDITION
  - pTerm1
      Purpose:       Term whose values should be swapped.
      Restrictions:  Pointer to a valid polynomial term.
  - pTerm2
      Purpose:       Term whose values should be swapped.
      Restrictions:  Pointer to a valid polynomial term.
POSTCONDITION
Success
  - Reason:        All cases.
  - Summary:       Swaps the coefficient and exponent of the terms.
  - Return value:  N/A
  - pTerm1:        The term it points to stores the coefficient and exponent of pTerm2.
  - pTerm2:        The term it points to stores the coefficient and exponent of pTerm1.
Failure
  - N/A
*/
static void swap(PolyTerm* pTerm1, PolyTerm* pTerm2);




/********** Definitions for polynomial interface functions declared in Polynomial.h **********/
Status poly_addTerm(POLY hPoly, int exp, double coeff) {
	Poly* pPoly = hPoly;    
	int idx;

	// exponent exists - add coefficient to existing coefficient and remove term if sum is 0
	if (poly_existsTermWithExp(hPoly, exp)) {
		idx = getIndexOfTermWithExp(pPoly, exp);
		pPoly->terms[idx].coeff += coeff;
		if (pPoly->terms[idx].coeff == 0)
			poly_removeTermWithExp(hPoly, exp);
	}
	// exponent doesn't exist - add the term and resize if necessary so long as the coefficient isn't 0
	else if (coeff != 0) {
		if (pPoly->size == pPoly->cap) {
			if (!resize(pPoly))
				return FAILURE;
		}
		pPoly->terms[pPoly->size].exp = exp;
		pPoly->terms[pPoly->size++].coeff = coeff;
	}

	return SUCCESS;
}


Status poly_calcDefIntegral(POLY hPoly, double LB, double UB, double* pResult, Boolean* pExpNegOneIntegrated, double* pCoeffExpNegOne, 
	Boolean* pPolyHasNoTerms, Boolean* pDivByZeroError, Boolean* pNatLogError)
{
	Poly* pPoly = hPoly;

	// the variables get set to the same value in many cases
	*pResult = 0;                  // 1, 2, 3, 4
	*pExpNegOneIntegrated = FALSE; // 1, 2, 3, 4
	*pCoeffExpNegOne = 0;          // 1, 2, 3, 4
	*pPolyHasNoTerms = FALSE;      // 2, 3, 4, 5
	*pDivByZeroError = FALSE;      // 1, 3, 4, 5
	*pNatLogError = FALSE;         // 1, 2, 4, 5

	// 1: polynomial has no terms - can't calculate the integral
	if (pPoly->size == 0) {
		*pPolyHasNoTerms = TRUE;
		return FAILURE;
	}

	// 2: only division by zero error - can't calculate the integral
	if (defIntegralDivByZeroError(pPoly, LB, UB) &&
		!defIntegralNatLogError(pPoly, LB, UB)) {
		*pDivByZeroError = TRUE;
		return FAILURE;
	}

	// 3: only natural log error - can't calculate the integral
	if (!defIntegralDivByZeroError(pPoly, LB, UB) &&
		defIntegralNatLogError(pPoly, LB, UB)) {
		*pNatLogError = TRUE;
		return FAILURE;
	}

	// 4: both division by zero and natural log error - can't calculate the integral
	if (defIntegralDivByZeroError(pPoly, LB, UB) &&
		defIntegralNatLogError(pPoly, LB, UB)) {
		*pDivByZeroError = TRUE;
		*pNatLogError = TRUE;
		return FAILURE;
	}
		
	// 5: polynomial has terms and no errors - calculate the integral
	// integrate and check for a term with an exponent of -1
	integratePoly(pPoly, pExpNegOneIntegrated, pCoeffExpNegOne);
	
	// calculate the lower and upper bound results for the definite integral
	// if a term with an exponent of -1 was integrated, the value stored in the
	// result is not correct because it doesn't include the natural log part
	*pResult = calcXValue(pPoly, UB) - calcXValue(pPoly, LB);

	return SUCCESS;
}


Status poly_calcIndefIntegral(POLY hPoly, Boolean* pExpNegOneIntegrated, double* pCoeffExpNegOne) {
	Poly* pPoly = hPoly;

	// polynomial has no terms - can't calculate the integral
	if (pPoly->size == 0) {
		*pExpNegOneIntegrated = FALSE;
		*pCoeffExpNegOne = 0;
		return FAILURE;
	}

	// polynomial has terms - integrate and check for a term with an exponent of -1
	integratePoly(pPoly, pExpNegOneIntegrated, pCoeffExpNegOne);

	return SUCCESS;
}


Status poly_calcNthDeriv(POLY hPoly, int n, Boolean* pNthDerivIsZero) {
	Poly* pPoly = hPoly;
	
	// polynomial has no terms or it has terms and the nth derivative doesn't reach zero
	*pNthDerivIsZero = FALSE;

	// polynomial has no terms - can't calculate nth derivative
	if (pPoly->size == 0)
		return FAILURE;

	// polynomial has terms - calcluate the nth derivative
	// differentiate the polynomial "n" times or as many times as possible before reaching 0
	for (int i = 0; i < n && !(*pNthDerivIsZero); ++i) {
		if (!diffPoly(pPoly))
			*pNthDerivIsZero = TRUE;
	}

	return SUCCESS;
}


Status poly_calcXValue(POLY hPoly, double x, double* pResult, Boolean* pPolyHasNoTerms) {
	Poly* pPoly = hPoly;

	// the variables get set to the same value in many cases
	*pResult = 0;             // 1 and 2
	*pPolyHasNoTerms = FALSE; // 2 and 3

	// 1: polynomial has no terms - can't calculate with the x-value
	if (pPoly->size == 0) {
		*pPolyHasNoTerms = TRUE;
		return FAILURE;
	}

	// 2: division by zero - can't calculate with the x-value
	if (x == 0 && poly_existsNegExp(hPoly))
		return FAILURE;

	// 3: polynomial has terms and no errors - calculate with the x-value
	*pResult = calcXValue(pPoly, x);

	return SUCCESS;
}


Status poly_copy(POLY* phPolyDest, POLY hPolySrc) {
	Poly* pPolySrc = hPolySrc;
	Boolean destPolyExists = TRUE;

	if (!(*phPolyDest)) {
		destPolyExists = FALSE;
		*phPolyDest = poly_initDefault();
		if (!(*phPolyDest))
			return FAILURE;			
	} 
	else
		poly_reset(*phPolyDest);

	for (int i = 0; i < pPolySrc->size; ++i)
		if (!poly_addTerm(*phPolyDest, pPolySrc->terms[i].exp, pPolySrc->terms[i].coeff)) {
			if (!destPolyExists)
				poly_destroy(phPolyDest);
			return FAILURE;
		}
			
	return SUCCESS;
}


Status poly_destroy(POLY* phPoly) {
	Poly* pPoly = *phPoly;

	if (pPoly) {
		free(pPoly->terms);
		free(pPoly);
		*phPoly = NULL;
		return SUCCESS;
	}

	return FAILURE;
}


Boolean poly_existsNegExp(POLY hPoly) {
	Poly* pPoly = hPoly;

	for (int i = 0; i < pPoly->size; ++i) {
		if (pPoly->terms[i].exp < 0)
			return TRUE;
	}

	return FALSE;
}


Boolean poly_existsTermWithExp(POLY hPoly, int exp) {
	Poly* pPoly = hPoly;

	for (int i = 0; i < pPoly->size; ++i) {
		if (pPoly->terms[i].exp == exp)
			return TRUE;
	}

	return FALSE;
}


int poly_getCapacity(POLY hPoly) {
	Poly* pPoly = hPoly;
	return pPoly->cap;
}


double poly_getCoeffOfExp(POLY hPoly, int exp, Boolean* pPolyHasNoTerms, Boolean* pExpExists) {
	Poly* pPoly = hPoly;
	double coeff = 0;
	*pExpExists = FALSE;
	*pPolyHasNoTerms = TRUE;

	if (pPoly->size > 0) {
		*pPolyHasNoTerms = FALSE;
		for (int i = 0; i < pPoly->size && !(*pExpExists); ++i) {
			if (pPoly->terms[i].exp == exp) {
				*pExpExists = TRUE;
				coeff = pPoly->terms[i].coeff;
			}
		}
	}

	return coeff;
}


int poly_getDegree(POLY hPoly, Boolean* pPolyHasNoTerms) {
	Poly* pPoly = hPoly;
	int degree;

	if (pPoly->size == 0) {
		*pPolyHasNoTerms = TRUE;
		degree = 0;
	} 
	else {
		*pPolyHasNoTerms = FALSE;
		degree = pPoly->terms[0].exp;
		for (int i = 1; i < pPoly->size; ++i) {
			if (pPoly->terms[i].exp > degree)
				degree = pPoly->terms[i].exp;
		}
	}

	return degree;
}


int poly_getSize(POLY hPoly) {
	Poly* pPoly = hPoly;
	return pPoly->size;
}


Boolean poly_hasNoTerms(POLY hPoly) {
	Poly* pPoly = hPoly;
	return pPoly->size == 0;
}


POLY poly_initCopy(POLY hPolySrc) {
	Poly* pPolySrc = hPolySrc;

	Poly* pPoly = malloc(sizeof(*pPoly));
	if (pPoly) {
		pPoly->cap = pPolySrc->cap;
		pPoly->size = pPolySrc->size;
		if (!(pPoly->terms = malloc(sizeof(*(pPoly->terms)) * pPoly->cap))) {
			free(pPoly);
			return NULL;
		}

		for (int i = 0; i < pPoly->size; ++i)
			pPoly->terms[i] = pPolySrc->terms[i];
	}

	return pPoly;
}


POLY poly_initDefault(void) {
	Poly* pPoly = malloc(sizeof(*pPoly));
	if (pPoly) {
		pPoly->cap = 1;
		pPoly->size = 0;
		if (!(pPoly->terms = malloc(sizeof(*(pPoly->terms)) * pPoly->cap))) {
			free(pPoly);
			return NULL;
		}
	}

	return pPoly;
}


POLY poly_initMove(POLY* phPolySrc) {
	Poly* pPoly = *phPolySrc;
	if (pPoly)
		*phPolySrc = NULL;
	return pPoly;
}


POLY poly_initPolyStr(const char* polyStr, Boolean* pPolyStrIsValid) {
	if (!poly_isValidPolyStr(polyStr)) {
		*pPolyStrIsValid = FALSE;
		return NULL;
	}

	*pPolyStrIsValid = TRUE;

	Poly* pPoly = malloc(sizeof(*pPoly));
	if (pPoly) {
		pPoly->cap = getMaxNumOfTerms(polyStr);
		pPoly->size = 0;
		if (!(pPoly->terms = malloc(sizeof(*(pPoly->terms)) * pPoly->cap))) {
			free(pPoly);
			return NULL;
		}

		if (!newPoly(pPoly, polyStr)) {
			free(pPoly->terms);
			free(pPoly);
			return NULL;
		}
	}

	return pPoly;
}


Boolean poly_isValidPolyStr(const char* polyStr) {
	char comp[500];                  // one component from the polynomial (term or operator)
	Boolean prevCompIsOp = FALSE;    // indicates if the previous component is an operator
	Boolean isFirstComp = TRUE;      // indicates if it's the first component in the polynomial
	int i = 0;                       // index of polyStr
	int j = 0;                       // index of comp


	// empty string - user only pressed enter
	if (!strcmp(polyStr, ""))
		return FALSE;

	// get each inidividual term and check its validity
	while (polyStr[i] != '\0') {
		while (isspace(polyStr[i]) && polyStr[i] != '\0')   // clear whitespace
			++i;
		while (!isspace(polyStr[i]) && polyStr[i] != '\0')  // convert a single component
			comp[j++] = polyStr[i++];
		comp[j] = '\0';
		j = 0;

		if (!strcmp(comp, ""))   // extra whitespace at the end
			break;

		if (!isValidComp(comp))  // invalid component
			return FALSE;

		// component is valid, check for invalid placement of valid component
		// first component, can't be operator
		if (isFirstComp) {
			if (!strcmp(comp, "+") || !strcmp(comp, "-"))
				return FALSE;
			isFirstComp = FALSE;
		}
		// not first component, can't have consecutive operators or consecutive terms
		else {
			if (!strcmp(comp, "+") || !strcmp(comp, "-")) {
				if (prevCompIsOp)
					return FALSE;
				prevCompIsOp = TRUE;
			}
			else {
				if (!prevCompIsOp)
					return FALSE;
				prevCompIsOp = FALSE;
			}
		}
	}

	// all components are valid, final component can't be operator
	return !prevCompIsOp;
}


Status poly_move(POLY* phPolyDest, POLY* phPolySrc) {
	// source polynomial doesn't exist
	if (!(*phPolySrc))
		return FAILURE;

	// source polynomial exists
	// destroy polynomial in the destination handle if it exists in preparation for the move
	poly_destroy(phPolyDest);

	// move the source polynomial into the destination handle
	*phPolyDest = *phPolySrc;

	// remove ownership of the polynomial from the source handle
	*phPolySrc = NULL;

	return SUCCESS;
}


Status poly_newPoly(POLY hPoly, const char* polyStr, Boolean* pPolyStrIsValid) {
	Poly* pPoly = hPoly;

	if (!poly_isValidPolyStr(polyStr)) {
		*pPolyStrIsValid = FALSE;
		return FAILURE;
	}

	*pPolyStrIsValid = TRUE;
	
	// reset polynomial in preparation for new one
	poly_reset(hPoly);
	
	// add all the new terms to the polynomial
	if (!newPoly(pPoly, polyStr))
		return FAILURE;

	return SUCCESS;
}


Status poly_print(POLY hPoly) {
	Poly* pPoly = hPoly;
	double coeff;
	int exp;

	// polynomial has no terms
	if (poly_hasNoTerms(hPoly)) {
		return FAILURE;
	}

	// polynomial has terms - print out the terms
	for (int i = 0; i < pPoly->size; ++i) {
		coeff = pPoly->terms[i].coeff;
		exp = pPoly->terms[i].exp;
		
		// except for first term, all negative coefficients are displayed as positives after an - sign
		// for example, the polynomial "-2x^2 + -2x" would display as "-2x^2 - 2x"
		
		// print coefficient
		// constant term
		if (exp == 0) {
			if (i == 0)
				printf("%g", coeff);
			else
				printf("%g", fabs(coeff));
		}
		// non-constant term
		else {
			if (i == 0) {
				// print "-1" as just "-", don't print "1", and print others as normal
				if (coeff == -1)
					printf("-");
				else if (coeff != 1)
					printf("%g", coeff);
			}
			else {
				// don't print 1 or -1, print others as normal
				if (fabs(coeff) != 1)
					printf("%g", fabs(coeff));
			}
		}

		// print x and exponent
		// only print x if non-constant term
		// only print exponent if non-constant term and exponent isn't 1
		if (exp != 0) {
			printf("x");
			if (exp != 1)
				printf("^%d", exp);
		}

		// print operator if not at end of polynomial
		if (i < pPoly->size - 1) {
			if (pPoly->terms[i + 1].coeff < 0)
				printf(" - ");
			else
				printf(" + ");
		}
	}

	return SUCCESS;
}


Status poly_removeTermWithExp(POLY hPoly, int exp) {
	Poly* pPoly = hPoly;    
	int idx;

	// exponent exists - remove the term
	if (poly_existsTermWithExp(hPoly, exp)) {
		idx = getIndexOfTermWithExp(pPoly, exp);
		for (int i = idx; i < pPoly->size - 1; ++i)
			pPoly->terms[i] = pPoly->terms[i + 1];
		--pPoly->size;
		return SUCCESS;
	}

	// exponent doesn't exist - do nothing
	return FAILURE;
}


void poly_reset(POLY hPoly) {
	Poly* pPoly = hPoly;
	pPoly->size = 0;
}


void poly_sort(POLY hPoly) {
	Poly* pPoly = hPoly;

	for (int i = 0; i < pPoly->size - 1; ++i) {
		int indexOfMax = i;
		for (int j = i + 1; j < pPoly->size; ++j) {
			if (pPoly->terms[j].exp > pPoly->terms[indexOfMax].exp)
				indexOfMax = j;
		}

		if (i != indexOfMax)
			swap(&pPoly->terms[i], &pPoly->terms[indexOfMax]);
	}
}




/********** Helper function definitions **********/
static Boolean allCompCharsAreValid(const char* comp) {
	Boolean isValidChar;

	for (int i = 0; comp[i] != '\0'; ++i) {
		isValidChar = FALSE;
		for (int j = 0; j < validCompCharsSize && !isValidChar; ++j) {
			if (comp[i] == validCompChars[j])
				isValidChar = TRUE;
		}
		if (!isValidChar)
			return FALSE;
	}
	
	return TRUE;
}


static double calcXValue(const Poly* pPoly, double x) {
	double result = 0;

	for (int i = 0; i < pPoly->size; ++i) {
		if (pPoly->terms[i].exp == 0)
			result += pPoly->terms[i].coeff;
		else
			result += pPoly->terms[i].coeff * pow(x, pPoly->terms[i].exp);
	}

	return result;
}


static Status diffPoly(Poly* pPoly) {
	PolyTerm derivOfTerm;                         // derivative of each term
	Boolean constTermIsDifferentiated = FALSE;    // indicates if a constant term gets differentiated
	int j = 0;                                    // index of new terms, can't use i b/c constants get removed


	// polynomial has no terms - can't calculate the derivative
	if (pPoly->size == 0)
		return FAILURE;

	// polynomial has terms - calculate the derivative
	// differentiate every term, constants become 0 and get removed
	for (int i = 0; i < pPoly->size; ++i) {
		derivOfTerm = diffTerm(pPoly->terms[i]);
		if (!(derivOfTerm.coeff == 0 && derivOfTerm.exp == 0))
			pPoly->terms[j++] = derivOfTerm;
		else
			constTermIsDifferentiated = TRUE;
	}

	// the polynomial has lost a term a constant term is differentiated
	if (constTermIsDifferentiated)
		--pPoly->size;

	return SUCCESS;
}


static PolyTerm diffTerm(PolyTerm term) {
	PolyTerm deriv;;

	// constant term - derivative is 0
	if (term.exp == 0) {
		deriv.coeff = 0;
		deriv.exp = 0;
	}
	// non-constant term - d/dx of (k)(x^n) = (nk)(x^(n-1))
	else {
		deriv.coeff = term.coeff * term.exp;
		deriv.exp = term.exp - 1;
	}

	return deriv;
}


static Boolean defIntegralDivByZeroError(const Poly* pPoly, double LB, double UB) {
	// contains at least one negative exponent and 0 is in the range of the lower and upper bound
	if (poly_existsNegExp((POLY)pPoly) &&
		((LB <= 0 && UB >= 0) || (UB <= 0 && LB >= 0))) {
		// if there's only one negative exponent and it's -1 then it's a natural logarithm error, not division by zero
		if (getNumOfNegExps(pPoly) == 1) {
			if (poly_existsTermWithExp((POLY)pPoly, -1)) 
				return FALSE;
			return TRUE;
		}
		// more than one negative exponent so at least one of them won't be -1
		else
			return TRUE;
	}

	// there are no negative exponents, or there are but the range of the lower and upper bounds doesn't include 0
	return FALSE;
}


static Boolean defIntegralNatLogError(const Poly* pPoly, double LB, double UB) {
	return (poly_existsTermWithExp((POLY)pPoly, -1) && ((LB <= 0 && UB >= 0) || (UB <= 0 && LB >= 0)));
}


static double getCoeffOfTerm(const char* term) {
	char coeff[500] = { '\0' };    // coefficient from term string
	int i = 0;                     // index of term string
	int j = 0;                     // index of coefficient string


	// get all characters of the coefficient
	while (term[i] != '\0' && term[i] != 'x' && term[i] != 'X')
		coeff[j++] = term[i++];

	// term is x
	if (i == 0)
		return 1;

	// term is -x
	if (!strcmp(coeff, "-"))
		return -1;

	// term is a constant or x with a coefficient
	// with double, 0.0 and -0.0 are separate values, treat both as 0.
	double convertedCoeff = strtod(coeff, NULL);
	return (fabs(convertedCoeff) == 0) ? 0 : convertedCoeff;
}


static int getExpOfTerm(const char* term) {
	char exp[500] = { '\0' };    // exponent from term string
	int i = 0;		     // index of term string
	int j = 0;		     // index of exponent string


	// remove coefficient if it exists
	while (term[i] != '\0' && term[i] != 'x' && term[i] != 'X')
		++i;

	// term is a constant in the form of just a number, exponent is 0
	if (term[i] == '\0')
		return 0;

	// term is x or -x, exponent is 1
	if ((term[i] == 'x' || term[i] == 'X') && term[i + 1] == '\0')
		return 1;

	// term is x to any exponent
	i += 2;
	while (term[i] != '\0')
		exp[j++] = term[i++];

	return atoi(exp);
}


static int getIndexOfTermWithExp(const Poly* pPoly, int exp) {
	for (int i = 0; i < pPoly->size; ++i) {
		if (pPoly->terms[i].exp == exp)
			return i;
	}

	return -1;
}


static int getMaxNumOfTerms(const char* polyStr) {
	char comp[500];      // component from the polynomial string (term or operator)
	int i = 0;           // index of polynomial string
	int j = 0;           // index of component string
	int numTerms = 0;    // number of terms in the polynomial string


	while (polyStr[i] != '\0') {
		while (isspace(polyStr[i]) && polyStr[i] != '\0')  // clear whitespace
			++i;

		while (!isspace(polyStr[i]) && polyStr[i] != '\0') // get all characters of component
			comp[j++] = polyStr[i++];
		comp[j] = '\0';
		j = 0;

		// increment numTerms so long as a new term exists and it isn't an operator
		if (strcmp(comp, "") && strcmp(comp, "-") && strcmp(comp, "+"))
			++numTerms;
	}

	return numTerms;
}


static int getNumOfNegExps(const Poly* pPoly) {
	int negExpCount = 0;

	for (int i = 0; i < pPoly->size; ++i)
		if (pPoly->terms[i].exp < 0)
			++negExpCount;

	return negExpCount;
}


static Boolean isValidComp(const char* comp) {
	char coeffOrExp[500];    // coefficient or exponent from component string
	int i = 0;               // index of component string
	int j = 0;               // index of buf

	// check that all characters are valid
	if (!allCompCharsAreValid(comp))
		return FALSE;

	// all characters are valid - check that it's a valid component
	// string of size 1 - must be an operator, the term x by itself, or a single digit constant
	if (strlen(comp) == 1) {
		if (!strcmp(comp, "x") || !strcmp(comp, "X") || !strcmp(comp, "-") || !strcmp(comp, "+") || isdigit(comp[i]))
			return TRUE;
		else
			return FALSE;
	}

	// string of size > 1 - must be a term
	// get the coefficient if it exists
	while (comp[i] != 'x' && comp[i] != 'X' && comp[i] != '\0')
		coeffOrExp[j++] = comp[i++];
	coeffOrExp[j] = '\0';

	// coefficient exists - form must be: [double]...
	if (strcmp(coeffOrExp, "")) {
		if (strcmp(coeffOrExp, "-") && !inputsAreValidDoubles(coeffOrExp, 1)) // coefficient is invalid double except for "-" in term "-x"
			return FALSE;
		if (comp[i] == '\0') // coefficient is the whole term for a constant - form must be: [double]
			return TRUE;
	}

	// coefficient didn't exist, or coefficient existed, is valid, and is not a constant term
	// form must be: [double][x]... or [x]...
	++i;
	if (comp[i] == '\0') // x by itself with or without coefficient and no exponent - form must be: [double][x] or [x]
		return TRUE;

	if (comp[i] != '^') // if characters exist after x, must be ^ for an exponent - form must be: ...[x][^]...
		return FALSE;

	// get the exponent
	++i;
	j = 0;
	while (comp[i] != '\0')
		coeffOrExp[j++] = comp[i++];
	coeffOrExp[j] = '\0';

	// exponent must be an integer
	return inputsAreValidInts(coeffOrExp, 1);
}


Boolean inputsAreValidDoubles(const char* input, int expectedNums) {
	Boolean negExists = FALSE;    // indicates if a negative sign has already been encountered in the same number
	Boolean decExists = FALSE;    // indicates if a decimal point has already been encountered in the same number
	int totalNums = 0;            // counts the amount of numbers in input to compare to expectedNums
	int i = 0;                    // index of input
	int j;                        // saves value of i to deal with an edge case


	// clear leading whitespace, space is the only valid whitespace character
	while (isspace(input[i])) {
		if (input[i] != ' ')
			return FALSE;
		++i;
	}
	if (input[i] == '\0') // user only pressed "enter" or whitespace followed by "enter" 
		return FALSE;

	// validate that all inputs are valid doubles
	while (input[i] != '\0') {
		// something other than a digit, negative sign, decimal point, or space is invalid
		if (!isdigit(input[i]) && input[i] != ' ' && input[i] != '-' && input[i] != '.')
			return FALSE;

		// repeat decimals points/negative signs are ok if they're in separate numbers
		// space indicates a new number or trailing whitespace, so reset them
		else if (input[i] == ' ') {
			negExists = FALSE;
			decExists = FALSE;
		}

		// negative sign
		else if (input[i] == '-') {
			if (negExists) // multiple negatives in a single number is invalid
				return FALSE;
			else
				negExists = TRUE;

			// negative sign is the first character of input
			if (i == 0) {
				// invalid if the next character isn't a digit or decimal point
				if (!isdigit(input[i + 1]) && input[i + 1] != '.')
					return FALSE;
			}
			// negative sign isn't the first character of input
			else {
				// invalid if the next character isn't a digit or decimal point, or if the previous character isn't a space
				if ((!isdigit(input[i + 1]) && input[i + 1] != '.') || input[i - 1] != ' ')
					return FALSE;
			}
		}

		// decimal point
		else if (input[i] == '.') {
			if (decExists) // multiple decimal points in a single number is invalid.
				return FALSE;
			else
				decExists = TRUE;

			// decimal point is the first character of input
			if (i == 0) {
				// invalid if the next character isn't a digit
				if (!isdigit(input[i + 1]))
					return FALSE;
			}
			// decimal point isn't the first character of input
			else {
				// invalid if the next character isn't a digit, or if the previous character isn't a space, negative sign, or number
				if (!isdigit(input[i + 1]) || (!isdigit(input[i - 1]) && input[i - 1] != ' ' && input[i - 1] != '-'))
					return FALSE;
			}
		}

		++i;
	}


	// all inputs are valid doubles, now verify the correct amount of numbers have been entered
	i = 0;
	while (input[i] != '\0') {
		while (input[i] == ' ')                     // clear whitespace
			++i;
		j = i;
		while (input[i] != ' ' && input[i] != '\0') // get the number
			++i;
		if (i != j)                                 // don't want to increment totalNums if user entered extra whitespace at end
			++totalNums;
	}

	return totalNums == expectedNums;
}


Boolean inputsAreValidInts(const char* input, int expectedNums) {
	Boolean negExists = FALSE;    // indicates if a negative sign has already been encountered in the same number
	int totalNums = 0;            // counts the amount of numbers in input to compare to expectedNums
	int i = 0;                    // index of input
	int j;                        // saves value of i to deal with an edge case


	// clear leading whitespace, space is the only valid whitespace character
	while (isspace(input[i])) {
		if (input[i] != ' ')
			return FALSE;
		++i;
	}
	if (input[i] == '\0') // user only pressed "enter" or whitespace followed by "enter"
		return FALSE;

	// validate that all inputs are valid integers
	while (input[i] != '\0') {
		// something other than a digit, negative sign, or space is invalid
		if (!isdigit(input[i]) && input[i] != ' ' && input[i] != '-')
			return FALSE;

		// repeat negative signs are ok if they're in separate numbers
		// space indicates a new number or trailing whitespace, so reset it
		else if (input[i] == ' ')
			negExists = FALSE;

		// negative sign
		else if (input[i] == '-') {
			if (negExists) // multiple negatives in a single number is invalid
				return FALSE;
			else
				negExists = TRUE;

			// negative sign is the first character of input
			if (i == 0) {
				// invalid if the next character isn't a digit
				if (!isdigit(input[i + 1]))
					return FALSE;
			}
			// negative sign isn't the first character of input
			else {
				// invalid if the next character isn't a digit, or the previous character isn't a space
				if (!isdigit(input[i + 1]) || input[i - 1] != ' ')
					return FALSE;
			}
		}
		++i;
	}


	// all inputs are valid integers, now verify the correct amount of numbers have been entered
	i = 0;
	while (input[i] != '\0') {
		while (input[i] == ' ')                     // clear whitespace
			++i;
		j = i;
		while (input[i] != ' ' && input[i] != '\0') // get the number
			++i;
		if (i != j)                                 // don't want to incremement totalNums if user entered extra whitespace at end
			++totalNums;
	}

	return totalNums == expectedNums;
}


static Status integratePoly(Poly* pPoly, Boolean* pExpNegOneIntegrated, double* pCoeffExpNegOne) {
	PolyTerm integralOfTerm;                // integral of each term
	Boolean expNegOneIntegrated = FALSE;    // indicates if a term with an exponent of -1 gets integrated
	double coeffExpNegOne = 0;              // coefficient of term with exponent of -1 if it exists
	int j = 0;                              // index of terms to keep (integral of term with exponent of -1 isn't kept)


	// polynomial has no terms or it has terms but there is no term with an exponent of -1
	// in either case, set the Boolean to FALSE and the coefficient to 0
	*pExpNegOneIntegrated = FALSE;
	*pCoeffExpNegOne = 0;

	// polynomial has no terms - can't calculate the integral
	if (pPoly->size == 0) {
		return FAILURE;
	}

	// polynomial has terms - calculate the integral
	// if there is a term with an exponent of -1 it gets removed because its integral is ln|x| which can't be stored in a polynomial object
	for (int i = 0; i < pPoly->size; ++i) {
		integralOfTerm = integrateTerm(pPoly->terms[i], &expNegOneIntegrated, &coeffExpNegOne);
		if (expNegOneIntegrated && !(*pExpNegOneIntegrated)) {
			*pExpNegOneIntegrated = TRUE;
			*pCoeffExpNegOne = coeffExpNegOne;
		}
		else
			pPoly->terms[j++] = integralOfTerm;
	}

	// the polynomial has lost a term if one of its terms before integration had an exponent of -1
	if (*pExpNegOneIntegrated)
		--pPoly->size;

	return SUCCESS;
}


static PolyTerm integrateTerm(PolyTerm term, Boolean* pExpNegOneIntegrated, double* pCoeffExpNegOne) {
	PolyTerm integral;

	// term has an exponent of -1 - use default values
	if (term.exp == -1) {
		*pExpNegOneIntegrated = TRUE;
		*pCoeffExpNegOne = term.coeff;
		integral = term;
	}
	// term doesn't have an exponent of -1, use integral formula: integral of (k)(x^n) = (k)((x)^(n+1)) / (n+1)
	else {
		*pExpNegOneIntegrated = FALSE;
		*pCoeffExpNegOne = 0;
		integral.coeff = term.coeff / (term.exp + 1);
		integral.exp = term.exp + 1;
	}

	return integral;
}


static Status newPoly(Poly* pPoly, const char* polyStr) {
	char comp[500];                // component from the polynomial string (term or operator)
	int i = 0;                     // index of polynomial string
	int j = 0;                     // index of component string
	int exp;                       // exponent of term
	double coeff;                  // coefficient of term
	Boolean opIsMinus = FALSE;     // indicates if the operator is -
	Boolean isFirstComp = TRUE;    // indicates if component is the first component in the polynomial


	while (polyStr[i] != '\0') {
		while (isspace(polyStr[i]) && polyStr[i] != '\0')      // clear whitespace
			++i;

		// not at end of polynomial
		if (polyStr[i] != '\0') {
			while (!isspace(polyStr[i]) && polyStr[i] != '\0') // get the component
				comp[j++] = polyStr[i++];
			comp[j] = '\0';
			j = 0;

			// component is operator
			if (!strcmp(comp, "+") || !strcmp(comp, "-"))
				opIsMinus = !strcmp(comp, "-") ? TRUE : FALSE;
			// component is term
			else {
				// get the coefficient and exponent, ignore term if coefficient is 0
				coeff = getCoeffOfTerm(comp);
				if (coeff != 0) {
					if (!isFirstComp && opIsMinus) // minus operator negates sign of coefficient
						coeff *= -1;
					exp = getExpOfTerm(comp);
					if (!poly_addTerm((POLY)pPoly, exp, coeff))
						return FAILURE;
				}

				if (isFirstComp)
					isFirstComp = FALSE;
			}
		}
	}

	return SUCCESS;
}


static Status resize(Poly* pPoly) {
	PolyTerm* terms;

	if (!(terms = realloc(pPoly->terms, sizeof(*terms) * (pPoly->cap + 1))))
		return FAILURE;
	pPoly->terms = terms;
	++pPoly->cap;

	return SUCCESS;
}


static void swap(PolyTerm* pTerm1, PolyTerm* pTerm2) {
	PolyTerm temp = *pTerm1;
	*pTerm1 = *pTerm2;
	*pTerm2 = temp;
}