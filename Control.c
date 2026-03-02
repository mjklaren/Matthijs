/*

  Task for controlling linear actuators for animatronics. No measurement of position of actuator; commands are relative to current position (time-based).
  Reset often to a predictable position for best results.

  Commands are set using the TISM messaging system, by using messages sent to this task:
  - MessageType   = DeviceID of the device,
  - Message       = The command, see below.
  - Specification = The duration of the command; in % of maximum travel time for actuators, in msec for pulse and servo. Ignored for SWITCH_ON and SWITCH_OFF commands.

  Copyright (c) 2025 Maarten Klarenbeek (https://github.com/mjklaren)
  Distributed under the GPLv3 license

*/

#include "TISM.h"
#include "pca9685.h"
#include "hardware/adc.h"
#include <math.h>


// Device types
#define LINEARACTUATOR          1   // Each actuator uses 2 GPIOs to be controlled.
#define SWITCH                  2
#define SERVO                   3

// Device commands
#define ACTUATOR_STOP           0
#define ACTUATOR_EXTEND         1   // Binary '01'
#define ACTUATOR_RETRACT        2   // Binary '10'
#define SWITCH_OFF              0
#define SWITCH_ON               1
#define SWITCH_PULSE            2   // Specify lenght of pulse in Specification (in msec).

// Device IDs and GPIOs
// Servos
#define CONTROL_SDA_GPIO        4
#define CONTROL_SCL_GPIO        5
#define CONTROL_SERVO0          0
#define CONTROL_SERVO1          1
#define CONTROL_SERVO2          2
#define CONTROL_SERVO3          3
#define CONTROL_SERVO4          4
#define CONTROL_SERVO5          5
#define CONTROL_SERVO6          6
#define CONTROL_SERVO7          7
#define CONTROL_SERVO8          8
#define CONTROL_SERVO9          9
#define CONTROL_SERVO10         10
#define CONTROL_SERVO11         11
#define CONTROL_SERVO_DELAY     8  // Delay between each step, in msec.

// Linear Actuator
#define CONTROL_ACTUATOR0       10
#define CONTROL_ACTUATOR0_GPIO0 0
#define CONTROL_ACTUATOR0_GPIO1 1
#define CONTROL_ACTUATOR1       11
#define CONTROL_ACTUATOR1_GPIO0 2
#define CONTROL_ACTUATOR1_GPIO1 3
#define CONTROL_ACTUATOR2       12
#define CONTROL_ACTUATOR2_GPIO0 8
#define CONTROL_ACTUATOR2_GPIO1 9
#define CONTROL_ACTUATOR3       13
#define CONTROL_ACTUATOR3_GPIO0 10
#define CONTROL_ACTUATOR3_GPIO1 11
//#define CONTROL_TRAVEL          8000 // Travel time for actuators. Now set for all devices. In msec.

// Switches 
#define CONTROL_SWITCH0         20  // Eyes
#define CONTROL_SWITCH0_GPIO    12
#define CONTROL_SWITCH1         21  // Candle
#define CONTROL_SWITCH1_GPIO    13
#define CONTROL_SWITCH2         22  // 433Mhz transmitter #1
#define CONTROL_SWITCH2_GPIO    21
#define CONTROL_SWITCH3         23  // 433Mhz transmitter #2
#define CONTROL_SWITCH3_GPIO    22
#define CONTROL_PULSE           500  // Pulse width for switch

// Servo #0 used for mouth; following the ADC
#define CONTROL_MOUTH_ACTIVE    32
#define CONTROL_MOUTH_INACTIVE  33
#define CONTROL_MOUTH_MAX       34
#define CONTROL_MOUTH_MIN       35

#define CONTROL_DEVICES         12  // Maximum number of devices connected

// Used for mouth animation
#define ADC_PIN         26
#define ENVELOPE_ALPHA  0.1f
#define ENVELOPE_GAIN   5.0f
#define ADC_MAX         4095
#define OFFSET          2048         // To be calibrated
#define SERVO_MIN_DEG   60
#define SERVO_MAX_DEG   145
#define SERVO_DELAY     0
//#define SERVO_INVERT    1            // Invert the calculated angle of the servo. Uncomment if not needed.

