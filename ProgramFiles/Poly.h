 /*
  Author:       Benjamin G. Friedman
  Date:         01/02/2022
  File:         Poly.h
  Description:  Header file for the polynomial opaque object interface.
*/


#ifndef POLY_H
#define POLY_H

#include "Status.h"

typedef void* POLY; // opaque object handle




/*
FUNCTION
  - Name:     poly_addTerm
  - Purpose:  Adds a term to a polynomial with a given exponent and coefficient.
PRECONDITION
  - hPoly
      Purpose:       Polynomial to add a term to.
      Restrictions:  Handle to a valid polynomial object.
  - exp
      Purpose:       Exponent of the new term.
      Restrictions:  None.
  - coeff
      Purpose:       Coefficient of the new term.
      Restrictions:  None.
POSTCONDITION
Success
  - Reason:        No memory allocation failure.
  - Summary:       New term with the coefficient and exponent is added to the polynomial.
  - Return value:  SUCCESS
  - hPoly:         If the coefficient is 0, the term isn't added and the state of the polynomial before the function call is preserved.
                   If the coefficient isn't 0, the term is added the polynomial.
                     - If a term with the exponent already exists, the existing term is updated by summing the current and new coefficient.
                       If the sum equals 0, the term is removed.
                     - If a term with the exponent doesn't exist, a new term is added.
Failure
  - Reason:        Memory allocation failure.
  - Summary:       New term isn't added to the polynomial and nothing of significance happens.
  - Return value:  FAILURE
  - hPoly:         The state of the polynomial before the function call is preserved.
EXAMPLES
  - Add term with new exponent
      polynomial before: x^2 + x + 1    exponent: 3    coefficient: 4     polynomial after: x^2 + x + 1 + 4x^3
  - Add term with existing exponent
      polynomial before: x^2 + x + 1    exponent: 2    coefficient: 4     polynomial after: 5x^2 + x + 1
  - Add term with existing exponent resulting in term removal
      polynomial before: x^2 + x + 1    exponent: 2    coefficient: -1    polynomial after: x + 1
*/
Status poly_addTerm(POLY hPoly, int exp, double coeff);


