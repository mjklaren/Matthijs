/*

  "Matthijs" - A skeleton animatronic project

  Copyright (c) 2025 Maarten Klarenbeek (https://github.com/mjklaren)
  Distributed under the GPLv3 license

*/

#include "TISM.h"
#include "pico/stdlib.h"
#include <time.h>


// Submenus
#define MAIN_MENU         100
#define SYSTEM_MENU       110
#define ACTUATORS_MENU    120
#define SERVOS_MENU       130
#define SWITCHES_MENU     140
#define SCENES_MENU       150
#define SOUNDS_MENU       160
#define FACIAL_MENU       170
#define LIGHTS_MENU       180

// Basic actions
#define STOP_PLAYING      73
#define PLAY_SOUND_1      74
#define PLAY_SOUND_2      75
#define PLAY_SOUND_3      76
#define PLAY_SOUND_4      77
#define PLAY_SOUND_5      78
#define START_POSITION    79
#define LOOK_FORWARD      80 
#define LOOK_UP           81
#define LOOK_UP_LEFT      82
#define LOOK_UP_RIGHT     83
#define LOOK_DOWN         84
#define LOOK_DOWN_LEFT    85
#define LOOK_DOWN_RIGHT   86
#define LEAN_FORWARD      87
#define LEAN_BACKWARD     88
#define RAISE_CANDLE      89
#define NUDGE_CANDLE_UP   90
#define LOWER_CANDLE      91
#define NUDGE_CANDLE_DOWN 92
#define RAISE_LARM        93
#define NUDGE_LARM_UP     94
#define LOWER_LARM        95
#define NUDGE_LARM_DOWN   96
#define RAISE_LHAND       97
#define NUDGE_LHAND_UP    98
#define LOWER_LHAND       99
#define NUDGE_LHAND_DOWN  100
#define LOOK_ALIVE        101
#define LOOK_ALIVE_ON     102
#define LOOK_ALIVE_OFF    103
#define EYES_ON           104
#define EYES_OFF          105
#define TALKING_ON        106
#define TALKING_OFF       107
#define MOUTH_OPEN        108
#define MOUTH_CLOSE       109
#define CANDLE_ON         110
#define CANDLE_OFF        111
#define LIGHTNING_ON      112
#define ALL_LIGHTS_OFF    113
#define MAGIC_COLORS_ON   114
#define PUMPKIN_1_ON      115    
#define PUMPKIN_2_ON      116  
#define AUTOPLAY_NEXT     117
#define SCENE_A           118
#define SCENE_B           119 
#define SCENE_C           120

#define LOOK_ALIVE_TIMER  2000  // in msec
#define SERVOS_DELAY      8     // in msec; lower values increase servo speed
#define AUTOPLAY_TIMER    100000 // in msec; time between scenes in AutoPlay mode
#define SOUND_INIT_VOL    27    // Default volume
#define SOUND_MAX_VOL     30    // Maximum volume

/*
  The structure containing all data for this task to run. These variables allow the task to remain its state as the stack 
  and heap are not saved between runs. We use a struct, this is not mandatory (just an easy way to make usre variable
  names are unique in the whole system).
*/
struct ScenesData
{
  uint8_t ActiveScene,ControlTaskID,SoundsTaskID,LightsTaskID;
  int Character,ActiveMenu,SoundVolume;
  bool DisplayHelpNext,LookAlive,AutoPlay;
  uint64_t TimestampLastCommand;
} ScenesData;


