/*
 * Ustawienia Arduino:
 * 
 * Płytka: Arduino/Genuino UNO
 * Port: ...
 * Programator: AVRISP mkII
 * 
 */

#include "ACS712.h"


// wordk_A, work_B - variables shown what status is gateway wing
int work_A = 0;
int work_B = 0;

// time when gate will stop - doesn't matter if sensors alert about high power compsuntion
int czas_bezpieczenstwa = 0;
int max_czas_bezpieczenstwa = 60;

// pin statuses
int state_pin_open = 0;
int state_pin_close = 0;
String status_zasuwki = "NONE";


int pin_otwierajacy = 10;
int pin_zamykajacy = 11;
int pin_zarowka_mrugajaca = 3;
int skrzydlo_A1 = 9;
int skrzydlo_A2 = 8;
int skrzydlo_B1 = 7;
int skrzydlo_B2 = 6;
int zauwka_UP = 5;
int zauwka_DOWN = 4;
int ladowanie_akulumatora = 2;
int nic = 9; //unused now

// analog pins reading ampers that motors are taken
int pin_prad_1 = A4;
int pin_prad_2 = A5;

// connectors to wifi module
int pin_wifi_on = A1;
int pin_wifi_off = A0;
int pin_wifi_door = A2;
int pin_wifi_status = A3;

// class to get power consumption
ACS712 power_A(ACS712_30A, pin_prad_1);
ACS712 power_B(ACS712_30A, pin_prad_2);

// otwieranie_A - 1, when leave A is opening in this moment, 0 when is not active
int otwieranie_A = 0;
// zamykanie_A - 1, when leave A is closing in this moment, 0 when is not active
int zamykanie_A = 0;
// otwieranie_B - 1, when leave B is opening in this moment, 0 when is not active
int otwieranie_B = 0;
// zamykanie_B - 1, when leave B is closing in this moment, 0 when is not active
int zamykanie_B = 0;

// time, show how log gates are working
int czas_A = 0;
int czas_B = 0;

// delay, how long gate B will wait to open after gate A
int opoznienie = 8;
int opoznienie_tmp = 0;

// signal lamp turn on/off - 0 is off
int lampa_wlaczona = 0;
// is he blinking ?
int lampa_blink = 0;


/*
 * This functoion show all parametes to Serial pring
 */
void show()
  {
  // Diagnostic comment, can be removed
  Serial.println("Guzik_Otwieranie="+String(state_pin_open)+" "
                +"Guzik_Zamykanie="+String(state_pin_close)+" "
                +"OtwieranieA="+String(otwieranie_A)+" "
                +"ZamykanieA="+String(zamykanie_A)+" "
                +"OtwieranieB="+String(otwieranie_B)+" "
                +"ZamykanieB="+String(zamykanie_B)+" "
                +"PowerA="+String(power_A.getCurrentDC())+" "
                +"PowerB="+String(power_B.getCurrentDC())+" "
                +"Wifi_ON="+String(analogRead(pin_wifi_on))+" "
                +"Wifi_OFF="+String(analogRead(pin_wifi_off))+" "
                +"Wifi_Door="+String(analogRead(pin_wifi_door))+" "
                +"Czas bezp.="+String(czas_bezpieczenstwa)+" "                
                );
  }

/*
 * Set status for WiFi connector to OPEN
 */
void set_gate_status_open()
  {
    analogWrite(pin_wifi_status, 500);    
  }


/*
 * Set status for WiFi connector to CLOSE
 */
void set_gate_status_close()
  {
    analogWrite(pin_wifi_status, 0);    
  }


/*
 * Main setup
 */
