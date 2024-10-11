/*
  Author:       Benjamin G. Friedman
  Date:         01/02/2022
  File:         Menu.c
  Description:  Implementation file for the menu interface.
*/


#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Menu.h"


typedef struct menuOptionMessages {
	MenuOption menuOption;
	char message[100];
} MenuOptionMessages;

const MenuOptionMessages menuOptionMessages[] = {
	{ QUIT, "Quit" },
	{ X_VALUE, "The value of a polynomial at an x-value" },
	{ NTH_DERIV, "The nth derivative of a polynomial" },
	{ NTH_DERIV_X_VALUE, "The value of an nth derivative of a polynomial at an x-value" },
	{ INDEF_INTEGRAL, "The indefinite integral of a polynomial" },
	{ DEF_INTEGRAL, "The definite integral of a polynomial" },
};
const int menuOptionMessagesSize = sizeof(menuOptionMessages) / sizeof(*menuOptionMessages);

#define POLY_BUFFER_CAP 500




/*********** Declarations for helper functions defined in Polynomial.c **********/
Boolean inputsAreValidDoubles(const char* input, int expectedNums);
Boolean inputsAreValidInts(const char* input, int expectedNums);




/*********** Declarations for helper functions defined in this file **********/
/*
FUNCTION
  - Name:     createOrdinalNum
  - Purpose:  Create an ordinal number as a string from a given integer.
              An ordinal number is a number with two characters appended to it to be used in a sentence.
              For example, the ordinal number of 1 is 1st.
PRECONDITION
  - ordNum
      Purpose:       Array of characters to hold the string representation of the ordinal number.
      Restrictions:  Capacity is large enough to hold the ordinal number as a string.
  - num
      Purpose:       Number from which to create the ordinal number.
      Restrictions:  N/A
POSTCONDITION
Success
  - Reason:        All cases.
  - Summary:       Creates an ordinal number as a string from a given integer.
  - Return value:  N/A
  - ordNum:        The ordinal number of the number as a string is stored in the array.
Failure
  - N/A
*/
static void createOrdinalNum(char* ordNum, int num);


/*
FUNCTION
  - Name:     displayResultsPolyDefIntegral
  - Purpose:  Display the results of the polynomial definite integral calculation.
PRECONDITION
  - The polynomial definite integral calculation happened.
  - hPoly
Purpose:  Polynomial used in the calculation.
	  Restrictions:  Handle to a valid polynomial object.
  - hPolyOrig
      Purpose:       Polynomial before the calculation happened.
      Restrictions:  Handle to a valid polynomial object and has at least one term.
  - LB
      Purpose:       Lower bound of the definite integral.
      Restrictions:  None.
  - UB
      Purpose:       Upper bound of the definite integral.
      Restrictions:  None.
  - result
      Purpose:       Result of the definite integral.
      Restrictions:  None.
  - expNegOneIntegrated
      Purpose:       Indicate if a term with an exponent of -1 was integrated.
      Restrictions:  Not NULL.
  - coeffExpNegOne
      Purpose:       Store the coefficient of the term with an exponent of -1 that was integrated if it exists.
      Restrictions:  Not NULL.
POSTCONDITION
Success
  - Reason:        All cases.
  - Summary:       Displays the results of the polynomial definite integral calculation.
  - Return value:  N/A
Failure
  - N/A
*/
static void displayResultsPolyDefIntegral(POLY hPoly, POLY hPolyOrig, double LB, double UB, double result, Boolean expNegOneIntegrated, double coeffExpNegOne);


/*
FUNCTION
  - Name:     displayResultsPolyIndefIntegral
  - Purpose:  Display the results of the polynomial indefinite integral calculation.
PRECONDITION
  - The polynomial indefinite integral calculation happened.
  - hPoly
      Purpose:       Polynomial used in the calculation.
      Restrictions:  Handle to a valid polynomial object.
  - hPolyOrig
      Purpose:       Polynomial before the calculation happened.
      Restrictions:  Handle to a valid polynomial object and has at least one term.
  - expNegOneIntegrated
      Purpose:       Indicate if a term with an exponent of -1 was integrated.
      Restrictions:  None.
  - coeffExpNegOne
      Purpose:       Store the coefficient of the term that had an exponent of -1 that was integrated if it existed.
      Restrictions:  None.
POSTCONDITION
Success
  - Reason:        All cases.
  - Summary:       Displays the results of the polynomial indefinite integral calculation.
  - Return value:  N/A
Failure
  - N/A
*/
static void displayResultsPolyIndefIntegral(POLY hPoly, POLY hPolyOrig, Boolean expNegOneIntegrated, double coeffExpNegOne);