/*
FUNCTION
  - Name:     poly_calcDefIntegral
  - Purpose:  Calculates the definite integral of a polynomial with a given lower and upper bound.
PRECONDITION
  - hPoly
      Purpose:       Polynomial to calculate the definite integral on.
      Restrictions:  Handle to a valid polynomial object.
  - LB
      Purpose:       Lower bound of the definite integral.
      Restrictions:  None.
  - UB
      Purpose:       Upper bound of the definite integral.
      Restrictions:  None.
  - pResult
      Purpose:       Store the result of the calculation.
      Restrictions:  Not NULL.
  - pExpNegOneIntegrated
      Purpose:       Indicate if a term with an exponent of -1 gets integrated.
      Restrictions:  Not NULL.
  - pCoeffExpNegOne
      Purpose:       Store the coefficient of the term with an exponent of -1 that gets integrated if it exists.
      Restrictions:  Not NULL.
  - pPolyHasNoTerms
      Purpose:       Indicate if the polynomial has no terms.
      Restrictions:  Not NULL.
  - pDivByZeroError
      Purpose:       Indicate if there's a division by zero error.
                     This happens if the the polynomial has at least one term with a negative exponent that isn't -1, and the range of the lower and upper bound includes 0.
                     During the definite integral calculation, this results in division by zero which is undefined.
      Restrictions:  Not NULL.
  - pNatLogError
      Purpose:       Indicates if there's a natural logarithm error.
                     This happens if the polynomial has a term with an exponent of -1, and the range of the lower and upper bound includes 0.
                     During the definite integral calculation, this results in taking the natural logarithm of zero which is undefined.
      Restrictions:  Not NULL.
POSTCONDITION
Success
  - Reason:                The polynomial has terms before integrating, there's no division by zero error, and there's no natural logarithm error.
  - Summary:               The definite integral is calculated.
  - Return value:          SUCCESS
  - hPoly:                 The indefinite integral is stored in the polynomial with the exception of the constant C because it can't be stored as a valid polynomial term.
                             - If there's no term with an exponent of -1 before integrating, then all terms excluding the constant C are in the polynomial.
                             - If otherwise, then all terms excluding the integral of that term and the constant C are in the polynomial.
                               This is because the integral of a term with an exponent of -1 includes the natural logarithm which can't stored as a valid polynomial term.
  - pResult:               The double it points to stores the result of the calculation.
                           If a term with an exponent of -1 was integrated, the result doesn't contain the value from the natural logarithm so it isn't accurate.
  - pExpNegOneIntegrated:  The Boolean it points to is set accordingly.
                             - FALSE if there's no term with an exponent of -1 that gets integrated.
                             - TRUE if otherwise.
  - pCoeffExpNegOne:       The double it points to is set accordingly.
                             - 0 if there's no term with an exponent of -1 that gets integrated.
                             - The term's coefficient if otherwise.
  - pPolyHasNoTerms:       The Boolean it points to is set to FALSE.
  - pDivByZero:            The Boolean it points to is set to FALSE.
  - pNatLogError:          The Boolean it points to is set to FALSE.
Failure
  - Reason:                The polynomial has no terms, there's a division by zero error, or there's a natural logarithm error.
  - Summary:               The definite integral isn't calculated and nothing of significance happens.
  - Return value:          FAILURE
  - hPoly:                 The state of the polynomial before the function call is preserved.
  - pResult:               The double it points to is set to 0.
  - pExpNegOneIntegrated:  The Boolean it points to is set to FALSE.
  - pCoeffExpNegOne:       The double it points to is set to 0.
  - pPolyHasNoTerms:       The Boolean it points to is set accordingly.
                             - TRUE if the polynomial has no terms.
                             - FALSE if otherwise.
  - pDivByZero:            The Boolean it points to is set accordingly.
                             - TRUE if there's a division by zero error.
                             - FALSE if otherwise.
  - pNatLogError:          The Boolean it points to is set accordingly.
                             - TRUE if there's a natural logarithm error.
                             - FALSE if otherwise.
EXAMPLES
  - Division by zero error
      polynomial: x^-2           lower bound: -3    upper bound: 1
  - No division by zero error
      polynomial: x^-2           lower bound: -3    upper bound: -2
  - Natural logarithm error
      polynomial: x^-1           lower bound: -3    upper bound: 1
  - No natural logarithm error
      polynomial: x^-1           lower bound: -3    upper bound: -2
  - Division by zero and natural logarithm error
      polynomial: x^-2 + x^-1    lower bound: -3    upper bound: 1
  - No division by zero and natural logarithm error
      polynomial: x^-2 + x^-1    lower bound: -3    upper bound: -2
*/
Status poly_calcDefIntegral(POLY hPoly, double LB, double UB, double *pResult, Boolean* pExpNegOneIntegrated, double* pCoeffExpNegOne,
    Boolean* pPolyHasNoTerms, Boolean *pDivByZeroError, Boolean* pNatLogError);


/*
FUNCTION
  - Name:     poly_calcIndefIntegral
  - Purpose:  Calculates the indefinite integral of a polynomial.
PRECONDITION
  - hPoly
      Purpose:       Polynomial to calculate the indefinite integral of.
      Restrictions:  Handle to a valid polynomial object.
  - pExpNegOneIntegrated
      Purpose:       Indicate if a term with an exponent of -1 gets integrated.
      Restrictions:  Not NULL.
  - pCoeffExpNegOne
      Purpose:       Store the coefficient of the term with an exponent of -1 that gets integrated if it exists.
      Restrictions:  Not NULL.
POSTCONDITION
Success
  - Reason:                The polynomial has terms before integrating.
  - Summary:               The indefinite integral is calculated.
  - Return value:          SUCCESS
  - hPoly:                 The indefinite integral is stored in the polynomial with the exception of the constant C because it can't be stored as a valid polynomial term.
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
  - Summary:               The indefinite integral isn't calculated and nothing of significance happens.
  - Return value:          FAILURE
  - hPoly:                 The state of the polynomial before the function call is preserved.
  - pExpNegOneIntegrated:  The Boolean it points to is set to FALSE.
  - pCoeffExpNegOne:       The double it points to is set to 0.
EXAMPLES
  - No term with an exponent of -1
      polynomial before: 2x^2 + 1 - 3x^-3    polynomial after: 0.67x^3 + x + 1.5x^-2    actual polynomial after: 0.67x^3 + x + 1.5x^-2
  - Term with an exponent of -1
      polynomial before: 2x^2 + 1 - 3x^-1    polynomial after: 0.67x^3 + x              actual polynomial after: 0.67x^3 + x - 3ln|x|
*/
Status poly_calcIndefIntegral(POLY hPoly, Boolean* pExpNegOneIntegrated, double* pCoeffExpNegOne);