void WriteScene(TISM_Task ThisTask,uint8_t Scene)
{
  // Create scenes by scheduling actions using the software timer.
  uint16_t TimerMsec;
  switch(Scene)
  {
    case SCENE_A: // First scene
                  // Write the movements to the scheduler
                  TimerMsec=1000;
                  TISM_SoftwareTimerSet(ThisTask,START_POSITION,false,TimerMsec);      // Start with setting everything to starting position.
                  TimerMsec+=2000;                                                     // 2 second delay
                  TISM_SoftwareTimerSet(ThisTask,MOUTH_CLOSE,false,TimerMsec);         // Close mouth
                  TimerMsec+=3000;                                                     // 3 second delay
                  TISM_SoftwareTimerSet(ThisTask,LOOK_DOWN,false,TimerMsec);           // Look down
                  TISM_SoftwareTimerSet(ThisTask,LEAN_FORWARD,false,TimerMsec);        // Lean forward
                  TimerMsec+=3000;                                                     // 6 second delay
                  TISM_SoftwareTimerSet(ThisTask,EYES_ON,false,TimerMsec);             // Turn eyes on
                  TimerMsec+=1000;                                                     // 1 second delay
                  TISM_SoftwareTimerSet(ThisTask,RAISE_CANDLE,false,TimerMsec);        // Raise candle
                  TISM_SoftwareTimerSet(ThisTask,TALKING_ON,false,TimerMsec);          // Talking on
                  TimerMsec+=4000;                                                     // 3 second delay
                  TISM_SoftwareTimerSet(ThisTask,PLAY_SOUND_4,false,TimerMsec);        // Play sound 4
                  TISM_SoftwareTimerSet(ThisTask,CANDLE_ON,false,TimerMsec);           // Candle on
                  TimerMsec+=500;                                                      // 0.5 second delay
                  TISM_SoftwareTimerSet(ThisTask,LEAN_BACKWARD,false,TimerMsec);       // Lean backward
                  TISM_SoftwareTimerSet(ThisTask,LOOK_UP,false,TimerMsec);             // Look up
                  TimerMsec+=4000;                                                     // 4 second delay
                  TISM_SoftwareTimerSet(ThisTask,LOOK_FORWARD,false,TimerMsec);        // Look forward
                  TISM_SoftwareTimerSet(ThisTask,LOOK_ALIVE_ON,false,TimerMsec);       // Look alive
                  TimerMsec+=3000;                                                     // 3 second delay
                  TISM_SoftwareTimerSet(ThisTask,RAISE_LARM,false,TimerMsec);          // Raise left arm
                  TimerMsec+=5000;                                                     // 5 second delay
                  TISM_SoftwareTimerSet(ThisTask,NUDGE_LHAND_UP,false,TimerMsec);      // Nudge hand up
                  TimerMsec+=1500;                                                     // 1.5 second delay
                  TISM_SoftwareTimerSet(ThisTask,NUDGE_LHAND_DOWN,false,TimerMsec);    // Nudge hand down
                  TimerMsec+=1500;                                                     // 1.5 second delay
                  TISM_SoftwareTimerSet(ThisTask,NUDGE_LHAND_UP,false,TimerMsec);      // Nudge hand up
                  TimerMsec+=1500;                                                     // 1.5 second delay
                  TISM_SoftwareTimerSet(ThisTask,NUDGE_LHAND_DOWN,false,TimerMsec);    // Nudge hand down
                  TimerMsec+=1500;                                                     // 1.5 second delay
                  TISM_SoftwareTimerSet(ThisTask,NUDGE_LHAND_UP,false,TimerMsec);      // Nudge hand up
                  TimerMsec+=1500;                                                     // 1.5 second delay
                  TISM_SoftwareTimerSet(ThisTask,NUDGE_LHAND_DOWN,false,TimerMsec);    // Nudge hand down              
                  TimerMsec+=4000;                                                     // 4 second delay
                  TISM_SoftwareTimerSet(ThisTask,LOWER_LARM,false,TimerMsec);          // Arm down
                  TISM_SoftwareTimerSet(ThisTask,LOWER_LHAND,false,TimerMsec);         // Hand down
                  TimerMsec+=4000;                                                     // 4 second delay
                  TISM_SoftwareTimerSet(ThisTask,NUDGE_CANDLE_UP,false,TimerMsec);     // Nudge Candle down
                  TimerMsec+=4000;                                                     // 4 second delay
                  TISM_SoftwareTimerSet(ThisTask,NUDGE_CANDLE_DOWN,false,TimerMsec);   // Nudge Candle down
                  TimerMsec+=4000;                                                     // 4 second delay
                  TISM_SoftwareTimerSet(ThisTask,RAISE_LARM,false,TimerMsec);          // Raise left arm
                  TISM_SoftwareTimerSet(ThisTask,RAISE_LHAND,false,TimerMsec);         // Raise left hand
                  TISM_SoftwareTimerSet(ThisTask,LEAN_FORWARD,false,TimerMsec);        // Lean forward
                  TimerMsec+=3000;                                                     // 3 second delay
                  TISM_SoftwareTimerSet(ThisTask,PLAY_SOUND_5,false,TimerMsec);        // Play sound 5
                  TISM_SoftwareTimerSet(ThisTask,LOWER_LHAND,false,TimerMsec);         // Lower left hand
                  TimerMsec+=1000;                                                     // 1 second delay
                  TISM_SoftwareTimerSet(ThisTask,LIGHTNING_ON,false,TimerMsec);        // Lightning effect
                  TimerMsec+=3000;                                                     // 3 second delay
                  TISM_SoftwareTimerSet(ThisTask,MAGIC_COLORS_ON,false,TimerMsec);     // Magic colors effect
                  TimerMsec+=2000;                                                     // 2 second delay
                  TISM_SoftwareTimerSet(ThisTask,LEAN_BACKWARD,false,TimerMsec);       // Lean forward
                  TISM_SoftwareTimerSet(ThisTask,MOUTH_CLOSE,false,TimerMsec);         // Close mouth
                  TISM_SoftwareTimerSet(ThisTask,LOWER_LARM,false,TimerMsec);          // Raise left arm
                  TISM_SoftwareTimerSet(ThisTask,LOWER_LHAND,false,TimerMsec);         // Lower left hand
                  TimerMsec+=2000;                                                     // 5 second delay
                  TISM_SoftwareTimerSet(ThisTask,CANDLE_OFF,false,TimerMsec);          // Candle off
                  TISM_SoftwareTimerSet(ThisTask,LOWER_CANDLE,false,TimerMsec);        // Candle down

                  // Scene done
                  TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "============================");
                  TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Scene A written to scheduler");
                  TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "============================");
                  TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Time between first and last command: %d msec.", TimerMsec);
                  break;
    case SCENE_B: // Second scene
                  // Write the movements to the scheduler
                  TimerMsec=1000;
                  TISM_SoftwareTimerSet(ThisTask,START_POSITION,false,TimerMsec);      // Start with setting everything to starting position.
                  TimerMsec+=2000;                                                     // 2 second delay
                  TISM_SoftwareTimerSet(ThisTask,MOUTH_CLOSE,false,TimerMsec);         // Close mouth
                  TimerMsec+=3000;                                                     // 3 second delay
                  TISM_SoftwareTimerSet(ThisTask,LOOK_FORWARD,false,TimerMsec);           // Look down
                  TISM_SoftwareTimerSet(ThisTask,LEAN_FORWARD,false,TimerMsec);        // Lean forward
                  TimerMsec+=3000;                                                     // 6 second delay
                  TISM_SoftwareTimerSet(ThisTask,EYES_ON,false,TimerMsec);             // Turn eyes on
                  TimerMsec+=1000;                                                     // 1 second delay
                  TISM_SoftwareTimerSet(ThisTask,RAISE_CANDLE,false,TimerMsec);        // Raise candle
                  TISM_SoftwareTimerSet(ThisTask,TALKING_ON,false,TimerMsec);          // Talking on
                  TimerMsec+=4000;                                                     // 3 second delay
                  TISM_SoftwareTimerSet(ThisTask,PLAY_SOUND_2,false,TimerMsec);        // Play sound 2
                  TISM_SoftwareTimerSet(ThisTask,CANDLE_ON,false,TimerMsec);           // Candle on
                  TimerMsec+=3000;                                                     // 3 second delay
                  TISM_SoftwareTimerSet(ThisTask,RAISE_LARM,false,TimerMsec);          // Raise left arm
                  TimerMsec+=5000;                                                     // 5 second delay
                  TISM_SoftwareTimerSet(ThisTask,LOOK_ALIVE_OFF,false,TimerMsec);       // Look alive OFF
                  TimerMsec+=1000;                                                     // 1 second delay
                  TISM_SoftwareTimerSet(ThisTask,LOOK_UP,false,TimerMsec); 
                  TISM_SoftwareTimerSet(ThisTask,NUDGE_LHAND_UP,false,TimerMsec);      // Nudge hand up
                  TimerMsec+=1500;                                                     // 1.5 second delay
                  TISM_SoftwareTimerSet(ThisTask,NUDGE_LHAND_DOWN,false,TimerMsec);    // Nudge hand down
                  TimerMsec+=1500;                                                     // 1.5 second delay
                  TISM_SoftwareTimerSet(ThisTask,NUDGE_LHAND_UP,false,TimerMsec);      // Nudge hand up
                  TimerMsec+=1500;                                                     // 1.5 second delay
                  TISM_SoftwareTimerSet(ThisTask,NUDGE_LHAND_DOWN,false,TimerMsec);    // Nudge hand down
                  TimerMsec+=1500;                                                     // 1.5 second delay
                  TISM_SoftwareTimerSet(ThisTask,NUDGE_LHAND_UP,false,TimerMsec);      // Nudge hand up
                  TimerMsec+=1500;                                                     // 1.5 second delay
                  TISM_SoftwareTimerSet(ThisTask,NUDGE_LHAND_DOWN,false,TimerMsec);    // Nudge hand down
                  TimerMsec+=1000;                                                     // 1 second delay
                  TISM_SoftwareTimerSet(ThisTask,LOOK_FORWARD,false,TimerMsec);          
                  TISM_SoftwareTimerSet(ThisTask,LOOK_ALIVE_ON,false,TimerMsec);       // Look alive ON
                  TimerMsec+=4000;                                                     // 4 second delay
                  TISM_SoftwareTimerSet(ThisTask,LOWER_LARM,false,TimerMsec);          // Arm down
                  TISM_SoftwareTimerSet(ThisTask,LOWER_LHAND,false,TimerMsec);         // Hand down
                  TimerMsec+=4000;                                                     // 4 second delay
                  TISM_SoftwareTimerSet(ThisTask,NUDGE_CANDLE_UP,false,TimerMsec);     // Nudge Candle down
                  TimerMsec+=4000;                                                     // 4 second delay
                  TISM_SoftwareTimerSet(ThisTask,NUDGE_CANDLE_DOWN,false,TimerMsec);   // Nudge Candle down
                  TimerMsec+=4000;                                                     // 4 second delay
                  TISM_SoftwareTimerSet(ThisTask,RAISE_LARM,false,TimerMsec);          // Raise left arm
                  TISM_SoftwareTimerSet(ThisTask,RAISE_LHAND,false,TimerMsec);         // Raise left hand
                  TISM_SoftwareTimerSet(ThisTask,LEAN_FORWARD,false,TimerMsec);        // Lean forward
                  TimerMsec+=3000;                                                     // 3 second delay
                  TISM_SoftwareTimerSet(ThisTask,PLAY_SOUND_5,false,TimerMsec);        // Play sound 5
                  TISM_SoftwareTimerSet(ThisTask,LOWER_LHAND,false,TimerMsec);         // Lower left hand
                  TimerMsec+=1000;                                                     // 1 second delay
                  TISM_SoftwareTimerSet(ThisTask,LIGHTNING_ON,false,TimerMsec);        // Lightning effect
                  TimerMsec+=2000;                                                     // 2 second delay
                  TISM_SoftwareTimerSet(ThisTask,LOOK_UP,false,TimerMsec);             // Look up
                  TimerMsec+=1000;                                                     // 1 second delay
                  TISM_SoftwareTimerSet(ThisTask,MAGIC_COLORS_ON,false,TimerMsec);     // Magic colors effect
                  TimerMsec+=1000;                                                     // 3 second delay
                  TISM_SoftwareTimerSet(ThisTask,LEAN_BACKWARD,false,TimerMsec);       // Lean forward
                  TISM_SoftwareTimerSet(ThisTask,MOUTH_CLOSE,false,TimerMsec);         // Close mouth
                  TISM_SoftwareTimerSet(ThisTask,LOWER_LARM,false,TimerMsec);          // Raise left arm
                  TISM_SoftwareTimerSet(ThisTask,LOWER_LHAND,false,TimerMsec);         // Lower left hand
                  TimerMsec+=2000;                                                     // 5 second delay
                  TISM_SoftwareTimerSet(ThisTask,CANDLE_OFF,false,TimerMsec);          // Candle off
                  TISM_SoftwareTimerSet(ThisTask,LOWER_CANDLE,false,TimerMsec);        // Candle down
                  TISM_SoftwareTimerSet(ThisTask,LOOK_FORWARD,false,TimerMsec);             // Look up
                  TISM_SoftwareTimerSet(ThisTask,LOOK_ALIVE_ON,false,TimerMsec);       // Look alive ON

                  // Scene done
                  TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "============================");
                  TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Scene B written to scheduler");
                  TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "============================");
                  TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Time between first and last command: %d msec.", TimerMsec);
                  break;
    case SCENE_C: // Third scene
                  // Write the movements to the scheduler
                  TimerMsec=1000;
                  TISM_SoftwareTimerSet(ThisTask,START_POSITION,false,TimerMsec);      // Start with setting everything to starting position.
                  TimerMsec+=2000;                                                     // 2 second delay
                  TISM_SoftwareTimerSet(ThisTask,MOUTH_CLOSE,false,TimerMsec);         // Close mouth
                  TimerMsec+=3000;                                                     // 3 second delay
                  TISM_SoftwareTimerSet(ThisTask,LOOK_DOWN,false,TimerMsec);           // Look down
                  TISM_SoftwareTimerSet(ThisTask,LEAN_FORWARD,false,TimerMsec);        // Lean forward
                  TimerMsec+=3000;                                                     // 6 second delay
                  TISM_SoftwareTimerSet(ThisTask,EYES_ON,false,TimerMsec);             // Turn eyes on
                  TimerMsec+=1000;                                                     // 1 second delay
                  TISM_SoftwareTimerSet(ThisTask,RAISE_CANDLE,false,TimerMsec);        // Raise candle
                  TISM_SoftwareTimerSet(ThisTask,TALKING_ON,false,TimerMsec);          // Talking on
                  TimerMsec+=4000;                                                     // 3 second delay
                  TISM_SoftwareTimerSet(ThisTask,PLAY_SOUND_3,false,TimerMsec);        // Play sound 3
                  TISM_SoftwareTimerSet(ThisTask,CANDLE_ON,false,TimerMsec);           // Candle on
                  TimerMsec+=500;                                                      // 0.5 second delay
                  TISM_SoftwareTimerSet(ThisTask,LEAN_BACKWARD,false,TimerMsec);       // Lean backward
                  TISM_SoftwareTimerSet(ThisTask,LOOK_UP,false,TimerMsec);             // Look up
                  TimerMsec+=4000;                                                     // 4 second delay
                  TISM_SoftwareTimerSet(ThisTask,LOOK_FORWARD,false,TimerMsec);        // Look forward
                  TISM_SoftwareTimerSet(ThisTask,LOOK_ALIVE_ON,false,TimerMsec);       // Look alive
                  TimerMsec+=3000;                                                     // 3 second delay
                  TISM_SoftwareTimerSet(ThisTask,RAISE_LARM,false,TimerMsec);          // Raise left arm
                  TimerMsec+=5000;                                                     // 5 second delay
                  TISM_SoftwareTimerSet(ThisTask,NUDGE_LHAND_UP,false,TimerMsec);      // Nudge hand up
                  TimerMsec+=1500;                                                     // 1.5 second delay
                  TISM_SoftwareTimerSet(ThisTask,NUDGE_LHAND_DOWN,false,TimerMsec);    // Nudge hand down
                  TimerMsec+=1500;                                                     // 1.5 second delay
                  TISM_SoftwareTimerSet(ThisTask,NUDGE_LHAND_UP,false,TimerMsec);      // Nudge hand up
                  TimerMsec+=1500;                                                     // 1.5 second delay
                  TISM_SoftwareTimerSet(ThisTask,NUDGE_LHAND_DOWN,false,TimerMsec);    // Nudge hand down
                  TimerMsec+=1500;                                                     // 1.5 second delay
                  TISM_SoftwareTimerSet(ThisTask,NUDGE_LHAND_UP,false,TimerMsec);      // Nudge hand up
                  TimerMsec+=1500;                                                     // 1.5 second delay
                  TISM_SoftwareTimerSet(ThisTask,NUDGE_LHAND_DOWN,false,TimerMsec);    // Nudge hand down              
                  TimerMsec+=4000;                                                     // 4 second delay
                  TISM_SoftwareTimerSet(ThisTask,LOWER_LARM,false,TimerMsec);          // Arm down
                  TISM_SoftwareTimerSet(ThisTask,LOWER_LHAND,false,TimerMsec);         // Hand down
                  TimerMsec+=4000;                                                     // 4 second delay
                  TISM_SoftwareTimerSet(ThisTask,NUDGE_CANDLE_UP,false,TimerMsec);     // Nudge Candle down
                  TimerMsec+=4000;                                                     // 4 second delay
                  TISM_SoftwareTimerSet(ThisTask,NUDGE_CANDLE_DOWN,false,TimerMsec);   // Nudge Candle down
                  TimerMsec+=4000;                                                     // 4 second delay
                  TISM_SoftwareTimerSet(ThisTask,RAISE_LARM,false,TimerMsec);          // Raise left arm
                  TISM_SoftwareTimerSet(ThisTask,RAISE_LHAND,false,TimerMsec);         // Raise left hand
                  TISM_SoftwareTimerSet(ThisTask,LEAN_FORWARD,false,TimerMsec);        // Lean forward
                  TimerMsec+=3000;                                                     // 3 second delay
                  TISM_SoftwareTimerSet(ThisTask,PLAY_SOUND_5,false,TimerMsec);        // Play sound 5
                  TISM_SoftwareTimerSet(ThisTask,LOWER_LHAND,false,TimerMsec);         // Lower left hand
                  TimerMsec+=1000;                                                     // 1 second delay
                  TISM_SoftwareTimerSet(ThisTask,LIGHTNING_ON,false,TimerMsec);         // Lightning effect
                  TimerMsec+=3000;                                                      // 2 second delay
                  TISM_SoftwareTimerSet(ThisTask,MAGIC_COLORS_ON,false,TimerMsec);      // Magic colors effect
                  TimerMsec+=2000;                                                      // 3 second delay
                  TISM_SoftwareTimerSet(ThisTask,LEAN_BACKWARD,false,TimerMsec);        // Lean forward
                  TISM_SoftwareTimerSet(ThisTask,MOUTH_CLOSE,false,TimerMsec);         // Close mouth
                  TISM_SoftwareTimerSet(ThisTask,LOWER_LARM,false,TimerMsec);          // Raise left arm
                  TISM_SoftwareTimerSet(ThisTask,LOWER_LHAND,false,TimerMsec);         // Lower left hand
                  TimerMsec+=2000;                                                     // 5 second delay
                  TISM_SoftwareTimerSet(ThisTask,CANDLE_OFF,false,TimerMsec);          // Candle off
                  TISM_SoftwareTimerSet(ThisTask,LOWER_CANDLE,false,TimerMsec);        // Candle down

                  // Scene done
                  TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "============================");
                  TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Scene C written to scheduler");
                  TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "============================");
                  TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Time between first and last command: %d msec.", TimerMsec);
                  break;
  }
}



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
uint8_t Scenes (TISM_Task ThisTask)
{
  if (ThisTask.TaskDebug==DEBUG_HIGH) TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Run starting.");
  
  switch(ThisTask.TaskState)   
  {
    case INIT:  // Activities to initialize this task (e.g. initialize ports or peripherals).
                if (ThisTask.TaskDebug) TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Initializing with priority %d.", ThisTask.TaskPriority);
				        
                // For tasks that only respond to events (=messages) we could set the sleep attribute to ´true'.
                // TISM_SchedulerSetMyTaskAttribute(ThisTask,TISM_SET_TASK_SLEEP,true);

                TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "===================================");
                TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "           - Matthijs -");
                TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Console running, press '?' for help");
                TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "===================================");
                ScenesData.ControlTaskID=TISM_GetTaskID("Control");
                ScenesData.SoundsTaskID=TISM_GetTaskID("Sounds" );
                ScenesData.LightsTaskID=TISM_GetTaskID("Lights" );
                ScenesData.ActiveMenu=MAIN_MENU;
                ScenesData.DisplayHelpNext=true;
                ScenesData.TimestampLastCommand=0;
                ScenesData.SoundVolume=SOUND_INIT_VOL;
                ScenesData.LookAlive=false;

                // Set a timer for the Look Alive feature
                TISM_SoftwareTimerSet(ThisTask,LOOK_ALIVE,true,LOOK_ALIVE_TIMER);          // Repetitive timer for Look Alive feature

                // Schedule a scene to start automatically
                ScenesData.AutoPlay=true;
                ScenesData.ActiveScene=SCENE_C;                                            // Next scene will be SCENE_A
                TISM_SoftwareTimerSet(ThisTask,AUTOPLAY_NEXT,true,AUTOPLAY_TIMER);         // Schedule autoplay events using a repetitive timer.
                TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Autoplay scheduled, interval %d seconds.", AUTOPLAY_TIMER/1000);
                
                srand(time(NULL));
                TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ThisTask.TaskID,START_POSITION,0,0);   // Everything in starting position
				        break;
	  case RUN:   // Do the work.						
		      	    if (ThisTask.TaskDebug==DEBUG_HIGH) TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Doing work with priority %d on core %d.", ThisTask.TaskPriority, ThisTask.RunningOnCoreID);

                // First check for incoming messages and process them.
                uint8_t MessageCounter=0;
                TISM_Message *MessageToProcess;
                while((TISM_PostmanTaskMessagesWaiting(ThisTask)>0) && (MessageCounter<MAX_MESSAGES))
                {
                  MessageToProcess=TISM_PostmanTaskReadMessage(ThisTask);

                  if (ThisTask.TaskDebug) TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Message '%ld' type %d from TaskID %d (HostID %d) received.", MessageToProcess->Payload0, MessageToProcess->MessageType, MessageToProcess->SenderTaskID, MessageToProcess->SenderHostID);

                  // Process the message, then delete it.
                  switch(MessageToProcess->MessageType)
                  {
                    case TISM_PING:         // Check if this process is still alive. Reply with a ECHO message type; return same message payload.
                                            TISM_PostmanTaskWriteMessage(ThisTask,MessageToProcess->SenderHostID,MessageToProcess->SenderTaskID,TISM_ECHO,MessageToProcess->Payload0,0);
                                            break;


// ============================== Basic actions of the animatronic: sounds ==============================
                    case STOP_PLAYING:      // Stop all sound effects
                                            TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.SoundsTaskID,SOUNDS_STOP,0,0);                                           
                                            break;              
                    case PLAY_SOUND_1:      // Play the first sound effect
                                            TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.SoundsTaskID,SOUNDS_PLAY,1,0);
                                            break;
                    case PLAY_SOUND_2:      // Play the second sound effect
                                            TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.SoundsTaskID,SOUNDS_PLAY,2,0);
                                            break;
                    case PLAY_SOUND_3:      // Play the third sound effect
                                            TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.SoundsTaskID,SOUNDS_PLAY,3,0);
                                            break;
                    case PLAY_SOUND_4:      // Play the fourth sound effect
                                            TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.SoundsTaskID,SOUNDS_PLAY,4,0);
                                            break;   
                    case PLAY_SOUND_5:      // Play the fourth sound effect
                                            TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.SoundsTaskID,SOUNDS_PLAY,5,0);
                                            break;        

              
