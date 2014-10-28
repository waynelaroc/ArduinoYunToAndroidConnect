#include <Bridge.h>
#include <YunServer.h>
#include <YunClient.h>
#include <LiquidCrystal.h>
#include <Servo.h> 

// initialize the library with the numbers of the interface pins
//LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

#define PORT 6666

Servo panServo;
Servo tiltServo;
int pos = 0;

YunServer server(PORT);
int forwardCnt = 0;
int backwardCnt = 0;
int leftCnt = 0;
int rightCnt = 0;

const int leftMotor = 3;
const int rightMotor = 5;

long lininoBaud = 250000; // Baud rate for communicating with Linux side

void setup() 
{
   panServo.attach(8);
   tiltServo.attach(9);
  // set up the LCD's number of columns and rows:
  //lcd.begin(16, 2);
  //lcd.noAutoscroll();
  
  Serial.begin(115200);
  Bridge.begin();
  
  Serial1.begin(lininoBaud); // open serial connection to Linino
  
  server.noListenOnLocalhost();
  server.begin();
  
}

void loop() 
{
  
  int panPosition = 90;
  int tiltPosition = 90;
  int panAmount = 5;
  int tiltAmount = 5;
  int minTiltPosition = 10;
  int maxTiltPosition = 170;
  int minPanPosition = 10;
  int maxPanPosition = 170;
  
//  if (Serial.available()) {           // got anything from USB-Serial?
//    char c = (char)Serial.read();     // read from USB-serial
//      Serial1.write(c);
//  }
//  if (Serial1.available()) {          // got anything from Linino?
//    char c = (char)Serial1.read();    // read from Linino
//    Serial.write(c);          // write to USB-serial
//  }

   Process wifiCheck;  // initialize a new process
   //wifiCheck.runShellCommand("ifconfig wlan0 | grep addr | grep -v HWaddr | cut -c21-35");  // command you want to run
   //lcd.clear();
   //lcd.setCursor(0, 0);
   // while there's any characters coming back from the
   // process, print them to the serial monitor:
   while (wifiCheck.available() > 0)
   {      
     char c = wifiCheck.read();
     //lcd.print(c);
   }
  
  YunClient client = server.accept();

  if(client.connected())
  {
    //lcd.setCursor(0, 1);
    //lcd.print("CLIENT CONNECTED");
    Serial.println("CLIENT CONNECTED!");
    
   

    String string = "";


    while(client.connected())
    {
      Serial.println("CLIENT 
      CONNECTED!");
      if(client.available())
      {        
        //Serial.println("CLIENT AVAILABLE!");
        int received = client.read();
        
          switch (received)
         {
           case 1:
           {
             Serial.println("forward");
             
             forwardCnt++;
             
             if(forwardCnt == 5)
             {
               forwardCnt = 0;
             }
             
             int speed = map(forwardCnt, 0, 4, 0, 255);             
             //turn on motors!
             analogWrite(leftMotor,speed);
             analogWrite(rightMotor,speed);
             
             client.write(received);
              break;
           }
           case 2:
           {
             Serial.println("backward");
             backwardCnt++;
             
             if(backwardCnt == 5)
             {
               backwardCnt = 0;
             }
             client.write(received);
              break;
           } 
           case 3:
           {
             Serial.println("left");
             leftCnt++;
             
             if(leftCnt == 5)
             {
               leftCnt = 0;
             }
              int speed = map(leftCnt, 0, 4, 0, 255);
             analogWrite(leftMotor,0);
             analogWrite(rightMotor,speed);
             client.write(received);
              break;
           } 
           case 4:
           {
             Serial.println("right");
             rightCnt++;
             
             if(rightCnt == 5)
             {
               rightCnt = 0;
             }
             int speed = map(rightCnt, 0, 4, 0, 255);
             analogWrite(leftMotor,speed);
             analogWrite(rightMotor,0);
             client.write(received);
              break;
           } 
            case 5:
           {
             Serial.println("halt");
             client.write(received);
             analogWrite(leftMotor,0);
             analogWrite(rightMotor,0);
             break;
           }
           case 6:
           {
             Serial.println("Start Video");
             client.write(received);
             break;
           }
           case 7:
           {
             Serial.println("Pan left");
             panPosition -= panAmount;
             panPosition = constrain(panPosition, minPanPosition, maxPanPosition);
             panServo.write(panPosition); 
             delay(15);  // waits 15ms for the servo to reach the position 
              
             client.write(received);
             break;
           }
            case 8:
           {
             Serial.println("Pan Right");
             panPosition += panAmount;
             panPosition = constrain(panPosition, minPanPosition, maxPanPosition);
             panServo.write(panPosition); 
             delay(15);  // waits 15ms for the servo to reach the position 
              
             client.write(received);
             break;
           }

            case 9:
           {
             Serial.println("Tilt Up");
             tiltPosition += tiltAmount;
             tiltPosition = constrain(tiltPosition, minTiltPosition, maxTiltPosition);
             tiltServo.write(tiltPosition); 
             delay(15);  // waits 15ms for the servo to reach the position               
             client.write(received);
             break;
           }

            case 10:
           {
             Serial.println("Tilt Down");
             tiltPosition -= tiltAmount;
             tiltPosition = constrain(tiltPosition, minTiltPosition, maxTiltPosition);
             tiltServo.write(tiltPosition); 
             delay(15);  // waits 15ms for the servo to reach the position             
             client.write(received);
             break;
           }
          default:
           {
              break;
           }  
         }
      }
    }
    client.stop();
  }
  else 
  {
    //lcd.setCursor(0, 1);
    //lcd.print("no client       ");
    Serial.println("no client connected, retrying");
  }
  delay(1000);
}