/*
FUNCTION
  - Name:     poly_calcNthDeriv
  - Purpose:  Calculates the nth derivative of a polynomial.
PRECONDITION
  - hPoly
      Purpose:       Polynomial to calculate the nth derivative of.
      Restrictions:  Handle to a valid polynomial object.
  - n
      Purpose:       Amount of derivatives to calculate.
      Restrictions:  Any integer > 0.
  - pNthDerivIsZero
      Purpose:       Indicate if the nth derivative is 0.
      Restrictions:  Not NULL.
POSTCONDITION
Success
  - Reason:           The polynomial has terms before differentiating.
  - Summary:          The nth derivative is calculated.
  - Return value:     SUCCESS
  - hPoly:            The nth derivative is stored in the polynomial.
                        - If the nth derivative isn't 0, the polynomial has whatever terms are remaining.
                        - If otherwise, the polynomial has no terms.
  - pNthDerivIsZero:  The Boolean it points to is set accordingly.
                        - FALSE if the nth derivative isn't 0.
                        - TRUE if otherwise.
Failure
  - Reason:           The polynomial has no terms before differentiating.
  - Summary:          The nth derivative isn't be calculated and nothing of significance happens.
  - Return value:     FAILURE
  - hPoly:            The state of the polynomial before the function call is preserved.
  - pNthDerivIsZero:  The Boolean it points to is set to FALSE.
EXAMPLES
  - Nth derivative doesn't cause term removal
      polynomial before: x^2 + x        n: 1    polynomial after: 2x + 1
  - Nth derivative does cause term removal and doesn't reach 0
      polynomial before: x^2 + x + 1    n: 1    polynomial after: 2x + 1
      polynomial before: x^2 + x + 1    n: 2    polynomial after: 2
  - Nth derivative does cause term removal and reaches 0
      polynomial before: x^2 + x + 1    n: 3    polynomial after: no terms
      polynomial before: x^2 + x + 1    n: 4    polynomial after: no terms
*/
Status poly_calcNthDeriv(POLY hPoly, int n, Boolean* pNthDerivIsZero);


/*
FUNCTION
  - Name:     poly_calcXValue
  - Purpose:  Calculates a polynomial with a given x-value.
PRECONDITION
  - hPoly
      Purpose:       Polynomial to calculate with the x-value.
      Restrictions:  Handle to a valid polynomial object.
  - x
      Purpose:       x-value used in the calculation.
      Restrictions:  None.
  - pResult
      Purpose:       Store the result of the calculation.
      Restrictions:  Not NULL.
  - pPolyHasNoTerms
      Purpose:       Indicate if the polynomial has no terms.
      Restrictions:  Not NULL.
POSTCONDITION
Success
  - Reason:           The polynomial has terms and there's no division by zero error.
  - Summary:          The polynomial is calculated with the x-value.
  - Return value:     SUCCESS
  - hPoly:            The state of the polynomial before the function call is preserved.
  - pResult:          The double it points to stores the result of the calculation.
  - pPolyHasNoTerms:  The Boolean it points to is set to FALSE.
Failure
  - Reason:           The polynomial has no terms, or it has terms but there's a division by zero error.
                      This happens if the the polynomial has at least one term with a negative exponent that isn't -1, and the range of the lower and upper bound includes 0.
                      During the x-value calculation, this results in division by zero which is undefined.
  - Summary:          The polynomial can't be calculated with the x-value and nothing of significance happens.
  - Return value:     FAILURE
  - hPoly:            The state of the polynomial before the function call is preserved.
  - pResult:          The double it points to is set to 0.
  - pPolyHasNoTerms:  The Boolean it points to is set accordingly.
                        - TRUE if the polynomial has no terms.
                        - FALSE if otherwise.                
EXAMPLES
  - hPoly: x^2 + x + 1    x: 2    result: 7
*/
Status poly_calcXValue(POLY hPoly, double x, double* pResult, Boolean* pPolyHasNoTerms);


