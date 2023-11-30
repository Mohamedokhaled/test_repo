 #include "task.h"
#include "stm32g0b0xx.h"

/*------------------------------------------------------------------*/
TASK_Str    VTask;
/*------------------------------------------------------------------*/

 /**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/


/*===================================================================
//description    ::  initatation task queue
//function      ::  InitTaskQue
//input         ::  none
//output        ::  none
//call          ::  none
//effect        ::  vtask.Buff
//==================================================================*/
void	InitTaskQue( void )
{
    uint8_t	i;
    for ( i = 0; i < TASK_Num_MAX; i++ )
    {
        VTask.Buff[i] = 0;
    }
    VTask.offset = 0;
}
/*===================================================================
//decription    ::  put task into task queue in interrupt state
//function      ::  PutTaskIntoQue_INT
//input         ::  TaskID
//output        ::  1:success, 0=failure
//call          ::  none
//effect        ::  vtask.offset
//===================================================================*/
uint8_t    PutTaskIntoQue_INT( TASK_CONST TaskID )
{
    if ( VTask.offset >= TASK_Num_MAX )
    {
        VTask.offset = TASK_Num_MAX >> 1;
        return ( 0 );
    }
    VTask.Buff[VTask.offset] = TaskID;
    VTask.offset++;

    return ( 1 );
}
/*===================================================================
//decription    ::  put task into task queue
//function      ::  PutTaskIntoQue
//input         ::  TaskID
//output        ::  1:success, 0=failure
//call          ::  none
//effect        ::  vtask.offset
//===================================================================*/
uint8_t    PutTaskIntoQue( TASK_CONST TaskID )
{
    uint8_t	i;
    __disable_irq();
    i = PutTaskIntoQue_INT( TaskID );
    __enable_irq();

    return ( i );
}
/*===================================================================
//decription    ::  get task from task queue
//function      ::  GetTaskFromQue
//input         ::  none
//output        ::  taskID
//call          ::  none
//effect        ::  vtask.offset
//===================================================================*/
uint8_t    GetTaskFromQue( void )
{
    uint8_t    i;
    uint8_t   j;
    __disable_irq();
    j = VTask.Buff[0];
    if ( VTask.offset > TASK_Num_MAX )
    {
        VTask.offset = ( TASK_Num_MAX >> 1 );
    }
    if ( VTask.offset )
    {
        for ( i = 0; i < ( VTask.offset - 1 ); i++ )
        {
            VTask.Buff[i] = VTask.Buff[i + 1];
        }
        VTask.Buff[VTask.offset - 1] = 0;
        VTask.offset--;
    }/*endif (vtask.offset)*/
    __enable_irq();
    return ( j );
}


/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
