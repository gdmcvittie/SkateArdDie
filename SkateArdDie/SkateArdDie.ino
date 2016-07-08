#include <ArduboyExtra.h>
#include <physics.h>
#include <sprites.h>

#include <Arduboy.h>

#include "bitmaps.h"

Arduboy arduboy;

#include "audio.h"

Sprites sprites(arduboy);

/*
 * game variables
 */ 
int CURRENT_LEVEL = 0;
int GAME_STATE = 0;
/*
* 0 = intro
* 1 = picker
* 2 = play
* 3 = boss
* 4 = win
* 5 = game over
*/
//health
int HEARTS = 3;
int LIVES = 3;
int DING_EVERY_X_FRAMES = 120;

//boss
int BOSS_HEALTH = 9;
int BOSS_HIT = false;
int BOSS_X = 86;
int BOSS_Y = 16;
int BALL_X = 86;
int BALL_Y = 46;
bool BOSS_DOWN = false;
bool BOSS_FIRED = false;
bool BOSS_TIME = false;


//score
int SCORE = 0;
int PRAISE_COUNT = 300;
bool NEEDS_PRAISE = false;
bool NEEDS_HEART = false;
bool NEEDS_LIFE = false;
int WHICH_PRAISE = 0;

//state
bool OLLYING = false;
bool CROUCHING = false;
bool GOT_CAN = false;
bool IS_HIT = false;
bool IS_SPILL = false;
bool MOVING = false;
bool MOVING_LEFT = false;
bool MOVING_RIGHT = false;
bool MOVING_UP = false;
bool MOVING_DOWN = false;

//positions
int PLAYER_X = 24;
int PLAYER_Y = 40;
int OLLIE_Y = 8;

//items, goodies, baddies, grindables
int TRASHCAN_X = random(90,126);
int TRASHCAN_Y = 39;
int BENCH_X = random(128,256);
int BENCH_Y = 48;
int RAIL_X = random(300,400);
int RAIL_Y = 48;

int ROCK_X = random(300,500);
int ROCK_Y = 48;
int SPIKES_X = random(600,900);
int SPIKES_Y = 24;
int ROCK_X2 = random(24,86);
int ROCK_Y2 = random(70,200);
int PILON_X = random(24,86);
int PILON_Y = random(70,200);
int SPILL_X = random(28,75);
int SPILL_Y = random(70,200);
int BIG_SPIKES_X = 900;
int BIG_SPIKES_Y = 8;

int ONEUP_X = random(600,800);
int ONEUP_Y = 16;
int SODA_X = random(400,600);
int SODA_Y = 16;
int ONEUP_X2 = random(24,86);
int ONEUP_Y2 = random(180,360);
int SODA_X2 = random(24,86);
int SODA_Y2 = random(120,200);

int PLATFORM_X = 36;
int PLATFORM_Y = 36;
bool PLATFORM_OVER = false;

bool ON_PLATFORM = false;

//seconds played
int SECS_PLAYED = 0;
int LEVEL_LENGTH = 45;

//scroll amount for background
int BG_SCROLL = 0;

//sounds
bool SOUND_ENABLED = false;
bool SOUND_PLAYED = false;
 

void setup() {
  arduboy.beginNoLogo();
  arduboy.setFrameRate(60); 
}

void loop() {
  if (!(arduboy.nextFrame()))
    return;

  //clean slate
  arduboy.clear();
  
  //build level
  buildLevel();
}






/*
 * build level
 */
