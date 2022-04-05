#define S1ON digitalRead(S1)==HIGH    // Red button on 
#define S2ON digitalRead(S2)==HIGH    // Yellow button on
#define S3ON digitalRead(S3)==HIGH    // Green button on
#define S1OFF digitalRead(S1)==LOW    // Red button off    
#define S2OFF digitalRead(S2)==LOW    // Yellow button off 
#define S3OFF digitalRead(S3)==LOW    // Green button off 
#define D1ON digitalWrite(D1, HIGH)   // Red LED on 
#define D2ON digitalWrite(D2, HIGH)   // Yellow LED on
#define D1OFF digitalWrite(D1, LOW)   // Red LED off   
#define D2OFF digitalWrite(D2, LOW)   // Yellow LED off 
#define B1ON digitalWrite(B1, HIGH)   // Buzzer on
#define B1OFF digitalWrite(B1, LOW)   // Buzzer off

int d_a = 6, d_b = 2, d_c = 11, d_d = 9, d_e = 8, d_f = 5, d_g = 12, d_h = 10;
// Define the pin of 4-digit 7 Segment Display

int COM1 = 7, COM2 = 4, COM3 = 3, COM4 = 13;
// Define the COM pin

#define S1 A0
#define S2 A2
#define S3 A5
#define D1 A1
#define D2 A3
#define B1 A4  // LED, button and buzzer using analog pin

int ms = 0, s = 0, m = 0, h = 12, State = 0, Clock_Type = 0;
int Alarm_h = h, Alarm_m = m, Alarm_Switch = 0, Clock_Switch = 1;  
// Define the variables.

int num[17][8] = {
 //a  b  c  d  e  f  g  h 
  {0, 0, 0, 0, 0, 0, 1, 1},     //0
  {1, 0, 0, 1, 1, 1, 1, 1},     //1
  {0, 0, 1, 0, 0, 1, 0, 1},     //2
  {0, 0, 0, 0, 1, 1, 0, 1},     //3
  {1, 0, 0, 1, 1, 0, 0, 1},     //4
  {0, 1, 0, 0, 1, 0, 0, 1},     //5
  {0, 1, 0, 0, 0, 0, 0, 1},     //6
  {0, 0, 0, 1, 1, 1, 1, 1},     //7
  {0, 0, 0, 0, 0, 0, 0, 1},     //8
  {0, 0, 0, 0, 1, 0, 0, 1},     //9
  {1, 1, 1, 1, 1, 1, 1, 1},     //.
};

void setup() {
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(B1, OUTPUT);  // Output pin defination
  
  pinMode(S1, INPUT);
  pinMode(S2, INPUT);
  pinMode(S3, INPUT);   // Input pin defination

  pinMode(d_a, OUTPUT);   
  pinMode(d_b, OUTPUT);
  pinMode(d_c, OUTPUT);
  pinMode(d_d, OUTPUT);
  pinMode(d_e, OUTPUT);
  pinMode(d_f, OUTPUT);
  pinMode(d_g, OUTPUT);
  pinMode(d_h, OUTPUT);

  pinMode(COM1, OUTPUT);
  pinMode(COM2, OUTPUT);
  pinMode(COM3, OUTPUT);
  pinMode(COM4, OUTPUT);
}

void loop() {
  
  if(State == 0)
    Clock_Type = 0, Clock(0);  // Display the 12-hour clock
  else if(State == 1)
    Clock_Type = 1, Clock(0);  // Display the 24-hour clock
  else if(State == 2)
    Clock(1);                  // Display MM_SS clock
  else if(State == 3)
    Change_Time(h, m, 0, 0);   // Change the clock time by hour
  else if(State == 4)
    Change_Time(h, m, 1, 0);   // Change the clock time by minute
  else if(State == 5)
    Change_Time(h, m, 0, 1);   // Change the alarm time by hour
  else if(State == 6)
    Change_Time(h, m, 1, 1);   // Change the alarm time by minute
  else if(State == 7)
    Clock(2);                  // Display the alarm time
  else
    Clock(3);                  // Clock sleep
    
}