// ============================== Basic actions of the animatronic: movements ==============================
                    case START_POSITION:    // Servos around degrees position
                                            TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ThisTask.TaskID,MOUTH_CLOSE,0,0);
                                            TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SERVO1,100,SERVOS_DELAY);  
                                            TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SERVO2,80,SERVOS_DELAY);
                                            //TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SERVO3,90,SERVOS_DELAY);                                                                                                          
                                            TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_ACTUATOR0,ACTUATOR_RETRACT,3000);
                                            TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_ACTUATOR1,ACTUATOR_EXTEND,5000);  
                                            TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_ACTUATOR2,ACTUATOR_EXTEND,4000);  
                                            TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_ACTUATOR3,ACTUATOR_RETRACT,3000);
                                          
                                            // Switches to off (e.g. eyes) and candle off
                                            TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SWITCH0,SWITCH_OFF,0);                                           
                                            TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SWITCH1,SWITCH_OFF,0);                                           
                                            //TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SWITCH2,SWITCH_OFF,0);                                           
                                            //TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SWITCH3,SWITCH_OFF,0); 

                                            // Stop sounds playing, no talking or head movement, no lights.
                                            TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.SoundsTaskID,SOUNDS_STOP,0,0);                                           
                                            TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_MOUTH_INACTIVE,0,0);
                                            TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.LightsTaskID,LIGHTS_OFF,0,0);                                                                               
                                            ScenesData.LookAlive=false;
                                            break;
                    case LOOK_FORWARD:      // Look straight ahead
                                            TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SERVO1,100,SERVOS_DELAY);  
                                            TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SERVO2,80,SERVOS_DELAY);                    
                                            break;
                    case LOOK_UP:           TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SERVO1,0,SERVOS_DELAY);  
                                            TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SERVO2,180,SERVOS_DELAY);  
                                            break;    
                    case LOOK_UP_LEFT:      TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SERVO1,180,SERVOS_DELAY);  
                                            TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SERVO2,135,SERVOS_DELAY);     
                                            break;
                    case LOOK_UP_RIGHT:     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SERVO1,90,SERVOS_DELAY);  
                                            TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SERVO2,45,SERVOS_DELAY);     
                                            break;
                    case LOOK_DOWN:         TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SERVO1,170,SERVOS_DELAY);  
                                            TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SERVO2,10,SERVOS_DELAY);   
                                            break;
                    case LOOK_DOWN_LEFT:    TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SERVO1,180,SERVOS_DELAY);  
                                            TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SERVO2,45,SERVOS_DELAY);   
                                            break;
                    case LOOK_DOWN_RIGHT:   TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SERVO1,90,SERVOS_DELAY);  
                                            TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SERVO2,0,SERVOS_DELAY);   
                                            break;
                    case LEAN_FORWARD:      TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_ACTUATOR0,ACTUATOR_EXTEND,2000);  
                                            break;
                    case LEAN_BACKWARD:     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_ACTUATOR0,ACTUATOR_RETRACT,3000);  
                                            break;
                    case RAISE_CANDLE:      TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_ACTUATOR1,ACTUATOR_RETRACT,2500);  
                                            break;
                    case NUDGE_CANDLE_UP:   TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_ACTUATOR1,ACTUATOR_RETRACT,1000);
                                            break;
                    case LOWER_CANDLE:      TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_ACTUATOR1,ACTUATOR_EXTEND,5000);  
                                            break;
                    case NUDGE_CANDLE_DOWN: TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_ACTUATOR1,ACTUATOR_EXTEND,1000);
                                            break; 
                    case RAISE_LHAND:       TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_ACTUATOR2,ACTUATOR_RETRACT,4000);                                                                                                         
                                            break;
                    case NUDGE_LHAND_UP:    TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_ACTUATOR2,ACTUATOR_RETRACT,1000);                                                                                                         
                                            break;
                    case LOWER_LHAND:       TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_ACTUATOR2,ACTUATOR_EXTEND,4000);  
                                            break;
                    case NUDGE_LHAND_DOWN:  TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_ACTUATOR2,ACTUATOR_EXTEND,1000);
                                            break;
                    case RAISE_LARM:        TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_ACTUATOR3,ACTUATOR_EXTEND,3000);                                                     
                                            break;
                    case NUDGE_LARM_UP:     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_ACTUATOR3,ACTUATOR_EXTEND,1000);
                                            break;
                    case LOWER_LARM:        TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_ACTUATOR3,ACTUATOR_RETRACT,3000);                                                                                                        
                                            break;
                    case NUDGE_LARM_DOWN:   TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_ACTUATOR3,ACTUATOR_RETRACT,1000);                                                                                                        
                                            break;                                           
                    case TALKING_ON:        //                                                      
                                            TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_MOUTH_ACTIVE,0,0);                                           
                                            break;
                                            //
                                            break;
                    case TALKING_OFF:       //
                                            TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_MOUTH_INACTIVE,0,0);                                           
                                            break;
                    case MOUTH_OPEN:        // Open mouth fully, servo #0
                                            TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_MOUTH_MIN,0,0);         // Servo is connected reverse!                                  
                                            break;
                    case MOUTH_CLOSE:        // Turn mouth via servo #0 OFF
                                            TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_MOUTH_MAX,0,0);         // Servo is connected reverse!    
                                            break;
                    case PUMPKIN_1_ON:      // Switch pumpkin light #1 ON
                                            TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.LightsTaskID,CONTROL_SWITCH2,SWITCH_PULSE,500);                                                                                   
                                            break;
                    case PUMPKIN_2_ON:      // Switch pumpkin light #1 ON
                                            TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.LightsTaskID,CONTROL_SWITCH3,SWITCH_PULSE,500);                                                                                   
                                            break;  

              
