/* This is a sketch modified in order to be intergrated into main code for 
 * transmitter control.
 * Author: Ziyue Hu
 * Date: Feb 4, 2015
 */
  #include <SoftwareSerial.h>
 
 SoftwareSerial xbee(0,1);//RX,TX
 
 //Testing LED
 int LED = 4;
 
 //arrays for data restoring;
 char Data[30];//reading array from xbee
 char temp[4];
 char spd[8];
 char curr[3];
 
 //index used to read data
 int dataId=0;
 
 void setup(){
   xbee.begin(9600);
   Serial.begin(9600);
   
   //test
   pinMode(LED,OUTPUT);
 }
 
 void loop(){
  boolean dataReady = false;
  while(!dataReady){//when we have no data coming in we will wait a bit
    digitalWrite(LED,HIGH);
  if(xbee.available() > 0){

    char incoming = xbee.read();
    int id = 0;//used for temp curr and spd array
    //store incoming into data reading array.(remember to reset index)
    Data[dataId]=incoming;
    dataId++;//get ready for next reading
    int temperature;
    int rotateSpd;
    int current;
    
   
    if(incoming == '>'){//see if a whole set of data has been read from xbee
      dataReady = true;
      dataId=0;//reset data reading id
    }
    
    //if we have collected one complete set of the data, we can start 
    //interpreting it.
    if(dataReady){
      int i = 0;//index variable used to iterate through the Data reading array
      int countBar = 0;// delimeteter '|', used to keep track of the data order
      
      while( Data[i] == '<')//iterate through the start of the array
        i++;
      
      for( id = 0 ; countBar == 0 ;i++, id++){//see if it is TEMPERATURE reading(cBar = 0)
          if(Data[i]!='|'){
            temp[id]=Data[i];//store Data to temp array
          }
          else//when we meet a count bar, we add one to it and loop out for()
            countBar++;
            
         temperature = atoi(temp);//grab temperature in int

      }

      for(id = 0; countBar == 1 ; i++, id++){//for SPEED data gathering
          if(Data[i]!='|'){
            spd[id]=Data[i];
          }
          else
            countBar++;
            
          rotateSpd = atoi(spd);//grab speed 
      }
        
      for(id = 0; countBar == 2;i++, id++){//for Current sensor
          if(Data[i]!='>'){//see if we are at the end of data set
            curr[id] = Data[i];
          }
          else
            countBar++;//no meaning but to loop out
            current = atoi(curr);//grab current
      }


    char toPrint[30];
    sprintf(toPrint,"temperature %i, speed %i, current %i",temperature,rotateSpd,current);
    Serial.println(toPrint);
    
    //After interpretation is done,
    //Reset interpreting arrays and make room for new readings
    for(id = 0;id < 4;id++) temp[id]=0;
    for(id = 0;id < 3;id++) curr[id]=0;
    for(id = 0;id < 8;id++) spd[id]=0;
      
    int resetData;
    for(resetData = 0; resetData < 30 ; resetData++)
       Data[resetData]=0;
      
    }



   // dataReady = false;//reset ready status 
 
  }
  }
while(1);
 }
 