/*
FUNCTION
  - Name:     poly_copy
  - Purpose:  Copies the data from one polynomial into another.
              C Opaque object design version of the copy assignment operator in C++.
PRECONDITION
  - phPolyDest
      Purpose:       Polynomial object to copy into.
      Restrictions:  Pointer to a handle to a valid polynomial object or NULL handle.
  - hPolySrc
      Purpose:       Polynomial object to copy from.
      Restrictions:  Handle to a valid polynomial object.
POSTCONDITION
Success
  - Reason:        No memory allocation failure.
  - Summary:       Creates a copy of hPolySrc and stores it in the handle pointed to by phPolyDest.
  - Return value:  SUCCESS
  - phPolyDest:    The handle it points to stores a copy of hPolySrc.
                     - If it points to a valid polynomial object, hPolySrc is copied into the existing polynomial.
                     - If it points to a NULL handle, a new polynomial is first created after which hPolySrc is copied into it.
  - hPolySrc:      The state of the polynomial before the function call is preserved.
Failure
  - Reason:        Memory allocation failure.
  - Summary:       Doesn't create a copy of hPolySrc and nothing of significance happens.
  - Return value:  FAILURE
  - phPolyDest:    If it is a pointer to a handle to valid polynomial object, the handle still points to the polynomial, but the state of the polynomial before the function call isn't guaranteed to be preserved.
                   If it is a pointer to a NULL handle, the handle remains NULL.
  - hPolySrc:      The state of the polynomial before the function call is preserved.
*/
Status poly_copy(POLY* phPolyDest, POLY hPolySrc);


/*
FUNCTION
  - Name:     poly_destroy
  - Purpose:  Destroys a polynomial.
              C Opaque object design version of the destructor in C++.
PRECONDITION
  - phPoly
      Purpose:       Polynomial to destroy.
      Restrictions:  Pointer to a handle to a valid polynomial object or NULL handle.
POSTCONDITION
Success
  - Reason:        The handle it points to isn't NULL.
  - Summary:       Destroys the polynomial.
  - Return value:  SUCCESS
  - phPoly:        Frees all memory associated with the polynomial and sets the handle to NULL.
Failure
  - Reason:        The handle is NULL.
  - Summary:       No polynomial is destroyed and nothing of significance happens.
  - Return value:  FAILURE
  - phPoly:        The state of the handle it points to before the function call is preserved.
*/
Status poly_destroy(POLY* phPoly);


/*
FUNCTION
  - Name:     poly_existsNegExp
  - Purpose:  Checks if a polynomial contains a term with a negative exponent.
PRECONDITION
  - hPoly
      Purpose:       Polynomial to check if it contains a negative exponent.
      Restrictions:  Handle to a valid polynomial object.
POSTCONDITION
Success
  - Reason:        All cases.
  - Summary:       The correct value is returned accordingly.
  - Return value:  TRUE if the polynomial contains a term with a negative exponent.
                   FALSE if otherwise.
  - hPoly:         The state of the polynomial before the function call is preserved.
Failure
  - N/A
*/
Boolean poly_existsNegExp(POLY hPoly);


/*
FUNCTION
  - Name:     poly_existsTermWithExp
  - Purpose:  Checks if a polynomial contains a term with a given exponent.
PRECONDITION
  - hPoly
      Purpose:       Polynomial to check if a term with the exponent exists.
      Restrictions:  Handle to a valid polynomial object.
  - exp
      Purpose:       Exponent of the term to look for.
      Restrictions:  None.
POSTCONDITION
Success
  - Reason:        All cases.
  - Summary:       The correct value is returned accordingly.
  - Return value:  TRUE if the polynomial contains a term with the exponent.
                   FALSE if otherwise.
  - hPoly:         The state of the polynomial before the function call is preserved.
Failure
  - N/A
*/
Boolean poly_existsTermWithExp(POLY hPoly, int exp);