void buildLevel(){
  //intro screen
  if(GAME_STATE == 0){
    arduboy.clear();
    arduboy.drawSlowXYBitmap(0,0,the_intro, 128, 64, WHITE);
    if(SOUND_ENABLED){   
      trace("SOUND ON"); 
      if(!SOUND_PLAYED){
        soundGood();
        SOUND_PLAYED = true;
      }      
    } else {    
      trace("SOUND OFF");
      SOUND_PLAYED = false;
    }
    arduboy.display();
    //handle button inputs
    handleButtons();
    return;
    
  } else {
    
    //playing
    if(GAME_STATE == 2){
      arduboy.clear();

      //level 1
      if(CURRENT_LEVEL == 1){
        //level length
        LEVEL_LENGTH = (DING_EVERY_X_FRAMES * 20) / 60;

        if(BOSS_TIME){
          bossTime();         
        }
        arduboy.drawBitmap(BG_SCROLL,0,the_sky_full, 144, 16, WHITE);        
        arduboy.drawBitmap(BG_SCROLL,8,the_fence_full, 144, 16, WHITE);
        arduboy.drawBitmap(BG_SCROLL,24,the_wall_top_full, 144, 16, WHITE);
        arduboy.drawBitmap(BG_SCROLL,40,the_wall_btm_full, 144, 16, WHITE);
        arduboy.drawBitmap(BG_SCROLL,56,the_brick_full, 144, 8, WHITE);
      }
      
      //level 2
      if(CURRENT_LEVEL == 2){
        //level length
        LEVEL_LENGTH = (DING_EVERY_X_FRAMES * 20) / 90;
        
        arduboy.drawBitmap(0,BG_SCROLL,the_level2_sl, 32, 80, WHITE);        
        arduboy.drawBitmap(96,BG_SCROLL,the_level2_sr, 32, 80, WHITE);
        for(int i=32; i<96; i=i+8){
          for(int j=0; j<64; j=j+8){
            arduboy.drawBitmap(i,j,the_white_bg, 8, 8, WHITE);
          }          
        }

        if(BOSS_TIME){
          bossTime();        
        }
      }
      
      //praise player
      if(NEEDS_PRAISE){
        
        switch(WHICH_PRAISE){
          case 0:
            trace("GNARLY!");
          break;
          case 1:
            trace("DOPE!");
          break;
          case 2:
            trace("SICK, DUDE!");
          break;
          case 3:
            trace("SO FRESH!");
          break;
          case 4:
            trace("NICE!");
          break;
          case 5:
            trace("TASTY!");
          break;
          case 6:
            trace("BONUS!");
          break;
        }
        
        if(arduboy.everyXFrames(PRAISE_COUNT)){
          //add heart
          if(NEEDS_HEART){
            if(HEARTS<3){
              HEARTS++;
            }
          }
          //add life
          if(NEEDS_LIFE){
            if(LIVES<3){
              LIVES++;
              HEARTS = 3;
            }
          }
          NEEDS_LIFE = false;
          NEEDS_HEART = false;
          NEEDS_PRAISE = false;
          WHICH_PRAISE = 0;
        }
      } 
      
      //hurt player
      if(IS_HIT){
        if(IS_SPILL){
          trace("WATCH OUT!");
        } else {
          trace("OUCH!");
        }        
        if(arduboy.everyXFrames(PRAISE_COUNT)){
          //ding the health
          if(HEARTS > 0){
            HEARTS = HEARTS--;
          }
          if(HEARTS == 0){
            if(LIVES == 0){
              //game over
              GAME_STATE = 5;
            } else {
              LIVES--;
              HEARTS = 3;
              //start over
            }
          }          
          IS_HIT = false;
        }
      } 

      //add player
      addPlayer(false);

      //add baddies
      addItems();
      
      //handle button inputs
      handleButtons();

      //hud
      addHud(true,true);      

      //timed death
      timedDeath();

      //check seconds
      checkSecsPlayed();
      
      arduboy.display();
      return;
    }



    //boss
    if(GAME_STATE == 3){
      arduboy.clear();
      //bosses
      doBosses();
      //hud
      addHud(false,true);
      //boss health
      addBossHealthBar();
      //add player
      addPlayer(true);
      //handle button inputs
      handleButtons();      
      arduboy.display();
      return;
    }


    //beat boss, next level
    if(GAME_STATE == 4){
      MOVING = false;
      IS_HIT = false;
      OLLYING = false;
      BG_SCROLL = 0;
      arduboy.setCursor(32,12);
      arduboy.print("SWEET, BRAH!");
      arduboy.setCursor(24,28);
      arduboy.print("SCORE:");
      arduboy.setCursor(62,28);
      arduboy.print(SCORE);
      arduboy.setCursor(68,55);
      arduboy.print("CONTINUE >");
      //handle button inputs
      handleButtons();  
      arduboy.display();
    }


    //game over
    if(GAME_STATE == 5){
      MOVING = false;
      IS_HIT = false;
      OLLYING = false;
      arduboy.drawSlowXYBitmap(48,8,the_game_over,32,32,WHITE);
      arduboy.setCursor(24,50);
      arduboy.print("SCORE:");
      arduboy.setCursor(62,50);
      arduboy.print(SCORE);
      handleButtons();
      arduboy.display();
      if(SOUND_ENABLED){
        if(!SOUND_PLAYED){
          SOUND_PLAYED = true;
          soundGameOver();
        }
      }     
      return;
    }
  }


  
}


/*
 * add hud
 */