/*
  
*/
struct Device
{
  uint8_t DeviceID,DeviceType,Command,GPIO0,GPIO1,CurrentPosition;
  uint16_t Delay;                                                   // In msec
  uint64_t Timer;                                                   // In usec
  bool Invert;                                                      // Invert GPIO output when needed
};


struct ControlData
{
  struct Device Device[CONTROL_DEVICES];
  uint8_t NumberOfDevices;
  PCA9685_t ServoDriver;                   // Currently one I2C PWM board supported
  
  // For mouth animation
  uint8_t MouthServoID;
  bool MouthIsActive;
  uint16_t ADC;
  float Envelope,Value,ScaledEnv;
} ControlData;


void ControlRegisterDevice(uint8_t DeviceID,uint8_t DeviceType,uint8_t GPIO0,uint8_t GPIO1,uint16_t Delay,bool Invert)
{
  ControlData.Device[ControlData.NumberOfDevices].DeviceID=DeviceID;
  ControlData.Device[ControlData.NumberOfDevices].DeviceType=DeviceType;
  ControlData.Device[ControlData.NumberOfDevices].Command=0;
  ControlData.Device[ControlData.NumberOfDevices].GPIO0=GPIO0;
  ControlData.Device[ControlData.NumberOfDevices].GPIO1=GPIO1;
  ControlData.Device[ControlData.NumberOfDevices].CurrentPosition=0;
  ControlData.Device[ControlData.NumberOfDevices].Delay=Delay;
  ControlData.Device[ControlData.NumberOfDevices].Timer=0;
  ControlData.Device[ControlData.NumberOfDevices].Invert=Invert;
  ControlData.NumberOfDevices++;
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
uint8_t Control(TISM_Task ThisTask)
{
  if (ThisTask.TaskDebug==DEBUG_HIGH) TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Run starting.");
  
  /*
  
  */
  switch(ThisTask.TaskState)   
  {
    case INIT:  // Activities to initialize this task (e.g. initialize ports or peripherals).
                if (ThisTask.TaskDebug) TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Initializing with priority %d.", ThisTask.TaskPriority);
				        
                // Register the attached devices
                ControlData.NumberOfDevices=0;
             
                // Linear Actuators
                ControlRegisterDevice(CONTROL_ACTUATOR0,LINEARACTUATOR,CONTROL_ACTUATOR0_GPIO0,CONTROL_ACTUATOR0_GPIO1,0,false);
                ControlRegisterDevice(CONTROL_ACTUATOR1,LINEARACTUATOR,CONTROL_ACTUATOR1_GPIO0,CONTROL_ACTUATOR1_GPIO1,0,false);
                ControlRegisterDevice(CONTROL_ACTUATOR2,LINEARACTUATOR,CONTROL_ACTUATOR2_GPIO0,CONTROL_ACTUATOR2_GPIO1,0,false);
                ControlRegisterDevice(CONTROL_ACTUATOR3,LINEARACTUATOR,CONTROL_ACTUATOR3_GPIO0,CONTROL_ACTUATOR3_GPIO1,0,false);

                // Switches
                ControlRegisterDevice(CONTROL_SWITCH0,SWITCH,CONTROL_SWITCH0_GPIO,0,CONTROL_PULSE,false);
                ControlRegisterDevice(CONTROL_SWITCH1,SWITCH,CONTROL_SWITCH1_GPIO,0,CONTROL_PULSE,false);
                //ControlRegisterDevice(CONTROL_SWITCH2,SWITCH,CONTROL_SWITCH2_GPIO,0,CONTROL_PULSE,true);
                //ControlRegisterDevice(CONTROL_SWITCH3,SWITCH,CONTROL_SWITCH3_GPIO,0,CONTROL_PULSE,true);
                
                // Servos, connected via a PCA9685
                ControlRegisterDevice(CONTROL_SERVO0,SERVO,CONTROL_SDA_GPIO,CONTROL_SCL_GPIO,CONTROL_SERVO_DELAY,false);
                ControlRegisterDevice(CONTROL_SERVO1,SERVO,CONTROL_SDA_GPIO,CONTROL_SCL_GPIO,CONTROL_SERVO_DELAY,false);
                ControlRegisterDevice(CONTROL_SERVO2,SERVO,CONTROL_SDA_GPIO,CONTROL_SCL_GPIO,CONTROL_SERVO_DELAY,false);
                //ControlRegisterDevice(CONTROL_SERVO3,SERVO,CONTROL_SDA_GPIO,CONTROL_SCL_GPIO,CONTROL_SERVO_DELAY);

                // One servo is used for a mouth movement
                ControlData.MouthServoID=CONTROL_SERVO0;
                ControlData.MouthIsActive=false;

                // Initialize the ADC for the mouth animation
                adc_init();
                adc_gpio_init(ADC_PIN);
                adc_select_input(0);
                ControlData.Envelope=0.0f;

                // Servos connected to a PCA9685 PWM-board - initialize.
                if (!pca9685_init(&ControlData.ServoDriver,i2c0,PCA9685_I2C_ADDRESS,CONTROL_SDA_GPIO,CONTROL_SCL_GPIO)) 
                {
                  TISM_EventLoggerLogEvent(ThisTask,TISM_LOG_EVENT_NOTIFY,"Error initializing PCA9685 at address %d, SDA-GPIO %d and SCL_GPIO %d.",PCA9685_I2C_ADDRESS,CONTROL_SDA_GPIO,CONTROL_SCL_GPIO);
                  return(ERR_INITIALIZING);
                }

                // Initialize the GPIOs of the registered devices and set to 0. Servos are set in the 90 degree (=middle) position.
                for(uint8_t counter=0;counter<ControlData.NumberOfDevices;counter++)
                {
                  switch(ControlData.Device[counter].DeviceType)
                  {
                    case LINEARACTUATOR: // Initialize the GPIOs for the actuator
                                         gpio_init(ControlData.Device[counter].GPIO0);
                                         gpio_set_dir(ControlData.Device[counter].GPIO0,GPIO_OUT);
                                         gpio_put(ControlData.Device[counter].GPIO0,SWITCH_OFF);
                                         gpio_init(ControlData.Device[counter].GPIO1);
                                         gpio_set_dir(ControlData.Device[counter].GPIO1,GPIO_OUT);
                                         gpio_put(ControlData.Device[counter].GPIO1,SWITCH_OFF);
                                         break;
                    case SWITCH:         // Initialize the single GPIO for the switch
                                         gpio_init(ControlData.Device[counter].GPIO0);
                                         gpio_set_dir(ControlData.Device[counter].GPIO0,GPIO_OUT);
                                         if(ControlData.Device[counter].Invert)
                                           gpio_put(ControlData.Device[counter].GPIO0,SWITCH_ON);
                                         else 
                                           gpio_put(ControlData.Device[counter].GPIO0,SWITCH_OFF);   // Some devices need inverted logic
                                         break;
                    case SERVO:          // Servo board is already initialized; set the servo in a known position (90 degrees).
                                         ControlData.Device[counter].Command=90;
                                         ControlData.Device[counter].CurrentPosition=90;
                                         pca9685_set_servo_angle(&ControlData.ServoDriver,ControlData.Device[counter].DeviceID,(float)ControlData.Device[counter].CurrentPosition);
                                         break;
                  }
                }

                // For tasks that only respond to events (=messages) we could set the sleep attribute to ´true'. This task continues to run.
                // TISM_SchedulerSetMyTaskAttribute(ThisTask,TISM_SET_TASK_SLEEP,true);
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

                  // Processed the message; delete it.
                  switch(MessageToProcess->MessageType)
                  {
                    case TISM_PING:              // Check if this process is still alive. Reply with a ECHO message type; return same message payload.
                                                 TISM_PostmanTaskWriteMessage(ThisTask,MessageToProcess->SenderHostID,MessageToProcess->SenderTaskID,TISM_ECHO,MessageToProcess->Payload0,0);
                                                 break;
                    case CONTROL_MOUTH_ACTIVE:   // Enable the mouth animation via servo #0.
                                                 ControlData.MouthIsActive=true;
                                                 for(uint8_t counter=0; counter<ControlData.NumberOfDevices; counter++)
                                                   if(ControlData.Device[counter].DeviceID==ControlData.MouthServoID)                                                   
                                                     ControlData.Device[counter].Timer=1;  // Set low value for this servo, in order to activate it.
                                                 break;
                    case CONTROL_MOUTH_INACTIVE: // Disable the mouth animation.
                                                 ControlData.MouthIsActive=false;
                                                 for(uint8_t counter=0; counter<ControlData.NumberOfDevices; counter++)
                                                   if(ControlData.Device[counter].DeviceID==ControlData.MouthServoID)                                                   
                                                     ControlData.Device[counter].Timer=1;  // Set zero value for this servo, in order to stop it.
                                                 break;
                    case CONTROL_MOUTH_MAX:      // Open mouth to the maximum. Find device ControlData.MouthServoID.
                                                 for(uint8_t counter=0; counter<ControlData.NumberOfDevices; counter++)
                                                   if(ControlData.Device[counter].DeviceID==ControlData.MouthServoID)
                                                   {
                                                     ControlData.Device[counter].CurrentPosition=SERVO_MAX_DEG;
                                                     pca9685_set_servo_angle(&ControlData.ServoDriver,ControlData.Device[counter].DeviceID,(float)ControlData.Device[counter].CurrentPosition);
                                                   }
                                                 break;
                    case CONTROL_MOUTH_MIN:       // Close mouth, without forcing
                                                 for(uint8_t counter=0; counter<ControlData.NumberOfDevices; counter++)
                                                   if(ControlData.Device[counter].DeviceID==ControlData.MouthServoID)
                                                   {
                                                     ControlData.Device[counter].CurrentPosition=SERVO_MIN_DEG;
                                                     pca9685_set_servo_angle(&ControlData.ServoDriver,ControlData.Device[counter].DeviceID,(float)ControlData.Device[counter].CurrentPosition);
                                                   }
                                                 break;
                    default:                     // Other message type received; check if this is a known DeviceID.
                                                 for(uint8_t counter=0;counter<ControlData.NumberOfDevices;counter++)
                                                 {
                                                   if(MessageToProcess->MessageType==ControlData.Device[counter].DeviceID)
                                                   {
                                                     // Device ID found. Set the specified command in the register. Action varies based on device type.
                                                     ControlData.Device[counter].Command=(uint8_t)MessageToProcess->Payload0;

                                                     if (ThisTask.TaskDebug) TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Command %d received for device %d. Specification: %d. Device has travel time of %d (in msec).",(uint8_t)MessageToProcess->Payload0,MessageToProcess->MessageType,MessageToProcess->Payload1,ControlData.Device[counter].Delay);

                                                     switch(ControlData.Device[counter].DeviceType)
                                                     {
                                                       case LINEARACTUATOR: // Set the GPIOs of the specified actuator.                                          
                                                                           gpio_put(ControlData.Device[counter].GPIO0,ControlData.Device[counter].Command & 1); // Set according to the 1st bit of the command.
                                                                           gpio_put(ControlData.Device[counter].GPIO1,ControlData.Device[counter].Command & 2); // Set according to the 2nd bit of the command.
                                                                           ControlData.Device[counter].CurrentPosition=0; // Not used for actuators, we're only tracking time it is moving. 

                                                                           // Register how long this command is valid.
                                                                           ControlData.Device[counter].Timer=time_us_64()+((float)MessageToProcess->Payload1*1000);

                                                                           if (ThisTask.TaskDebug) TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Actuator %d will run until %llu (in usec).",MessageToProcess->MessageType,ControlData.Device[counter].Timer);
                                                                           break;
                                                       case SWITCH:         // Set the GPIO of the specified switch, depending on the command.
                                                                           ControlData.Device[counter].Delay=(uint16_t)MessageToProcess->Payload1;
                                                                           switch(ControlData.Device[counter].Command)
                                                                           {
                                                                             case SWITCH_PULSE: // Briefly turn the switch 'on', then 'off' again.
                                                                                                 ControlData.Device[counter].Timer=time_us_64()+ControlData.Device[counter].Delay*1000;
                                                                                                 if(ControlData.Device[counter].Invert)
                                                                                                   gpio_put(ControlData.Device[counter].GPIO0,SWITCH_OFF);
                                                                                                 else 
                                                                                                   gpio_put(ControlData.Device[counter].GPIO0,SWITCH_ON);
                                                                                                 ControlData.Device[counter].CurrentPosition=SWITCH_ON;
                                                                                                 if (ThisTask.TaskDebug) TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Switch %d will PULSE until %llu (in usec).",ControlData.Device[counter].DeviceID,ControlData.Device[counter].Timer);
                                                                                                 break;
                                                                             case SWITCH_OFF:   // Turn the switch off.
                                                                                                 ControlData.Device[counter].Timer=0;
                                                                                                 if(ControlData.Device[counter].Invert)
                                                                                                   gpio_put(ControlData.Device[counter].GPIO0,SWITCH_ON);
                                                                                                 else 
                                                                                                   gpio_put(ControlData.Device[counter].GPIO0,SWITCH_OFF);
                                                                                                 ControlData.Device[counter].CurrentPosition=SWITCH_OFF;
                                                                                                 ControlData.Device[counter].Timer=0;                              
                                                                                                 if (ThisTask.TaskDebug) TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Switch %d turned OFF.",ControlData.Device[counter].DeviceID);
                                                                                                 break;
                                                                             case SWITCH_ON:    // Turn the switch on.
                                                                                                 ControlData.Device[counter].Timer=0;
                                                                                                 if(ControlData.Device[counter].Invert)
                                                                                                   gpio_put(ControlData.Device[counter].GPIO0,SWITCH_OFF);
                                                                                                 else 
                                                                                                   gpio_put(ControlData.Device[counter].GPIO0,SWITCH_ON);
                                                                                                 ControlData.Device[counter].CurrentPosition=SWITCH_ON;
                                                                                                 ControlData.Device[counter].Timer=0;
                                                                                                 if (ThisTask.TaskDebug) TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Switch %d turned ON.",ControlData.Device[counter].DeviceID);
                                                                                                 break;
 
                                                                               // TODO: SWITCH_BLINK en SWITCH_RANDOM
                                                                               
                                                                           }
                                                                           break;
                                                       case SERVO:          // Set the new target position for the specified servo.
                                                                           ControlData.Device[counter].Delay=(uint16_t)MessageToProcess->Payload1;
                                                                           if(ControlData.Device[counter].CurrentPosition!=ControlData.Device[counter].Command && ControlData.Device[counter].Delay>0)
                                                                           {
                                                                             // Gradual change of position
                                                                             ControlData.Device[counter].Timer=time_us_64()+(ControlData.Device[counter].Delay*1000);
                                                                           }
                                                                           else
                                                                           {
                                                                             // Set the position now.
                                                                             ControlData.Device[counter].CurrentPosition=ControlData.Device[counter].Command;
                                                                             pca9685_set_servo_angle(&ControlData.ServoDriver,ControlData.Device[counter].DeviceID,(float)ControlData.Device[counter].CurrentPosition);
                                                                           }
                                                                           if (ThisTask.TaskDebug) TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Servo %d will move to angle %d (in degrees), delay %d.",MessageToProcess->MessageType,ControlData.Device[counter].Command,ControlData.Device[counter].Delay);
                                                                           break;
                                                       default: // Unknown device, ignore.
                                                     }
                                                   }
                                                 }
                                                 break;
                  }
                  TISM_PostmanTaskDeleteMessage(ThisTask);
                  MessageCounter++;
                }

                // Messages processed; now run through the register and set/reset GPIOs where needed.
                for(uint8_t counter=0;counter<ControlData.NumberOfDevices;counter++)
                {
                  // Is this an 'active' command for this device and has it expired?
                  if((ControlData.Device[counter].Timer!=0) && (ControlData.Device[counter].Timer<time_us_64()))
                  {
                    // Yes. Take action, depending on the device type.
                    switch(ControlData.Device[counter].DeviceType)
                    {
                      case LINEARACTUATOR: // Set both GPIOs for this actuator to 0.
                                           gpio_put(ControlData.Device[counter].GPIO0,SWITCH_OFF);
                                           gpio_put(ControlData.Device[counter].GPIO1,SWITCH_OFF);
                                           ControlData.Device[counter].Timer=0;                            	          
                                           if (ThisTask.TaskDebug) TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Stopping actuator with ID %d.",ControlData.Device[counter].DeviceID);
                                           break;
                      case SWITCH:         // Set the GPIO of the switch to 0; no 2nd GPIO.
                                           if(ControlData.Device[counter].Invert)
                                             gpio_put(ControlData.Device[counter].GPIO0,SWITCH_OFF);
                                           else 
                                             gpio_put(ControlData.Device[counter].GPIO0,SWITCH_ON);
                                         ControlData.Device[counter].CurrentPosition=SWITCH_OFF;
                                           ControlData.Device[counter].Timer=0;
                                           if (ThisTask.TaskDebug) TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Turning switch with ID %d OFF.",ControlData.Device[counter].DeviceID);
                                           break;
                      case SERVO:          // Is this servo used for mouth animation?
                                           if(ControlData.MouthServoID==ControlData.Device[counter].DeviceID && ControlData.MouthIsActive==true)
                                           {
                                             // This is the servo used for the animation and the mouth is active.
                                             // Read the value of the ADC and set the angle of Servo #0 accordingly.
                                             ControlData.ADC=adc_read();
                                             ControlData.Value=fabsf((float)ControlData.ADC-OFFSET);
                                             ControlData.Envelope+=ENVELOPE_ALPHA*(ControlData.Value-ControlData.Envelope);
                                             ControlData.ScaledEnv=ControlData.Envelope*ENVELOPE_GAIN;
                                             if(ControlData.ScaledEnv>ADC_MAX)
                                               ControlData.ScaledEnv=ADC_MAX;

                                             // Mapping envelope -> degrees (0 to 180) for servo #0
//#ifdef SERVO_INVERT
                                             ControlData.Device[counter].CurrentPosition=SERVO_MAX_DEG+SERVO_MIN_DEG-(SERVO_MIN_DEG+(uint16_t)((ControlData.ScaledEnv/ADC_MAX)*(SERVO_MAX_DEG-SERVO_MIN_DEG)));
//#else
                                             //ControlData.Device[counter].CurrentPosition=SERVO_MIN_DEG+(uint16_t)((ControlData.ScaledEnv/ADC_MAX)*(SERVO_MAX_DEG-SERVO_MIN_DEG));
//#endif

                                             pca9685_set_servo_angle(&ControlData.ServoDriver,ControlData.Device[counter].DeviceID,(float)ControlData.Device[counter].CurrentPosition);
                                             //ControlData.Device[counter].Timer=time_us_64()+1000;  // Sample each msec.
                                           }
                                           else
                                           {
                                             // Move the servo with one degree, then set the next timer, allowing us to control the speed.
                                             ControlData.Device[counter].CurrentPosition+=(ControlData.Device[counter].Command>ControlData.Device[counter].CurrentPosition?1:-1);
                                             pca9685_set_servo_angle(&ControlData.ServoDriver,ControlData.Device[counter].DeviceID,(float)ControlData.Device[counter].CurrentPosition);

                                             // Are we in the desired position? Set the timer accordingly.
                                             ControlData.Device[counter].Timer=(ControlData.Device[counter].CurrentPosition==ControlData.Device[counter].Command?0:time_us_64()+ControlData.Device[counter].Delay*1000);
                                             if (ThisTask.TaskDebug) TISM_EventLoggerLogEvent(ThisTask, TISM_LOG_EVENT_NOTIFY, "Setting servo with ID %d to %d degrees.",ControlData.Device[counter].DeviceID,ControlData.Device[counter].CurrentPosition);
                                           }
                                           break;
                      default:             // Unknown - ignore.
                                           break;
                    }
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