/*
FUNCTION
  - Name:     poly_getCapacity
  - Purpose:  Get the capacity of a polynomial.
PRECONDITION
  - hPoly
      Purpose:       Polynomial to get the capacity of.
      Restrictions:  Handle to a valid polynomial object.
POSTCONDITION
Success
  - Reason:        All cases.
  - Summary:       Returns the capacity of the polynomial.
  - Return value:  The capacity of the polynomial.
  - hPoly:         The state of the polynomial before the function call is preserved.
Failure
  - N/A
*/
int poly_getCapacity(POLY hPoly);


/*
FUNCTION
  - Name:     poly_getCoeffOfExp
  - Purpose:  Get the coefficient of term with an exponent.
PRECONDITION
  - hPoly
      Purpose:       Polynomial for which to get the coefficient of the term with the exponent.
      Restrictions:  Handle to a valid polynomial object.
  - exp
      Purpose:       Exponent of the term whose coefficient should be returned.
	    Restrictions:  None.
  - pPolyHasNoTerms
      Purpose:       Indicate if the polynomial has no terms.
      Restrictions:  Not NULL.
  - pExpExists
      Purpose:       Indicate if a term with the exponent exists.
      Restrictions:  Not NULL.
POSTCONDITION
Success
  - Reason:           The polynomial has a term with the exponent.
  - Summary:          Returns the coefficient of the term with the exponent.
  - Return value:     The coefficient of the term with the exponent.
  - hPoly:            The state of the polynomial before the function call is preserved.
  - pPolyHasNoTerms:  The Boolean it points to is set to FALSE.
  - pExpExists:       The Boolean it points to is set to TRUE.
Failure
  - Reason:           The polynomial has no terms, or it has terms but there is no term with the exponent.
  - Summary:          The coefficient isn't returned but rather returns a special value and nothing of signifiance happens.
  - Return value:     0
  - hPoly:            The state of the polynomial before the function call is preserved.
  - pPolyHasNoTerms:  The Boolean it points to is set accordingly.
                        - TRUE if the polynomial has no terms.
                        - FALSE if otherwise.  
  - pExpExists:       The Boolean it points to is set to FALSE.
*/
double poly_getCoeffOfExp(POLY hPoly, int exp, Boolean* pPolyHasNoTerms, Boolean* pExpExists);


/*
FUNCTION
  - Name:     poly_getDegree
  - Purpose:  Gets the degree of a polynomial (highest exponent).
PRECONDITION
  - hPoly
      Purpose:       Polynomial to get the degree of.
      Restrictions:  Handle to a valid polynomial object.
  - pPolyHasNoTerms
      Purpose:       Indicate if the polynomial has no terms.
      Restrictions:  Not NULL.
POSTCONDITION
Success
  - Reason:           Polynomial has terms.
  - Summary:          Returns the degree of the polynomial.
  - Return value:     The degree of the polynomial.
  - hPoly:            The state of the polynomial before the function call is preserved.
  - pPolyHasNoTerms:  The Boolean it points to is set to FALSE.
Failure
  - Reason:           Polynomial has no terms.
  - Summary:          The degree of the polynomial isn't returned but rather returns a special value and nothing of signifiance happens.
  - Return value:     0
  - hPoly:            The state of the polynomial before the function call is preserved.
  - pPolyHasNoTerms:  The Boolean it points to is set to TRUE.
*/
int poly_getDegree(POLY hPoly, Boolean* pPolyHasNoTerms);


/*
FUNCTION
  - Name:     poly_getSize
  - Purpose:  Gets the size of a polynomial (number of terms).
PRECONDITION
  - hPoly
      Purpose:       Polynomial to get the size of.
      Restrictions:  Handle to a valid polynomial object.
POSTCONDITION
Success
  - Reason:        All cases.
  - Summary:       Returns the size of the polynomial.
  - Return value:  The size of the polynomial.
  - hPoly:         The state of the polynomial before the function call is preserved.
Failure
  - N/A
*/
int poly_getSize(POLY hPoly);