void addHud(bool incTime, bool incScore){
  arduboy.fillRect(0,0,128,11,1);
  //add hearts
  for(int i = 0; i < HEARTS; i++){
    int16_t heartx = (119 - (i*10));
    int16_t hearty = 1;
    arduboy.drawSlowXYBitmap(heartx,hearty,the_heart,8,8,BLACK);
  }      
  //add lives
  for(int i = 0; i < LIVES; i++){
    int16_t livesx = (1 + (i*10));
    int16_t livesy = 1;
    arduboy.drawSlowXYBitmap(livesx,livesy,the_lives,8,8,BLACK);
  }

  //add time
  if(incTime){
    arduboy.setCursor(34,1);
    arduboy.print((LEVEL_LENGTH-SECS_PLAYED));
  }
  //add score
  if(incScore){
    arduboy.setCursor(64,1);
    arduboy.print(SCORE);
  }
}


/*
 * boss health bar
 */
void addBossHealthBar(){
  int hbw = BOSS_HEALTH*8;
  int hbx = 127-hbw;
  arduboy.fillRect(hbx,12,hbw,4,1);
}

/*
 * button handler
 */
void handleButtons(){
  /*
   * intro screen
   */
  if(GAME_STATE == 0){
    delay(50);
    //sound
    if(arduboy.pressed(A_BUTTON) ){
      if(SOUND_ENABLED){
        SOUND_ENABLED = false;
      } else {
        SOUND_ENABLED = true;      
      }
    }
    if ( arduboy.pressed(B_BUTTON) ){
      SOUND_PLAYED = false;
      LIVES = 3;
      HEARTS = 3;
      SCORE = 0;
      SECS_PLAYED = 0;
      GAME_STATE = 2;
      CURRENT_LEVEL = 1;  
      delay(50);   
      return;
    }
  }

  /*
   * game play
   */
   if(GAME_STATE == 2){
    //level 1
    if(CURRENT_LEVEL == 1){
      //olly
      if( arduboy.pressed(B_BUTTON) && (PLAYER_Y > 0) && (PLAYER_Y < 32)){
        if(!OLLYING){
          OLLYING = true;
          
          if(!CROUCHING){
            OLLIE_Y = -16;
          } else {
            OLLIE_Y = 0;
          }        
          PLAYER_X = 42;
        }
      }
      //move
      if( arduboy.pressed(RIGHT_BUTTON) ){
        //update scrolling
        BG_SCROLL = BG_SCROLL-2;
        if(BG_SCROLL > -16){
          //?
        } else {
          BG_SCROLL = 0;
        }
        if(BG_SCROLL > 16){
          BG_SCROLL = 0;
        }
        MOVING = true;
      } else {
        MOVING = false;
      }
      //crouch
      if( arduboy.pressed(DOWN_BUTTON) || arduboy.pressed(A_BUTTON) ){
        CROUCHING = true;
      } else {
        CROUCHING = false;
      }
    }


    //level 2
    if(CURRENT_LEVEL == 2){
      //move
      if( arduboy.pressed(B_BUTTON) || arduboy.pressed(RIGHT_BUTTON)){
        MOVING = true;
        MOVING_RIGHT = true;
      } else {
        MOVING_RIGHT = false;
        MOVING = false;
      }

      if( arduboy.pressed(A_BUTTON) || arduboy.pressed(LEFT_BUTTON)){
        MOVING = true;
        MOVING_LEFT = true;
      } else {
        MOVING_LEFT = false;
        MOVING = false;
      }

      
      
      if( arduboy.pressed(DOWN_BUTTON) ){
        //update scrolling
        BG_SCROLL = BG_SCROLL-2;
        if(BG_SCROLL > -16){
          //?
        } else {
          BG_SCROLL = 0;
        }
        if(BG_SCROLL > 16){
          BG_SCROLL = 0;
        }
        MOVING = true;
        MOVING_DOWN = true;
      } else {
        MOVING_DOWN = false;
        MOVING = false;
      }
    }
    

    return;
   }


   //boss
   if(GAME_STATE == 3){
    
    //olly
    if( arduboy.pressed(B_BUTTON) && (PLAYER_Y > 0) && (PLAYER_Y < 32)){
      OLLYING = true;
      if(!CROUCHING){
        OLLIE_Y = -16;
      } else {
        OLLIE_Y = 0;
      }        
    }

    //move
    if( arduboy.pressed(RIGHT_BUTTON) ){
      MOVING_RIGHT = true;
      MOVING_LEFT = false;
    }
    if( arduboy.pressed(LEFT_BUTTON) ){
      MOVING_LEFT = true;
      MOVING_RIGHT = false;
    }

    //crouch
    if( arduboy.pressed(DOWN_BUTTON) || arduboy.pressed(A_BUTTON) ){
      CROUCHING = true;
    } else {
      CROUCHING = false;
    }
    return;
   }


  //win
  if(GAME_STATE == 4){
     if ( arduboy.pressed(A_BUTTON) || arduboy.pressed(B_BUTTON) ){
        LIVES = 3;
        HEARTS = 3;
        OLLYING = false;
        MOVING = false;
        MOVING_DOWN = true;
        delay(50); 
        GAME_STATE = 2;
        SECS_PLAYED = 0;
        if(CURRENT_LEVEL==1){
          BOSS_HEALTH = 6;
          CURRENT_LEVEL = 2;
        }                      
        return;
     }
  }


   //game over
   if(GAME_STATE == 5){
    delay(50);
    if ( arduboy.pressed(A_BUTTON) || arduboy.pressed(B_BUTTON)){
      resetGame();
      delay(500);   
      return;
    }
   }

   return;
}