/*
FUNCTION
  - Name:     displayResultsPolyNthDeriv
  - Purpose:  Display the results of the polynomial nth derivative calculation.
PRECONDITION
  - The polynomial nth derivative calculation happened.
  - hPoly
      Purpose:       Polynomial used in the calculation.
      Restrictions:  Handle to a valid polynomial object.
  - hPolyOrig
      Purpose:       Polynomial before the calculation happened.
      Restrictions:  Handle to a valid polynomial object and has at least one term.
  - n 
      Purpose:       Number of derivatives calculated.
      Restrictions:  Any integer > 0.
  - nthDerivIsZero
      Purpose:       Indicate if the result of the calculation was 0.
      Restrictions:  None.
POSTCONDITION
Success
  - Reason:        All cases.
  - Summary:       Displays the results of the polynomial nth derivative calculation.
  - Return value:  N/A
Failure
  - N/A
*/
static void displayResultsPolyNthDeriv(POLY hPoly, POLY hPolyOrig, int n, Boolean nthDerivIsZero);


/*
FUNCTION
  - Name:     displayResultsPolyNthDerivXValue
  - Purpose:  Display the results of the polynomial nth derivative x-value calculation.
PRECONDITION
  - The polynomial nth derivative and x-value calculation happened.
  - hPoly
      Purpose:       Polynomial used in the calculation.
      Restrictions:  Handle to a valid polynomial object.
  - hPolyOrig
      Purpose:       Polynomial before the calculation happened.
      Restrictions:  Handle to a valid polynomial object that has at least one term.
  - x 
      Purpose:       x-value used in the calculation.
      Restrictions:  If the polynomial has at least one term with a negative exponent, it isn't 0.
  - n
      Purpose:       Number of derivatives calculated.
      Restrictions   Any integer > 0.
  - result
      Purpose:       The result of the calculation.
      Restrictions:  None.
  - nthDerivIsZero
      Purpose:       Indicate if the result of the calculation was 0.
      Restrictions:  None.
POSTCONDITION
Success
  - Reason:        All cases.
  - Summary:       Displays the results of the polynomial nth derivative x-value calculation.
  - Return value:  N/A
Failure
  - N/A
*/
static void displayResultsPolyNthDerivXValue(POLY hPoly, POLY hPolyOrig, double x, double result, int n, Boolean nthDerivIsZero);


/*
FUNCTION
  - Name:     displayResultsPolyXValue
  - Purpose:  Display the results of the polynomial x-value calculation.
PRECONDITION
  - The polynomial x-value calculation happened.
  - hPoly
      Purpose:       Polynomial used in the calculation.
      Restrictions:  Handle to a valid polynomial object that has at least one term.
  - x
      Purpose:       x-value used in the calculation.
      Restrictions:  If the polynomial has at least one term with a negative exponent, it isn't 0.
  - result
      Purpose:       The result of the calculation.
      Restrictions:  None.
POSTCONDITION
Success
  - Reason:        All cases.
  - Summary:       Displays the results of the polynomial x-value calculation.
  - Return value:  N/A
Failure
  - N/A
*/
static void displayResultsPolyXValue(POLY hPoly, double x, double result);


/*
FUNCTION
  - Name:     displayRoundMessage
  - Purpose:  Displays the menu each round prompting the user to select a polynomial calculation or quit.
PRECONDITION
  - menuOptions
      Purpose:       Contains the messages to display for the various menu options.
      Restrictions:  None.
  - menuOptionMessgesSize
      Purpose:       The size of menuOptionsMessages.
      Restrictions:  Equals the actual size of menuOptionsMessages.
POSTCONDITION
Success
  - Reason:        All cases.
  - Summary:       Displays the round message.
  - Return value:  N/A
Failure
  - N/A
*/
static void displayRoundMessage(const MenuOptionMessages menuOptionMessages[], int menuOptionMessagesSize);


/*
FUNCTION
  - Name:     inputsAreValidPositiveInts
  - Purpose:  Checks if a string contains a specific amount of valid positive integers.
PRECONDITION
  - input
      Purpose:       String to check if it contains only a specific amount of valid positive integers.
      Restrictions:  None.
  - expectedNums
      Purpose:       The expected amount of positive integers to be found in the input string.
      Restrictions:  Any integer > 0.
POSTCONDITION
Success
  - Reason:        All cases.
  - Summary:       The correct value is returned accordingly.
  - Return value:  TRUE if input only contains valid positive integers and the amount of positive integers equals expectedNums.
                   FALSE if otherwise.
Failure
  - N/A
*/
static Boolean inputsAreValidPositiveInts(const char* input, int expectedNums);


/*
FUNCTION
  - Name:     userInputGetBoundsOfDefIntegral
  - Purpose:  Gets the lower and uppber bounds for a definite integral from the user.
PRECONDITION
  - pLB
      Purpose:       Store the lower bound of the definite integral.
      Restrictions:  Not NULL.
  - pUB
      Purpose:       Store the upper bound of the definite integral.
      Restrictions:  Not NULL.
POSTCONDITION
  - Reason:        All cases.
  - Summary:       Prompts the user to enter the lower and upper bounds, validates that the inputs are valid doubles, and stores the inputs in the lower and upper bounds.
  - Return value:  N/A
  - pLB:           The double it points to stores the lower bound.
  - pUB:           The double it points to store the upper bound.
Failure
  - N/A
*/
static void userInputGetBoundsOfDefIntegral(double* pLB, double* pUB);


