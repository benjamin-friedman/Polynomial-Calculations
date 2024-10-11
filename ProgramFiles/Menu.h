/*
  Author:       Benjamin G. Friedman
  Date:         01/02/2022
  File:         Menu.h
  Description:  Header file for the menu interface.
*/


#ifndef MENU_H
#define MENU_H

#include "Poly.h"

typedef enum menuOption {
    QUIT,
    X_VALUE,
    NTH_DERIV,
    NTH_DERIV_X_VALUE,
    INDEF_INTEGRAL,
    DEF_INTEGRAL
} MenuOption;




/*
NOTES
  - Name:     menu_calcPolyDefIntegral
  - Purpose:  Implements the polynomial definite integral calulation.
PRECONDITION
  - User has selected to calculate the definite integral of a polynomial.
POSTCONDITION
Success
  - Reason:        No memory allocation failure.
  - Summary:       Implements the polynomial definite integral calculation.
                   Prompts the user to enter a polynomial and bounds of the definite integral, calculates the definite integral, and displays the results.
  - Return value:  SUCCESS
Failure
  - Reason:        Memory allocation failure.
  - Summary:       Doesn't implement the polynomial definite integral calculation.
                   Much or little could've happened up until the point of failure because it can happen on numerous occasions.
  - Return value:  FAILURE
*/
Status menu_calcPolyDefIntegral(void);


/*
NOTES
  - Name:     menu_calcPolyIndefIntegral
  - Purpose:  Implements the polynomial indefinite integral calulation.
PRECONDITION
  - User has selected to calculate the indefinite integral of a polynomial.
POSTCONDITION
Success
  - Reason:        No memory allocation failure.
  - Summary:       Implements the polynomial indefinite integral calculation.
                   Prompts the user to enter a polynomial, calculates the indefinite integral, and displays the results.
  - Return value:  SUCCESS
Failure
  - Reason:        Memory allocation failure.
  - Summary:       Doesn't implement the polynomial indefinite integral calculation.
                   Much or little could've happened up until the point of failure because it can happen on numerous occasions.
  - Return value:  FAILURE
*/
Status menu_calcPolyIndefIntegral(void);


/*
NOTES
  - Name:     menu_calcPolyNthDeriv
  - Purpose:  Implements the polynomial nth derivative calulation.
PRECONDITION
  - User has selected to calculate the nth derivative of a polynomial.
POSTCONDITION
Success
  - Reason:        No memory allocation failure.
  - Summary:       Implements the polynomial nth derivative calculation.
                   Prompts the user to enter a polynomial and number of derivatives, calculates the nth derivative, and displays the results.
  - Return value:  SUCCESS
Failure
  - Reason:        Memory allocation failure.
  - Summary:       Doesn't implement the polynomial nth derivative calculation.
                   Much or little could've happened up until the point of failure because it can happen on numerous occasions.
  - Return value:  FAILURE
*/
Status menu_calcPolyNthDeriv(void);


/*
NOTES
  - Name:     menu_calcPolyNthDerivXValue
  - Purpose:  Implements the polynomial nth derivative x-value calulation.
PRECONDITION
  - User has selected to calculate the nth derivative of a polynomial at an x-value.
POSTCONDITION
Success
  - Reason:        No memory allocation failure.
  - Summary:       Implements the polynomial nth derivative x-value calculation.
                   Prompts the user to enter a polynomial, number of derivatives, and x-value, calculates the nth derivative at the x-value, and displays the results.
  - Return value:  SUCCESS
Failure
  - Reason:        Memory allocation failure.
  - Summary:       Doesn't implement the polynomial nth derivative x-value calculation.
                   Much or little could've happened up until the point of failure because it can happen on numerous occasions.
  - Return value:  FAILURE
*/
Status menu_calcPolyNthDerivXValue(void);


/*
NOTES
  - Name:     menu_calcPolyXValue
  - Purpose:  Implements the polynomial x-value calulation.
PRECONDITION
  - User has selected to calculate the polynomial at an x-value.
POSTCONDITION
Success
  - Reason:        No memory allocation failure.
  - Summary:       Implements the polynomial x-value calculation.
                   Prompts the user to enter a polynomial and x-value, calculates the polynomial at the x-value, and displays the results.
  - Return value:  SUCCESS
Failure
  - Reason:        Memory allocation failure.
  - Summary:       Doesn't implement the polynomial x-value calculation.
                   Much or little could've happened up until the point of failure because it can happen on numerous occasions.
  - Return value:  FAILURE
*/
Status menu_calcPolyXValue(void);


/*
NOTES
  - Name:     menu_getUserChoice
  - Purpose:  Gets the user's choice from the menu (polynomial calcluation to perform or quit).
PRECONDITION
  - A new round has started in the program.
POSTCONDITION
Success
  - Reason:        All cases.
  - Summary:       Prompts the user to enter one of the numerical menu options, validates the input, and returns the user's choice.
  - Return value:  User's choice.
Failure
  - N/A
*/
MenuOption menu_getUserChoice(void);


/*
NOTES
  - Name:     menu_implementUserChoice
  - Purpose:  Implements the user's choice from the menu (polynomial calcluation to perform or quit).
PRECONDITION
  - userChoice
      Purpose:       Stores the user's choice for which polynomial calculation to implement or quit the program.
      Restrictions:  None.
POSTCONDITION
Success
  - Reason:        No memory allocation failure.
  - Summary:       Implements the user's choice.
  - Return value:  SUCCESS
Failure
  - Reason:        Memory allocation FAILURE.
  - Summary:       Doesn't implement the user's choice.
                   Much or little could've happened up until the point of failure because it can happen on numerous occasions.
  - Return value:  FAILURE
*/
Status menu_implementUserChoice(MenuOption userChoice);


#endif