/*
 * add player
 */
void addPlayer(bool white){
  if(MOVING && arduboy.everyXFrames(10) && SOUND_ENABLED){
    soundSkate();
  }
  
  int frame = 0;
  if(OLLYING){
    if(OLLIE_Y < PLAYER_Y){
      OLLIE_Y = OLLIE_Y + 2;      
      frame = 1;
    }
    
    //only in levels, not bosses
    if(GAME_STATE == 2){
      //on trashcan
      if((PLAYER_X+16) >= TRASHCAN_X && (PLAYER_X+16) < (TRASHCAN_X+16)){
        if((OLLIE_Y+32) >= TRASHCAN_Y && (OLLIE_Y+32) < (TRASHCAN_Y+16)){
          //riding
          OLLIE_Y = TRASHCAN_Y-32;
          frame = 1;
          WHICH_PRAISE = 1;
          if( MOVING && arduboy.everyXFrames(2) ){
            if(SOUND_ENABLED){
              soundGrind();
            }            
            addToScore();
          }        
        }
      }
      
      //on bench
      if((PLAYER_X+16) >= BENCH_X && (PLAYER_X+16) < (BENCH_X+16)){
        if((OLLIE_Y+32) >= BENCH_Y && (OLLIE_Y+32) < (BENCH_Y+8)){
          //riding
          OLLIE_Y = BENCH_Y-30;
          frame = 0;
          WHICH_PRAISE = 2;
          if( MOVING && arduboy.everyXFrames(2) ){
            if(SOUND_ENABLED){
              soundGrind();
            }
            addToScore();
          }
        }
      }
      
      //on rail
      if((PLAYER_X+16) >= RAIL_X && (PLAYER_X+16) < (RAIL_X+48)){
        if((OLLIE_Y+32) >= RAIL_Y && (OLLIE_Y+32) < (RAIL_Y+8)){
          //riding
          OLLIE_Y = RAIL_Y-30;
          frame = 1;
          if( MOVING && arduboy.everyXFrames(2) ){
            if(SOUND_ENABLED){
              soundGrind();
            }
            addToScore();
          }
        }
      }
    }

    //bosses
    if(GAME_STATE==3){
      //on platform
      if((PLAYER_X+16) >= PLATFORM_X && (PLAYER_X+16) < (PLATFORM_X+32)){
        if((OLLIE_Y+32) >= PLATFORM_Y && (OLLIE_Y+32) < (PLATFORM_Y+8)){
          //on the platform
          OLLIE_Y = PLATFORM_Y-30;
          ON_PLATFORM = true;
          frame = 0;
        }
      }
    }
    
    //finished ollying
    if(OLLIE_Y >= PLAYER_Y){
      OLLYING = false;
      OLLIE_Y = PLAYER_Y;
      frame = 0;
    }

  } else {

    if(GAME_STATE==2){
      if(CURRENT_LEVEL == 1){
        if(PLAYER_X>24){
          PLAYER_X--;
        }
      }
      
    }
    PLAYER_Y = 24;
    OLLIE_Y = 24;
    frame = 0;   
  }

  if(CROUCHING){
    frame = 2;
  }

  
  if(MOVING_LEFT){
    int xmax = 2;
    if(CURRENT_LEVEL==2 && GAME_STATE==2){
      xmax = 32;
    }
    if(PLAYER_X>xmax){
      PLAYER_X = PLAYER_X-2;
    }
    if(PLAYER_X==xmax){
      MOVING_LEFT = false;
    }
    if(CURRENT_LEVEL==2){
      frame = 1;
    }
    if(arduboy.everyXFrames(2)){
      MOVING_LEFT = false;
    }
  }

  if(MOVING_RIGHT){
    int xmax = 96;
    if(CURRENT_LEVEL==2 && GAME_STATE==2){
      xmax = 80;
    }
    if(PLAYER_X<xmax){
      PLAYER_X = PLAYER_X+2;
    }
    if(PLAYER_X==xmax){
      MOVING_RIGHT = false;
    }   
    if(CURRENT_LEVEL==2){
      frame = 2;
    } 
    if(arduboy.everyXFrames(2)){
      MOVING_RIGHT = false;
    }
  }

  if(!white){
    if(CURRENT_LEVEL==2 && GAME_STATE==2){
      if(frame == 0){
        arduboy.drawSlowXYBitmap(PLAYER_X,PLAYER_Y,the_player_top_f1,16,16,BLACK);
      } else if(frame == 1){
        arduboy.drawSlowXYBitmap(PLAYER_X,PLAYER_Y,the_player_top_f2,16,16,BLACK);
      } else if(frame == 2){
        arduboy.drawSlowXYBitmap(PLAYER_X,PLAYER_Y,the_player_top_f3,16,16,BLACK);
      }
    } else {
      if(frame == 0){
        arduboy.drawBitmap(PLAYER_X,OLLIE_Y,the_player_f1,32,32,BLACK);
      } else if(frame == 1){
        arduboy.drawBitmap(PLAYER_X,OLLIE_Y,the_player_f2,32,32,BLACK);
      } else if(frame == 2){
        arduboy.drawBitmap(PLAYER_X,OLLIE_Y,the_player_f3,32,32,BLACK);
      }
    }
    
  } else if(white==true){
    if(CURRENT_LEVEL==2 && GAME_STATE==2){
      if(frame == 0){
        arduboy.drawSlowXYBitmap(PLAYER_X,PLAYER_Y,the_player_top_f1,16,16,WHITE);
      } else if(frame == 1){
        arduboy.drawSlowXYBitmap(PLAYER_X,PLAYER_Y,the_player_top_f2,16,16,WHITE);
      } else if(frame == 2){
        arduboy.drawSlowXYBitmap(PLAYER_X,PLAYER_Y,the_player_top_f3,16,16,WHITE);
      }
    } else {
      if(frame == 0){
        arduboy.drawBitmap(PLAYER_X,OLLIE_Y,the_player_f1,32,32,WHITE);
      } else if(frame == 1){
        arduboy.drawBitmap(PLAYER_X,OLLIE_Y,the_player_f2,32,32,WHITE);
      } else if(frame == 2){
        arduboy.drawBitmap(PLAYER_X,OLLIE_Y,the_player_f3,32,32,WHITE);
      }
    }
  }

  collision();
  return;
}