/*
FUNCTION
  - Name:     userInputGetN
  - Purpose:  Gets the number of derivatives to calculate from the user.
PRECONDITION
  - N/A
POSTCONDITION
Success
  - Reason:        All cases.
  - Summary:       Prompts the user to enter the the number of derivatives to calculate, validates that the input is a valid integer > 0, and returns the number.
  - Return value:  The number of derivatives to calculate.
Failure
  - N/A
*/
static int userInputGetN(void);


/*
FUNCTION
  - Name:     userInputGetPolyStr
  - Purpose:  Gets a polynomial from the user.
PRECONDITION
  - polyStr
      Purpose:       Array of characters to hold the the polynomial string.
      Restrictions:  Capacity is large enough to hold the polynomial string.
  - polyCap
      Purpose:       The capacity of the array of characters to hold the polynomial string.
      Restrictions:  Equals the actual capacity of the array of characters to hold the polynomial string.
  - prompt
      Purpose:       Message to display that is used in the prompt for the user to enter a polynomial.
      Restrictions:  None.
POSTCONDITION
Success
  - Reason:        All cases.
  - Summary:       Prompts the user to enter a polynomial, validates that the input is a valid polynomial, and stores it as a polynomial string in the array of characters.
                   If more characters are entered than can fit based on the capacity, then they are ignored.
  - Return value:  N/A
  - polyStr:       The polynomial entered by the user is stored in it as a polynomial string.
Failure
  - N/A
*/
static void userInputGetPolyStr(char* polyStr, int polyStrCap, const char* prompt);


/*
FUNCTION
  - Name:     userInputGetX
  - Purpose:  Gets an x-value from the user.
PRECONDITION
  - N/A
POSTCONDITION
Success
  - Reason:        All cases.
  - Summary:       Prompts the user to enter the the x-value, validates that the input is a valid double, and returns the number.
  - Return value:  The x-value.
Failure
  - N/A
*/
static double userInputGetX(void);




/********** Definitions for menu interface functions declared in Menu.h **********/
Status menu_calcPolyDefIntegral(void) {
	POLY hPoly;                             // polynomial for definite integral
	POLY hPolyOrig;                         // polynomial before definite integral
	char polyStr[POLY_BUFFER_CAP];          // polynomial string - user input
	Boolean polyStrIsValid;                 // not actually used but needed for function call (validation done in userInputGetPolyStr)
	double LB;                              // lower bound of definite integral - user input
	double UB;                              // upper bound of definite integral - user input
	double result;                          // result of definite integral
	Boolean expNegOneIntegrated = FALSE;    // indicates if a term with an exponent of -1 gets integrated
	double coeffExpNegOne = 0;              // coefficient of term with exponent of -1 if it exists
	Boolean polyHasNoTerms = FALSE;         // indicates if the polynomial has no terms
	Boolean divByZeroError = FALSE;         // indicates if there is a division by zero error when integrating
	Boolean natLogError = FALSE;            // indicates if there is a natural logarithm error when integrating
	Boolean calcIsSuccessful;               // indicates if the calculation is successful


	// initialize polynomials
	hPoly = poly_initDefault();
	if (!hPoly)
		return FAILURE;

	hPolyOrig = poly_initDefault();
	if (!hPolyOrig) {
		poly_destroy(&hPoly);
		return FAILURE;
	}

	do {
		// get polynomial from user input
		userInputGetPolyStr(polyStr, POLY_BUFFER_CAP, "Enter the polynomial to calculate the definite integral.");
		
		// create new polynomials with user input
		if (!poly_newPoly(hPoly, polyStr, &polyStrIsValid)) {
			poly_destroy(&hPoly);
			poly_destroy(&hPolyOrig);
			return FAILURE;
		}
		if (!poly_newPoly(hPolyOrig, polyStr, &polyStrIsValid)) {
			poly_destroy(&hPoly);
			poly_destroy(&hPolyOrig);
			return FAILURE;
		}

		// get lower and upper bound from user input
		userInputGetBoundsOfDefIntegral(&LB, &UB);

		// calculate definite integral
		// the polynomial is guaranteed to not be empty so if the calculation fails here it's from division by zero or natural logarithm error
		if (!poly_calcDefIntegral(hPoly, LB, UB, &result, &expNegOneIntegrated, &coeffExpNegOne, 
			&polyHasNoTerms, &divByZeroError, &natLogError)) {
			calcIsSuccessful = FALSE;

			// division by zero and natural logarithm error
			if (divByZeroError && natLogError) {
				printf("Error - division by zero and natural logarithm errors.\n"
					"- The the polynomial has at least one term with a negative exponent that is not -1, and the range of the lower and upper bound includes 0. "
					"During the definite integral calculation, this results in division by zero which is undefined.\n"
					"- The polynomial has a term with an exponent of -1, and the range of the lower and upper bound includes 0. "
					"During the definite integral calculation, this results in taking the natural logarithm of zero which is undefined.\n");
			}
			// only division by zero error
			else if (divByZeroError) {
				printf("Error - division by zero error. "
					"The the polynomial has at least one term with a negative exponent that is not -1, and the range of the lower and upper bound includes 0. "
					"During the definite integral calculation, this results in division by zero which is undefined.\n");
			}
			// only natural logarithm error
			else if (natLogError) {
				printf("Error - natural logarithm error. "
					"The polynomial has a term with an exponent of -1, and the range of the lower and upper bound includes 0. "
					"During the definite integral calculation, this results in taking the natural logarithm of zero which is undefined.\n");
			}
		}
		else {
			calcIsSuccessful = TRUE;
		}
	} while (!calcIsSuccessful);

	// display results
	poly_sort(hPoly);
	poly_sort(hPolyOrig);
	displayResultsPolyDefIntegral(hPoly, hPolyOrig, LB, UB, result, expNegOneIntegrated, coeffExpNegOne);

	// clean up memory
	poly_destroy(&hPoly);
	poly_destroy(&hPolyOrig);

	return SUCCESS;
}