// ============================== Basic actions of the animatronic: lights ==============================
                    case LIGHTNING_ON:      // Switch lightning effect ON
                                            TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.LightsTaskID,LIGHTNING,0,0);
                                            break;
                    case ALL_LIGHTS_OFF:    // Switch lights OFF
                                            TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.LightsTaskID,LIGHTS_OFF,0,0);                                                                               
                                            break;
                    case MAGIC_COLORS_ON:   // Switch magic colors effect ON
                                            TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.LightsTaskID,MAGIC_COLORS,0,0);
                                            break;
                    case CANDLE_ON:         // Switch candle ON
                                            TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SWITCH1,SWITCH_ON,0);                                                                                   
                                            break;
                    case CANDLE_OFF:        // Switch candle OFF
                                            TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SWITCH1,SWITCH_OFF,0);                                                                                   
                                            break;   
                    case EYES_ON:           // Eyes ON - device CONTROL_SWITCH0
                                            TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SWITCH0,SWITCH_ON,0);                                                                                   
                                            break;
                    case EYES_OFF:          // Eyes OFF - device CONTROL_SWITCH0
                                            TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SWITCH0,SWITCH_OFF,0);    
                                            break;                                                                                     
                                            
              
// ============================== Repetitive actions, triggered by a timer ==============================
                    case LOOK_ALIVE_ON:     // Enable Look Alive; respond to LOOK_ALIVE messages
                                            ScenesData.LookAlive=true;
                                            break;
                    case LOOK_ALIVE_OFF:    // Disable Look Alive; ignore LOOK_ALIVE messages
                                            ScenesData.LookAlive=false;
                                            break;                     
                    case LOOK_ALIVE:        // Is LookAlive true? Then make something happen. This message is triggered by a repetitive timer.
                                            if(ScenesData.LookAlive==true)
                                            {
                                              // Move the head to a random position
                                              switch(rand()%5)
                                              {
                                                case 1: TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ThisTask.TaskID,LOOK_UP_LEFT,0,0);                                           
                                                        break; 
                                                case 2: TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ThisTask.TaskID,LOOK_UP_RIGHT,0,0); 
                                                        break;              
                                                case 3: TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ThisTask.TaskID,LOOK_DOWN_LEFT,0,0); 
                                                        break;
                                                case 4: TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ThisTask.TaskID,LOOK_DOWN_RIGHT,0,0); 
                                                        break;  
                                              }            
                                            }
                                            break;                                               
                    case AUTOPLAY_NEXT:     // Schedule the next scene
                                            if(ScenesData.AutoPlay==true)
                                            {
                                              switch(ScenesData.ActiveScene)
                                              {
                                                case SCENE_A: ScenesData.ActiveScene=SCENE_B;
                                                              WriteScene(ThisTask,SCENE_B);
                                                              break;
                                                case SCENE_B: ScenesData.ActiveScene=SCENE_C;
                                                              WriteScene(ThisTask,SCENE_C);
                                                              break;
                                                case SCENE_C: ScenesData.ActiveScene=SCENE_A;
                                                              WriteScene(ThisTask,SCENE_A);
                                                              break;
                                                default:      ScenesData.ActiveScene=SCENE_A;
                                                              WriteScene(ThisTask,SCENE_A);
                                                              break;
                                              }
                                            }
                                            break;                                            
                    default:                // Unknown message type - ignore.
                                            break;
                  }
                  TISM_PostmanTaskDeleteMessage(ThisTask);
                  MessageCounter++;
                }


