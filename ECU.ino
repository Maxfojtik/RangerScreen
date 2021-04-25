//#include <FlexCAN_T4.h>
//#define MODE1               0x01        //Show current data
//#define MODE2               0x02        //Show freeze frame data
//#define MODE3               0x03        //Show stored Diagnostic Trouble Codes
//#define MODE4               0x04        //Clear Diagnostic Trouble Codes and stored values
//
//#define PID_SUPPORTED       0x00
//#define MONITOR_STATUS      0x01
//#define ENGINE_COOLANT_TEMP 0x05
//#define ENGINE_RPM          0x0C
//#define VEHICLE_SPEED       0x0D
//#define MAF_SENSOR          0x10
//#define THROTTLE            0x11
//#define O2_VOLTAGE          0x14
//
//#define MODE1_RESPONSE      0x41
//#define MODE3_RESPONSE      0x43
//#define MODE4_RESPONSE      0x44
//#define PID_REQUEST         0x7DF
//#define ECU_REPLY           0x7E8
//
////FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> Can0;
//void ECUinit(uint32_t baud) {
//
//  Can0.begin();
//  Can0.setBaudRate(baud);
//}
//uint8_t proc(int *pid, int *engine_data)
//{
//  CAN_message_t can_MsgRx;
//  if(Can0.read(can_MsgRx)) 
//  { 
//    if(can_MsgRx.id == ECU_REPLY)
//    { 
//      if(can_MsgRx.buf[1] == MODE1_RESPONSE)
//      {
//        *pid = can_MsgRx.buf[2];
//        switch(can_MsgRx.buf[2])
//        {   /* Details from http://en.wikipedia.org/wiki/OBD-II_PIDs */
//          case ENGINE_RPM:              //   ((A*256)+B)/4    [RPM]
//              *engine_data =  ((can_MsgRx.buf[3]*256) + can_MsgRx.buf[4])/4;
//              break;
//          
//          case ENGINE_COOLANT_TEMP:     //     A-40              [degree C]
//              *engine_data =  can_MsgRx.buf[3] - 40;
//          
//          break;
//          
//          case VEHICLE_SPEED:         // A                  [km]
//              *engine_data =  can_MsgRx.buf[3];
//          
//          break;
//        
//          case MAF_SENSOR:               // ((256*A)+B) / 100  [g/s]
//              *engine_data =  ((can_MsgRx.buf[3]*256) + can_MsgRx.buf[4])/100;
//          
//          break;
//        
//          case O2_VOLTAGE:            // A * 0.005   (B-128) * 100/128 (if B==0xFF, sensor is not used in trim calc)
//              *engine_data = can_MsgRx.buf[3]*0.005;
//        
//          case THROTTLE:            //
//              *engine_data = int((can_MsgRx.buf[3]*100.0)/255);
//        
//          break;
//        }
//        return 1;
//      }     
//    }
//    else if(can_MsgRx.buf[1] == MODE3_RESPONSE)
//    {
//      
//    }
//  }
//  return 0;
//}
//void ECUrequest(uint8_t pid) 
//{
//  CAN_message_t can_MsgTx;
//  can_MsgTx.buf[0] = 0x02;  
//  can_MsgTx.buf[1] = 0x01;
//  can_MsgTx.buf[2] = pid;  
//  can_MsgTx.buf[3] = 0;
//  can_MsgTx.buf[4] = 0;  
//  can_MsgTx.buf[5] = 0;
//  can_MsgTx.buf[6] = 0;  
//  can_MsgTx.buf[7] = 0;
//  can_MsgTx.len = 8; 
//  //can_MsgTx.ext = 0; 
//  can_MsgTx.flags.extended = 0; 
//  can_MsgTx.flags.remote = 0;
//  can_MsgTx.id = PID_REQUEST;
////  can_MsgTx.timeout = 500;
//  Can0.write(can_MsgTx);  
//}
