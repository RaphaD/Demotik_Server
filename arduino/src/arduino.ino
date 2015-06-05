//Program constants
int const BAUDRATE = 9600;
int const WAVE_BOUNCE =  30;
int const COMMAND_SIZE = 4;
int const PRIMARY_SIZE = 25;
int const DELAY = 5;

// Pin values
int const SCREEN = 7;
int const SONO = 8;

int const RED_LED = 9;
int const GREEN_LED = 10;
int const BLUE_LED = 11;

boolean isWave = false ;
boolean isWaveIncrementing = false ;
boolean isMusic = false;
unsigned waveValue = 0 ;
long waveTime = 0 ;

int colorValue[3] = {0, 0, 0};
float waveSteps[3] = {0, 0, 0};
float currentColorWave[3] = {0, 0, 0};

int getMessage() ;
void variousD(int buffer) ;
void intensityHandler(int buffer) ;
void redIntensity(int buffer) ;
void greenIntensity(int buffer) ;
void blueIntensity(int buffer) ;
void waveMusicD(int buffer) ;
void waveHandler();
void waveStart();
void waveStop();
void increaseWave();
void decreaseWave();
int roundOf(float f);

/* ***************************************************** */
/*                                                       */
/*      ===========     COMMANDS    ===============      */
/*                                                       */
/* 1 <= command <= 257 : 
/* 258 <= command <= 514 : 
/* 515 <= command <= 771 : 
/* command == 772 : 
/* command == 773 : 
/* command == 774 : 
/* command == 775 : 
/* command == 776 : 
/* command == 777 : 
/* command == 778 : 
/* command == 779 : 
/* command == 780 : 
/*
/* ***************************************************** */

int instruc = 0;

void setup() {
        Serial.begin(BAUDRATE) ;

        pinMode(SCREEN, OUTPUT) ;
        pinMode(SONO, OUTPUT) ;

        digitalWrite(SCREEN, HIGH) ;
        digitalWrite(SONO, HIGH) ;

        analogWrite(RED_LED, 0) ;
        analogWrite(GREEN_LED, 0) ;
        analogWrite(BLUE_LED, 0) ;
}


void loop(){
        int buffer = getMessage() ;
        if (buffer != 0) {
              intensityHandler(buffer) ;
	      variousD(buffer) ;
              waveMusicD(buffer) ;
        }
        if (isWave)
              waveHandler() ;
}

void variousD(int buffer) {
        if (buffer == 777) {
		if (digitalRead(SCREEN) == HIGH )
			digitalWrite(SCREEN, LOW) ;
        } else if (buffer == 778) {
		if (digitalRead(SCREEN) == LOW )
			digitalWrite(SCREEN, HIGH) ;
	} else if (buffer == 779) {
		if (digitalRead(SONO) == HIGH )
			digitalWrite(SONO, LOW) ;
	} else if (buffer == 780) {
		if (digitalRead(SONO) == LOW )
                        digitalWrite(SONO, HIGH) ;
	}
}

void intensityHandler(int buffer) {
        if (buffer >=1 && buffer <= 257)
                redIntensity(buffer) ;
        else if (buffer >= 258 && buffer <=514)
                greenIntensity(buffer) ;
        else if (buffer >= 515 && buffer <=771)
                blueIntensity(buffer) ;
	else if (buffer == 772){
		analogWrite(RED_LED, 0) ;
		analogWrite(GREEN_LED, 0) ;
		analogWrite(BLUE_LED, 0) ;
	}
    if (buffer >=1 && buffer <= 772)
        isWave = false;
}

void redIntensity(int buffer) {
	if (buffer == 256)
		--buffer ;
        analogWrite (RED_LED, (buffer - 1)) ;
        colorValue[0] = (buffer-1);
}

void greenIntensity(int buffer) {
	if (buffer == 514)
		--buffer ;
        analogWrite (GREEN_LED, (buffer - 258) ) ;
        colorValue[1] = (buffer-258);
}