/*
FUNCTION
  - Name:     poly_hasNoTerms
  - Purpose:  Checks if a polynomial has no terms (size is 0).
PRECONDITION
  - hPoly
      Purpose:       Polynomial to check if it has no terms.
      Restrictions:  Handle to a valid polynomial object.
POSTCONDITION
Success
  - Reason:        All cases.
  - Summary:       The correct value is returned accordingly.
  - Return value:  TRUE if the polynomial has no terms.
                   FALSE if otherwise.
  - hPoly:         The state of the polynomial before the function call is preserved.
Failure
  - N/A
*/
Boolean poly_hasNoTerms(POLY hPoly);


/*
FUNCTION
  - Name:     poly_initCopy
  - Purpose:  Initializes a new polynomial that's a copy of another.
              C Opaque object design version of the copy constructor in C++.
PRECONDITION
  - hPolySrc
      Purpose:       Polynomial object to copy from.
      Restrictions:  Handle to a valid polynomial object.
POSTCONDITION
Success
  - Reason:        No memory allocation failure.
  - Summary:       Initializes and returns a new polynomial that's a copy of hPolySrc.
  - Return value:  Handle to a valid polynomial object that's a copy of hPolySrc.
  - hPolySrc:      The state of the polynomial before the function call is preserved.
Failure
  - Reason:        Memory allocation failure.
  - Summary:       Doesn't initialize and return a new polynomial and nothing of significance happens.
  - Return value:  NULL
  - hPolySrc:      The state of the polynomial before the function call is preserved.
*/
POLY poly_initCopy(POLY hPolySrc);


/*
FUNCTION
  - Name:     poly_initDefault
  - Purpose:  Initializes a new polynomial in a default empty state (no terms).
              C Opaque object design version of the default constructor in C++.
PRECONDITION
  - N/A
POSTCONDITION
Success
  - Reason:        No memory allocation failure.
  - Summary:       Initializes and returns a new polynomial in a default empty state.
  - Return value:  Handle to a valid polynomial object in a default empty state.
Failure
  - Reason:        Memory allocation failure.
  - Summary:       Doesn't initialize and return a new polynomial and nothing of significance happens.
  - Return value:  NULL
*/
POLY poly_initDefault(void);


/*
FUNCTION
  - Name:     poly_initMove
  - Purpose:  Initialize a new polynomial handle by moving an existing polynomial in a source handle into the new handle.
              It is a transfer of ownership of the polynomial and not a copy.
              C Opaque object design version of the move constructor in C++.
PRECONDITION
  - phPolySrc
      Purpose:       Pointer to the polynomial handle to move from.
      Restrictions:  Pointer to a handle to a valid polynomial object or NULL handle.
POSTCONDITION
Success
  - Reason:        The handle pointed to by phPolySrc is a handle to a valid polynomial object.
  - Summary:       Initializes a new polynomial handle by moving an existing polynomial in a source handle into the new handle.
                   The source handle loses ownership of the polynomial object.
  - Return value:  Handle to a valid polynomial object that is the same polynomial of the handle pointed to by phPolySrc.
  - phPolySrc:     The handle it points to loses ownership of the polynomial and is set to NULL.
Failure
  - Reason:        The handle pointed to by phPolySrc is a NULL handle.
  - Summary:       Doesn't initialize a new polynomial handle and nothing of significance happens.
  - Return value:  NULL
  - phPolySrc:     The handle it points to remains NULL.
*/
POLY poly_initMove(POLY* phPolySrc);


/*
FUNCTION
  - Name:     poly_initPolyStr
  - Purpose:  Initializes a new polynomial based on polynomial string.
              C Opaque object design version of a custom constructor in C++.
PRECONDITION
  - polyStr
      Purpose:       Polynomial string to initialize a polynomial object.
      Restrictions:  None.
  - pPolyStrIsValid
      Purpose:       Indicate if the polynomial string is valid.
      Restrictions:  Not NULL.
POSTCONDITION
Success
  - Reason:          No memory allocation failure and the polynomial string is valid.
  - Summary:         Initializes and returns a new polynomial based on polynomial string.
  - Return value:    Handle to a valid polynomial object initialized based on the polynomial string.
                     The terms in the new polynomial match the terms in the polynomial string.
                     Terms with the same exponent in the polynomial string are combined into one term in the polynomial.
  - polyStrIsValid:  The Boolean it points to is set to TRUE.
Failure
  - Reason:           Memory allocation failure or the polynomial string is invalid.
  - Summary:          Doesn't initialize and return a new polynomial object based on the polynomial string and nothing of significance happens.
  - pPolyStrIsValid:  The Boolean it points to is set accordingly.
                        - TRUE if polynomial string is valid.
                        - FALSE if otherwise.
  - Return value:     NULL
EXAMPLES
  - polynomial string has no terms with the same exponent
      polynomial string: "x^2 + 1"          polynomial object: x^2 + 1
  - polynomial string has terms with the same exponent
      polynomial string: "x^2 + x^2 + 1"    polynomial object: 2x^2 + 1
*/
POLY poly_initPolyStr(const char* polyStr, Boolean* pPolyStrIsValid);


