//
// Created by tanaka on 22-May-20.
//

#include <string.h>
#include <stdlib.h>
#include "Structs.h"
#include "math.h"

#define FAILURE 0
#define SUCCESS 1
#define SQUARED(x) (x)*(x)

/**
 * Doxygen comment in the header
 */
int stringCompare(const void *a, const void *b)
{
    const char *str1 = (char*) a;
    const char *str2 = (char*) b;
    return strcmp(str1, str2);
}

/**
 * Doxygen comment in the header
 */
void freeString(void *s)
{
    if (s != NULL)
    {
        free(s);
        s = NULL;
    }
}

/**
 * Doxygen comment in the header
 */
int concatenate(const void *word, void *pConcatenated)
{
    char *wordToAdd = (char*) word;
    char *wordToExtend = (char*) pConcatenated;
    strcat(wordToExtend, wordToAdd);
    return SUCCESS;
}

/**
 * Calculates the minimum between a and b
 */
int minimum(const int a, const int b)
{
    if (a < b)
    {
        return a;
    }
    return b;
}

/**
 * Doxygen comment in the header
 */
int vectorCompare1By1(const void *a, const void *b)
{
    Vector *first = (Vector*) a;
    Vector *second = (Vector*) b;
    int minLength = minimum(first->len, second->len);
    for (int i = 0; i < minLength; i++)
    {
        if ((first->vector)[i] > (second->vector)[i]) //a > b
        {
            return 1;
        }
        else if ((second->vector)[i] > (first->vector)[i]) //b > a
        {
            return -1;
        }
    }
    if (first->len > minLength) //a > b
    {
        return 1;
    }
    else if (second->len > minLength) //b > a
    {
        return -1;
    }
    return 0; //b == a
}

/**
 * Doxygen comment in the header
 */
void freeVector(void *pVector)
{
    if (pVector != NULL)
    {
        Vector *pvec = (Vector*) pVector;
        if (pvec->vector != NULL)
        {
            free(pvec->vector); //needed..?
            pvec->vector = NULL;
        }
        free(pvec); //or free(pVector)???
        pvec = NULL;
    }
}


/**
 * Calculates the norm of a vector
 * @return the Norm if everything is correct,  0 if pVector doisn't takin
 */
double calculateNorm(const Vector *pVector)
{
    if (!pVector || !(pVector->vector))
    {
        return 0;
    }
    double normSquared = SQUARED(pVector->vector[0]);
    for (int i = 1; i < pVector->len; i++)
    {
        normSquared += SQUARED(pVector->vector[0]);
    }
    return sqrt(normSquared);
}

/**
 * Doxygen comment in the header
 */
int copyIfNormIsLarger(const void *pVector, void *pMaxVector)
{
    const Vector *pvec = (Vector*) pVector;
    if (calculateNorm(pvec) > calculateNorm((Vector*) pMaxVector))
    {
        pMaxVector = realloc(pMaxVector, sizeof(pVector));
        if (!pMaxVector)
        {
            return 0;
        }
        Vector *pmaxvec = (Vector*) pMaxVector;
        pmaxvec->len = pvec->len;
        pmaxvec->vector = pvec->vector;
    }
    return 0;
}

/**
 * Doxygen comment in the header
 */
Vector *findMaxNormVectorInTree(RBTree *tree)
{
    Vector *maxVec = (Vector*) tree->root;
    forEachRBTree(tree, copyIfNormIsLarger, maxVec);
    return maxVec;
}