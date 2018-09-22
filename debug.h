#ifndef __DEBUG_H__
#define __DEBUG_H__

#define DEBUG_BUFFER_SIZE 512
_iq15 debug_buffer[DEBUG_BUFFER_SIZE];
Uint16 debug_i = 0;

typedef enum {
    DEBUG_WR_CONT = 0,
    DEBUG_WR_ON_START = 1,
    DEBUG_WR_ON_FAIL = 2,
} debug_write_mode_t;

extern System_t system;

void debug_write(debug_write_mode_t m, int stride, _iq v)
{
    static int counter = 0;

    if (counter == stride)
    {
        if (m == DEBUG_WR_CONT)
        {
                debug_buffer[debug_i++] = v;
                if (debug_i > (DEBUG_BUFFER_SIZE-1))
                    debug_i = 0;
        }
        else if (m == DEBUG_WR_ON_START)
        {
            if (system.state == running_state)
            {
                if (debug_i < (DEBUG_BUFFER_SIZE-1))
                    debug_buffer[debug_i++] = v;
            }
        }
        counter = 0;
    }
    else
        counter++;

//    else if (m == DEBUG_WR_ON_FAIL)
//    {
//        if (system.state != fail_state)
//        {
//            debug_buffer[debug_i++] = v;
//            if (debug_i > (DEBUG_BUFFER_SIZE-1))
//                debug_i = 0;
//        }
//    }

}

#endif // __DEBUG_H__