Status menu_calcPolyIndefIntegral(void) {
	POLY hPoly;                             // polynomial for indefinite integral
	POLY hPolyOrig;                         // polynomial before indefinite integral
	char polyStr[POLY_BUFFER_CAP];          // polynomial string - user input
	Boolean polyStrIsValid;                 // not actually used but needed for function call (validation done in userInputGetPolyStr)
	double coeffExpNegOne = 0;              // coefficient of term with exponent of -1 if it exists
	Boolean expNegOneIntegrated = FALSE;    // indicates if a term with an exponent of -1 gets integrated
	

	// get polynomial from user input
	userInputGetPolyStr(polyStr, POLY_BUFFER_CAP, "Enter the polynomial to calculate the indefinite integral.");
	
	// initialize polynomials with user input
	hPoly = poly_initPolyStr(polyStr, &polyStrIsValid);
	if (!hPoly)
		return FAILURE;

	hPolyOrig = poly_initPolyStr(polyStr, &polyStrIsValid);
	if (!hPolyOrig) {
		poly_destroy(&hPoly);
		return FAILURE;
	}

	// calculate indefinite integral
	poly_calcIndefIntegral(hPoly, &expNegOneIntegrated, &coeffExpNegOne);

	// display results
	poly_sort(hPoly);
	poly_sort(hPolyOrig);
	displayResultsPolyIndefIntegral(hPoly, hPolyOrig, expNegOneIntegrated, coeffExpNegOne);
	
	// clean up memory
	poly_destroy(&hPoly);
	poly_destroy(&hPolyOrig);

	return SUCCESS;
}


Status menu_calcPolyNthDeriv(void) {
	POLY hPoly;                        // polynomial for nth derivative calculation
	POLY hPolyOrig;                    // polynomial before nth derivative calculation
	char polyStr[POLY_BUFFER_CAP];     // polynomial string - user input
	Boolean polyStrIsValid;            // not actually used but needed for function call (validation done in userInputGetPolyStr)
	int n;                             // number of derivatives to calculate - user input
	Boolean nthDerivIsZero = FALSE;    // indicates if the nth derivative is 0
	

	// get polynomial from user input
	userInputGetPolyStr(polyStr, POLY_BUFFER_CAP, "Enter the polynomial to calculate the nth derivative.");

	// initialize polynomials with user input
	hPoly = poly_initPolyStr(polyStr, &polyStrIsValid);
	if (!hPoly)
		return FAILURE;

	hPolyOrig = poly_initPolyStr(polyStr, &polyStrIsValid);
	if (!hPolyOrig) {
		poly_destroy(&hPoly);
		return FAILURE;
	}

	// get number of derivatives from user input
	n = userInputGetN();
	
	// calculate nth derivative
	poly_calcNthDeriv(hPoly, n, &nthDerivIsZero);

	// display results
	poly_sort(hPoly);
	poly_sort(hPolyOrig);
	displayResultsPolyNthDeriv(hPoly, hPolyOrig, n, nthDerivIsZero);

	// clean up memory
	poly_destroy(&hPoly);
	poly_destroy(&hPolyOrig);

	return SUCCESS;
}