/*
FUNCTION
  - Name:     poly_isValidPolyStr
  - Purpose:  Checks if a string is a valid polynomial string.
PRECONDITION
  - polyStr
      Purpose:       String to check if it's a valid polynomial string.
      Restrictions:  None.
POSTCONDITION
Success
  - Reason:        All cases.
  - Summary:       The correct value is returned accordingly.
  - Return value:  TRUE if the string is a valid polynomial string.
                   FALSE if otherwise.
Failure
  - N/A
*/
Boolean poly_isValidPolyStr(const char* polyStr);


/*
FUNCTION
  - Name:     poly_move
  - Purpose:  Move an existing polynomial in a source handle into an existing destination handle.
              It is a transfer of ownership of the polynomial and not a copy.
              C Opaque object design version of the move assignment operator in C++.
PRECONDITION
  - phPolyDest
      Purpose:       Pointer to the polynomial handle to move to.
      Restrictions:  Pointer to a handle to a valid polynomial object or NULL handle.
  - phPolySrc
      Purpose:       Pointer to the polynomial handle to move from.
      Restrictions:  Pointer to a handle to a valid polynomial object or NULL handle.
POSTCONDITION
Success
  - Reason:        The handle pointed to by phPolySrc is a handle to a valid polynomial object.
  - Summary:       Move the polynomial in the source handle pointed to by phPolySrc into the destination handle pointed to by phPolyDest.
                   The source handle pointed to by phPolySrc loses ownership of the polynomial object.
  - Return value:  SUCCESS
  - phPolyDest:    The handle it points to stores the polynomial in the source handle pointed to by phPolySrc.
                   If prior to the move it is a pointer to a handle to a valid polynomial object and not a NULL handle, the polynomial object is destroyed.
  - phPolySrc:     The handle it points to has its polynomial moved to the destination handle pointed to by phPolyDest.
                   It then loses ownership of the polynomial by being set to NULL.
Failure
  - Reason:        The handle pointed to by phPolySrc is a NULL handle.
  - Summary:       There is no move and nothing of significance happens.
  - Return value:  FAILURE
  - phPolyDest:    If it is a pointer to a handle to a valid polynomial object, the state of the polynomial before the function call is preserved.
                   If it is a pointer to a NULL handle, the handle it points to remains NULL.
  - phPolySrc:     The handle it points to remains NULL.
*/
Status poly_move(POLY* phPolyDest, POLY* phPolySrc);


