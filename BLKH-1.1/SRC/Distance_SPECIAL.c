#include "LKH.h"

/*
 * The Distance_SPECIAL function may be used to specify a user defined
 * distance fuction. The function is used when the EDGE_WEIGHT_TYPE is
 * SPECIAL. 
 * 
 * Example:
 *  
 *      int Distance_SPECIAL(Node * Na, Node * Nb) 
 *      {
 *           double dx = Na->X - Nb->X;
 *           double dy = Na->Y - Nb->Y;
 *           return (int) (1000 * sqrt(dx * dx + dy * dy));
 *      }           
 */

int Distance_SPECIAL(Node * Na, Node * Nb)
{
     return M - OriginalDistance(Na, Nb);
}