Status menu_calcPolyNthDerivXValue(void) {
	POLY hPoly;                        // polynomial for nth derivative and x-value calculation
	POLY hPolyOrig;                    // polynomial before nth derivative and x-value calculation
	char polyStr[POLY_BUFFER_CAP];     // polynomial string  - user input
	Boolean polyStrIsValid;            // not actually used but needed for function call (validation done in userInputGetPolyStr)
	int n;                             // number of derivatives to calculate - user input
	double x;                          // x-value - user input
	double result;                     // result of nth derivative and x-value calculation
	Boolean nthDerivIsZero = FALSE;    // indicates if the nth derivative is 0
	Boolean polyHasNoTerms = FALSE;    // indicates if the polynomial has no terms
	Boolean calcIsSuccessful;          // indicates if the calculation is successful


	// initialize polynomials
	hPoly = poly_initDefault();
	if (!hPoly)
		return FAILURE;

	hPolyOrig = poly_initDefault();
	if (!hPolyOrig) {
		poly_destroy(&hPoly);
		return FAILURE;
	}
		
	do {
		// get polynomial from user input
		userInputGetPolyStr(polyStr, POLY_BUFFER_CAP, "Enter the polynomial to calculate the nth derivative at an x-value.");

		// create new polynomials with user input
		if (!poly_newPoly(hPoly, polyStr, &polyStrIsValid)) {
			poly_destroy(&hPoly);
			poly_destroy(&hPolyOrig);
			return FAILURE;
		}
		if (!poly_newPoly(hPolyOrig, polyStr, &polyStrIsValid)) {
			poly_destroy(&hPoly);
			poly_destroy(&hPolyOrig);
			return FAILURE;
		}

		// get number of derivatives from user input
		n = userInputGetN();

		// get x-value from user input
		x = userInputGetX();

		// calculate nth derivative
		poly_calcNthDeriv(hPoly, n, &nthDerivIsZero);

		// calculate with the x-value
		// the polynomial is guaranteed to not be empty so if the calculation fails here it's from division by zero
		if (!poly_calcXValue(hPoly, x, &result, &polyHasNoTerms)) {
			calcIsSuccessful = FALSE;
			printf("Error - the nth derivative of the polynomial has at least one negative exponent and cannot be summed with an x-value of 0 due to division by zero.\n");
		}
		else {
			calcIsSuccessful = TRUE;
		}
	} while (!calcIsSuccessful);

	// display results
	poly_sort(hPoly);
	poly_sort(hPolyOrig);
	displayResultsPolyNthDerivXValue(hPoly, hPolyOrig, x, result, n, nthDerivIsZero);

	// clean up memory
	poly_destroy(&hPoly);
	poly_destroy(&hPolyOrig);

	return SUCCESS;
}


Status menu_calcPolyXValue(void) {
	POLY hPoly;                        // polynomial for x-value calculation
	char polyStr[POLY_BUFFER_CAP];     // polynomial string - user input
	Boolean polyStrIsValid;            // not actually used but needed for function call (validation done in userInputGetPolyStr)
	double x;                          // x-value - user input
	double result;                     // result of x-value calculation
	Boolean polyHasNoTerms = FALSE;    // indicates if the polynomial has no terms
	Boolean calcIsSuccessful;          // indicates if the calculation is successful


	// initialize polynomial
	hPoly = poly_initDefault();
	if (!hPoly)
		return FAILURE;

	do {
		// get polynomial from user input
		userInputGetPolyStr(polyStr, POLY_BUFFER_CAP, "Enter the polynomial to calculate at an x-value.");

		// create new polynomial with user input
		if (!poly_newPoly(hPoly, polyStr, &polyStrIsValid)) {
			poly_destroy(&hPoly);
			return FAILURE;
		}

		// get x-value from user input
		x = userInputGetX();

		// calculate with the x-value
		// the polynomial is guaranteed to not be empty so if the calculation fails here it's from division by zero
		if (!poly_calcXValue(hPoly, x, &result, &polyHasNoTerms)) {
			calcIsSuccessful = FALSE;
			printf("Error - a polynomial with at least one negative exponent cannot be summed with an x-value of 0 due to division by zero.\n");
		}
		else {
			calcIsSuccessful = TRUE;
		}
	} while (!calcIsSuccessful);

	// display results
	poly_sort(hPoly);
	displayResultsPolyXValue(hPoly, x, result);

	// clean up memory
	poly_destroy(&hPoly);

	return SUCCESS;
}


MenuOption menu_getUserChoice(void) {
	char userChoiceStr[500];
	int userChoice;
	Boolean userChoiceIsValid = TRUE;
	MenuOption menuOption = QUIT;

	do {
		displayRoundMessage(menuOptionMessages, menuOptionMessagesSize);
		fgets(userChoiceStr, 500, stdin);
		userChoiceStr[strlen(userChoiceStr) - 1] = '\0';
		if (!inputsAreValidInts(userChoiceStr, 1) || atoi(userChoiceStr) < 0 || atoi(userChoiceStr) > (menuOptionMessagesSize - 1)) {
			printf("Error - you must enter an integer between 0 and %d.\n\n", menuOptionMessagesSize - 1);
			userChoiceIsValid = FALSE;
		}
		else
			userChoiceIsValid = TRUE;
	} while (!userChoiceIsValid);

	userChoice = atoi(userChoiceStr);

	for (int i = 0; i < menuOptionMessagesSize; ++i) {
		if (userChoice == (int)menuOptionMessages[i].menuOption) {
			menuOption = menuOptionMessages[i].menuOption;
			break;
		}
	}

	return menuOption;
}