/*
 * collision
 */
void collision(){
  //only level play, not bosses
  if(GAME_STATE==2){
    if(CURRENT_LEVEL==1){
      //rock
      if((PLAYER_X+32) == ROCK_X && (PLAYER_X+32) < (ROCK_X+8)){
        if((PLAYER_Y+32) >= ROCK_Y){
          IS_HIT = true;
          delay(60);
          return;
        }
      }
      //spikes
      if((PLAYER_X+32) == SPIKES_X && (PLAYER_X+32) < (SPIKES_X+24)){
        if(!CROUCHING){
          IS_HIT = true;
          delay(60);
          return;
        } 
      }
    }

    //level 2
    if(CURRENT_LEVEL==2){
      //rock
      if(MOVING_DOWN){
        if( ROCK_Y2 <= (PLAYER_Y+16) && ROCK_Y2 >= PLAYER_Y){
          if(ROCK_X2 >= PLAYER_X && ROCK_X2 <= (PLAYER_X+16)){
            IS_HIT = true;
            if(arduboy.everyXFrames(10)){
              if(HEARTS>0){
                HEARTS--;
              }
            }
            delay(10);
            return;
          }          
        }
        if( PILON_Y <= (PLAYER_Y+16) && PILON_Y >= PLAYER_Y){
          if(PILON_X >= PLAYER_X && PILON_X <= (PLAYER_X+16)){
            IS_HIT = true;
            if(arduboy.everyXFrames(10)){
              if(HEARTS>0){
                HEARTS--;
              }
            }
            
            delay(10);
            return;
          }          
        }
        if( SPILL_Y <= (PLAYER_Y+16) && SPILL_Y >= PLAYER_Y){
          if(SPILL_X >= PLAYER_X && SPILL_X <= (PLAYER_X+16)){
            IS_HIT = true;
            IS_SPILL = true;
            delay(50);
            return;
          }          
        }
        //goodies
        if( SODA_Y2 <= (PLAYER_Y+16) && SODA_Y2 >= PLAYER_Y){
          if(SODA_X2 >= PLAYER_X && SODA_X2 <= (PLAYER_X+16)){
            SODA_Y2 = 360;
            if(!NEEDS_HEART){
              NEEDS_HEART = true;
              if(SOUND_ENABLED){
                soundGood();
              }
              WHICH_PRAISE = random(0,3);
            }
            addToScore();
            return;
          }          
        }
        if( ONEUP_Y2 <= (PLAYER_Y+16) && ONEUP_Y2 >= PLAYER_Y){
          if(ONEUP_X2 >= PLAYER_X && ONEUP_X2 <= (PLAYER_X+16)){
            ONEUP_Y2 = 500;
            if(!NEEDS_LIFE){
              NEEDS_LIFE = true;
              if(SOUND_ENABLED){
                soundGood();
              }
              WHICH_PRAISE = random(0,3);
            }
            addToScore();
            return;
          }          
        }
        
        
      }
      
    }
        
  }

  /*
   * bosses
   */
  //ball
  if(GAME_STATE==3){
    if((PLAYER_X+28) == BALL_X && (PLAYER_X+28) < (BALL_X+8)){
      if((OLLIE_Y+28) >= BALL_Y){
        IS_HIT = true;
        if(HEARTS>0){
          HEARTS--;
        } else {
          if(LIVES>0){
            LIVES--;
            HEARTS = 3;
            if(SOUND_ENABLED){
              soundOuch();
            }
          } else {
            GAME_STATE = 5;//game over
          }
        }
        delay(50);
        return;
      }
    }
  
    //boss
    if((PLAYER_X+28) == BOSS_X && (PLAYER_X+28) < (BOSS_X+24)){
      if((OLLIE_Y+28) >= BOSS_Y){
        BOSS_HIT = true;
        if(SOUND_ENABLED){
          soundGood();
        }
        return;
      } else {
        //take hit
        if(HEARTS>0){
          HEARTS--;
        } else {
          if(LIVES>0){
            LIVES--;
            HEARTS = 3;
            if(SOUND_ENABLED){
              soundOuch();
            }
          } else {
            GAME_STATE = 5;//game over
          }
        }
      }
    }
  }
  

  /*
   * goodies
   */
  //one ups  
  if(CURRENT_LEVEL == 1 && arduboy.everyXFrames(2)){
    if( (PLAYER_X+36) >= ONEUP_X && PLAYER_X <= (ONEUP_X+8)){
      if(OLLIE_Y <= ONEUP_Y){
        ONEUP_Y = 64;
        if(!NEEDS_LIFE){
          NEEDS_LIFE = true;
          if(SOUND_ENABLED){
            soundGood();
          }
          WHICH_PRAISE = 6;
        }
        addToScore();
        return;
      }
    }
  
    //sodas 
    if( (PLAYER_X+36) >= SODA_X && PLAYER_X <= (SODA_X+8)){
      if(OLLIE_Y <= SODA_Y){
        SODA_Y = 64;
        if(!NEEDS_HEART){
          NEEDS_HEART = true;
          if(SOUND_ENABLED){
            soundGood();
          }
          WHICH_PRAISE = 5;
        }
        addToScore();
        return;
      }
    }
  }
  
}

