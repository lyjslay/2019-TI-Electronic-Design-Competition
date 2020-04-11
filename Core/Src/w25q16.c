/**
  ******************************************************************************
  * @file           : w25q16.c
  * @brief          : w25q16 external flash file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

#include "w25q16.h" 
#include "spi.h"

//SPI��дһ���ֽ�
uint8_t SPI1_ReadWriteByte(uint8_t TxData)
{
	uint8_t d_read;
	uint8_t d_send = TxData;
	if(HAL_SPI_TransmitReceive(&hspi1,&d_send,&d_read,1,0xFFFF) != HAL_OK)
		d_read = ' ';
	return d_read;
} 

//��ȡW25QXX��״̬�Ĵ���
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
//TB,BP2,BP1,BP0:FLASH����д��������
//WEL:дʹ������
//BUSY:æ���λ(1,æ;0,����)
//Ĭ��:0x00
uint8_t W25QXX_ReadSR(void)   
{  
	uint8_t byte=0;   
	HAL_GPIO_WritePin(FCS_GPIO_Port,FCS_Pin,GPIO_PIN_RESET);                            //ʹ������  
	SPI1_ReadWriteByte(W25X_ReadStatusReg);    //���Ͷ�ȡ״̬�Ĵ�������    
	byte=SPI1_ReadWriteByte(0Xff);             //��ȡһ���ֽ�  
	HAL_GPIO_WritePin(FCS_GPIO_Port,FCS_Pin,GPIO_PIN_SET);                            //ȡ��Ƭѡ     
	return byte;   
}

//дW25QXX״̬�Ĵ���
//ֻ��SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)����д!!!
void W25QXX_Write_SR(uint8_t sr)   
{   
	HAL_GPIO_WritePin(FCS_GPIO_Port,FCS_Pin,GPIO_PIN_RESET);                            //ʹ������   
	SPI1_ReadWriteByte(W25X_WriteStatusReg);   //����дȡ״̬�Ĵ�������    
	SPI1_ReadWriteByte(sr);               //д��һ���ֽ�  
	HAL_GPIO_WritePin(FCS_GPIO_Port,FCS_Pin,GPIO_PIN_SET);                            //ȡ��Ƭѡ     	      
}

//W25QXXдʹ��	
//��WEL��λ   
void W25QXX_Write_Enable(void)   
{
	HAL_GPIO_WritePin(FCS_GPIO_Port,FCS_Pin,GPIO_PIN_RESET);                            //ʹ������   
	SPI1_ReadWriteByte(W25X_WriteEnable);      //����дʹ��  
	HAL_GPIO_WritePin(FCS_GPIO_Port,FCS_Pin,GPIO_PIN_SET);                            //ȡ��Ƭѡ     	      
}

//W25QXXд��ֹ	
//��WEL����  
void W25QXX_Write_Disable(void)   
{  
	HAL_GPIO_WritePin(FCS_GPIO_Port,FCS_Pin,GPIO_PIN_RESET);                            //ʹ������   
	SPI1_ReadWriteByte(W25X_WriteDisable);     //����д��ָֹ��    
	HAL_GPIO_WritePin(FCS_GPIO_Port,FCS_Pin,GPIO_PIN_SET);                            //ȡ��Ƭѡ     	      
} 

//��ȡоƬID
//����ֵ����:				   
//0XEF13,��ʾоƬ�ͺ�ΪW25Q80  
//0XEF14,��ʾоƬ�ͺ�ΪW25Q16    
//0XEF15,��ʾоƬ�ͺ�ΪW25Q32  
//0XEF16,��ʾоƬ�ͺ�ΪW25Q64 
//0XEF17,��ʾоƬ�ͺ�ΪW25Q128 	  
uint16_t W25QXX_ReadID(void)
{
	uint16_t Temp = 0;	  
	HAL_GPIO_WritePin(FCS_GPIO_Port,FCS_Pin,GPIO_PIN_RESET);				    
	SPI1_ReadWriteByte(0x90);//���Ͷ�ȡID����	    
	SPI1_ReadWriteByte(0x00); 	    
	SPI1_ReadWriteByte(0x00); 	    
	SPI1_ReadWriteByte(0x00); 	 			   
	Temp|=SPI1_ReadWriteByte(0xFF)<<8;  
	Temp|=SPI1_ReadWriteByte(0xFF);	 
	HAL_GPIO_WritePin(FCS_GPIO_Port,FCS_Pin,GPIO_PIN_SET);	
	return Temp;
}  

//��ȡSPI FLASH  
//��ָ����ַ��ʼ��ȡָ�����ȵ�����
//pBuffer:���ݴ洢��
//ReadAddr:��ʼ��ȡ�ĵ�ַ(24bit)
//NumByteToRead:Ҫ��ȡ���ֽ���(���65535)
void W25QXX_Read(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead)   
{ 
	uint16_t i;   										    
	HAL_GPIO_WritePin(FCS_GPIO_Port,FCS_Pin,GPIO_PIN_RESET);                            //ʹ������   
	SPI1_ReadWriteByte(W25X_ReadData);         //���Ͷ�ȡ����   
	SPI1_ReadWriteByte((uint8_t)((ReadAddr)>>16));  //����24bit��ַ    
	SPI1_ReadWriteByte((uint8_t)((ReadAddr)>>8));   
	SPI1_ReadWriteByte((uint8_t)ReadAddr);   
	for(i=0;i<NumByteToRead;i++)
	{ 
		pBuffer[i]=SPI1_ReadWriteByte(0XFF);   //ѭ������  
	}
	HAL_GPIO_WritePin(FCS_GPIO_Port,FCS_Pin,GPIO_PIN_SET);  				    	      
}  

//SPI��һҳ(0~65535)��д������256���ֽڵ�����
//��ָ����ַ��ʼд�����256�ֽڵ�����
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!	 
void W25QXX_Write_Page(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)
{
	uint16_t i;  
	W25QXX_Write_Enable();                  //SET WEL 
	HAL_GPIO_WritePin(FCS_GPIO_Port,FCS_Pin,GPIO_PIN_RESET);                            //ʹ������   
	SPI1_ReadWriteByte(W25X_PageProgram);      //����дҳ����   
	SPI1_ReadWriteByte((uint8_t)((WriteAddr)>>16)); //����24bit��ַ    
	SPI1_ReadWriteByte((uint8_t)((WriteAddr)>>8));   
	SPI1_ReadWriteByte((uint8_t)WriteAddr);   
	for(i=0;i<NumByteToWrite;i++)
		SPI1_ReadWriteByte(pBuffer[i]);//ѭ��д��  
	HAL_GPIO_WritePin(FCS_GPIO_Port,FCS_Pin,GPIO_PIN_SET);                            //ȡ��Ƭѡ 
	W25QXX_Wait_Busy();					   //�ȴ�д�����
} 

//�޼���дSPI FLASH 
//����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
//�����Զ���ҳ���� 
//��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)
//CHECK OK
void W25QXX_Write_NoCheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{ 			 		 
	uint16_t pageremain;	   
	pageremain=256-WriteAddr%256; //��ҳʣ����ֽ���		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//������256���ֽ�
	while(1)
	{	   
		W25QXX_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)
			break;//д�������
		else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //��ȥ�Ѿ�д���˵��ֽ���
			if(NumByteToWrite>256)pageremain=256; //һ�ο���д��256���ֽ�
			else pageremain=NumByteToWrite; 	  //����256���ֽ���
		}
	}	    
} 

//дSPI FLASH  
//��ָ����ַ��ʼд��ָ�����ȵ�����
//�ú�������������!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)						
//NumByteToWrite:Ҫд����ֽ���(���65535)   
uint8_t W25QXX_BUFFER[4096];		 
void W25QXX_Write(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{ 
	uint32_t secpos;
	uint16_t secoff;
	uint16_t secremain;	   
	uint16_t i;    
	uint8_t * W25QXX_BUF;	  
	W25QXX_BUF=W25QXX_BUFFER;	     
	secpos=WriteAddr/4096;//������ַ  
	secoff=WriteAddr%4096;//�������ڵ�ƫ��
	secremain=4096-secoff;//����ʣ��ռ��С   
	//printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//������
	
	if(NumByteToWrite<=secremain)
		secremain=NumByteToWrite;//������4096���ֽ�
	
	while(1) 
	{	
		W25QXX_Read(W25QXX_BUF,secpos*4096,4096);//������������������
		for(i=0;i<secremain;i++)//У������
		{
			if(W25QXX_BUF[secoff+i]!=0XFF)
				break;//��Ҫ����  	  
		}
		
		if(i<secremain)//��Ҫ����
		{
			W25QXX_Erase_Sector(secpos);//�����������
			for(i=0;i<secremain;i++)	   //����
				W25QXX_BUF[i+secoff]=pBuffer[i];	  
			W25QXX_Write_NoCheck(W25QXX_BUF,secpos*4096,4096);//д����������  
		}
		else 
			W25QXX_Write_NoCheck(pBuffer,WriteAddr,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 		
		
		if(NumByteToWrite==secremain)
			break;//д�������
		else//д��δ����
		{
			secpos++;//������ַ��1
			secoff=0;//ƫ��λ��Ϊ0 	 

			pBuffer+=secremain;  //ָ��ƫ��
			WriteAddr+=secremain;//д��ַƫ��	   
			NumByteToWrite-=secremain;				//�ֽ����ݼ�
			if(NumByteToWrite>4096)secremain=4096;	//��һ����������д����
			else secremain=NumByteToWrite;			//��һ����������д����
		}	 
	}	 
}

//��������оƬ		  
//�ȴ�ʱ�䳬��...
void W25QXX_Erase_Chip(void)   
{                                   
	W25QXX_Write_Enable();                  //SET WEL 
	W25QXX_Wait_Busy();   
	HAL_GPIO_WritePin(FCS_GPIO_Port,FCS_Pin,GPIO_PIN_RESET);                            //ʹ������   
	SPI1_ReadWriteByte(W25X_ChipErase);        //����Ƭ��������  
	HAL_GPIO_WritePin(FCS_GPIO_Port,FCS_Pin,GPIO_PIN_SET);                            //ȡ��Ƭѡ     	      
	W25QXX_Wait_Busy();   				   //�ȴ�оƬ��������
}

//����һ������
//Dst_Addr:������ַ ����ʵ����������
//����һ��ɽ��������ʱ��:150ms
void W25QXX_Erase_Sector(uint32_t Dst_Addr)   
{  
	//����falsh�������,������   
	//printf("fe:%x\r\n",Dst_Addr);	  
	Dst_Addr*=4096;
	W25QXX_Write_Enable();                  //SET WEL 	 
	W25QXX_Wait_Busy();   
	HAL_GPIO_WritePin(FCS_GPIO_Port,FCS_Pin,GPIO_PIN_RESET);                            //ʹ������   
	SPI1_ReadWriteByte(W25X_SectorErase);      //������������ָ�� 
	SPI1_ReadWriteByte((uint8_t)((Dst_Addr)>>16));  //����24bit��ַ    
	SPI1_ReadWriteByte((uint8_t)((Dst_Addr)>>8));   
	SPI1_ReadWriteByte((uint8_t)Dst_Addr);  
	HAL_GPIO_WritePin(FCS_GPIO_Port,FCS_Pin,GPIO_PIN_SET);                            //ȡ��Ƭѡ     	      
	W25QXX_Wait_Busy();   				   //�ȴ��������
}

//�ȴ�����
void W25QXX_Wait_Busy(void)   
{   
	while((W25QXX_ReadSR()&0x01)==0x01);   // �ȴ�BUSYλ���
}

//�������ģʽ
void W25QXX_PowerDown(void)   
{ 
	HAL_GPIO_WritePin(FCS_GPIO_Port,FCS_Pin,GPIO_PIN_RESET);                            //ʹ������   
	SPI1_ReadWriteByte(W25X_PowerDown);        //���͵�������  
	HAL_GPIO_WritePin(FCS_GPIO_Port,FCS_Pin,GPIO_PIN_SET);                            //ȡ��Ƭѡ     	      
	HAL_Delay(3);                               //�ȴ�TPD  
} 

//����
void W25QXX_WAKEUP(void)   
{  
	HAL_GPIO_WritePin(FCS_GPIO_Port,FCS_Pin,GPIO_PIN_RESET);                            //ʹ������   
	SPI1_ReadWriteByte(W25X_ReleasePowerDown);   //  send W25X_PowerDown command 0xAB    
	HAL_GPIO_WritePin(FCS_GPIO_Port,FCS_Pin,GPIO_PIN_SET);                            //ȡ��Ƭѡ     	      
	HAL_Delay(3);                               //�ȴ�TRES1
}   

//W25QXX_Write((uint8_t*)TEXT_Buffer,W25Q16_SIZE-1000,TX_Size);	
//W25QXX_Read(Receive_Buffer,W25Q16_SIZE-1000,TX_Size);					//�ӵ�����100����ַ����ʼ,����SIZE���ֽ�	


//sprintf((char *)result,"%f",12.3999);		//double to string
//W25QXX_Write(result,W25Q16_SIZE-1000,6);
//W25QXX_Read(recevive,W25Q16_SIZE-1000,6);


//strtod((char*)read_value,NULL);  string to double



















