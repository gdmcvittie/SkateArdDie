/*
 * Grant D. McVittie
 * PixelPlatforms
 */
#include <Arduboy2.h>
#include <ArduboyTones.h>
#include "sprites.h"
#include "bitmaps.h"

Arduboy2 arduboy;
ArduboyTones sound(arduboy.audio.enabled);
Sprites sprites(arduboy.getBuffer());

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
int boss_health = 9;
int boss_x = 86;
int boss_y = 16;
int ball_x = 86;
int ball_y = 46;
int BOSS_HIT = false;
bool BOSS_DOWN = false;
bool BOSS_FIRED = false;
bool BOSS_TIME = false;


//score
int SCORE = 0;
int PRAISE_COUNT = 60;
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
int player_x = 24;
int player_y = 40;
int ollie_y = 8;

//items, goodies, baddies, grindables
int trashcan_x = random(90,126);
int trashcan_y = 39;
int bench_x = random(128,256);
int bench_y = 48;
int rail_x = random(300,400);
int rail_y = 48;

int rock_x = random(300,500);
int rock_y = 48;
int spikes_x = random(600,900);
int spikes_y = 24;
int rock_x2 = random(24,86);
int rock_y2 = random(70,200);
int pilon_x = random(24,86);
int pilon_y = random(70,200);
int spill_x = random(28,75);
int spill_y = random(70,200);
int big_spikes_x = 900;
int big_spikes_y = 8;

int oneup_x = random(600,800);
int oneup_y = 16;
int soda_x = random(400,600);
int soda_y = 16;
int oneup_x2 = random(24,86);
int oneup_y2 = random(180,360);
int soda_x2 = random(24,86);
int soda_y2 = random(120,200);