/*
 * add goodies and baddies
 */
void addItems(){
  if(GAME_STATE == 2){
    //level 1
    if(CURRENT_LEVEL == 1){
      //trashcans
      doTrashcans();  
      //benches
      doBenches();
      //rails
      doRails();
      //rocks
      doRocks();
      //do 1ups
      doOneUps();
      //sodas
      doSodas();
      //spikes
      doSpikes();
    }

    //level 2
    if(CURRENT_LEVEL == 2){
      //rocks
      doLevel2Rocks();
      //pilons
      doLevel2Pilons();
      //spills
      doLevel2Spills();
      //do 1ups
      doLevel2OneUps();
      //sodas
      doLevel2Sodas();
    }
  }
  
  
  
}

/*
 * items
 */

 //rocks
void doRocks(){
  if(MOVING){
    if(ROCK_X == -8){
      ROCK_X = random(128,360);
    }
    ROCK_X--;
  }    
  arduboy.drawSlowXYBitmap(ROCK_X,ROCK_Y,the_rock,8,8,BLACK);
  return;
}
//rails
void doRails(){
  if(MOVING){
    if(RAIL_X == -48){
      RAIL_X = random(400,900);
    }
    RAIL_X--;
  }
  arduboy.drawBitmap(RAIL_X,RAIL_Y,the_rail,48,8,BLACK);
  return;
}
//benches
void doBenches(){
  if(MOVING && arduboy.everyXFrames(1) ){
    if(BENCH_X == -16){
      BENCH_X = random(200,600);
    }
    BENCH_X--;
  }
  arduboy.drawBitmap(BENCH_X,BENCH_Y,the_bench,16,8,BLACK);
  return;
}
//trashcans
void doTrashcans(){
  if(MOVING && arduboy.everyXFrames(3) ){
    if(TRASHCAN_X == -16){
      TRASHCAN_X = random(500,800);
    }
    TRASHCAN_X--;
  }    
  arduboy.drawBitmap(TRASHCAN_X,TRASHCAN_Y,the_trashcan,16,16,BLACK);
  return;
}
//spikes
void doSpikes(){
  if(MOVING){
    if(SPIKES_X == -16){
      SPIKES_X = 600;
    }
    SPIKES_X--;
  }    
  arduboy.drawSlowXYBitmap(SPIKES_X,SPIKES_Y,the_three_spikes,24,8,BLACK);
  return;
}

