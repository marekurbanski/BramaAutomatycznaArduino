// wordk_A, work_B - variables shown what status is gateway wing
int work_A = 0;
int work_B = 0;

// pin statuses
int state_pin_open = 0;
int state_pin_close = 0;


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

// defalut values to consumption power in regular work
int power_A = 500;
int power_B = 500;

// delay, how long gate B will wait to open after gate A
int opoznienie = 20;
int opoznienie_tmp = 0;

// signal lamp turn on/off - 0 is off
int lampa_wlaczona = 0;
// is he blinking ?
int lampa_blink = 0;


void show()
  {

// Diagnostic comment, can be removed
Serial.println("Guzik_Otwieranie="+String(state_pin_open)+" "
              +"Guzik_Zamykanie="+String(state_pin_close)+" "
              +"OtwieranieA="+String(otwieranie_A)+" "
              +"ZamykanieA="+String(zamykanie_A)+" "
              +"OtwieranieB="+String(otwieranie_B)+" "
              +"ZamykanieB="+String(zamykanie_B)+" "
              +"PowerA="+String(power_A)+" "
              +"PowerB="+String(power_B)+" "
              );

  }

void setup() {

  // initalize Serial
  Serial.begin(9600);

  // Setting pins at the begining
  pinMode(11, INPUT);
  pinMode(12, INPUT);

  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);

  pinMode(13, OUTPUT); // lamp

  digitalWrite(7, HIGH);
  digitalWrite(8, HIGH);
  digitalWrite(9, HIGH);
  digitalWrite(10, HIGH);

  digitalWrite(13, LOW);


}

// funtion turning on lamp
// remove the comment for blinking, elsewere this will only shines
// this is removed because to the lapm now is connected to the staple for blocking
// closed gate leaves
void wlacz_lampe()
  {
    lampa_wlaczona = 1;
    /*
    if(lampa_blink == 0)
      {
      lampa_blink = 1;
      digitalWrite(13, LOW);
      }
    else
      {
      lampa_blink = 0;
      digitalWrite(13, HIGH);
      }
    //delay(500);
    */
    digitalWrite(13, HIGH);
}

// tufn off the lamp
void wylacz_lampe()
  {
      lampa_wlaczona = 0;
      digitalWrite(13, LOW);
  }

// stop working leave A
void stop_A() {
    czas_A = 0;
    work_A = 0;
    otwieranie_A = 0;
    zamykanie_A = 0;
    digitalWrite(7, HIGH);
    digitalWrite(8, HIGH);
    show();
}

// stop working leave B
void stop_B() {
    czas_B = 0;
    work_B = 0;
    otwieranie_B = 0;
    zamykanie_B = 0;
    digitalWrite(9, HIGH);
    digitalWrite(10, HIGH);
    show();
}

// start working leave A
void otworz_A() {
      otwieranie_A = 1;
      zamykanie_A = 0;
      work_A = 1;
      digitalWrite(7, LOW);
      digitalWrite(8, HIGH);

  }


// stop leave A
void zamknij_A() {
      otwieranie_A = 0;
      zamykanie_A = 1;
      work_A = 1;
      digitalWrite(7, HIGH);
      digitalWrite(8, LOW);
  }

// start leave B
void otworz_B() {
      otwieranie_B = 1;
      zamykanie_B = 0;
      work_B = 1;
      digitalWrite(9, LOW);
      digitalWrite(10, HIGH);

  }

// stop leave B
void zamknij_B() {
      otwieranie_B = 0;
      zamykanie_B = 1;
      work_B = 1;
      digitalWrite(9, HIGH);
      digitalWrite(10, LOW);
  }


// check status, maybe leaves are closed enough,
// if they are, they will consume more power
// so we should stop them
void sprawdz_status() {

   power_A = analogRead(A0);
   power_B = analogRead(A1);

  czas_A++;
  czas_B++;
  if(power_A < 430 || power_A > 600)
    {
    stop_A();
    }
  if(power_B < 430 || power_B > 600)
    {
    stop_B();
    }


}

void loop() {


state_pin_open = digitalRead(11);
state_pin_close = digitalRead(12);


if(work_A == 0 && work_B == 0)
  {
    // nic nie dziala
    opoznienie_tmp = 0;

    if(state_pin_open == 1)
      {
        // otwieranie bo bylo zamkniete
        wlacz_lampe();
        otworz_B();
      }
    if(state_pin_close == 1)
      {
        // otwieranie bo bylo zamkniete
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
      }
    if((zamykanie_A == 1 || zamykanie_B == 1) && state_pin_open == 1)
      {
      stop_A();
      stop_B();
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
