int RedLedPin2 = 2;    
int GreenLedPin3 = 3;    
int BlueLedPin4 = 4; 
int RedLedPin5 = 5;    
int GreenLedPin6 = 6;    
int BlueLedPin7 = 7; 
int RedLedPin9 = 9;    
int GreenLedPin10 = 10;    
int BlueLedPin11 = 11;    
int count, R, G, B, tempR, tempG, tempB;
//int RedLedPot = 0;    // Pot connected to analog pin A0
//int GreenLedPot = 1;    // Pot connected to analog pin A1
//int BlueLedPot = 2;    // Pot connected to analog pin A2
 
void setup()  { 
  Serial.begin(9600);
  count = 0;
  R = 0; 
  G = 0;
  B = 0;
  tempR = 0;
  tempG = 0;
  tempB = 0;
} 
 
void loop()  { 
 
  R = Serial.read();
  G = Serial.read();
  B = Serial.read();
  count++;
  
  if (R < 0)
    R = 0;
  else
    tempR = R;
  if (G < 0)
    G = 0;
  else 
    tempG = G;
  if (B < 0) 
    B = 0;
  else 
    tempB = B;
    
  if(R > 0) 
    count = 0;
    
  Serial.print("count = ");
  Serial.println(count);
  Serial.print("\t");    
  Serial.print("\t");    
  Serial.print(R);
  Serial.print(" ");
  Serial.print(G);
  Serial.print(" ");
  Serial.println(B);
  //Serial.print(G + " ");
  //Serial.println(B);
  //analogWrite(BlueLedPin, 255);
  
  analogWrite(RedLedPin9, tempR);
  analogWrite(RedLedPin5, tempR);
  analogWrite(RedLedPin2, tempR);
  
  analogWrite(GreenLedPin10, tempG);  
  analogWrite(GreenLedPin6, tempG);
  analogWrite(GreenLedPin3, tempG);
  
  analogWrite(BlueLedPin11, tempB); 
  analogWrite(BlueLedPin7, tempB);
  analogWrite(BlueLedPin4, tempB);            
 
 /*
 analogWrite(RedLedPin9, 0);
 analogWrite(GreenLedPin10, 0); 
 analogWrite(BlueLedPin11, 255);
 
 analogWrite(RedLedPin5, 255);
 analogWrite(GreenLedPin6, 0); 
 analogWrite(BlueLedPin7, 255);
 
 analogWrite(RedLedPin2, 255);
 analogWrite(GreenLedPin3, 255); 
 analogWrite(BlueLedPin4, 0);
 */
}