//level 2 rocks
void doLevel2Rocks(){
  if(MOVING_DOWN){
    if(ROCK_Y2 <= -8){
      ROCK_Y2 = random(70,150);
      ROCK_X2 = random(26,85);
    }
    ROCK_Y2--;
  }    
  arduboy.drawSlowXYBitmap(ROCK_X2,ROCK_Y2,the_rock,8,8,BLACK); 
  return;
}
//level 2 pilons
void doLevel2Pilons(){
  if(MOVING_DOWN){
    if(PILON_Y <= -8){
      PILON_Y = random(70,150);
      PILON_X = random(26,85);
    }
    PILON_Y--;
  }    
  arduboy.drawSlowXYBitmap(PILON_X,PILON_Y,the_pilon,8,8,BLACK); 
  return;
}
//level 2 spills
void doLevel2Spills(){
  if(MOVING_DOWN){
    if(SPILL_Y <= -8){
      SPILL_Y = random(70,150);
      SPILL_X = random(30,70);
    }
    SPILL_Y--;
  }    
  arduboy.drawSlowXYBitmap(SPILL_X,SPILL_Y,the_spill,16,16,BLACK); 
  return;
}

//one ups
void doOneUps(){
  if( arduboy.everyXFrames(2) ){
    if(ONEUP_X == -16){
      ONEUP_X = random(600,1000);
      ONEUP_Y = random(12,24);
    }
    ONEUP_X--;
  }    
  arduboy.drawBitmap(ONEUP_X,ONEUP_Y,the_one_up,8,8,BLACK);
  return;
}
//level 2 one ups
void doLevel2OneUps(){
  if(MOVING_DOWN){
    if( arduboy.everyXFrames(2) ){
      if(ONEUP_Y2 <= -8){
        ONEUP_Y2 = random(70,150);
        ONEUP_X2 = random(30,70);
      }
      ONEUP_Y2--;
    }
  }    
  arduboy.drawBitmap(ONEUP_X2,ONEUP_Y2,the_one_up,8,8,BLACK); 
  return;
}
//sodas
void doSodas(){
  if( arduboy.everyXFrames(1) ){
    if(SODA_X == -16){
      SODA_X = random(200,400);
      SODA_Y = random(12,20);
    }
    SODA_X--;
  }    
  arduboy.drawSlowXYBitmap(SODA_X,SODA_Y,the_soda,8,8,BLACK);
  return;
}
//level 2 sodas
void doLevel2Sodas(){
  if(MOVING_DOWN){
    if( arduboy.everyXFrames(1) ){
      if(SODA_Y2 <= -8){
        SODA_Y2 = random(70,150);
        SODA_X2 = random(30,70);
      }
      SODA_Y2--;
    }
  }    
  arduboy.drawSlowXYBitmap(SODA_X2,SODA_Y2,the_soda,8,8,BLACK); 
  return;
}