Status menu_implementUserChoice(MenuOption userChoice) {
	Status status = SUCCESS;

	switch (userChoice) {
	case X_VALUE:
		if (!menu_calcPolyXValue())
			status = FAILURE;
		break;
	case NTH_DERIV:
		if (!menu_calcPolyNthDeriv())
			status = FAILURE;
		break;
	case NTH_DERIV_X_VALUE:
		if (!menu_calcPolyNthDerivXValue())
			status = FAILURE;
		break;
	case INDEF_INTEGRAL:
		if (!menu_calcPolyIndefIntegral())
			status = FAILURE;
		break;
	case DEF_INTEGRAL:
		if (!menu_calcPolyDefIntegral())
			status = FAILURE;
		break;
	default: // QUIT
		break;
	}

	return status;
}




/********** Definitions for helper functions  **********/
static void createOrdinalNum(char* ordNum, int num) {
	char ordInd[3] = { '\0' };    // ordinal indicator

	if (num >= 4 && num <= 19)    // special case of th - [4, 19] i.e 4th, 5th...19th
		strcpy(ordInd, "th");
	else if ((num - 1) % 10 == 0) // st i.e. 1st, 21st, 31st...
		strcpy(ordInd, "st");
	else if ((num - 2) % 10 == 0) // nd i.e. 2nd, 22nd, 32nd...
		strcpy(ordInd, "nd");
	else if ((num - 3) % 10 == 0) // rd i.e 3rd, 23rd, 33rd...
		strcpy(ordInd, "rd");
	else                          // th i.e. 20th, 2[4, 9]th, 30th, 3[4, 9]th...
		strcpy(ordInd, "th");
	
	sprintf(ordNum, "%d%s", num, ordInd);
}


static void displayResultsPolyDefIntegral(POLY hPoly, POLY hPolyOrig, double LB, double UB, double result, Boolean expNegOneIntegrated, double coeffExpNegOne)
{
	printf("\nRESULTS OF CALCULATION\n");
	
	// term with exponent of -1 was integrated
	// displaying results gets more complicated due to natural logarithms
	if (expNegOneIntegrated) {
		// these get used quite a lot, more convenient to store them once in a variable
		double absLB = fabs(LB);
		double absUB = fabs(UB);
		double absCoeffExpNegOne = fabs(coeffExpNegOne);

		// original polynomial and bounds
		printf("%-60s", "Polynomial");
		poly_print(hPolyOrig);
		printf("\n%-60s%g\n", "Lower Bound", LB);
		printf("%-60s%g\n", "Upper Bound", UB);

		// indefinite integral
		// format: [polynomial terms] + kln|x| + C
		printf("%-60s", "Indefinite integral");
		if (poly_hasNoTerms(hPoly)) { // no terms, only print coefficient
			if (coeffExpNegOne == -1)
				printf("-");
			else if (coeffExpNegOne != 1)
				printf("%g", coeffExpNegOne);
		}
		else {                              // terms, print polynomial and coefficient
			poly_print(hPoly);
			(coeffExpNegOne < 0) ? printf(" - ") : printf(" + ");
			if (absCoeffExpNegOne != 1)
				printf("%g", absCoeffExpNegOne);
		}
		printf("ln(|x|) + C\n");

		// definite integral with logs
		// format [polynomial terms result upper and lower bound] + kln(|UB|)- kln(|LB|)
		// note that ln|1| = 0 in which case the 0 shouldn't be displayed, this occurs when the lower/upper bound is 1 or -1
		printf("%-60s", "Definite integral (with natural logs)");
		// result isn't 0, display both result and natural log part of result
		if (result != 0) {
			printf("%g", result);
			// result + kln|UB| - kln|LB|
			if (absUB != 1 && absLB != 1) {
				(coeffExpNegOne < 0) ? printf(" - ") : printf(" + ");
				if (absCoeffExpNegOne != 1)
					printf("%g", absCoeffExpNegOne);
				printf("ln(%g)", absUB);
				(coeffExpNegOne < 0) ? printf(" + ") : printf(" - ");
				if (absCoeffExpNegOne != 1)
					printf("%g", absCoeffExpNegOne);
				printf("ln(%g)\n", absLB);
			}
			// result + kln|UB|
			else if (absUB != 1 && absLB == 1) {
				(coeffExpNegOne < 0) ? printf(" - ") : printf(" + ");
				if (absCoeffExpNegOne != 1)
					printf("%g", absCoeffExpNegOne);
				printf("ln(%g)\n", absUB);
			}
			// result - kln|LB|
			else if (absUB == 1 && absLB != 1) {
				(coeffExpNegOne < 0) ? printf(" + ") : printf(" - ");
				if (absCoeffExpNegOne != 1)
					printf("%g", absCoeffExpNegOne);
				printf("ln(%g)\n", absLB);
			}
			// result
			else
				printf("\n");
		}
		// result is  0, only display natural log part
		else {
			// kln|7| - kln|6| for example
			if (absUB != absLB) {
				// kln|UB| - kln|LB|
				if (absUB != 1 && absLB != 1) {
					if (coeffExpNegOne == -1)
						printf("-");
					else if (coeffExpNegOne != 1)
						printf("%g", coeffExpNegOne);
					printf("ln(%g)", absUB);
					(coeffExpNegOne < 0) ? printf(" + ") : printf(" - ");
					if (absCoeffExpNegOne != 1)
						printf("%g", absCoeffExpNegOne);
					printf("ln(%g)\n", absLB);
				}
				// kln|UB|
				else if (absUB != 1 && absLB == 1) {
					if (coeffExpNegOne == -1)
						printf("-");
					else if (coeffExpNegOne != 1)
						printf("%g", coeffExpNegOne);
					printf("ln(%g)\n", absUB);
				}
				// - kln|LB|
				else if (absUB == 1 && absLB != 1) {
					if (coeffExpNegOne == 1)
						printf("-");
					else if (coeffExpNegOne != -1)
						printf("%g", coeffExpNegOne * -1);
					printf("ln(%g)\n", absLB);
				}
				// kln|1| - kln|1| = 0 - 0 = 0, so just 0 should display
				else
					printf("0\n");
			}
			// kln|7| - kln|7| for example
			else
				printf("0\n");
		}

		// definite integral with logs approximated
		printf("%-60s", "Definite integral (with natural logs approximated)");
		printf("%g\n", result + (coeffExpNegOne * log(absUB)) - (coeffExpNegOne * log(absLB)));

		printf("\n\n\n");
	}
	
	// term with exponent of -1 was not integrated, displaying results is straightforward
	else {
		// original polynomial and bounds
		printf("%-20s", "Polynomial");
		poly_print(hPolyOrig);
		printf("\n%-20s%g\n", "Lower Bound", LB);
		printf("%-20s%g\n", "Upper Bound", UB);

		// indefinite integral
		printf("%-20s", "Indefinite integral");
		poly_print(hPoly);
		printf(" + C\n");

		// definite integral
		printf("%-20s", "Definite integral");
		printf("%g\n", result);

		printf("\n\n\n");
	}
}