int platform_x = 36;
int platform_y = 36;
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
  arduboy.boot();
  arduboy.systemButtons();
  arduboy.audio.begin();
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
      if(CURRENT_LEVEL==1){
        arduboy.print("CONTINUE >");
      } else {
        arduboy.print("AGAIN >");     
      }
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
  int hbw = boss_health*8;
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
        arduboy.audio.off();
      } else {
        SOUND_ENABLED = true;  
        arduboy.audio.on();    
      }
      delay(50);
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
      if( arduboy.pressed(B_BUTTON) && (player_y > 0) && (player_y < 32)){
        if(!OLLYING){
          OLLYING = true;
          
          if(!CROUCHING){
            ollie_y = -16;
          } else {
            ollie_y = 0;
          }        
          player_x = 42;
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
    if( arduboy.pressed(B_BUTTON) && (player_y > 0) && (player_y < 32)){
      OLLYING = true;
      if(!CROUCHING){
        ollie_y = -16;
      } else {
        ollie_y = 0;
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
          boss_health = 6;
          CURRENT_LEVEL = 2;
        }else if(CURRENT_LEVEL==2){
           resetGame();   
           delay(500);                   
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
  if(MOVING && arduboy.everyXFrames(10)){
    soundGrind();
  }
  
  int frame = 0;
  if(OLLYING){
    if(ollie_y < player_y){
      ollie_y = ollie_y + 2;      
      frame = 1;
    }
    
    //only in levels, not bosses
    if(GAME_STATE == 2){
      //on trashcan
      if((player_x+16) >= trashcan_x && (player_x+16) < (trashcan_x+16)){
        if((ollie_y+32) >= trashcan_y && (ollie_y+32) < (trashcan_y+16)){
          //riding
          ollie_y = trashcan_y-32;
          frame = 1;
          WHICH_PRAISE = 1;
          if( MOVING && arduboy.everyXFrames(2) ){
            soundGrind();          
            addToScore();
          }        
        }
      }
      
      //on bench
      if((player_x+16) >= bench_x && (player_x+16) < (bench_x+16)){
        if((ollie_y+32) >= bench_y && (ollie_y+32) < (bench_y+8)){
          //riding
          ollie_y = bench_y-30;
          frame = 0;
          WHICH_PRAISE = 2;
          if( MOVING && arduboy.everyXFrames(2) ){
            soundGrind();
            addToScore();
          }
        }
      }
      
      //on rail
      if((player_x+16) >= rail_x && (player_x+16) < (rail_x+48)){
        if((ollie_y+32) >= rail_y && (ollie_y+32) < (rail_y+8)){
          //riding
          ollie_y = rail_y-30;
          frame = 1;
          if( MOVING && arduboy.everyXFrames(2) ){
            soundGrind();
            addToScore();
          }
        }
      }
    }

    //bosses
    if(GAME_STATE==3){
      //on platform
      if((player_x+16) >= platform_x && (player_x+16) < (platform_x+32)){
        if((ollie_y+32) >= platform_y && (ollie_y+32) < (platform_y+8)){
          //on the platform
          ollie_y = platform_y-30;
          ON_PLATFORM = true;
          frame = 0;
        }
      }
    }
    
    //finished ollying
    if(ollie_y >= player_y){
      OLLYING = false;
      ollie_y = player_y;
      frame = 0;
    }

  } else {

    if(GAME_STATE==2){
      if(CURRENT_LEVEL == 1){
        if(player_x>24){
          player_x--;
        }
      }
      
    }
    player_y = 24;
    ollie_y = 24;
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
    if(player_x>xmax){
      player_x = player_x-2;
    }
    if(player_x==xmax){
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
    if(player_x<xmax){
      player_x = player_x+2;
    }
    if(player_x==xmax){
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
        arduboy.drawSlowXYBitmap(player_x,player_y,the_player_top_f1,16,16,BLACK);
      } else if(frame == 1){
        arduboy.drawSlowXYBitmap(player_x,player_y,the_player_top_f2,16,16,BLACK);
      } else if(frame == 2){
        arduboy.drawSlowXYBitmap(player_x,player_y,the_player_top_f3,16,16,BLACK);
      }
    } else {
      if(frame == 0){
        arduboy.drawBitmap(player_x,ollie_y,the_player_f1,32,32,BLACK);
      } else if(frame == 1){
        arduboy.drawBitmap(player_x,ollie_y,the_player_f2,32,32,BLACK);
      } else if(frame == 2){
        arduboy.drawBitmap(player_x,ollie_y,the_player_f3,32,32,BLACK);
      }
    }
    
  } else if(white==true){
    if(CURRENT_LEVEL==2 && GAME_STATE==2){
      if(frame == 0){
        arduboy.drawSlowXYBitmap(player_x,player_y,the_player_top_f1,16,16,WHITE);
      } else if(frame == 1){
        arduboy.drawSlowXYBitmap(player_x,player_y,the_player_top_f2,16,16,WHITE);
      } else if(frame == 2){
        arduboy.drawSlowXYBitmap(player_x,player_y,the_player_top_f3,16,16,WHITE);
      }
    } else {
      if(frame == 0){
        arduboy.drawBitmap(player_x,ollie_y,the_player_f1,32,32,WHITE);
      } else if(frame == 1){
        arduboy.drawBitmap(player_x,ollie_y,the_player_f2,32,32,WHITE);
      } else if(frame == 2){
        arduboy.drawBitmap(player_x,ollie_y,the_player_f3,32,32,WHITE);
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
      if((player_x+32) == rock_x && (player_x+32) < (rock_x+8)){
        if((player_y+32) >= rock_y){
          IS_HIT = true;
          delay(60);
          return;
        }
      }
      //spikes
      if((player_x+32) == spikes_x && (player_x+32) < (spikes_x+24)){
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
        if( rock_y2 <= (player_y+16) && rock_y2 >= player_y){
          if(rock_x2 >= player_x && rock_x2 <= (player_x+16)){
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
        if( pilon_y <= (player_y+16) && pilon_y >= player_y){
          if(pilon_x >= player_x && pilon_x <= (player_x+16)){
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
        if( spill_y <= (player_y+16) && spill_y >= player_y){
          if(spill_x >= player_x && spill_x <= (player_x+16)){
            IS_HIT = true;
            IS_SPILL = true;
            delay(50);
            return;
          }          
        }
        //goodies
        if( soda_y2 <= (player_y+16) && soda_y2 >= player_y){
          if(soda_x2 >= player_x && soda_x2 <= (player_x+16)){
            soda_y2 = 360;
            if(!NEEDS_HEART){
              NEEDS_HEART = true;
              soundGood();
              WHICH_PRAISE = random(0,4);
            }
            addToScore();
            return;
          }          
        }
        if( oneup_y2 <= (player_y+16) && oneup_y2 >= player_y){
          if(oneup_x2 >= player_x && oneup_x2 <= (player_x+16)){
            oneup_y2 = 500;
            if(!NEEDS_LIFE){
              NEEDS_LIFE = true;
              soundGood();
              WHICH_PRAISE = random(0,4);
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
    if((player_x+28) == ball_x && (player_x+28) < (ball_x+8)){
      if((ollie_y+28) >= ball_y){
        IS_HIT = true;
        dingHealth();
        delay(50);
        return;
      }
    }
  
    //boss
    if((player_x+28) == boss_x && (player_x+28) < (boss_x+24)){
      if((ollie_y+28) >= boss_y){
        BOSS_HIT = true;
        soundGood();
        return;
      } else {
        dingHealth();
      }
    }
  }
  

  /*
   * goodies
   */
  //one ups  
  if(CURRENT_LEVEL == 1 && arduboy.everyXFrames(2)){
    if( (player_x+36) >= oneup_x && player_x <= (oneup_x+8)){
      if(ollie_y <= oneup_y){
        oneup_y = 64;
        if(!NEEDS_LIFE){
          NEEDS_LIFE = true;
          soundGood();
          WHICH_PRAISE = 6;
        }
        addToScore();
        return;
      }
    }
  
    //sodas 
    if( (player_x+36) >= soda_x && player_x <= (soda_x+8)){
      if(ollie_y <= soda_y){
        soda_y = 64;
        if(!NEEDS_HEART){
          NEEDS_HEART = true;
          soundGood();
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
    if(rock_x == -8){
      rock_x = random(128,360);
    }
    rock_x--;
  }    
  arduboy.drawSlowXYBitmap(rock_x,rock_y,the_rock,8,8,BLACK);
  return;
}
//rails
void doRails(){
  if(MOVING){
    if(rail_x == -48){
      rail_x = random(400,900);
    }
    rail_x--;
  }
  arduboy.drawBitmap(rail_x,rail_y,the_rail,48,8,BLACK);
  return;
}
//benches
void doBenches(){
  if(MOVING){
    if(bench_x == -16){
     bench_x = random(200,600);
    }
    bench_x--;
  }
  arduboy.drawBitmap(bench_x,bench_y,the_bench,16,8,BLACK);
  return;
}
//trashcans
void doTrashcans(){
  if(MOVING && arduboy.everyXFrames(3) ){
    if(trashcan_x == -16){
      trashcan_x = random(500,800);
    }
    trashcan_x--;
  }    
  arduboy.drawBitmap(trashcan_x,trashcan_y,the_trashcan,16,16,BLACK);
  return;
}
//spikes
void doSpikes(){
  if(MOVING){
    if(spikes_x == -16){
      spikes_x = 600;
    }
    spikes_x--;
  }    
  arduboy.drawSlowXYBitmap(spikes_x,spikes_y,the_three_spikes,24,8,BLACK);
  return;
}

//level 2 rocks
void doLevel2Rocks(){
  if(MOVING_DOWN){
    if(rock_y2 <= -8){
      rock_y2 = random(70,150);
      rock_x2 = random(26,85);
    }
    rock_y2--;
  }    
  arduboy.drawSlowXYBitmap(rock_x2,rock_y2,the_rock,8,8,BLACK); 
  return;
}
//level 2 pilons
void doLevel2Pilons(){
  if(MOVING_DOWN){
    if(pilon_y <= -8){
      pilon_y = random(70,150);
      pilon_x = random(26,85);
    }
    pilon_y--;
  }    
  arduboy.drawSlowXYBitmap(pilon_x,pilon_y,the_pilon,8,8,BLACK); 
  return;
}
//level 2 spills
void doLevel2Spills(){
  if(MOVING_DOWN){
    if(spill_y <= -8){
      spill_y = random(70,150);
      spill_x = random(30,70);
    }
    spill_y--;
  }    
  arduboy.drawSlowXYBitmap(spill_x,spill_y,the_spill,16,16,BLACK); 
  return;
}

//one ups
void doOneUps(){
  if( arduboy.everyXFrames(2) ){
    if(oneup_x == -16){
      oneup_x = random(600,1000);
      oneup_y = random(12,24);
    }
    oneup_x--;
  }    
  arduboy.drawBitmap(oneup_x,oneup_y,the_one_up,8,8,BLACK);
  return;
}
//level 2 one ups
void doLevel2OneUps(){
  if(MOVING_DOWN){
    if( arduboy.everyXFrames(2) ){
      if(oneup_y2 <= -8){
        oneup_y2 = random(64,100);
        oneup_x2 = random(30,70);
      }
      oneup_y2--;
    }
  }    
  arduboy.drawBitmap(oneup_x2,oneup_y2,the_one_up,8,8,BLACK); 
  return;
}
//sodas
void doSodas(){
  if(soda_x == -16){
    soda_x = random(200,400);
    soda_y = random(12,20);
  }
  soda_x--;
  arduboy.drawSlowXYBitmap(soda_x,soda_y,the_soda,8,8,BLACK);
  return;
}
//level 2 sodas
void doLevel2Sodas(){
  if(MOVING_DOWN){
    if(soda_y2 <= -8){
      soda_y2 = random(70,150);
      soda_x2 = random(30,70);
    }
    soda_y2--;
  }    
  arduboy.drawSlowXYBitmap(soda_x2,soda_y2,the_soda,8,8,BLACK); 
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
        if(platform_x == 2){
          PLATFORM_OVER = true;
        }
        if(platform_x == 86){
          PLATFORM_OVER = false;
        }
        if(platform_x>2 && !PLATFORM_OVER){
          platform_x--;
        }        
        if(platform_x < 86 && PLATFORM_OVER){
          platform_x++;
        }
      }
      arduboy.drawSlowXYBitmap(platform_x,platform_y,the_platform,32,8,WHITE);
      //boss
      if(boss_y==8){
        BOSS_DOWN = false;
        boss_y++;
      }
      if(boss_y==36){
        BOSS_DOWN = true;
        if(!BOSS_FIRED){
          BOSS_FIRED = true;
        }
      }
      if(!BOSS_DOWN){
        boss_y++;
      }
      if(BOSS_DOWN){
        boss_y--;      
      }

      if(BOSS_FIRED){
        if(ball_x>-8){
          ball_x--;
          if(CURRENT_LEVEL==2){
            ball_y = boss_y;
          }          
        } else {
          BOSS_FIRED = false;
          ball_x = boss_x+8;
        }
        arduboy.drawBitmap(ball_x,ball_y,the_ball,8,8,WHITE);     
      }
      int bossframe = 0;
      if(BOSS_HIT){
        bossframe = 1;
      }
      if(CURRENT_LEVEL==2 && arduboy.everyXFrames(120)){
        boss_x = random(12,96);     
      }
      sprites.draw(boss_x,boss_y,the_boss1_f1,bossframe,the_boss1_f1_mask,bossframe);
      if(arduboy.everyXFrames(30)){
        if(BOSS_HIT){
          if(boss_health>0){
            boss_health--;
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
    if(oneup_x>128 || oneup_x<0){
      oneup_x = 90;
    }
    if(arduboy.everyXFrames(6)){
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
      dingHealth();
    }
    //add to seconds played
    if(arduboy.everyXFrames(60)){
      SECS_PLAYED++;   
    }
  }
}


/*
 * ding health
 */
 void dingHealth(){
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
      soundOuch();
    }
  }
 }



void bossTime(){
  MOVING = false;
  if(BG_SCROLL>-144){
    BG_SCROLL = BG_SCROLL-4;    
  }else{
    BOSS_TIME = false;
    //get goodies off the screen for boss fight
    trashcan_x = random(90,126);
    bench_x = random(128,256);
    rail_x = random(300,400);
    rock_x = random(300,500);
    spikes_x = random(600,900);
    oneup_x = random(600,800);
    soda_x = random(400,600);
    
    OLLYING = true;
    ollie_y = -16;       
    player_x = 8;
    GAME_STATE = 3;         
  } 
  return;
}


void resetGame(){
  HEARTS = 3;
  LIVES = 3;
  boss_health = 9;
  BOSS_HIT = false;
  BOSS_DOWN = false;
  BOSS_FIRED = false;
  BOSS_TIME = false;
  SCORE = 0;
  OLLYING = false;
  CROUCHING = false;
  player_x = 24;
  player_y = 40;
  ollie_y = 8;
  trashcan_x = random(90,126);
  bench_x = random(128,256);
  rail_x = random(300,400);
  rock_x = random(300,500);
  spikes_x = random(600,900);
  rock_x2 = random(24,86);
  rock_y2 = random(70,200);
  pilon_x = random(24,86);
  pilon_y = random(70,200);
  spill_x = random(28,75);
  spill_y = random(70,200);
  oneup_x = random(600,800);
  soda_x = random(400,600);
  oneup_x2 = random(24,86);
  oneup_y2 = random(180,360);
  soda_x2 = random(24,86);
  soda_y2 = random(120,200);
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



