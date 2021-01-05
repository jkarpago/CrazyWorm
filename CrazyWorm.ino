#define PULSE_LENGTH 2000
#include "Serial.h"
ServicePortSerial Serial;

enum Modes {
    NOT_SET, 
    CLOCK,
    INITIAL_BLINK,
    WAITING,
    UNION,
    ALONE,
    SHOWING_CHANGES,
    FINAL_BLINK
};
byte mode = NOT_SET;
int facesvalues[] = { 0 , 0 ,0 ,0 ,0 , 0 };
int initialvalues[] = { 0 , 0 ,0 ,0 ,0 , 0 };
int changepositions[] = {0,0,0};
long resetinitial = 0;
long resetclick = 0;
long starttime = 0;
bool isinit = false;
int minutes = 0;
Timer clocktimer;
int clockstep = -3;
bool clockstarted = false;
bool clockend = false;
Timer changestimer;
int changesavailable = 0;
bool changesselecting = false;

void setup() {    
    //Serial.begin();
    restartgame();
}

void loop() {

    bool isclicked = false;
    int keyBufferColor[] = { 1, 2, 3, 4, 5, 6 };
    int neighbours[] = {};
    int neighbourscnt = 0;
    bool dopaint = false;
    int changevaluesmod = -1;
    bool ischangevaluesmod = false;
    
    if(resetinitial > 0 && resetinitial < millis()){
        resetgame(false);
        setValueSentOnAllFaces(8); 
        resetinitial = 0;
        resetclick = millis() + 400;
    }else if(resetclick > 0 && resetclick < millis()){
        resetclick = 0;
        isclicked = true;
        mode = INITIAL_BLINK;
        isinit = true;
    }
  
    if (buttonSingleClicked()) {
        if(mode == NOT_SET){ 
           if(isAlone()){
              mode = CLOCK;
              setColor(OFF);
              setColorOnFace(getvalueColor(minutes + 1), 0);
           }else{
              if(isinit == false){
                changesavailable++;
                changesselecting = true;
                if(changesavailable == 1){
                   setColor(OFF); 
                }else if(changesavailable > 6){
                   setColor(OFF); 
                   changesavailable = 0;   
                }
                setColorOnFace(BLUE, changesavailable - 1);
              } 
           }
       }else if(mode == CLOCK){
          minutes++;
          if(minutes > 5){
            setColor(OFF);
            minutes = 0;  
          }
          setColorOnFace(getvalueColor(minutes + 1), minutes);
        }else if(mode == INITIAL_BLINK){         

            if(changesavailable > 0){

              int tempfaces[] = {facesvalues[0],facesvalues[1],facesvalues[2],facesvalues[3],facesvalues[4],facesvalues[5]};
              changesavailable--;          
              
              for (int i = 0; i < 3; i++){
                  int actpos = i-1;
                  if(i==0){
                    actpos = 2;
                  }
                  setColorOnFace(getvalueColor(facesvalues[changepositions[actpos]]), changepositions[i]);
                  tempfaces[changepositions[i]] = facesvalues[changepositions[actpos]];
                  initialvalues[changepositions[i]] = tempfaces[changepositions[i]];
                  setValueSentOnFace(facesvalues[changepositions[actpos]] + 10, changepositions[i]);                
              }
  
              for (int i = 0; i < 6; i++){
                facesvalues[i] = tempfaces[i];               
                if(facesvalues[i] == 7){
                    byte options[] = { changesavailable };                      
                    sendDatagramOnFace(&options,sizeof(options),i);
                }
              }    

            }else{
              mode = SHOWING_CHANGES;
              setColor(RED);
              changestimer.set(1000);  
            }
           
        }
    }

    if (buttonDoubleClicked()) {
      if(mode == CLOCK){
        setColor(YELLOW);
        clockstep = -3;
        starttime = millis();
        clocktimer.set(1000);
        clockstarted = true;
      }else{
        resetgame(true);
        setValueSentOnAllFaces(8); 
        resetclick = millis() + 400;
      }
    }

    if (buttonMultiClicked()) {
      if(mode == INITIAL_BLINK){
        mode = SHOWING_CHANGES;
        setColor(OFF);
        for (int i = 0; i < changesavailable; i++){
           setColorOnFace(BLUE, i);
        }
        changestimer.set(1500);
      }
    }

    if(buttonLongPressed()){
      restartgame();
    }

    if(mode == SHOWING_CHANGES){
      if(changestimer.isExpired()){
          mode = INITIAL_BLINK;
          for (int i = 0; i < 6; i++){
              setColorOnFace(getvalueColor(initialvalues[i]), i);
          }
      }
    }else if(mode == CLOCK){
      if(clockstarted == true){
        
        if(clocktimer.isExpired()){
          clockstep++;
          if(clockstep == -2){
            setColor(BLUE);
          }else if(clockstep == -1){
            setColor(RED);
          }else if(clockstep > -1){
            int actualminute = ceil(clockstep / 60);
            int actualsecond = clockstep % (actualminute * 60);

            if(actualminute >= (minutes + 1)){
              clockstarted = false;
              clockend = true;
            }else{
              if(actualsecond == 0){
                setColor(OFF);  
              }
  
              int lighton = actualsecond % 2;
              if(actualsecond % 10 == 0){
                lighton = 1;
              }
  
              if ( lighton == 0){
                setColorOnFace(OFF, ceil(actualsecond / 10));
              }else{
                setColorOnFace(getvalueColor((minutes - actualminute) + 1), ceil(actualsecond / 10));
              }
            }
          }
         
          clocktimer.set(1000);
        }

      }else if(clockend == true){

        int pulseProgress = millis() % PULSE_LENGTH;
        byte pulseMapped = map(pulseProgress, 0, PULSE_LENGTH, 0, 255);      
        byte dimness = sin8_C(pulseMapped);
        
        setColor(makeColorHSB(0, 255, dimness));
        
      }
      
    }else{
      
      if(isAlone()){
        
          setColor(OFF);
  
          if(mode == WAITING){
              resetfaces();
              mode = ALONE;
          }else if(mode == FINAL_BLINK){
              for (int i = 0; i < 6; i++){
                  if(facesvalues[i] > 0){
                      int expandcolor = facesvalues[i];
                      setColorOnFace(getvalueColor(expandcolor), i);                
                  }else{
                      facesvalues[i] = 0;
                  }
              }
          }   
          
      }else{
          
          FOREACH_FACE(f) { //check every face
              
              if (isValueReceivedOnFaceExpired(f)) { //no one there
                   if(mode != FINAL_BLINK && mode != INITIAL_BLINK){ 
                        if(isinit){
                          setColorOnFace(OFF, f);
                        }else if(isinit == false && changesselecting == false){
                          setColorOnFace(GREEN, f);
                        } 
                        
                        facesvalues[f] = 0;
                        setValueSentOnFace(-1, f); 
                    }
              } else { 
  
                  if(isclicked){
                      neighbours[neighbourscnt] = f;
                  }else{
                      bool valuechanged = didValueOnFaceChange(f);
                      int val = getLastValueReceivedOnFace(f);
                      int facevalue = facesvalues[f];
                      if(val == 9 && valuechanged){
                          restartgame();
                      }else if(val == 8 && valuechanged){
                          resetgame(true);
                          setValueSentOnAllFaces(8); 
                      }else if(val > -1 && valuechanged){
                          isinit = true;
                          if(mode == NOT_SET || mode == WAITING){
                              mode = NOT_SET;
                              facesvalues[f] = val + 10;
                              dopaint = true;
                          }else  if(mode == INITIAL_BLINK){
                              if(getLastValueReceivedOnFace(f) > 20){
                                
                              }
                          }else  if(mode == FINAL_BLINK){
                             if(getLastValueReceivedOnFace(f) > 10){
                                  if(val - 10 == facevalue){                                                                        
                                      resetinitial = millis() + 300;                                 
                                      setColor(GREEN);
                                  }else{
                                      setColor(RED);
                                  }
                              }
                          }
                          
                      }else{
                          if(mode == NOT_SET){
                              facesvalues[f] = 7;
                          }else{
                            if ( isDatagramReadyOnFace( f ) ) {
                              const byte *datagramPayload = getDatagramOnFace(f);
                              if(getDatagramLengthOnFace(f) == 1){
                                changesavailable = datagramPayload[0];
                                markDatagramReadOnFace(f); 
                                ischangevaluesmod = true;                               
                              }              
                            }else{
                                changevaluesmod = f;                              
                            } 
                          }    
                      }
                                        
                  }
                  
                  neighbourscnt++;              
  
              }  
                    
          } 
  
      }
  
      if(!isinit){
          if(changesselecting == false){
              setColor(GREEN);
          }
      }else{

          if(ischangevaluesmod && changevaluesmod > -1){
            byte options[] = { changesavailable };                      
            sendDatagramOnFace(&options,sizeof(options),changevaluesmod); 
          }
  
          if(isclicked && mode == INITIAL_BLINK && neighbourscnt == 1){  
              
              int *randcolors = randomizeColors(keyBufferColor);
              
              for (int i = 0; i < 6; i++){  
                  int facecol = randcolors[i];
                  setColorOnFace(getvalueColor(facecol), i);
                  initialvalues[i] = facecol;
                  
                  if(i == neighbours[0]){
                      setValueSentOnFace(randcolors[i], i);                      
      
                      int opositeblink = (i+3) % FACE_COUNT;
                      int left = opositeblink - 1;
                      if(left < 0){
                          left = 5;
                      }
                  
                      int right = opositeblink + 1;
                      if(right > 5){
                          right = 0;
                      }
      
                      facesvalues[opositeblink] = randcolors[opositeblink];
                      facesvalues[left] = randcolors[left];
                      facesvalues[right] = randcolors[right];

                      changepositions[0] = left;
                      changepositions[1] = opositeblink;
                      changepositions[2] = right;
                  
                      byte options[] = { randcolors[opositeblink], randcolors[left], randcolors[right], changesavailable };                      
                      sendDatagramOnFace(&options,sizeof(options),i); 
                  }else{
                    setValueSentOnFace((randcolors[i] + 10), i);         
                  }
              }
              
              
          }else if(mode == NOT_SET && neighbourscnt > 1 && dopaint == true){
      
              mode = WAITING;
              int expandcolor = 0;
              int expandface = 0;
              byte options[] = {0,0,0,0};
              for (int i = 0; i < 6; i++){
                  if(facesvalues[i] > 10){
                      expandcolor = facesvalues[i] - 10;
                      expandface = i;
                      setColorOnFace(getvalueColor(expandcolor), i);
                      if ( isDatagramReadyOnFace( i ) ) {
      
                        const byte *datagramPayload = getDatagramOnFace(i);
                        options[0] = datagramPayload[0];
                        options[1] = datagramPayload[1];
                        options[2] = datagramPayload[2];
                        if(changesavailable == 0 || datagramPayload[3] < changesavailable){
                          options[3] = datagramPayload[3];
                        }else{
                          options[3] = changesavailable;
                        }
                        changesavailable = options[3];
                
                      }
                      break;
                  }
              }
      
      
              for (int i = 0; i < 6; i++){
                  if(facesvalues[i] > 0 && facesvalues[i] < 10){
                      sendDatagramOnFace(&options,sizeof(options),i);
                      setColorOnFace(getvalueColor(expandcolor), i);
                      setValueSentOnFace(expandcolor, i);
                      facesvalues[i] = expandcolor;
                  }
              }
              markDatagramReadOnFace(expandface);
              dopaint = false;
          }else if(mode == NOT_SET && neighbourscnt == 1 && dopaint == true){
                    
              setColor(OFF);
              mode = FINAL_BLINK;
              int opositeblink = 0;
              int expandcolor = 0;
              int expandface = 0;
              byte options[] = {0,0,0};
              for (int i = 0; i < 6; i++){
                  if(facesvalues[i] > 10){
                      expandcolor = facesvalues[i] - 10;
                      expandface = i;
                      setColorOnFace(getvalueColor(expandcolor), i);
                      opositeblink = (i+3) % FACE_COUNT;
                      ////Serial.println("antes de options final");
                      //options = getDatagramOnFace(i);
                      if ( isDatagramReadyOnFace( i ) ) { //received ball
      
                        const byte *datagramPayload = getDatagramOnFace(i);
                        options[0] = datagramPayload[0];
                        options[1] = datagramPayload[1];
                        options[2] = datagramPayload[2];
                        changesavailable = datagramPayload[3];
                
                      }
                  }
              }
            
              int rndcolor = random(2);
              int actualcolor = options[rndcolor];
                     
              facesvalues[opositeblink] = actualcolor;
              setValueSentOnFace(20 + actualcolor, opositeblink);  
              setColorOnFace(getvalueColor(actualcolor), opositeblink);
              dim(getvalueColor(actualcolor), 40);
              markDatagramReadOnFace(expandface);
              dopaint = false;
      
          }

      }

    }
    
}