static void displayResultsPolyIndefIntegral(POLY hPoly, POLY hPolyOrig, Boolean expNegOneIntegrated, double coeffExpNegOne) {
	printf("\nRESULTS OF CALCULATION\n");
	printf("%-20s", "Polynomial");
	poly_print(hPolyOrig);
	printf("\n%-20s", "Indefinite integral");
	poly_print(hPoly);
	if (expNegOneIntegrated) {
		(coeffExpNegOne < 0) ? printf(" - ") : printf(" + ");
		if (fabs(coeffExpNegOne) != 1)
			printf("%g", fabs(coeffExpNegOne));
		printf("ln(|x|)");
	}
	printf(" + C\n");

	printf("\n\n\n");
}


static void displayResultsPolyNthDeriv(POLY hPoly, POLY hPolyOrig, int n, Boolean nthDerivIsZero) {
	char ordinalNumOfN[100] = { '\0' };
	
	createOrdinalNum(ordinalNumOfN, n);
	poly_sort(hPoly);
	poly_sort(hPolyOrig);
	printf("\nRESULTS OF CALCULATION\n");
	printf("%-20s", "Polynomial");
	poly_print(hPolyOrig);
	printf("\n%-20s", strcat(ordinalNumOfN, " derivative"));
	if (!nthDerivIsZero) {
		poly_print(hPoly);
		printf("\n");
	}
	else
		printf("0\n");

	printf("\n\n\n");
}


static void displayResultsPolyNthDerivXValue(POLY hPoly, POLY hPolyOrig, double x, double result, int n, Boolean nthDerivIsZero) {
	char ordinalNumOfN[100] = { '\0' };

	createOrdinalNum(ordinalNumOfN, n);
	printf("%-20s", "Polynomial");
	poly_print(hPolyOrig);
	printf("\n%-20s", strcat(ordinalNumOfN, " derivative"));
	if (!nthDerivIsZero)
		poly_print(hPoly);
	else
		printf("0");
	printf("\n%-20s%g\n", "x-value", x);
	printf("%-20s%g\n", "Result", result);

	printf("\n\n\n");
}


static void displayResultsPolyXValue(POLY hPoly, double x, double result) {
	printf("\nRESULTS OF CALCULATION\n");
	printf("%-20s", "Polynomial");
	poly_print(hPoly);
	printf("\n%-20s%g\n", "x-value", x);
	printf("%-20s%g\n", "Result", result);

	printf("\n\n\n");
}


