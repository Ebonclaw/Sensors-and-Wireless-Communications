/* this is a combination of sensor code
 * Author:Ziyue Hu
 * Date: Jan20,2015
 */
 #define SERIESRESISTOR 10000 
 #include <SendOnlySoftwareSerial.h>

 
 SendOnlySoftwareSerial xbee(3);//RX,TX
 
 //------Define functions-----
 float maxR(float array[90]);
 float minR(float array[90]);
 
 //-------Pin Number--------
 const int spdPin = 10;//SPEED sensor Pin
 const int currPin = 1;//CURRENT sensor Pin
 const int thermister_Pin = 5;//TEMPERATURE sensor Pin
 
 //testing LED
 int LED=9;
 //-------Average Arrays--------
 float currentRA[90]={0};//Current Reading Array
 float thermRA[90]={0};//thermister Reading Array

 void setup(){
   //Devices set up
   Serial.begin(9600);
   xbee.begin(9600);
   pinMode(LED,OUTPUT);
   pinMode(spdPin,INPUT);

 }

 void loop(){

  //---------SPEED sensor---------
   float period = 0;
   float freq = 0;


   while(period <= 3333){//ignore misreads(adopted from Arash's code)
     
     period = pulseIn(spdPin,LOW);//timeout 0.05s: in order to 
                                        //speed up the data reading rate, 
     if(period ==0){
       freq = 0;
       
       break;
     }
     
     else 
     freq = 500000/period;
   
   }

 //Variables for running average
  int indexW;//writing index for running data

 
 //-------CURRENT sensor-------
   float current = 0;
   //5V Vcc/AC input
   //a for loop to add and pull data like a Queue in order to perform running 
   //array behavior

   //we have two pointers here, one is reading index to read
   //the next value and write index to write into the current slot
   for(indexW = 0; indexW < 90;indexW++){
     currentRA[indexW]= analogRead(currPin);
  
   }  

   float maxCurr = maxR(currentRA);
   float minCurr = minR(currentRA);

   
   current = ((maxCurr*2.4/512 - minCurr*2.4/512)/sqrt(2))/0.04/2;
   //formula we found to calculate for measrueing AC current

     
  //-------TEMPERATURE sensor--------

   
   long thermReadingSum = 0;//for average
     
     

   //runing array
   for(indexW = 0; indexW < 90; indexW ++){
      thermRA[indexW] = analogRead(thermister_Pin);
   }
   
   //sum of resistance array
   for(int i = 0; i < 90; i++){
     thermReadingSum += thermRA[i];
   }
   
   float thermReadingAvg = thermReadingSum/90;
   
   //1.Analog Reading -------> Resitance
   float resistance = (1023/thermReadingAvg)-1;
   
   resistance = SERIESRESISTOR / resistance;


   
   //2.Resistance --------> Temperature
   
   //steinhart equation:
   float temperature = 337.6-39.24*log(resistance)+0.06221*log(resistance)*log(resistance)*log(resistance);
   //constants gained by curve fitting

  
   //----Data Transmitting and printing----
   float spd = freq;
   char toPrint[20];
   sprintf(toPrint,"<%i|%i|%i>",(int)temperature,(int)spd,(int)current);
   xbee.print(toPrint);


     
   digitalWrite(LED,HIGH);
  //Delay time suggested by Arash: 1 set data per 10 sec
  delay(10000);
  
 }//loop over
 
 
 //--------Functions-------
 /** find the maximum value in an array
  * @param: an array of size 100
  * @return: the maximum value
  */
 float maxR(float array[90]){
   int idR = 0;//id to read
   float bigNum = array[0];//the greater number
   
   for(idR = 0;idR < 90;idR++){
     if(bigNum < array[idR])
       bigNum = array[idR];
   }
   
   return bigNum;
 }
 
 /** find the minimum vlue in an array
  * @param: an array of size 100
  * @return: the minimum value of the array
  */
  float minR(float array[90]){
   int idR = 0;//id to read
   float smallNum = array[0];//the smaller number
   int misRead = 0;
   
   
   for(idR =0,smallNum = array[idR];idR < 90;idR++){
      if(array[idR] == 0)//if reads 0 then it is a misread
         array[idR]=array[idR+1];//we solve it by giving it a value 
                                 //that is from a nearby slot
      if(smallNum > array[idR])
         smallNum = array[idR];

    
    }

   
   return smallNum;
 }  