void setup() 
  {
      // initalize Serial
      Serial.begin(9600);
    
      // Setting pins at the begining

      pinMode(pin_wifi_status, OUTPUT);
      set_gate_status_open();
    
      pinMode(pin_otwierajacy, OUTPUT);
      pinMode(pin_zamykajacy, OUTPUT);
      digitalWrite(pin_otwierajacy, LOW);
      digitalWrite(pin_zamykajacy, LOW);  
      
      //pinMode(pin_otwierajacy, INPUT);
      //pinMode(pin_zamykajacy, INPUT);
    
      pinMode(skrzydlo_A1, OUTPUT);
      pinMode(skrzydlo_A2, OUTPUT);
      pinMode(skrzydlo_B1, OUTPUT);
      pinMode(skrzydlo_B2, OUTPUT);

      pinMode(pin_wifi_on, INPUT);
      pinMode(pin_wifi_off, INPUT);
      pinMode(pin_wifi_door, INPUT);
    
      pinMode(zauwka_UP, OUTPUT);
      pinMode(zauwka_DOWN, OUTPUT);
      pinMode(ladowanie_akulumatora, OUTPUT);
      pinMode(nic, OUTPUT);
      
      pinMode(pin_zarowka_mrugajaca, OUTPUT); // lamp
    
      digitalWrite(skrzydlo_A1, HIGH);
      digitalWrite(skrzydlo_A2, HIGH);
      digitalWrite(skrzydlo_B1, HIGH);
      digitalWrite(skrzydlo_B2, HIGH);
    
      digitalWrite(ladowanie_akulumatora, HIGH);
      digitalWrite(pin_zarowka_mrugajaca, HIGH);
      digitalWrite(zauwka_UP, HIGH);
      digitalWrite(zauwka_DOWN, HIGH);
      digitalWrite(nic, HIGH);
  }


/*
 * This function will open the central down lock
 * This was made because in my place there is a strone wind sometimes,
 * so I have to have central lock digged in the middle of two gates
 */
void zasuwka_go_up()
  {
    if(status_zasuwki != "UP")
      {
      status_zasuwki = "UP";
      digitalWrite(zauwka_DOWN, HIGH);
      digitalWrite(zauwka_UP, LOW);  
      delay(500);
      digitalWrite(zauwka_UP, HIGH);  
      }
  }

/*
 * This function will close the central down lock
 * This was made because in my place there is a strone wind sometimes,
 * so I have to have central lock digged in the middle of two gates
 */
void zasuwka_go_down()
  {
    if(status_zasuwki == "UP")
        {
          digitalWrite(zauwka_UP, HIGH);
          digitalWrite(zauwka_DOWN, LOW);
          delay(1000);
          digitalWrite(zauwka_DOWN, HIGH);
        }  
     status_zasuwki = "DOWN";        
  }
  

/*
 * funtion turning on lamp
 * remove the comment for blinking, elsewere this will only shines
 * this is removed because to the lapm now is connected to the staple for blocking
 * closed gate leaves
 */
void wlacz_lampe()
  {      
      czas_bezpieczenstwa++;
      zasuwka_go_up();
      lampa_wlaczona = 1;
      if(lampa_blink == 0)
          {
            lampa_blink = 1;
            digitalWrite(pin_zarowka_mrugajaca, LOW);
          }
      else
          {
            lampa_blink = 0;
            digitalWrite(pin_zarowka_mrugajaca, HIGH);
          }
      delay(500);
      digitalWrite(pin_zarowka_mrugajaca, HIGH);
  }




/* 
 * tufn off the lamp 
 */
void wylacz_lampe()
  {   
      zasuwka_go_down();
      lampa_wlaczona = 0;
      digitalWrite(pin_zarowka_mrugajaca, HIGH);
  }



/* 
 * stop working leave A 
 */
void stop_A() 
  {
      czas_A = 0;
      work_A = 0;
      otwieranie_A = 0;
      zamykanie_A = 0;
      digitalWrite(skrzydlo_A1, HIGH);
      digitalWrite(skrzydlo_A2, HIGH);
      show();
  }


/* 
 * stop working leave B 
 */