void Clock(int mode) {
  while(1) {
    Make_Time_Legal();   // make time in legal region
    for(int q = ms + 125 * 2; ms < q; ms++) {
      int input = Input_Judge();   // Judge how the button was pressed
      s = (ms + 1) / 125;     
      if(input == 1) State = 3;    // If button was pressed, change the state
      else if(input == 2) State = 5;
      else if(input == 3) Alarm_Switch ^= 1; // Turn off or turn on the alarm
      else if(input == 4) {
        Clock_Switch ^= 1;
        if(State == 8) State = 0;
        else State = 8;
      }
      else if(input == 5) State = 2;
      else if(input == 6) State = Clock_Type ^ 1; // 12-hour or 24-hour
      if(input && input != 3) return;
      if(Alarm_Switch == 1) {  // Alarm switch is on
        D1ON;
        if(h == Alarm_h && m == Alarm_m) {
          B1ON;
          delay(4);    // Let the buzzer ring
          B1OFF;
          delay(4);
          ms++;
        }
      }
      else D1OFF;  // Turn off the LED
      if(mode == 0) {  // If display the clock
        if(ms < q - 125) {
          if(h > 12 && Clock_Type == 0) 
            Clock_Display(h - 12, m, 0, 1, 1, 1, 1);    // Display the time
          else
            Clock_Display(h, m, 0, 1, 1, 1, 0);    // Let time display legally
        }
        else {
          if(h > 12 && Clock_Type == 0) 
            Clock_Display(h - 12, m, 0, 1, 1, 0, 1);   
          else
            Clock_Display(h, m, 0, 1, 1, 0, 0);
        }
      }
      else if(mode == 1) {
        D2OFF;   // Display minute and second
        Clock_Display(m, s, 1, 1, 1, 1, 0);
      }
      else if(mode == 2) {
        D2ON;  // Turn on the LED D2
        if(Alarm_h > 12 && Clock_Type == 0) 
          Clock_Display(Alarm_h - 12, Alarm_m, 0, 1, 1, 1, 1);   
        else
          Clock_Display(Alarm_h, Alarm_m, 0, 1, 1, 1, 0); // Display the alarm time
      }
      else {
        D2OFF;
        delay(8);
      }
    }
  }
}

int Input_Judge() {  
  if(S1ON) {
    for(int q = 0; q < 125 * 2; q++) {
      if(S1OFF) {
        delay(400);
        return 1;   // Whether the button is holdding for 2 seconds or not
      }
    }
    delay(400);
    return 2;   // Return the state
  }
  if(S2ON) {
    for(int q = 0; q < 125 * 2; q++) {
      if(S2OFF) {
        delay(400);   // Same as the S1's way to judge the button state
        return 3;
      }
    }
    delay(400);
    return 4;
  }
  if(S3ON) {
    for(int q = 0; q < 125 * 2; q++) {
      if(S3OFF) {
        delay(400);   // Same as the S1's way to judge the button state
        return 5;
      }
    }
    delay(400);
    return 6;
  }
  return 0;
}

void Clock_Display(int h_temp, int m_temp, bool type, bool first, bool second, bool midPoint, bool Point) {
  int times = 2;
  if(first ^ second == 1) times = 4;  // Dispaly two numbers and display the point
    if(first) {
        if(type)
          Display(1, h_temp / 10, 0);  // Display the time in appropriate way
        delay(times);
        if(midPoint)
          Display(2, h_temp % 10, 1);
        else
          Display(2, h_temp % 10, 0);
        delay(times);     // Let the delay time same when just display one number
    }
    if(second) {
        Display(3, m_temp / 10, 0);
        delay(times);
        if(Point)
          Display(4, m_temp % 10, 1);  // Display the minute
        else
          Display(4, m_temp % 10, 0);
        delay(times);
    }
}

void Make_Time_Legal() {
  if(ms > 249) ms -= 250, s += 2;
  if(s > 59) s -= 60, m++;
  if(m > 59) m -= 60, h++;
  if(h > 23) h -= 24;   // Let time in legal region
}

