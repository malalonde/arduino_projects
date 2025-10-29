#include <SparkFun_Bio_Sensor_Hub_Library.h>
#include <Wire.h>

// Reset pin, MFIO pin
int resPin = A3;
int mfioPin = A5;

// Takes address, reset pin, and MFIO pin.
SparkFun_Bio_Sensor_Hub bioHub(resPin, mfioPin); 

bioData body;  

void setup(){
  Serial.begin(115200);

  Wire.begin();
  bioHub.begin();
 
  Serial.println("Configuring Sensor...."); 
  bioHub.configBpm(MODE_ONE); // Configuring just the BPM settings. 

  // Data lags a bit behind the sensor, if you're finger is on the sensor when
  // it's being configured this delay will give some time for the data to catch
  // up. 
  Serial.println("Loading up the buffer with data....");
  delay(4000); 
}

void loop(){

    // Information from the readBpm function will be saved to our "body"
    // variable.  
    body = bioHub.readBpm();
    Serial.print("Heartrate: ");
    Serial.println(body.heartRate); 
    Serial.print("Confidence: ");
    Serial.println(body.confidence); 
    Serial.print("Status: ");
    Serial.println(body.status); 
    
    // Slow it down or your heart rate will go up trying to keep up
    // with the flow of numbers
    delay(250); 
}
