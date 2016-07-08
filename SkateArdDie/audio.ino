const uint16_t gameOver[] PROGMEM = {
  500,100, 300,100, 100,100, 300,100, 500,100, 50,160,
  TONES_END };

 
 /*
  * classic gameboy intro sound

void soundBegin(){
  sound.tone(987, 160);
  delay(160);
  sound.tone(1318, 400);
  delay(2000);
}
  */
 
 /*
 * lose health
 */
void soundOuch(){
  sound.tone(250, 160);
}

 /*
 * gain health
 */
void soundGood(){
  sound.tone(400, 400);
}

/*
 * skate
 */
void soundSkate(){
  sound.tone(25, 25);
}

/*
 * grind
 */
void soundGrind(){
  sound.tone(25, 25);
}

/*
 * get ready
 */
void soundGetReady(){
  sound.tone(50, 160);
}

/*
 * game over
 */
void soundGameOver(){
  sound.tones(gameOver);
  while (sound.playing()) { }
  sound.tones(gameOver);
  while (sound.playing()) { }
  delay(1000);
}