void Change_Time(int h_temp, int m_temp, int mode, int type) { 
  awaiting:    // Change the clock time or alarm time
  while(1) {
    Make_Time_Legal();
    for(int q = ms + 125 * 2; ms < q; ms++) {
      s = (ms + 1) / 125;
      if(S1ON) {
        ms += 50;
        delay(400);  // If pressed the button S1 
        State++;
        if(type == 0 && mode == 1) h = h_temp, m = m_temp, State = 0;
        if(type == 1 && mode == 1) Alarm_h = h_temp, Alarm_m = m_temp, Alarm_Switch = 1, State = 0;
                     // Change the time for hour and minute
        return;
      }
      if(S2ON) {
        ms += 50;
        delay(400);  // If pressed the button S2
        goto increase;
      }
      if(S3ON) {
        ms += 50;
        delay(400);  // If pressed the button S3
        goto decrease;
      }
      if(type == 1 && q / 125 % 2 == 0) D1ON; // Let the LED D1 blink
      else D1OFF;
      if(q / 62 % 2 == 0) {
        if(h_temp > 12 && Clock_Type == 0)
          Clock_Display(h_temp - 12, m_temp, 1, 1, 1, 1, 1);   // Display the Clock when changing time
        else
          Clock_Display(h_temp, m_temp, 1, 1, 1, 1, 0);
      }
      else {
        if(h_temp > 12 && Clock_Type == 0) {
          if(mode == 0)
            Clock_Display(h_temp - 12, m_temp, 1, 0, 1, 1, 1);  // Display the Clock when changing time
          else
            Clock_Display(h_temp - 12, m_temp, 1, 1, 0, 1, 1);
        }
        else {
          if(mode == 0)
            Clock_Display(h_temp, m_temp, 1, 0, 1, 1, 1); // Display the Clock when changing time
          else
            Clock_Display(h_temp, m_temp, 1, 1, 0, 1, 1);
        }
      }
    }
  }

  increase:  
  if(mode == 0) h_temp++;   // increase the h_temp
  else m_temp++;
  if(m_temp > 59) m_temp -= 60, h_temp++;
  if(h_temp > 23) h_temp -= 24;   // make the time leagal
  for(int q = ms + 125; ms < q; ms++) { 
    s = (ms + 1) / 125;
    if(S2OFF) {
      goto   awaiting;   // Go back to wating
    }
  }
  while(1) {
    Make_Time_Legal();
    if(S2OFF) goto awaiting;  // If holding the button over 2 seconds
    if(mode == 0) h_temp++;
    else m_temp++;
    if(m_temp > 59) m_temp -= 60, h_temp++;
    if(h_temp > 23) h_temp -= 24;  // Make the time legal
    for(int q = ms + 125 * 2; ms < q; ms++) {
      if(type == 1 && q / 125 % 2 == 1) D1ON;  // Turn on the D1
      else D1OFF;       // Turn off the D1
      s = (ms + 1) / 125;
      if(ms == q - 125) {
          if(mode == 0) h_temp++;  // Increase the hour 
          else m_temp++;
          if(m_temp > 59) m_temp -= 60, h_temp++;
          if(h_temp > 23) h_temp -= 24;
      }
      if(h_temp > 12 && Clock_Type == 0)
        Clock_Display(h_temp - 12, m_temp, 1, 1, 1, 1, 1); // Display the clock time
      else
        Clock_Display(h_temp, m_temp, 1, 1, 1, 1, 0);   
    }
  }


  decrease:   // When decreasing the time
  if(mode == 0) h_temp--;
  else m_temp--;
  if(m_temp < 0) m_temp = 59;
  if(h_temp < 0) m_temp = 23;  // Make the time display legally
  for(int q = ms + 125; ms < q; ms++) {
    s = (ms + 1) / 125;
    Make_Time_Legal();  // Make the time legal
    if(S2OFF) {
      goto   awaiting;  // Go back to waiting
    }
  }
  
  while(1) {
    Make_Time_Legal();  // Make the time legal
    if(S2OFF) goto awaiting;
    if(mode == 0) h_temp--;
    else m_temp--;
    if(m_temp < 0) m_temp = 59;
    if(h_temp < 0) m_temp = 23;
    for(int q = ms + 125 * 2; ms < q; ms++) {
      if(type == 1 && q / 125 % 2 == 1) D1ON;  // Let the D1 blink
      else D1OFF;
      s = (ms + 1) / 125;
      if(ms == q - 125) {
          if(mode == 0) h_temp--;
          else m_temp--;            // Decrease the time by one per second
          if(m_temp < 0) m_temp = 59;
          if(h_temp < 0) m_temp = 23;     
      }
      if(h_temp > 12 && Clock_Type == 0)
        Clock_Display(h_temp - 12, m_temp, 1, 1, 1, 1, 1);  // Display the time
      else
        Clock_Display(h_temp, m_temp, 1, 1, 1, 1, 0);
    }
  }
}


void Display(unsigned char com, unsigned char n, unsigned char Point) {
  digitalWrite(d_a, LOW);     // Display number on the 4-digit 7 Segment Display
  digitalWrite(d_b, LOW);
  digitalWrite(d_c, LOW);
  digitalWrite(d_d, LOW);    
  digitalWrite(d_e, LOW);
  digitalWrite(d_f, LOW);
  digitalWrite(d_g, LOW);
  digitalWrite(d_h, LOW);
 
  switch(com) {  // Judge whitch com should be displayed
    case 1:
      digitalWrite(COM1, HIGH);  
      digitalWrite(COM2, LOW);
      digitalWrite(COM3, LOW);
      digitalWrite(COM4, LOW);
      break;
    case 2:
      digitalWrite(COM1, LOW);
      digitalWrite(COM2, HIGH);
      digitalWrite(COM3, LOW);
      digitalWrite(COM4, LOW);
      break;
    case 3:
      digitalWrite(COM1, LOW);
      digitalWrite(COM2, LOW);
      digitalWrite(COM3, HIGH); 
      digitalWrite(COM4, LOW);
      break;
    case 4:
      digitalWrite(COM1, LOW);
      digitalWrite(COM2, LOW);
      digitalWrite(COM3, LOW);
      digitalWrite(COM4, HIGH);  
      break;
    default:break;
  }
 
  digitalWrite(d_a, num[n][0]);    
  digitalWrite(d_b, num[n][1]);
  digitalWrite(d_c, num[n][2]);
  digitalWrite(d_d, num[n][3]);
  digitalWrite(d_e, num[n][4]);
  digitalWrite(d_f, num[n][5]);
  digitalWrite(d_g, num[n][6]);
  if(Point == 0)
    digitalWrite(d_h, num[n][7]);  // Display the Point or not
  else
    digitalWrite(d_h, 0);
}
