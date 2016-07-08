 /*
  * classic gameboy intro sound

void soundBegin(){
  arduboy.tunes.tone(987, 160);
  delay(160);
  arduboy.tunes.tone(1318, 400);
  delay(2000);
}
  */
 
 /*
 * lose health
 */
void soundOuch(){
  arduboy.tunes.tone(250, 160);
}

 /*
 * gain health
 */
void soundGood(){
  arduboy.tunes.tone(400, 400);
}

/*
 * skate
 */
void soundSkate(){
  arduboy.tunes.tone(10, 50);
}

/*
 * grind
 */
void soundGrind(){
  arduboy.tunes.tone(25, 25);
}

/*
 * get ready
 */
void soundGetReady(){
  arduboy.tunes.tone(50, 160);
}

/*
 * game over
 */
void soundGameOver(){
  arduboy.tunes.tone(500, 160);
  delay(100);
  arduboy.tunes.tone(300, 160);
  delay(100);
  arduboy.tunes.tone(100, 160);
  delay(100);
  arduboy.tunes.tone(300, 160);
  delay(100);
  arduboy.tunes.tone(500, 160);
  delay(100);
  arduboy.tunes.tone(50, 160);
  delay(250);
  arduboy.tunes.tone(500, 160);
  delay(100);
  arduboy.tunes.tone(300, 160);
  delay(100);
  arduboy.tunes.tone(100, 160);
  delay(100);
  arduboy.tunes.tone(300, 160);
  delay(100);
  arduboy.tunes.tone(500, 160);
  delay(100);
  arduboy.tunes.tone(50, 160);
  delay(1000);
}
