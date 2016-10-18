

#include "Errors.h"
#define MaxErrorBufferSize  10

uint8_t ErrorList[MaxErrorBufferSize];
volatile uint8_t CurrentSystemErrorsCount = 0;

uint8_t displayIndex = 0;
uint8_t displayCount = 0;
/**
*   Sets the values of the Error list array to no error indicator sign i.e "#"
*/
void init_ErrorList()
{    
   uint8_t i=0;
   for(;i<MaxErrorBufferSize;i++)
   {
     ErrorList[i] = '#';//No Error 
   }  
}
/**
*   Add an error to the Error list array
*   @param Error_No The Error number to add to the error list 
*   @retrun 0 if the error list buffer is full and the current error cannot be added 
*           1 if the error is already  in the list or if it is added sucessfully  
*/
uint8_t AddError(uint8_t Error_No)
{
  uint8_t i = 0;
  for(;i<MaxErrorBufferSize;i++)
  {
    if(ErrorList[i] == Error_No)
    {
      return 1;  //the error is already in the buffer, no need to add it again 
    }
  }
  i=0;
  while(i<MaxErrorBufferSize)
  {
    if(ErrorList[i] == '#')//the buffer location is empty 
    {
      ErrorList[i]= Error_No;
      CurrentSystemErrorsCount++;
      break;
    }
    i++;
  }
  if(i>=MaxErrorBufferSize)
  {
    return 0; //the buffer is full 
  }
  return 1; //error added susuccfully 
  
}
/**
*  Removes an error from the Errors list array
*  @param Error_No The Error number to remove from the list of errors 
*  @return   1 removes the error and returns 1 whethere the error was there or not   
*/
uint8_t RemoveError(uint8_t Error_No)
{
  uint8_t i=0;
  while(i<MaxErrorBufferSize)
  {
    if(ErrorList[i] == Error_No)
    {
      ErrorList[i]= '#';
      CurrentSystemErrorsCount--;      
    }
    i++;
  }
  return 1;
}
/**
* method written to provide the next error number from the error list when called 
* @returns 
* 0: if there are no more errors to display 
* 1: if there is error to display 
*
* @param Error:- the error number to display next 
*/
uint8_t getNextError(uint8_t *Error)
{
  if(CurrentSystemErrorsCount > 0 && displayCount < CurrentSystemErrorsCount)
  {
    while( displayIndex < MaxErrorBufferSize)
    { 
       if( ErrorList[displayIndex] != '#' )
       {
         *Error = ErrorList[displayIndex];
           displayIndex++;
           displayCount++;
           return 1;
       }       
          displayIndex++;
    }    
  }  
    displayCount = 0;
    displayIndex = 0;
  return 0;
}
/**
*    returns the index of the current error number that is displayed on the LCD
*/
uint8_t getCurrentErrorNumber()
{
  return displayIndex;
}