/*

  A task to manage light effects using WS2812b LEDs.

  Copyright (c) 2025 Maarten Klarenbeek (https://github.com/mjklaren)
  Distributed under the GPLv3 license

*/

#include "TISM.h"
#include "WS2812b.h"


// Modes
#define LIGHTS_OFF             0
#define MAGIC_COLORS           1
#define LIGHTNING              2

// WS2812b configuration
#define LIGHTS_PIO             pio0        // PIO port connected to the WS2812b data line
#define LIGHTS_GPIO            15           // GPIO pin connected to the WS2812b data line
#define LIGHTS_NUM_PIXELS      74          // Number of WS2812b pixels in the strip
#define LIGHTS_MAX_PIXELS      3           // Maximum number of simultanous pixels in Magic Colors mode
#define LIGHTS_MAX_BRIGHTNESS  75          // Maximum brightness (0-255)
#define LIGHTS_DIM_STEP        5           // Step size for dimming
#define LIGHTS_DELAY_MS        50          // Delay between updates (in msec).

// For lightning effects
#define LIGHTNING_MULTICOLOR   1           // Set to 1 to enable multi-color lightning, 0 for white only
#define LIGHTNING_MIN_DELAY_MS 0
#define LIGHTNING_MAX_DELAY_MS 50
#define LIGHTNING_MAX_PAUSE_MS 100

/*
  The structure containing all data for this task to run. These variables allow the task to remain its state as the stack 
  and heap are not saved between runs. We use a struct, this is not mandatory (just an easy way to make sure variable
  names are unique in the whole system).
*/
struct LightsData
{
  uint8_t Mode; 
  WS2812bInstance *MyWS2812b;
  uint64_t Timer;
  bool Toggle,MultiColorLightning; 
} LightsData;