// ============================== The menu system ==============================
                // Read a character from STDIN and start a scene on request.
                ScenesData.Character=getchar_timeout_us(0);    // Non-blocking read
                if(ScenesData.Character!=PICO_ERROR_TIMEOUT || ScenesData.DisplayHelpNext==true)
                {
                  char Option;

                  // Do we need to display the help function?
                  if (ScenesData.DisplayHelpNext==true)
                  {
                    Option='?';
                    ScenesData.DisplayHelpNext=false;
                    TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "");                    
                  }
                  else
                  {
                    // Character available for reading.
                    Option=(char)(ScenesData.Character & 0xFF);
                  }

                  // Which submenu are we in?
                  switch(ScenesData.ActiveMenu)
                  {

// ============================== The System submenu ==============================
                    case SYSTEM_MENU:    // System menu
                                         switch (Option)
                                         {
                                           case 'I': // Initialize the system again
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Initializing the system again, may have adverse effects!");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Consider using 'R' for rebooting instead.");
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,TISM_GetTaskID("TISM_TaskManager"),TISM_SET_SYS_STATE,INIT,0);
                                                     break;
                                           case 'R': // Reboot the system
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Rebooting not implemented yet.");
                                                     //TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Rebooting the system.");
                                                     //TISM_PostmanTaskWriteMessage(TISM_Task ThisTask,HOSTID,TISM_GetTaskID("TISM_TaskManager"),TISM_SET_SYS_STATE,REBOOT,0);
                                                     break;
                                           case 'S': // Stop the system
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Stopping the system.");
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,TISM_GetTaskID("TISM_TaskManager"),TISM_SET_SYS_STATE,STOP,0);
                                                     break;
                                           case 'Q': // Back to the main menu
                                                     ScenesData.ActiveMenu=MAIN_MENU;
                                                     ScenesData.DisplayHelpNext=true;
                                           default:  // Unknown option or '?' key pressed. Show options.  
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Main Menu > TISM system options:");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "================================");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "I - Initialize the system again");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "R - Reboot the system");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "S - Stop the system");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Q - Quit, go back");                                          
                                                     break;
                                         }
                                         break;

