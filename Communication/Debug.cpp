#include "Basic.hpp"
#include "FreeRTOS.h"
#include "drv_USB.hpp"
#include "drv_Uart1.hpp"
#include "Sensors.hpp"
#include "MeasurementSystem.hpp"
#include "mavlink.h"
#include "Commulink.hpp"
#include "StorageSystem.hpp"
#include "ControlSystem.hpp"

float debug_test[30];

static void Debug_task(void* pvParameters)
{
	while(1)
	{
		uint8_t port_id = 0;
		const Port* port = get_CommuPort(port_id);
		if( port==0 || port->write==0 )
		{
			os_delay(1);
			continue;
		}
		mavlink_message_t msg_sd;
		
//		if( mavlink_lock_chan(1,0.01) )
//		{
//			mavlink_msg_vfr_hud_pack_chan(
//				1 ,	//system id
//				MAV_COMP_ID_AUTOPILOT1 ,	//component id
//				port_id , 	//chan
//				&msg_sd ,
//				0,0,0,0,0,0
//			);
//			mavlink_msg_to_send_buffer(Write_Uart1, 
//																 Lock_Uart1,
//																 Unlock_Uart1,
//																 &msg_sd, 0, -1);
//			mavlink_unlock_chan(1);
//		}
		extern int aaw[16];
		IMU_Sensor imu;
		GetMagnetometer( 2, &imu, 0.1 );
		if( mavlink_lock_chan(port_id,0.01) )
		{
			vector3<double> vec;
			IMU_Sensor sensor;
			Position_Sensor_Data pos;
			get_AccelerationNC_filted(&vec);
			GetPositionSensorData(1,&pos);
			GetMagnetometer( 2, &sensor );
			mavlink_msg_debug_vect_pack_chan( 
				get_CommulinkSysId() ,	//system id
				get_CommulinkCompId() ,	//component id
				port_id , 	//chan
				&msg_sd ,
				"1" ,	//name
				TIME::get_System_Run_Time() * 1e6 , 	//boot ms
				debug_test[0] ,
				debug_test[1] ,
				debug_test[2] );
			mavlink_msg_to_send_buffer(port->write, 
																 port->lock,
																 port->unlock,
																 &msg_sd, 0, -1);
					
			
			Position_Sensor_Data pos2;
			GetPositionSensorData(default_rtk_sensor_index,&pos);
			GetPositionSensorData(internal_baro_sensor_index,&pos2);
			mavlink_msg_debug_vect_pack_chan( 
				get_CommulinkSysId() ,	//system id
				get_CommulinkCompId() ,	//component id
				port_id , 	//chan
				&msg_sd ,
				"2" ,	//name
				TIME::get_System_Run_Time() * 1e6 , 	//boot ms
				get_YawHealthEst() ,
				sensor.data.x ,
				sensor.data.z );
			mavlink_msg_to_send_buffer(port->write, 
																 port->lock,
																 port->unlock,
																 &msg_sd, 0, -1);
																 
			get_AccelerationENU_Ctrl(&vec);
			mavlink_msg_debug_vect_pack_chan( 
				get_CommulinkSysId() ,	//system id
				get_CommulinkCompId() ,	//component id
				port_id , 	//chan
				&msg_sd ,
				"3" ,	//name
				TIME::get_System_Run_Time() * 1e6 , 	//boot ms
				debug_test[10] ,
				debug_test[11] ,
				debug_test[12] );
			mavlink_msg_to_send_buffer(port->write, 
																 port->lock,
																 port->unlock,
																 &msg_sd, 0, -1);
																 
		mavlink_msg_debug_vect_pack_chan( 
				get_CommulinkSysId() ,	//system id
				get_CommulinkCompId() ,	//component id
				port_id , 	//chan
				&msg_sd ,
				"4" ,	//name
				TIME::get_System_Run_Time() * 1e6 , 	//boot ms
				debug_test[10],
				pos2.position.z*0.01 - 89 ,
				pos.position.z*0.01 + 10 );
			mavlink_msg_to_send_buffer(port->write, 
																 port->lock,
																 port->unlock,
																 &msg_sd, 0, -1);
//																 
//			GetMagnetometer( 2, &sensor );
//			mavlink_msg_debug_vect_pack_chan( 
//				1 ,	//system id
//				MAV_COMP_ID_AUTOPILOT1 ,	//component id
//				port_id , 	//chan
//				&msg_sd ,
//				"Mag" ,	//name
//				TIME::get_System_Run_Time() * 1e6 , 	//boot ms
//				debug_test[21] ,
//				debug_test[22] ,
//				debug_test[24] );
//			mavlink_msg_to_send_buffer(port->write, 
//																 port->lock,
//																 port->unlock,
//																 &msg_sd, 0, -1);
			mavlink_unlock_chan(port_id);
		}

		bool inFlight;
		get_is_inFlight(&inFlight);
		if( inFlight )
		{
			double logbuf[6];
			logbuf[0] = TIM4->CCR3;
			logbuf[1] = TIM4->CCR4;
			logbuf[2] = TIM8->CCR1;
			logbuf[3] = TIM8->CCR2;
			logbuf[4] = TIM3->CCR1;
			logbuf[5] = TIM3->CCR2;
			SDLog_Msg_DebugVect( "dianji", logbuf, 6 );
		}
//		/*姿态*/
//			uint8_t buf[30];
//			uint8_t i = 0;
//			uint8_t checksum = 0;
//			buf[i] = 0xaa;
//			checksum += buf[i++];
//			buf[i] = 0xaa;
//			checksum += buf[i++];
//			buf[i] = 0x01;
//			checksum += buf[i++];
//			buf[i] = 6;
//			checksum += buf[i++];
//			
//			int16_t temp = debug_test[0]*5729.578f;
//			buf[i] = temp >> 8;
//			checksum += buf[i++];
//			buf[i] = temp;
//			checksum += buf[i++];
//			
//			temp = debug_test[1]*5729.578f;
//			buf[i] = temp >> 8;
//			checksum += buf[i++];
//			buf[i] = temp;
//			checksum += buf[i++];
//			
//			temp = debug_test[2]*5729.578f;
//			buf[i] = temp >> 8;
//			checksum += buf[i++];
//			buf[i] = temp;
//			checksum += buf[i++];
//			
//			buf[i++] = checksum;
//			Write_USBD_VCOM( buf , i );
//		/*姿态*/
//		
//		/*其它*/
//			checksum = i = 0;
//			buf[i] = 0xaa;
//			checksum += buf[i++];
//			buf[i] = 0xaa;
//			checksum += buf[i++];
//			buf[i] = 0x02;
//			checksum += buf[i++];
//			buf[i] = 18;
//			checksum += buf[i++];
//			
//			temp = debug_test[4];
//			buf[i] = temp >> 8;
//			checksum += buf[i++];
//			buf[i] = temp;
//			checksum += buf[i++];
//			
//			temp = debug_test[5];
//			buf[i] = temp >> 8;
//			checksum += buf[i++];
//			buf[i] = temp;
//			checksum += buf[i++];
//			
//			temp = debug_test[6];
//			buf[i] = temp >> 8;
//			checksum += buf[i++];
//			buf[i] = temp;
//			checksum += buf[i++];
//			
//			temp = debug_test[7];
//			buf[i] = temp >> 8;
//			checksum += buf[i++];
//			buf[i] = temp;
//			checksum += buf[i++];
//			
//			temp = debug_test[8];
//			buf[i] = temp >> 8;
//			checksum += buf[i++];
//			buf[i] = temp;
//			checksum += buf[i++];
//			
//			temp = debug_test[9];
//			buf[i] = temp >> 8;
//			checksum += buf[i++];
//			buf[i] = temp;
//			checksum += buf[i++];
//			
//			temp = debug_test[10];
//			buf[i] = temp >> 8;
//			checksum += buf[i++];
//			buf[i] = temp;
//			checksum += buf[i++];
//			
//			temp = debug_test[11];
//			buf[i] = temp >> 8;
//			checksum += buf[i++];
//			buf[i] = temp;
//			checksum += buf[i++];
//			
//			temp = debug_test[12];
//			buf[i] = temp >> 8;
//			checksum += buf[i++];
//			buf[i] = temp;
//			checksum += buf[i++];
//			
//			buf[i++] = checksum;
//			Write_USBD_VCOM( buf , i );
//		/*其它*/
		
		os_delay(0.02);
	}
}

void init_Debug()
{
	xTaskCreate( Debug_task , "Debug" ,2000,NULL,SysPriority_UserTask,NULL);
}