/*
  Description:
  This is the function that is registered in the TISM-system via the TISM_RegisterTask. A pointer to this function is used.
  For debugging purposes the TISM_EventLoggerLogEvent-function is used (not mandatory).

  Parameters:
  TISM_Task ThisTask - Struct containing all relevant information for this task to run. This is provided by the scheduler.
  
  Return value:
  <non-zero value>        - Task returned an error when executing. A non-zero value will stop the system.
  OK                      - Run succesfully completed.
*/
uint8_t Lights (TISM_Task ThisTask)
{
  if (ThisTask.TaskDebug==DEBUG_HIGH) TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Run starting.");
  
  /*
    The scheduler maintains the state of the task and the system. Specify here the actions per state.
    Using the states (and the case-statements below) is optional.
    Task states INIT, RUN, STOP and DOWN are predefined.
    INIT is used during startup; when all tasks have initialized correctly the system state is set to RUN.
    Once the system is in this state the task can then switch to custom states. When the system stops all tasks are switched
    to the STOP-state. Remember to always check for incoming messages in custom states.
  */
  switch(ThisTask.TaskState)   
  {
    case INIT:  // Activities to initialize this task (e.g. initialize ports or peripherals).
                if (ThisTask.TaskDebug) TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Initializing with priority %d.", ThisTask.TaskPriority);
				        
                // Initialize WS2812b instance.
                LightsData.MyWS2812b=WS2812bInitialize(LIGHTS_PIO,LIGHTS_GPIO,LIGHTS_NUM_PIXELS,LIGHTS_MAX_PIXELS,LIGHTS_MAX_BRIGHTNESS,LIGHTS_DIM_STEP);
                WS2812bEmptyBuffer(LightsData.MyWS2812b);
    //uint offset=pio_add_program(MyWS2812b->PIO,&ws2812_program);
                ws2812_program_init(LightsData.MyWS2812b->PIO,0,pio_add_program(LightsData.MyWS2812b->PIO,&ws2812_program),LightsData.MyWS2812b->GPIO,800000,false);
                LightsData.MyWS2812b->MaxBrightness=LIGHTS_MAX_BRIGHTNESS;
                LightsData.MultiColorLightning=(LIGHTNING_MULTICOLOR==1?true:false);
                LightsData.Mode=LIGHTS_OFF;
                LightsData.Timer=0;

                // For tasks that only respond to events (=messages) we could set the sleep attribute to ´true'.
                TISM_SchedulerSetMyTaskAttribute(ThisTask,TISM_SET_TASK_SLEEP,true);
				        break;
	  case RUN:   // Do the work.						
		      	    if (ThisTask.TaskDebug==DEBUG_HIGH) TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Doing work with priority %d on core %d.", ThisTask.TaskPriority, ThisTask.RunningOnCoreID);

                // First check for incoming messages and process them.
                uint8_t MessageCounter=0;
                TISM_Message *MessageToProcess;
                while((TISM_PostmanTaskMessagesWaiting(ThisTask)>0) && (MessageCounter<MAX_MESSAGES))
                {
                  MessageToProcess=TISM_PostmanTaskReadMessage(ThisTask);

                  if (ThisTask.TaskDebug) TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Message '%ld' type %d from TaskID %d (HostID %d) received.", MessageToProcess->Payload0, MessageToProcess->MessageType, MessageToProcess->SenderTaskID, MessageToProcess->SenderTaskID);

                  // Processed the message; delete it.
                  switch(MessageToProcess->MessageType)
                  {
                    case TISM_PING:    // Check if this process is still alive. Reply with a ECHO message type; return same message payload.
                                       TISM_PostmanTaskWriteMessage(ThisTask,MessageToProcess->SenderHostID,MessageToProcess->SenderTaskID,TISM_ECHO,MessageToProcess->Payload0,0);
                                       break;
                    case LIGHTS_OFF:   // Turn off lights.
                        	      	     if (ThisTask.TaskDebug) TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY,"Switching lights off");

                                       WS2812bEmptyBuffer(LightsData.MyWS2812b);
                                       WS2812bWriteBuffer(LightsData.MyWS2812b);
                                       LightsData.Mode=LIGHTS_OFF;
                                       TISM_SchedulerSetMyTaskAttribute(ThisTask,TISM_SET_TASK_SLEEP,true);  // Lights out, go to sleep.
                                       break;
                    case MAGIC_COLORS: // Switch to magic colors mode.
                       	      	       if (ThisTask.TaskDebug) TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY,"Activating magic colors effect");

                                       WS2812bEmptyBuffer(LightsData.MyWS2812b);
                                       WS2812bWriteBuffer(LightsData.MyWS2812b);
                                       LightsData.Mode=MAGIC_COLORS;
                                       TISM_SchedulerSetMyTaskAttribute(ThisTask,TISM_SET_TASK_SLEEP,false); // Stay awake.
                                       break;
                    case LIGHTNING:    // Switch to lightning mode.
                         	      	     if (ThisTask.TaskDebug) TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY,"Activating lightning effect");

                                       WS2812bEmptyBuffer(LightsData.MyWS2812b);
                                       WS2812bWriteBuffer(LightsData.MyWS2812b);
                                       LightsData.Mode=LIGHTNING;
                                       LightsData.Toggle=false;
                                       TISM_SchedulerSetMyTaskAttribute(ThisTask,TISM_SET_TASK_SLEEP,false); // Stay awake.
                                       break;
                    default:           // Unknown message type - turn off lights.
                                       if (ThisTask.TaskDebug) TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY,"Unknown command - turning lights off");

                                       WS2812bEmptyBuffer(LightsData.MyWS2812b);
                                       WS2812bWriteBuffer(LightsData.MyWS2812b);
                                       LightsData.Mode=LIGHTS_OFF;
                                       TISM_SchedulerSetMyTaskAttribute(ThisTask,TISM_SET_TASK_SLEEP,true);
                                       break;
                  }
                  TISM_PostmanTaskDeleteMessage(ThisTask);
                  MessageCounter++;
                }

                // Perform mode-specific actions.
                if(TISM_SoftwareTimerVirtualExpired(LightsData.Timer))  // Timer expired?
                {                
                  switch(LightsData.Mode)
                  {
                    case LIGHTS_OFF:   // Do nothing.   
                                       break;
                    case MAGIC_COLORS: // Magic colors effect.
                                       if(LightsData.MyWS2812b->NumberOfActiveLeds<LIGHTS_MAX_PIXELS)
                                       {
                                         int Options;
                                         switch(rand()%WS2812B_SPEED_STEPS)
                                         {
                                           case 3: Options=WS2812B_SPEED4;
                                                   break;
                                           case 2: Options=WS2812B_SPEED3;
                                                   break;
                                           case 1: Options=WS2812B_SPEED2;
                                                   break;
                                           case 0: Options=WS2812B_SPEED1;
                                                   break;
                                         }
                                         WS2812bSetRandomPixelInBuffer(LightsData.MyWS2812b,0x8000000|Options); // Set random pixel with random speed.
                                       }
                                        
                                       // Process the buffer and write it to the WS2812b device.
                                       WS2812bProcessBuffer(LightsData.MyWS2812b);
                                       WS2812bWriteBuffer(LightsData.MyWS2812b);  

                                       // Set a new timer for the next update.
                                       LightsData.Timer=TISM_SoftwareTimerSetVirtual(LIGHTS_DELAY_MS*1000);                                                                                                          
                                       break;
                    case LIGHTNING:    // Colorfull lightning effect.
                                       if(LightsData.Toggle==false)
                                       {
                                         // Random lightning strike on
                                         WS2812bFloodfillBuffer(LightsData.MyWS2812b,(LightsData.MultiColorLightning==true?WS2812bStandardColors[rand()%WS2812B_STANDARD_COLORS]:WS2812B_WHITE),0);
                                         LightsData.Toggle=true;
                                         LightsData.Timer=TISM_SoftwareTimerSetVirtual((LIGHTNING_MIN_DELAY_MS+(rand()%(LIGHTNING_MAX_DELAY_MS-LIGHTNING_MIN_DELAY_MS)))*1000);                                                                                                                  
                                       }
                                      else
                                      {
                                        // Lightning off
                                        WS2812bEmptyBuffer(LightsData.MyWS2812b);
                                        LightsData.Toggle=false;
                                        LightsData.Timer=TISM_SoftwareTimerSetVirtual((rand()%LIGHTNING_MAX_PAUSE_MS)*1000);                                                                                                                                                  
                                      }
                                      WS2812bWriteBuffer(LightsData.MyWS2812b);
                                      break;
                  }
                }                  
                break;
	  case STOP:  // Task required to stop this task.
		            if (ThisTask.TaskDebug) TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Stopping.");
		          
                // Set the task state to DOWN. 
                TISM_SchedulerSetMyTaskAttribute(ThisTask,TISM_SET_TASK_STATE,DOWN);
		            break;					
  }
		
  // Run completed.
  if (ThisTask.TaskDebug==DEBUG_HIGH) TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Run completed.");

  return (OK);
}