//bosses
void doBosses(){
  if(GAME_STATE==3){
    //if(CURRENT_LEVEL==1){
      //floor
      for(int i=0; i<128; i=i+16){
        arduboy.drawBitmap(i,53,the_boss1_btm_tile,16,16,WHITE);
      }
      //platform
      if(CURRENT_LEVEL==2 && arduboy.everyXFrames(4) ){
        if(PLATFORM_X == 2){
          PLATFORM_OVER = true;
        }
        if(PLATFORM_X == 86){
          PLATFORM_OVER = false;
        }
        if(PLATFORM_X>2 && !PLATFORM_OVER){
          PLATFORM_X--;
        }        
        if(PLATFORM_X < 86 && PLATFORM_OVER){
          PLATFORM_X++;
        }
      }
      arduboy.drawSlowXYBitmap(PLATFORM_X,PLATFORM_Y,the_platform,32,8,WHITE);
      //boss
      if(BOSS_Y==8){
        BOSS_DOWN = false;
        BOSS_Y++;
      }
      if(BOSS_Y==36){
        BOSS_DOWN = true;
        if(!BOSS_FIRED){
          BOSS_FIRED = true;
        }
      }
      if(!BOSS_DOWN){
        BOSS_Y++;
      }
      if(BOSS_DOWN){
        BOSS_Y--;      
      }

      if(BOSS_FIRED){
        if(BALL_X>-8){
          BALL_X--;
          if(CURRENT_LEVEL==2){
            BALL_Y = BOSS_Y;
          }          
        } else {
          BOSS_FIRED = false;
          BALL_X = BOSS_X+8;
        }
        arduboy.drawBitmap(BALL_X,BALL_Y,the_ball,8,8,WHITE);     
      }
      int bossframe = 0;
      if(BOSS_HIT){
        bossframe = 1;
      }
      if(CURRENT_LEVEL==2 && arduboy.everyXFrames(120)){
        BOSS_X = random(12,96);     
      }
      sprites.draw(BOSS_X,BOSS_Y,the_boss1_f1,bossframe,the_boss1_f1_mask,bossframe);
      if(arduboy.everyXFrames(300)){
        if(BOSS_HIT){
          if(BOSS_HEALTH>0){
            BOSS_HEALTH--;
          } else {
            //win! next level
            GAME_STATE = 4;
            delay(1000);
          }
          BOSS_HIT = false;
        }    
      }
    //}



    
  }
}







/*
 * how long have we played
 */
void checkSecsPlayed(){
  if(LEVEL_LENGTH-SECS_PLAYED<5){
    if(ONEUP_X>128 || ONEUP_X<0){
      ONEUP_X = 90;
    }
    if(SOUND_ENABLED && arduboy.everyXFrames(6)){
      soundGetReady();
    }    
    trace("GET READY!");
  }
  if(LEVEL_LENGTH-SECS_PLAYED <= 0 && HEARTS > 0){
    HEARTS = 3;
    SECS_PLAYED = 0;
    BOSS_TIME = true;
  } else {
    //GAME_STATE = 5;//game over
  }
  return;
}

 /*
  * add to score
  */
void addToScore(){
  SCORE++;
  if(!NEEDS_PRAISE){
    WHICH_PRAISE = 4;
    NEEDS_PRAISE = true;
  }   
  if(SCORE % 100 == 0){
    HEARTS = 3;    
  }
  if(SCORE % 300 == 0){
    if(LIVES<3){
      LIVES++;
    }
  }
}


/*
 * timed death
 */
void timedDeath(){
  if(GAME_STATE == 2){
    if(arduboy.everyXFrames(DING_EVERY_X_FRAMES)){
      //ding the health
      if(HEARTS > 0){
        HEARTS--;
      }
      if(HEARTS == 0){
        if(LIVES == 0){
          //game over
          GAME_STATE = 5;
        } else {
          LIVES--;
          HEARTS = 3;
          if(SOUND_ENABLED){
            soundOuch();
          }
        }
      }
    }
    //add to seconds played
    if(arduboy.everyXFrames(60)){
      SECS_PLAYED++;   
    }
  }
}



void bossTime(){
  MOVING = false;
  if(BG_SCROLL>-144){
    BG_SCROLL = BG_SCROLL-4;    
  }else{
    BOSS_TIME = false;
    GAME_STATE = 3;
    OLLYING = true;
    OLLIE_Y = -16;       
    PLAYER_X = 8;            
  } 
  return;
}


void resetGame(){
  HEARTS = 3;
  LIVES = 3;
  BOSS_HEALTH = 9;
  BOSS_HIT = false;
  BOSS_DOWN = false;
  BOSS_FIRED = false;
  BOSS_TIME = false;
  SCORE = 0;
  OLLYING = false;
  CROUCHING = false;
  PLAYER_X = 24;
  PLAYER_Y = 40;
  OLLIE_Y = 8;
  TRASHCAN_X = random(90,126);
  BENCH_X = random(128,256);
  RAIL_X = random(300,400);
  ROCK_X = random(300,500);
  SPIKES_X = random(600,900);
  ROCK_X2 = random(24,86);
  ROCK_Y2 = random(70,200);
  PILON_X = random(24,86);
  PILON_Y = random(70,200);
  SPILL_X = random(28,75);
  SPILL_Y = random(70,200);
  ONEUP_X = random(600,800);
  SODA_X = random(400,600);
  ONEUP_X2 = random(24,86);
  ONEUP_Y2 = random(180,360);
  SODA_X2 = random(24,86);
  SODA_Y2 = random(120,200);
  SECS_PLAYED = 0;
  BG_SCROLL = 0;
  //game
  GAME_STATE = 0;
  CURRENT_LEVEL = 0;
  return;
}



void trace(String thepraise){
  arduboy.setCursor(0,56);
  arduboy.print(thepraise);
}