void blueIntensity(int buffer) {
	if (buffer == 771)
		--buffer ;
        analogWrite (BLUE_LED, (buffer - 515) ) ;
        colorValue[2] = (buffer-515);
}

int getMessage() {
        if (Serial.available() > 0){
          delay(DELAY);
          char toConv[COMMAND_SIZE] ;
          int i = 0 ;
          char letter = 'a';
          while (letter != ';') {
                letter = Serial.read() ;
                toConv[i] = letter ;
                ++i ;
          }
          return atoi(toConv) ;
        }
        return 0 ;
}

void waveStart(){
	isMusic = false;
	waveSteps[0] = float(colorValue[0])/255;
	waveSteps[1] = float(colorValue[1])/255;
	waveSteps[2] = float(colorValue[2])/255;
	waveTime = 0;
	isWave = true;
	isWaveIncrementing = true;
}

void waveStop(){
	waveSteps[0] = 0;
	waveSteps[1] = 0;
	waveSteps[2] = 0;
	colorValue[0] = currentColorWave[0];
	colorValue[1] = currentColorWave[1];
	colorValue[2] = currentColorWave[2];
	currentColorWave[0] = 0;
	currentColorWave[1] = 0;
	currentColorWave[2] = 0;
	isWave = false;
	isWaveIncrementing = false;
}

void waveMusicD(int buffer) {
       if (buffer == 773){
                if (! isWave){
			waveStart();
                }
	} else if (buffer == 774) {
		waveStop();
        } else if (buffer == 775) {
                if (! isMusic){
                        isMusic = true ;
                        isWave = false ;
                }
        } else if (buffer == 776) {
		isMusic = false ;
	}
}

void waveHandler() {
        if (millis() - waveTime >= WAVE_BOUNCE) {
                waveTime = millis() ;
                if (isWaveIncrementing) {
                        if (((currentColorWave[0] + waveSteps[0]) > colorValue[0]) ||
				((currentColorWave[1] + waveSteps[1]) > colorValue[1]) ||
					((currentColorWave[2] + waveSteps[2]) > colorValue[2])){
                                isWaveIncrementing = false ;
                                currentColorWave[0] = colorValue[0];
                                currentColorWave[1] = colorValue[1];
                                currentColorWave[2] = colorValue[2];
                                decreaseWave();
                        } else
                                increaseWave();
                } else {
                        if (((currentColorWave[0] - waveSteps[0]) < 0) ||
				((currentColorWave[1] + waveSteps[1]) < 0) ||
					((currentColorWave[2] + waveSteps[2]) < 0)){
                                isWaveIncrementing = true ;
                                currentColorWave[0] = 0;
                                currentColorWave[1] = 0;
                                currentColorWave[2] = 0;
                               	increaseWave();
                        } else
                                decreaseWave();
                }
        }
}

void increaseWave(){
	analogWrite(RED_LED, roundOf(currentColorWave[0] + waveSteps[0]));
	currentColorWave[0] += waveSteps[0];
	analogWrite(GREEN_LED, roundOf(currentColorWave[1] + waveSteps[1]));
	currentColorWave[1] += waveSteps[1];
	analogWrite(BLUE_LED, roundOf(currentColorWave[2] + waveSteps[2]));
	currentColorWave[2] += waveSteps[2];
}

void decreaseWave(){
	analogWrite(RED_LED, roundOf(currentColorWave[0] - waveSteps[0]));
	currentColorWave[0] -= waveSteps[0];
	analogWrite(GREEN_LED, roundOf(currentColorWave[1] - waveSteps[1]));
	currentColorWave[1] -= waveSteps[1];
	analogWrite(BLUE_LED, roundOf(currentColorWave[2] - waveSteps[2]));
	currentColorWave[2] -= waveSteps[2];
}

int roundOf(float f){
	if ((f - int(f)) < 0.5){
		return int(f);
	} else {
		return (int(f) + 1);
	}
}