// ============================== The Actuators submenu ==============================
                    case ACTUATORS_MENU: // Actuators menu
                                         switch (Option)
                                         {
                                           case '1': // Extend actuator #0
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_ACTUATOR0,ACTUATOR_EXTEND,1000);
                                                     break;
                                           case '2': // Stop actuator #0
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_ACTUATOR0,ACTUATOR_STOP,0);
                                                     break;
                                           case '3': // Retract actuato #0 
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_ACTUATOR0,ACTUATOR_RETRACT,1000);
                                                     break;
                                           case 'q': // Extend actuator #1
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_ACTUATOR1,ACTUATOR_EXTEND,1000);
                                                     break;
                                           case 'w': // Stop actuator #1
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_ACTUATOR1,ACTUATOR_STOP,0);
                                                     break;
                                           case 'e': // Retract actuator #1
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_ACTUATOR1,ACTUATOR_RETRACT,1000);
                                                     break;
                                           case 'a': // Extend actuator #2
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_ACTUATOR2,ACTUATOR_EXTEND,1000);
                                                     break;
                                           case 's': // Stop actuator #2
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_ACTUATOR2,ACTUATOR_STOP,0);
                                                     break;
                                           case 'd': // Retract actuator #2
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_ACTUATOR2,ACTUATOR_RETRACT,1000);
                                                     break;
                                           case 'z': // Extend actuator #3
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_ACTUATOR3,ACTUATOR_EXTEND,1000);
                                                     break;
                                           case 'x': // Stop actuator #3
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_ACTUATOR3,ACTUATOR_STOP,0);
                                                     break;
                                           case 'c': // Retract actuator #3
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_ACTUATOR3,ACTUATOR_RETRACT,1000);
                                                     break;                                                                                                          
                                           case 'Q': // Back to the main menu
                                                     ScenesData.ActiveMenu=MAIN_MENU;
                                                     ScenesData.DisplayHelpNext=true;                                                     
                                           default:  // Unknown option or '?' key pressed. Show options.  
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Main menu > Actuator options:");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "=============================");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "1 - Extend actuator #0       q - Extend actuator #1");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "2 - Stop actuator #0         w - Stop actuator #1");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "3 - Retract actuato #0       e - Retract actuator #1");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "a - Extend actuator #2.      z - Extend actuator #3");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "s - Stop actuator #2         x - Stop actuator #3");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "d - Retract actuator #2      c - Retract actuator #3");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Q - Quit, go back");                                          
                                                     break;
                                         }
                                         break;
                                         
// ============================== The Servos submenu ==============================                                         
                    case SERVOS_MENU:    // Servos menu
                                         switch (Option)
                                         {
                                           case '1': // Servo #0 0 degrees
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SERVO0,0,SERVOS_DELAY);
                                                     break;
                                           case '2': // Servo #0 45 degrees
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SERVO0,45,SERVOS_DELAY);
                                                     break;
                                           case '3': // Servo #0 90 degrees
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SERVO0,90,SERVOS_DELAY);
                                                     break;
                                           case '4': // Servo #0 135 degrees
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SERVO0,135,SERVOS_DELAY);
                                                     break;
                                           case '5': // Servo #0 180 degrees
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SERVO0,180,SERVOS_DELAY);
                                                     break;                                                     
                                           case 'q': // Servo #1 0 degrees
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SERVO1,0,SERVOS_DELAY);
                                                     break;
                                           case 'w': // Servo #1 45 degrees
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SERVO1,45,SERVOS_DELAY);
                                                     break;
                                           case 'e': // Servo #1 90 degrees
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SERVO1,90,SERVOS_DELAY);
                                                     break;
                                           case 'r': // Servo #1 135 degrees
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SERVO1,135,SERVOS_DELAY);
                                                     break;
                                           case 't': // Servo #1 180 degrees
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SERVO1,180,SERVOS_DELAY);
                                                     break;
                                           case 'a': // Servo #2 0 degrees
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SERVO2,0,SERVOS_DELAY);
                                                     break;
                                           case 's': // Servo #2 45 degrees
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SERVO2,45,SERVOS_DELAY);
                                                     break;
                                           case 'd': // Servo #2 90 degrees
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SERVO2,90,SERVOS_DELAY);
                                                     break;
                                           case 'f': // Servo #2 135 degrees
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SERVO2,135,SERVOS_DELAY);
                                                     break;
                                           case 'g': // Servo #2 180 degrees
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SERVO2,180,SERVOS_DELAY);
                                                     break;
                                           case 'z': // Servo #3 0 degrees
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SERVO3,0,SERVOS_DELAY);
                                                     break;
                                           case 'x': // Servo #3 45 degrees
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SERVO3,45,SERVOS_DELAY);
                                                     break;
                                           case 'c': // Servo #3 90 degrees
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SERVO3,90,SERVOS_DELAY);
                                                     break;
                                           case 'v': // Servo #3 135 degrees
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SERVO3,135,SERVOS_DELAY);
                                                     break;
                                           case 'b': // Servo #3 180 degrees
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SERVO3,180,SERVOS_DELAY);
                                                     break;                                                                                                                                                                                                                    
                                           case 'Q': // Back to the main menu
                                                     ScenesData.ActiveMenu=MAIN_MENU;
                                                     ScenesData.DisplayHelpNext=true;                                                     
                                           default:  // Unknown option or '?' key pressed. Show options.  
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Main menu > Servo options:");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "==========================");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "1 - Servo #0 0 degrees       q - Servo #1 0 degrees");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "2 - Servo #0 45 degrees      w - Servo #1 45 degrees");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "3 - Servo #0 90 degrees      e - Servo #1 90 degrees");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "4 - Servo #0 135 degrees     r - Servo #1 135 degrees");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "5 - Servo #0 180 degrees     t - Servo #1 180 degrees");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "");                                                     
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "a - Servo #2 0 degrees       z - Servo #3 0 degrees");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "s - Servo #2 45 degrees      x - Servo #3 45 degrees");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "d - Servo #2 90 degrees      c - Servo #3 90 degrees");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "f - Servo #2 135 degrees     v - Servo #3 135 degrees");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "g - Servo #2 180 degrees     b - Servo #3 180 degrees");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Servo delay: %d in msec.", SERVOS_DELAY);
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Q - Quit, go back");                                                                                               
                                                     break;
                                         }
                                         break;
                                         
