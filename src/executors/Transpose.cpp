#include "global.h"
/**
 * @brief 
 * SYNTAX: LOAD relation_name
 */
bool syntacticParseTRANSPOSE()
{
    logger.log("syntacticParseTranspose");
    if (tokenizedQuery.size() != 2)
        {
            cout << "SYNTAX ERROR" << endl;
            return false;
        }
    parsedQuery.queryType = TRANSPOSE;
    parsedQuery.transposeRelationName = tokenizedQuery[1];
    return true;
    
    
}

bool semanticParseTRANSPOSE()
{
    logger.log("semanticParseTRANSPOSE");
    if (matrixCatalogue.isMatrix(parsedQuery.transposeRelationName))
    {
        cout << "SEMANTIC ERROR: Matrix already exists" << endl;
        return false;
    }

    if (!isFileExists(parsedQuery.transposeRelationName))
    {
        cout << "SEMANTIC ERROR: Data file doesn't exist" << endl;
        return false;
    }
    return true;
}


void executeTRANSPOSE()
{
    logger.log("executeTRANSPOSE");
    cout<<"asdasd"<<endl;
    
    Matrix matrix = *(matrixCatalogue.getMatrix(parsedQuery.transposeRelationName));
    cout<<"adasd"<<endl;
    for(int i = 0;i < matrix.matrixDim;i++)
    {
        for(int j = i+1;j < matrix.matrixDim;j++)
        {
            Cursor cursor = matrix.getCursor();
            cout<<"sdsdad"<<endl;
            Page pageij = cursor.getPageContainingElement(i,j);
            Page pageji = cursor.getPageContainingElement(j,i);
            cout<< pageij.matrixElements[0] <<endl;
            int pageijIndex =int( ((i * matrix.matrixDim) + j) / matrix.maxElementsPerBlock);
            int pagejiIndex = int (((j * matrix.matrixDim) + i) / matrix.maxElementsPerBlock);
            int positionijInVector = ((i * matrix.matrixDim) + j) % matrix.maxElementsPerBlock;
            int positionjiInVector = ((j * matrix.matrixDim) + i) % matrix.maxElementsPerBlock;
            int temp = pageij.matrixElements[positionijInVector];
            pageij.matrixElements[positionijInVector] = pageji.matrixElements[positionjiInVector];
            pageji.matrixElements[positionjiInVector] = temp;
            bufferManager.writePage(matrix.matrixName,pageijIndex,pageij.matrixElements,matrix.elementsPerBlockCount[pageijIndex]);
            bufferManager.writePage(matrix.matrixName,pagejiIndex,pageji.matrixElements,matrix.elementsPerBlockCount[pagejiIndex]);
        }
    }
    cout<<"Completed"<<endl;
    return;
}