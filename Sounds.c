/*

  Task to control the DY-SV17F sound board.

  Copyright (c) 2024 Maarten Klarenbeek (https://github.com/mjklaren)
  Distributed under the GPLv3 license

*/

#include "TISM.h"
#include "DY-SV17F.c"

#define SOUNDS_TX           16      // Pin 21
#define SOUNDS_RX           17      // Pin 22
#define SOUNDS_BAUDRATE     9600
#define SOUNDS_UART0        uart0
#define SOUNDS_VOLUME       74       // Default volume
#define SOUNDS_TIMER        500000   // Delay for checking status of the board.
#define SOUNDS_PLAY         2        // Play the specified sound effect
#define SOUNDS_STOP         3        // Stop playback
#define SOUNDS_SET_VOLUME   4        // Set the volume


/*
  The data for this task to run. 
*/
static uart_inst_t *UartID;
static bool IsPlaying;  
static uint64_t Timer;


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
uint8_t Sounds (TISM_Task ThisTask)
{
  if (ThisTask.TaskDebug==DEBUG_HIGH) TISM_EventLoggerLogEvent (ThisTask, TISM_LOG_EVENT_NOTIFY, "Run starting.");

  switch(ThisTask.TaskState)   
  {
    case INIT:  // Activities to initialize this task (e.g. initialize ports or peripherals).
                if (ThisTask.TaskDebug) TISM_EventLoggerLogEvent (ThisTask, TISM_LOG_EVENT_NOTIFY, "Initializing with priority %d.", ThisTask.TaskPriority);
				        
                // Initialize the sound board and set the volume.
                UartID=SOUNDS_UART0;
                DYSV17FInit(UartID, SOUNDS_BAUDRATE, SOUNDS_TX, SOUNDS_RX);
                IsPlaying=false;
                Timer=0;
                DYSV17FSetVolume(UartID, SOUNDS_VOLUME);

                // Report the number of sounds on the card.
                TISM_EventLoggerLogEvent (ThisTask, TISM_LOG_EVENT_NOTIFY, "Number of tracks on the device: %d.", DYSV17FGetSoundCount(UartID));

                // Go to sleep, we only respond to messages here.
                TISM_SchedulerSetMyTaskAttribute(ThisTask,TISM_SET_TASK_SLEEP,true);
				        break;
	  case RUN:   // Do the work.						
		      	    if (ThisTask.TaskDebug==DEBUG_HIGH) TISM_EventLoggerLogEvent (ThisTask, TISM_LOG_EVENT_NOTIFY, "Doing work with priority %d on core %d.", ThisTask.TaskPriority, ThisTask.RunningOnCoreID);

                // Check for incoming messages and process them - but only if we're not playing a sound.
                uint8_t MessageCounter=0;
                TISM_Message *MessageToProcess;

                // Only process the next message in the queue if no music is playing.
                while((IsPlaying==false) && (TISM_PostmanTaskMessagesWaiting(ThisTask)>0) && (MessageCounter<MAX_MESSAGES))
                {
                  MessageToProcess=TISM_PostmanTaskReadMessage(ThisTask);

                  if (ThisTask.TaskDebug) TISM_EventLoggerLogEvent (ThisTask, TISM_LOG_EVENT_NOTIFY, "Message '%ld' type %d from TaskID %d (%s) received.", MessageToProcess->Payload0, MessageToProcess->MessageType, MessageToProcess->SenderTaskID, System.Task[MessageToProcess->SenderTaskID].TaskName);

                  // Processed the message; delete it.
                  switch(MessageToProcess->MessageType)
                  {
                    case TISM_PING:               // Check if this process is still alive. Reply with a ECHO message type; return same message payload.
                                                  TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,MessageToProcess->SenderTaskID,TISM_ECHO,MessageToProcess->Payload0,0);
                                                  break;
                    case SOUNDS_PLAY:             // Play the specified sound effect. First select the track.
                                                  if (ThisTask.TaskDebug) TISM_EventLoggerLogEvent (ThisTask, TISM_LOG_EVENT_NOTIFY, "Playing sound effect %d", (uint16_t)MessageToProcess->Payload0);

                                                  DYSV17FPlaySpecified(UartID, (uint16_t)MessageToProcess->Payload0);
                                                  break;
                    case SOUNDS_STOP:             // Stop playback.
                                                  if (ThisTask.TaskDebug) TISM_EventLoggerLogEvent (ThisTask, TISM_LOG_EVENT_NOTIFY, "Stopping playback.");

                                                  DYSV17FStop(UartID);
                                                  IsPlaying=false;
                                                  break;        
                    case SOUNDS_SET_VOLUME:       // Set the volume
                                                  if (ThisTask.TaskDebug) TISM_EventLoggerLogEvent (ThisTask, TISM_LOG_EVENT_NOTIFY, "Setting volume to %d.", (uint8_t)MessageToProcess->Payload0);
                      
                                                  DYSV17FSetVolume(UartID, (uint8_t)MessageToProcess->Payload0);
                                                  break;
                    default:                      // Unknown message type - ignore.
                                                  break;
                  }
                  TISM_PostmanTaskDeleteMessage(ThisTask);
                  MessageCounter++;
                }

                // Are we still playing a sound?
                if(IsPlaying)
                {
                  // Did the virtual timer expire? If so, then check if the sound is still playing.
                  if(TISM_SoftwareTimerVirtualExpired(Timer))
                  {
                    if(DYSV17FIsPlaying(UartID))
                    {
                      // Sound is still being played.
       		      	    if (ThisTask.TaskDebug) TISM_EventLoggerLogEvent (ThisTask, TISM_LOG_EVENT_NOTIFY, "Sound is still being played. Waiting...");
                      Timer=TISM_SoftwareTimerSetVirtual(SOUNDS_TIMER);
                      IsPlaying=true;
                    }
                    else
                    {
                      // No sound is being played.
                      IsPlaying=false;
                    }
                  }
                }
                else
                {
                  // Is the message queue empty? Then go back to sleep.
                  if(TISM_PostmanTaskMessagesWaiting(ThisTask)==0)
                    TISM_SchedulerSetMyTaskAttribute(ThisTask,TISM_SET_TASK_SLEEP,true);
                }
				        break;
	  case STOP:  // Task required to stop this task.
		            if (ThisTask.TaskDebug) TISM_EventLoggerLogEvent (ThisTask, TISM_LOG_EVENT_NOTIFY, "Stopping.");
		          
                // Set the task state to DOWN. 
                TISM_SchedulerSetMyTaskAttribute(ThisTask,TISM_SET_TASK_STATE,DOWN);
		            break;					
  }
		
  // Run completed.
  if (ThisTask.TaskDebug==DEBUG_HIGH) TISM_EventLoggerLogEvent (ThisTask, TISM_LOG_EVENT_NOTIFY, "Run completed.");

  return (OK);
}