// ============================== The Sounds submenu ==============================                                         
                    case SOUNDS_MENU:  // Switches menu
                                         switch (Option)
                                         {
                                           case '1': // Play sound effect #1
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.SoundsTaskID,SOUNDS_PLAY,1,0);
                                                     break;
                                           case '2': // Play sound effect #2
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.SoundsTaskID,SOUNDS_PLAY,2,0);                                           
                                                     break;
                                           case '3': // Play sound effect #3
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.SoundsTaskID,SOUNDS_PLAY,3,0);
                                                     break;
                                           case '4': // Play sound effect #4
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.SoundsTaskID,SOUNDS_PLAY,4,0);                                           
                                                     break;  
                                           case '5': // Play sound effect #5
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.SoundsTaskID,SOUNDS_PLAY,5,0);                                           
                                                     break;                                                                                                                                                             
                                           case 's': // Stop playing
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.SoundsTaskID,SOUNDS_STOP,0,0);  
                                                     break;
                                           case '+': // Volume UP    
                                                     if(ScenesData.SoundVolume<SOUND_MAX_VOL)
                                                       ScenesData.SoundVolume++;  
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.SoundsTaskID,SOUNDS_SET_VOLUME,ScenesData.SoundVolume,0);  
                                                     ScenesData.DisplayHelpNext=true;                                                                                                   
                                                     break;
                                           case '-': // Volume DOWN   
                                                     if(ScenesData.SoundVolume>0)
                                                       ScenesData.SoundVolume--;                                              
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.SoundsTaskID,SOUNDS_SET_VOLUME,ScenesData.SoundVolume,0); 
                                                     ScenesData.DisplayHelpNext=true;                                                                                                   
                                                     break;                                                                                                                
                                           case 'Q': // Back to the main menu
                                                     ScenesData.ActiveMenu=MAIN_MENU;
                                                     ScenesData.DisplayHelpNext=true;
                                                     break;                                                     
                                           default:  // Unknown option or '?' key pressed. Show options.  
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Main menu > Sounds options:");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "=============================");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Current volume: %d",ScenesData.SoundVolume);
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "1 - Sound effect #1           2 - Sound effect #2");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "3 - Sound effect #3           4 - Sound effect #4");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "5 - Sound effect #5");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "s - Stop playing");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "+ - Volume UP                 - - Volume DOWN");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "");                                                     
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Q - Quit, go back");                                          
                                                     break;
                                         }
                                         break;
                                         
// ============================== The Switches submenu ==============================                                         
                    case SWITCHES_MENU:  // Switches menu
                                         switch (Option)
                                         {
                                           case '1': // Turn switch #0 ON
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SWITCH0,SWITCH_ON,0);
                                                     break;
                                           case '2': // Turn switch #0 OFF
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SWITCH0,SWITCH_OFF,0);                                           
                                                     break;
                                           case '3': // Turn switch #1 ON
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SWITCH1,SWITCH_ON,0);                                           
                                                     break;
                                           case '4': // Turn switch #1 OFF
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SWITCH1,SWITCH_OFF,0);                                           
                                                     break;
                                           case '5': // Turn switch #2 ON
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SWITCH2,SWITCH_ON,0);                                           
                                                     break;
                                           case '6': // Turn switch #2 OFF
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SWITCH2,SWITCH_OFF,0);                                           
                                                     break;
                                           case '7': // Turn switch #3 ON
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SWITCH3,SWITCH_ON,0);                                           
                                                     break;
                                           case '8': // Turn switch #3 OFF
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_SWITCH3,SWITCH_OFF,0);                                           
                                                     break;                                                     
                                           case 'Q': // Back to the main menu
                                                     ScenesData.ActiveMenu=MAIN_MENU;
                                                     ScenesData.DisplayHelpNext=true; 
                                                     break;                                                    
                                           default:  // Unknown option or '?' key pressed. Show options.  
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Main menu > Switches options:");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "=============================");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "1 - Turn switch #0 ON         3 - Turn switch #1 ON");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "2 - Turn switch #0 OFF        4 - Turn switch #1 OFF");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "5 - Turn switch #2 ON.        7 - Turn switch #3 ON");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "6 - Turn switch #2 OFF.       8 - Turn switch #3 OFF");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Q - Quit, go back");                                          
                                                     break;
                                         }
                                         break;
                                         
// ============================== The Facial submenu ==============================                                         
                    case FACIAL_MENU:    // Switches menu
                                         switch (Option)
                                         {
                                           case '1': // Turn mouth via servo #0 ON
                                                     //TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_MOUTH_ACTIVE,0,0);                                           
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ThisTask.TaskID,TALKING_ON,0,0);                                           
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Mouth via servo #0 turned ON");
                                                     break;
                                           case '2': // Turn mouth via servo #0 OFF
                                                     //TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.ControlTaskID,CONTROL_MOUTH_INACTIVE,0,0);                                           
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ThisTask.TaskID,TALKING_OFF,0,0);                                                                                                
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Mouth via servo #0 turned OFF");
                                                     break; 
                                           case '3': // Opem mouth fully, servo #0
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ThisTask.TaskID,MOUTH_OPEN,0,0);
                                                     break;
                                           case '4': // Turn mouth via servo #0 OFF
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ThisTask.TaskID,MOUTH_CLOSE,0,0);
                                                     break;   
                                           case '5': // Eyes ON
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ThisTask.TaskID,EYES_ON,0,0);
                                                     break;
                                           case '6': // Eyes OFF
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ThisTask.TaskID,EYES_OFF,0,0);
                                                     break;
                                           case 'Q': // Back to the main menu
                                                     ScenesData.ActiveMenu=MAIN_MENU;
                                                     ScenesData.DisplayHelpNext=true;
                                                     break;                                                                                                  
                                           default:  // Unknown option or '?' key pressed. Show options.  
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Main menu > Facial features options:");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "====================================");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "1 - Turn mouth via servo #0 ON");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "2 - Turn mouth via servo #0 OFF");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "3 - Open mouth maximum");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "4 - Close mouth");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "5 - Eyes ON");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "6 - Eyes OFF");                                                     
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Q - Quit, go back");                                          
                                                     break;
                                         }
                                         break;

