  <h1>Matrix.md</h1>
  <h3>Loading Matrix</h3>
  Matrix is Load into Blocks(Files) in form of Row_Major_order.
  <br>
  Based upon the Block size we decide on the number of elements to be stored in one Block.<br>
  Block size considered 1Kbytes and consider each element takes 32 bytes we store approximately 31 elements in each block.
  <hr>
  <h3>Transpose Matrix</h3>
  Initially consider upper triangular matrix say (0,1) index and we have to transpose it with index position (1,0) .so as we know the block size and number of elements stored in each block (row_major_order) we can identify in which block (1,0) index element is present.<br>
  Once we fetch the block and element we can swapping the corresponding elements i.e (0,1) and (1,0) 
  This method can be applied to all elements in upper triangular matrix.<br>
  For example if (i,j) are index then we can identify in which block the element is present in following way :<br>
  int pageijIndex =int( ((i * matrix.matrixDim) + j) / matrix.maxElementsPerBlock);<br>
            int pagejiIndex = int (((j * matrix.matrixDim) + i) / matrix.maxElementsPerBlock); <br>
  Here matrix.matrixDim represents number of columns<br>
  Once we have Identified the Pages or Blocks we can identify the position of element within the block by using following steps :<br>
  int positionijInVector = ((i * matrix.matrixDim) + j)\% matrix.maxElementsPerBlock; <br>
            int positionjiInVector = ((j * matrix.matrixDim) + i) % matrix.maxElementsPerBlock;<br>
  Once we have Identified the corresponding position we can apply swapping of these two elements.