void stop_B() 
  {
     czas_B = 0;
     work_B = 0;
     otwieranie_B = 0;
     zamykanie_B = 0;
     digitalWrite(skrzydlo_B1, HIGH);
     digitalWrite(skrzydlo_B2, HIGH);
     show();
  }



/* 
 * start working leave A 
 */
void otworz_A() 
  {      
      set_gate_status_open();
      otwieranie_A = 1;
      zamykanie_A = 0;
      work_A = 1;
      digitalWrite(skrzydlo_A1, LOW);
      digitalWrite(skrzydlo_A2, HIGH);
  }



/* 
 * stop leave A 
 */
void zamknij_A() 
  {
      set_gate_status_close();
      otwieranie_A = 0;
      zamykanie_A = 1;
      work_A = 1;
      digitalWrite(skrzydlo_A1, HIGH);
      digitalWrite(skrzydlo_A2, LOW);
  }



/* 
 * start leave B 
 */
void otworz_B() 
  {
      set_gate_status_open();
      otwieranie_B = 1;
      zamykanie_B = 0;
      work_B = 1;
      digitalWrite(skrzydlo_B1, LOW);
      digitalWrite(skrzydlo_B2, HIGH);
  }



/* 
 * stop leave B 
 */
void zamknij_B() 
  {
      set_gate_status_close();
      otwieranie_B = 0;
      zamykanie_B = 1;
      work_B = 1;
      digitalWrite(skrzydlo_B1, HIGH);
      digitalWrite(skrzydlo_B2, LOW);
  }



/*
 * check status, maybe leaves are closed enough,
 * if they are, they will consume more power
 * so we should stop them
 */
void sprawdz_status() 
  {
      czas_A++;
      czas_B++;
      show();
      if(power_A.getCurrentDC() < -5 || power_A.getCurrentDC() > 2)
        {
           stop_A();
        }
      if(power_B.getCurrentDC() < -5 || power_B.getCurrentDC() > 2)
        {
           stop_B();
        }

      if(czas_bezpieczenstwa > max_czas_bezpieczenstwa)
        {
            stop_A();
            stop_B();
        }
}



/*
 * Main loop
 */
void loop() 
  {
    state_pin_open = digitalRead(pin_otwierajacy);
    state_pin_close = digitalRead(pin_zamykajacy);

    // wlaczanie przez wifi
    if(analogRead(pin_wifi_on) > 600) state_pin_open = 1;
    if(analogRead(pin_wifi_off) > 600) state_pin_close = 1;
    
        
    if(work_A == 0 && work_B == 0)
      {
          // nic nie dziala
          opoznienie_tmp = 0;
    
          if(state_pin_open == 1)
            {
              // otwieranie bo bylo zamkniete
              czas_bezpieczenstwa = 0;
              wlacz_lampe();
              otworz_B();
            }
            
        if(state_pin_close == 1)
          {
              // zamykanie bo bylo zamkniete
              czas_bezpieczenstwa = 0;
              wlacz_lampe();
              zamknij_A();
          }
      } 
      else
      {
          sprawdz_status();
          opoznienie_tmp++;
          if(opoznienie_tmp >= opoznienie)
            {
              if(otwieranie_B == 1)
                  {
                  otworz_A();
                  }
              if(zamykanie_A == 1)
                  {
                  zamknij_B();
                  }
            }
    
        if((otwieranie_A == 1 || otwieranie_B == 1) && state_pin_close == 1)
          {
              stop_A();
              stop_B();
              wylacz_lampe();
          }
          
        if((zamykanie_A == 1 || zamykanie_B == 1) && state_pin_open == 1)
          {
              stop_A();
              stop_B();
              wylacz_lampe();
          }
          
      wlacz_lampe();
      }
    
    
    if(otwieranie_A == 0 && otwieranie_B == 0 && zamykanie_A == 0 && zamykanie_B == 0)
      {
        wylacz_lampe();
      }
    
    show();
    delay(100);

  }