static void displayRoundMessage(const MenuOptionMessages menuOptionMessages[], int menuOptionMessagesSize) {
	printf("---------------------------------------------------------------------------------\n");
	printf("Enter the number of the polynomial calculation to perform or 0 to quit the program.\n");
	for (int i = 0; i < menuOptionMessagesSize; ++i) {
		if (menuOptionMessages[i].menuOption != QUIT)
			printf("%d) %s\n", (int)menuOptionMessages[i].menuOption, menuOptionMessages[i].message);
	}
	printf("%d) %s\n", (int)menuOptionMessages[0].menuOption, menuOptionMessages[0].message);
	printf("Enter choice: ");
}


static Boolean inputsAreValidPositiveInts(const char* input, int expectedNums) {
	char singleNumStr[50];
	int idx = 0;
	int totalNums = 0;
	int i = 0;
	int j;


	// user only pressed "enter" or whitespace followed by "enter"
	// space is the only valid whitespace character
	while (isspace(input[i])) {
		if (input[i] != ' ')
			return FALSE;
		++i;
	}
	if (input[i] == '\0')
		return FALSE;


	// validate that all inputs are valid integers
	while (input[i] != '\0') {
		// something other than a digit or space is invalid
		if (!isdigit(input[i]) && input[i] != ' ')
			return FALSE;
		++i;
	}

	// all inputs are valid integers, now verify the correct amount of numbers have been entered and they're all > 0
	i = 0;
	while (input[i] != '\0') {
		while (input[i] == ' ')                     // clear whitespace
			++i;
		j = i;
		while (input[i] != ' ' && input[i] != '\0') // get the number
			singleNumStr[idx++] = input[i++];
		singleNumStr[idx] = '\0';
		idx = 0;

		if (i != j) {                               // don't want to increment totalNums or check the number if user entered extra whitespace at end
			if (atoi(singleNumStr) == 0)
				return FALSE;
			++totalNums;
		}

	}

	return totalNums == expectedNums;
}


static void userInputGetBoundsOfDefIntegral(double* pLB, double* pUB) {
	double LB;
	double UB;
	char boundStr[500];
	Boolean isValidBound;

	do {
		printf("Enter the lower bound of the definite integral: ");
		fgets(boundStr, 500, stdin);
		boundStr[strlen(boundStr) - 1] = '\0';
		if (!inputsAreValidDoubles(boundStr, 1)) {
			printf("Error - the lower bound entered is not valid. It must be a single number.\n");
			isValidBound = FALSE;
		}
		else {
			LB = strtod(boundStr, NULL);
			isValidBound = TRUE;
		}
	} while (!isValidBound);

	do {
		printf("Enter the upper bound of the definite integral: ");
		fgets(boundStr, 500, stdin);
		boundStr[strlen(boundStr) - 1] = '\0';
		if (!inputsAreValidDoubles(boundStr, 1)) {
			printf("Error - the upper bound entered is not valid. It must be a single number.\n");
			isValidBound = FALSE;
		}
		else {
			UB = strtod(boundStr, NULL);
			isValidBound = TRUE;
		}
	} while (!isValidBound);

	*pLB = LB;
	*pUB = UB;
}


static int userInputGetN(void) {
	Boolean isValidN;
	char n[500];

	do {
		printf("Enter the nth derivative: ");
		fgets(n, 500, stdin);
		n[strlen(n) - 1] = '\0';
		if (!inputsAreValidPositiveInts(n, 1)) {
			printf("Error - the nth derivative entered is not valid. It must be an integer greater than 0.\n");
			isValidN = FALSE;
		}
		else
			isValidN = TRUE;
	} while (!isValidN);

	return atoi(n);
}


static void userInputGetPolyStr(char* polyStr, int polyCap, const char* prompt) {
	Boolean isValidPoly;

	do {
		printf("\n%s\nRules:\n1) Use ^ for exponents.\n2) Use + and - for addition and subtraction.\n3) Coefficients can be any number.\n4) Exponents must be integers.\n", prompt);
		fgets(polyStr, polyCap, stdin);
		polyStr[strlen(polyStr) - 1] = '\0';
		if (!poly_isValidPolyStr(polyStr)) {
			printf("Error - the polynomial entered is not valid.\n");
			isValidPoly = FALSE;
		}
		else
			isValidPoly = TRUE;
	} while (!isValidPoly);
}


static double userInputGetX(void) {
	Boolean isValidX;
	char x[500];         

	do {
		printf("Enter the x-value: ");
		fgets(x, 500, stdin);
		x[strlen(x) - 1] = '\0';
		if (!inputsAreValidDoubles(x, 1)) {
			printf("Error - the x-value entered is not valid. It must be a single number.\n");
			isValidX = FALSE;
		}
		else
			isValidX = TRUE;
	} while (!isValidX);

	return strtod(x, NULL);
}