/*
FUNCTION
  - Name:     poly_newPoly
  - Purpose:  Creates a new polynomial with an existing polynomial object using a polynomial string.
PRECONDITION
  - hPoly
      Purpose:       Polynomial to create a new polynomial with.
      Restrictions:  Handle to a valid polynomial object.
  - polyStr
      Purpose:       Polynomial string containing the new polynomial.
      Restrictions:  None.
  - pPolyStrIsValid
      Purpose:       Indicate if the polynomial string is valid.
      Restrictions:  Not NULL.
POSTCONDITION
Success 
  - Reason:           No memory allocation failure.
  - Summary:          Creates a new polynomial in the existing polynomial object based on the polynomial string.
  - Return value:     SUCCESS
  - hPoly:            The new polynomial from the polynomial string is stored in the polynomial object.
  - pPolyStrIsValid:  The Boolean it points to is set to TRUE.
Failure
  - Reason:           Memory allocation failure or the polynomial string is invalid.
  - Summary:          Doesn't create a new polynomial based on the polynomial string and nothing of signifiance happens.
  - Return value:     FAILURE
  - hPoly:            The state of the polynomial depends.
                        - If it failed because the polynomial string is invalid, the state of the polynomial before the function call is preserved.
                        - If otherwise, the state of the polynomial before the function call is not guaranteed to be preserved.
  - pPolyStrIsValid:  The Boolean it points to is set accordingly.
                        - TRUE if polynomial string is valid.
                        - FALSE if otherwise.
EXAMPLES
  - All terms have unique exponent
      polynomial string: "x^2 + x + 1"               polynomial object: x^2 + x + 1
  - All terms don't have unique exponent
      polynomial string: "x^2 + 2x^2 + x - x + 1"    polynomial object: 3x^2 + 1
  - Special cases
      polynomial string: "x" or "x^1"                polynomial object: x
      polynomial string: "x^0" or  x^-0"             polynomial object: 1
      polynomial string: "3x^0" or "3x^-0"           polynomial object: 3
      polynomial string: "-x" or "-1x"               polynomial object: -x
*/
Status poly_newPoly(POLY hPoly, const char* polyStr, Boolean* pPolyStrIsValid);


/*
FUNCTION
  - Name:     poly_print
  - Purpose:  Prints the terms of a polynomial to stdout.
PRECONDITION
  - hPoly
      Purpose:       Polynomial to print.
      Restrictions:  Handle to a valid polynomial object.
POSTCONDITION
Success
  - Reason:        The polynomial has terms.
  - Summary:       Prints out the terms of the polynomial to stdout.
  - Return value:  SUCCESS
  - hPoly:         The state of the polynomial before the function call is preserved.
Failure
  - Reason:        The polynomial has no terms.
  - Summary:       Nothing of significance happens.
  - Return value:  FAILURE
  - hPoly:         The state of the polynomial before the function call is preserved.
*/
Status poly_print(POLY hPoly);


/*
FUNCTION
  - Name:     poly_removeTermWithExp
  - Purpose:  Removes a term from a polynomial with a given exponent if it exists.
PRECONDITION
  - hPoly
      Purpose:       Polynomial to remove a term from.
      Restrictions:  Handle to a valid polynomial object.
  - exp
      Purpose:       Exponent of term to remove.
      Restrictions:  None.
POSTCONDITION
Success
  - Reason:        A term with the exponent exists.
  - Summary:       Removes the term with exponent from the polynomial.
  - Return value:  SUCCESS
  - hPoly:         The term is removed from the polynomial.
Failure
  - Reason:        A term with the exponent doesn't exist.
  - Summary:       Doesn't remove the term from the polynomial and nothing of signifiance happens.
  - Return value:  FAILURE
  - hPoly:         The state of the polynomial before the function call is preserved.
*/
Status poly_removeTermWithExp(POLY hPoly, int exp);


/*
FUNCTION
  - Name:     poly_reset
  - Purpose:  Resets a polynomial by removing all of its terms.
PRECONDITION
  - hPoly
      Purpose:       Polynomial to reset.
      Restrictions:  Handle to a valid polynomial object.
POSTCONDITION
Success
  - Reason:        All cases.
  - Summary:       Resets the polynomial by removing all of its terms.
  - Return value:  N/A
  - hPoly:         The polynomial is reset by removing all of its terms (sets the size to 0).
Failure
  - N/A
*/
void poly_reset(POLY hPoly);


/*
FUNCTION
  - Name:     poly_sort
  - Purpose:  Sorts the terms of a polynomial in descending order of exponent.
PRECONDITION
  - hPoly
      Purpose:       Polynomial to sort.
      Restrictions:  Handle to a valid polynomial object.
POSTCONDITION
Success
  - Reason:        All cases.
  - Summary:       Sorts the terms of the polynomial in descending order of exponent.
  - Return value:  N/A
  - hPoly:         The terms of the polynomial are sorted in descending order of exponent.
Failure
  - N/A
EXAMPLES
  - hPoly before sort: x^-4 - x + 1 + x^2    hPoly after sort: x^2 - x + 1 + x^-4
*/
void poly_sort(POLY hPoly);


#endif