void restartgame(){
  randomize();
  changesavailable = 0;
  clockstarted = false;
  clockstep = -3;
  clockend = false;
  isinit = false;
  setColor(GREEN);
  resetgame(true);  
  setValueSentOnAllFaces(9);  
}

void resetgame(bool manualreset){
    changesselecting = false;
    resetfaces();
    minutes = 0;
    starttime = 0;
    clockstep = -3;
    mode = NOT_SET;
    if(manualreset == true){
      setColor(OFF);
    } 
}

void resetfaces(){

    for (int i = 0; i < 6; i++){
        facesvalues[i] = 0;
    }

}

Color getvalueColor(int color){
  Color dev = OFF;
  switch (color) {
    case 1:
      dev = RED;
      break;
    case 2:
      dev = ORANGE;
      break;
    case 3:
      dev = GREEN;
      break;
    case 4:
      dev = YELLOW;
      break;
    case 5:
      dev = BLUE;
      break;
    case 6:
      dev = WHITE;
      break;
    default:
      dev = OFF;
      break;
  }
  return dev;
}

int randomizeColors(int colorsdev[]){
  
  for (int i = 0; i < 6; i++) {
          int j = random((6 - 1) - i);     
        
          auto t = colorsdev[i];
          colorsdev[i] = colorsdev[j];
          colorsdev[j] = t;
        }

  return colorsdev;

}