// ============================== The Facial submenu ==============================
                    case SCENES_MENU:    // Scenes menu
                                         switch (Option)
                                         {
                                           case '0': // Start position
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ThisTask.TaskID,START_POSITION,0,0);                                           
                                                     break;
                                           case '1': // Look up
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ThisTask.TaskID,LOOK_UP,0,0);                                           
                                                     break;                                                                                                
                                           case '2': // Look up, right
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ThisTask.TaskID,LOOK_UP_RIGHT,0,0);                                           
                                                     break;  
                                           case '3': // Look up, left
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ThisTask.TaskID,LOOK_UP_LEFT,0,0);                                           
                                                     break;                                                                                                                       
                                           case '4': // Look down
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ThisTask.TaskID,LOOK_DOWN,0,0);                                           
                                                     break;      
                                           case '5': // Look down, left
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ThisTask.TaskID,LOOK_DOWN_LEFT,0,0);                                           
                                                     break; 
                                           case '6': // Look down, right
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ThisTask.TaskID,LOOK_DOWN_RIGHT,0,0);                                           
                                                     break; 
                                           case 'q': // Lean forward
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ThisTask.TaskID,LEAN_FORWARD,0,0);                                           
                                                     break; 
                                           case 'w': // Lean backward
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ThisTask.TaskID,LEAN_BACKWARD,0,0);                                           
                                                     break;                                                    
                                           case 'a': // Raise candle
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ThisTask.TaskID,RAISE_CANDLE,0,0);                                           
                                                     break; 
                                           case 's': // Lower candle
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ThisTask.TaskID,LOWER_CANDLE,0,0);                                           
                                                     break; 
                                           case 'd': // Candle ON
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ThisTask.TaskID,CANDLE_ON,0,0);                                           
                                                     break;
                                           case 'f': // Candle OFF
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ThisTask.TaskID,CANDLE_OFF,0,0);                                           
                                                     break;
                                           case 'z': // Raise left hand
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ThisTask.TaskID,RAISE_LHAND,0,0);                                           
                                                     break; 
                                           case 'x': // Lower left hand
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ThisTask.TaskID,LOWER_LHAND,0,0);                                           
                                                     break; 
                                           case 'c': // Raise left arm
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ThisTask.TaskID,RAISE_LARM,0,0);                                           
                                                     break; 
                                           case 'v': // Lower left arm
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ThisTask.TaskID,LOWER_LARM,0,0);                                           
                                                     break;                                                                                                                                                                                                                                                                                                                              
                                           case 'H': // Look alive on
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ThisTask.TaskID,LOOK_ALIVE_ON,0,0);                                                        
                                                     break; 
                                           case 'J': // Look alive off
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ThisTask.TaskID,LOOK_ALIVE_OFF,0,0);                                                        
                                                     break;  
                                           case '7': // Look alive off
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ThisTask.TaskID,LOOK_FORWARD,0,0);                                                        
                                                     break;                                                                                                            
                                           case 'A': // Start writing scene 'A'
                                                     WriteScene(ThisTask,SCENE_A);                                                     
                                                     break;  
                                           case 'B': // Start writing scene 'B'
                                                     WriteScene(ThisTask,SCENE_B);                                                     
                                                     break;      
                                           case 'C': // Start writing scene 'C'
                                                     WriteScene(ThisTask,SCENE_C);                                                     
                                                     break;                                                                                                                                                                                                                                                                                                                                                                                      
                                           case 'M': // Toggle autoplay mode
                                                     if(ScenesData.AutoPlay==true)
                                                     {
                                                       ScenesData.AutoPlay=false;
                                                       TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "AutoPlay mode stopped.");
                                                     }
                                                     else
                                                     {
                                                       ScenesData.AutoPlay=true;
                                                       TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "AutoPlay mode started; starting scene A");
                                                       ScenesData.ActiveScene=SCENE_C;   // Next scene will be scene A.
                                                     }
                                                     break;     
                                           case 'Q': // Back to the main menu
                                                     ScenesData.ActiveMenu=MAIN_MENU;
                                                     ScenesData.DisplayHelpNext=true; 
                                                     break;                                                                                                 
                                           default:  // Unknown option or '?' key pressed. Show options.  
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Main menu > Scene options:");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "==========================");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "0 - Start position");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "1 - Look up                  4 - Look down"); 
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "2 - Look up, right           5 - Look down, left");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "3 - Look up, left            6 - Look down, right");                                                                                                          
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "7 - Look forward");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "q - Lean forward             w - Lean backward");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "a - Raise candle             s - Lower candle");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "d - Candle ON                f - Candle OFF");                                                     
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "z - Raise left hand          x - Lower left hand");    
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "c - Raise left arm           v - Lower left arm");    
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "H - Look alive ON            J - Look alive OFF");                                                                
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "A - Play Scene A ");   
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "B - Play Scene B ");   
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "C - Play Scene C ");   
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Q - Quit, go back");                                          
                                                     break;
                                         }
                                         break;
                                         
// ============================== The Lights submenu ==============================                                         
                    case LIGHTS_MENU:    // Lights menu
                                         switch (Option)
                                         {
                                           case '1': // Activate Magic Colors mode
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Magic Colors mode activated.");
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.LightsTaskID,MAGIC_COLORS,0,0);
                                                     break;
                                           case '2': // Activate Lightning mode
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Lightning mode activated.");
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.LightsTaskID,LIGHTNING,0,0);
                                                     break;
                                           case '3': // Turn off lights
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Lights turned off.");
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ScenesData.LightsTaskID,LIGHTS_OFF,0,0);                                                     break;
                                           case 'Q': // Back to the main menu
                                                     ScenesData.ActiveMenu=MAIN_MENU;
                                                     ScenesData.DisplayHelpNext=true;
                                                     break;
                                           default:  // Unknown option or '?' key pressed. Show options.  
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Main Menu > Lights options:");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "================================");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "1 - Activate Magic Colors mode");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "2 - Activate Lightning mode");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "3 - Turn off lights");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Q - Quit, go back");                                          
                                                     break;
                                         }
                                         break;

// ============================== The Main menu (default) ==============================
                    default:             // Default is Main menu.
                                         switch(Option)
                                         {
                                           case '1': // Activate Actuator menu
                                                     ScenesData.ActiveMenu=ACTUATORS_MENU;
                                                     ScenesData.DisplayHelpNext=true;                                                     
                                                     break;
                                           case '2': // Activate Switch menu
                                                     ScenesData.ActiveMenu=SWITCHES_MENU;
                                                     ScenesData.DisplayHelpNext=true;                                                     
                                                     break;
                                           case '3': // Activate Servo menu
                                                     ScenesData.ActiveMenu=SERVOS_MENU;
                                                     ScenesData.DisplayHelpNext=true;                                                     
                                                     break;
                                           case '4': // Activate Sounds menu
                                                     ScenesData.ActiveMenu=SOUNDS_MENU;
                                                     ScenesData.DisplayHelpNext=true;                                                     
                                                     break;
                                           case '5': // Activate facial menu
                                                     ScenesData.ActiveMenu=FACIAL_MENU;
                                                     ScenesData.DisplayHelpNext=true;                                                     
                                                     break;
                                           case '6': // Activate Scenes menu
                                                     ScenesData.ActiveMenu=SCENES_MENU;
                                                     ScenesData.DisplayHelpNext=true;                                                     
                                                     break;            
                                           case '7': // Activate Lights menu
                                                     ScenesData.ActiveMenu=LIGHTS_MENU;
                                                     ScenesData.DisplayHelpNext=true;                                                     
                                                     break;       
                                           case 'r': // Reset all devices to starting positions
                                                     // Set actuators to either extended or retracted positions
                                                     TISM_PostmanTaskWriteMessage(ThisTask,HOSTID,ThisTask.TaskID,START_POSITION,0,0);
                                                     break;                                                                                                                                                              
                                           case 'T': // Activate System menu
                                                     ScenesData.ActiveMenu=SYSTEM_MENU;
                                                     ScenesData.DisplayHelpNext=true;                                                     
                                                     break;                           
                                           default:  // Unknown option or '?' key pressed. Show options.               
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Main menu options:");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "==================");                    
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "1 - Actuators menu");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "2 - Switches menu");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "3 - Servos menu");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "4 - Sounds menu");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "5 - Facial features menu");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "6 - Scenes menu");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "7 - Lights menu");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "r - Reset devices");                                                                                                         
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "");
                                                     TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "T - TISM system options");                                          
                                                     break;
                                         }
                  } 
                  
                  // Show how long ago previous command was sent, helps with determining the position of the actuator and pulse widths.
                  // As we're using the messaging system there is some delay, but this should be reasonably constant.
                  if(ScenesData.TimestampLastCommand>0)
                    TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Previous command sent %d msecs ago.", (int)((time_us_64()-ScenesData.TimestampLastCommand)/1000));
                  ScenesData.TimestampLastCommand=time_us_64();                 
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

