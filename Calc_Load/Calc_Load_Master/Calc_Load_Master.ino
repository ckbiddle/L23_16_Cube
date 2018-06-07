/*******************************************************************************
 *
 *  Copyright 2016 Christopher Biddle
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 *
 ******************************************************************************/

#include <Wire.h>

#define MAX_FRAMES               32
#define NUM_COLORS                3    // 0=RED, 1=GREEN, 2=BLUE
#define NUM_PANELS                2
#define NUM_TOTAL_PANELS         16
#define NUM_ROWS                 16
#define NUM_SINE_WAVE_FRAMES     16
#define BYTES_PER_ROW             2
#define FRAME_DURATION          100
#define SLAVE_ADDRESS           100

#define MODE_STANDARD_PLAY        2
#define MODE_RUN_KINETIC_BALL     3
#define MODE_LOAD_MAKERHQ         5
#define MODE_SCROLL_PANEL_BACK    6
#define MODE_SCROLL_RIGHT_LEFT    7
#define MODE_LOAD_HELIX           8
#define MODE_LOAD_BLUE_SINE_WAVE  9
#define MODE_LOAD_PLANES         10
#define MODE_LOAD_RIBBONS        11
#define MODE_LOAD_HORSE          12
#define MODE_LOAD_HOUSE          14   // No number 13. I'm superstitious
#define MODE_LOAD_SOLID_COLOR    15
#define MODE_LOAD_DINER_EN_BLANC 16
#define MODE_LOAD_WHITE_HELIX    17
#define MODE_LOAD_WHITE_WAVES    18
#define MODE_LOAD_JEWEL          19
#define MODE_RUN_WHITE_BALL      20
#define MODE_LOAD_TEL_BOXES      21
#define MODE_LOAD_TREBORDENCO    22
#define MODE_LOAD_IDEAS_ACT      23
#define MODE_LOAD_PNK_RIBBONS    24
#define MODE_LOAD_PNK_HOUSE      25
#define MODE_LOAD_OBRA           26
#define MODE_3D_PONG             27
#define MODE_LOAD_KINGS          28
#define MODE_LOAD_LIGHTWAV       29

const byte interruptReceivePin = 2;
const byte digitalPinToInterrupt = 0;

byte pongMode = B00001011;      // Normal demo mode
byte paddleCoords[2][2] =
  {
    { B00000000, B00000000 },   // Paddle 1 X and Z
    { B00000000, B00000000 }    // Paddle 2 X and Z
  };

float pongBallXcoord = 1.0;
float pongBallYcoord = 1.0;
float pongBallZcoord = 1.0;

float pongBallXvelocity = 0.5;
float pongBallYvelocity = 1.0;
float pongBallZvelocity = 0.5;

int  pongIterations = 0;
byte pongPassIterations = 0;

volatile boolean gotNewData = false;

void setup()
{
  Wire.begin();            // join i2c bus (address optional for master)
  delay( 5 * 1000 );       // Allow other parts of system to catch up
  Serial.begin( 9600 );
  
  pinMode( interruptReceivePin, INPUT_PULLUP );
  // attachInterrupt( digitalPinToInterrupt( interruptReceivePin ), getSerialData, RISING );
  attachInterrupt( digitalPinToInterrupt, flagNewData, RISING );
  
  pinMode( LED_BUILTIN, OUTPUT );
  digitalWrite( LED_BUILTIN, LOW );
}

void loop()
{
  // The boolean gotNewData gets flagged true by the ISR flagNewData().
  // That, in turn, prompts the logic for reading serial data being fed
  // from the controller tied to the Real Sense or joystick component
  // reading the paddle position.
  
  // pongMode = B10000000;  // single player
  // pongMode = B10010000;  // multi-player
  // pongMode = B10100000;  // pong demo
  // pongMode = B10110000;  // normal demo

  if ( gotNewData )
  {
    while ( Serial.available() == 0 );  // idle while waiting for data

    while( Serial.available() > 0 )
    {
      byte inputData = Serial.read();

      parseSerialData( inputData );
    }
    
    gotNewData = false;
  }    

  if ( pongMode != B00001011 )  // If not equal to normal demo mode
  {
    run3Dpong( 50 );
    // run3Dpong( 100 );
      
    pongIterations++;

    // if frame duration is 100ms, then 100 iterations means every 10 seconds.    
    if ( pongIterations == 100 )
    {
      pongIterations = 0;
    }
  }
  else
  {
    if ( !gotNewData || pongMode == B00001011 )
    {
      flashLightWav( 75 );      
    }

    if ( !gotNewData || pongMode == B00001011 )
    {
      loadHelixAnimation();   // load the first half of the buffer with the helix animation
      
      cycleThroughAnimation( 0,   // pStartFrameIndex
                             15,  // pEndFrameIndex,
                             15,  // pAnimationDuration  // how long you want the animation to run in seconds (e.g. 10)
                             100, // pFrameDuration      // how long each frame should persist in milliseconds (e.g. 100)
                             -1   // pIterations         // number of iterations from pStartFrameIndex to pEndFrameIndex (-1 indicates NA)
                            );
    }      

    if ( !gotNewData || pongMode == B00001011 )
    {
      flashLightWav( 75 );      
    }

    if ( !gotNewData || pongMode == B00001011 )
    {    
      loadBlueSineWaves();    // load the second half with the sine wave pattern
      cycleThroughAnimation( 16, 31, 15, 100, -1 );
    }

    if ( !gotNewData || pongMode == B00001011 )
    {
      flashLightWav( 75 );      
    }
    
    if ( !gotNewData || pongMode == B00001011 )
    {
      loadIntersectingPlanes();
      cycleThroughAnimation( 0, 15, 15, 100, -1 );
    }

    if ( !gotNewData || pongMode == B00001011 )
    {
      flashLightWav( 75 );      
    }

    if ( !gotNewData || pongMode == B00001011 )
    {    
      loadSineWaveRibbons();
      cycleThroughAnimation( 16, 31, 15, 100, -1 );
    }

    if ( !gotNewData || pongMode == B00001011 )
    {
      flashLightWav( 75 );      
    }

    if ( !gotNewData || pongMode == B00001011 )
    {    
      runKineticBall( 15, 50 );
    }

    if ( !gotNewData || pongMode == B00001011 )
    {
      flashLightWav( 75 );      
    }

    /*
    if ( !gotNewData && pongMode == B00001011 )
    {
      runRunningMan( 30, 50 );
    }

    if ( !gotNewData && pongMode == B00001011 )
    {
      flashLightWav( 75 );      
    }
    */

    if ( !gotNewData || pongMode == B00001011 )
    {
      loadHorse();
      cycleThroughAnimation( 0, 14, 20, 75, -1 );
    }

    if ( !gotNewData || pongMode == B00001011 )
    {
      flashLightWav( 75 );      
    }

    /*
    if ( !gotNewData && pongMode == B00001011 )
    {
      loadHouse();
      cycleThroughAnimation(  0,  0, 5, 100, -1 );  // display the house in one orientation
      cycleThroughAnimation(  1,  7, 0, 100,  1 );  // rotate 90 degrees
      cycleThroughAnimation(  8,  8, 5, 100, -1 );  // hold
      cycleThroughAnimation(  9, 15, 0, 100,  1 );  // rotate again 90 degrees
      cycleThroughAnimation( 16, 16, 5, 100, -1 );  // hold
      cycleThroughAnimation( 17, 23, 0, 100,  1 );  // rotate again 90 degrees
      cycleThroughAnimation( 24, 24, 5, 100, -1 );  // hold
      cycleThroughAnimation( 25, 31, 0, 100,  1 );  // rotate again 90 degrees
      cycleThroughAnimation(  0,  0, 5, 100, -1 );  // hold
    }

    if ( pongMode == B00001011 )
    {
      flashLightWav( 75 );      
    }
    */

    if ( !gotNewData || pongMode == B00001011 )
    {
      loadTelescopingBoxes();
      cycleThroughAnimation(  0, 27, 15, 75, -1 );  // hold for three seconds
    }

    if ( !gotNewData || pongMode == B00001011 )
    {
      flashLightWav( 75 );      
    }
    
    if ( !gotNewData || pongMode == B00001011 )
    {
      loadDinerEnBlanc();
      cycleThroughAnimation( 0, 0, 10, 100, -1 );    // hold on the static image for 10 seconds
      cycleThroughAnimation( 0, 31, 20, 50, 10 );    // cycle through the frames rotating the Eiffel Tower
    }

    if ( !gotNewData || pongMode == B00001011 )
    {
      flashLightWav( 75 );      
    }
    
    if ( !gotNewData || pongMode == B00001011 )
    {
      loadWhiteHelix();
      cycleThroughAnimation( 0, 31, 30, 50, -1 );    // 30 seconds of the helix
    }

    if ( !gotNewData || pongMode == B00001011 )
    {
      flashLightWav( 75 );      
    }

    if ( !gotNewData || pongMode == B00001011 )
    {
      loadWhiteWaves();
      cycleThroughAnimation( 16, 31, 30, 100, -1 );   // 30 seconds of the waves
    }

    if ( !gotNewData || pongMode == B00001011 )
    {
      flashLightWav( 75 );      
    }

    if ( !gotNewData || pongMode == B00001011 )
    {  
      cleanOutCube();
      runWhiteBall( 30, 25 );
    }

    if ( !gotNewData || pongMode == B00001011 )
    {
      flashLightWav( 75 );      
    }

    if ( !gotNewData || pongMode == B00001011 )
    {  
      loadJewel();
      cycleThroughAnimation(  0,  0,  3, 100, -1 );    //  3 seconds of the jewel holding still
      cycleThroughAnimation(  0, 31, 15, 100, -1 );    // 15 seconds of the jewel rotating
    }
  }
}

void flagNewData()
{
  gotNewData = true;
  digitalWrite( LED_BUILTIN, HIGH );
  delay( 10 );
  digitalWrite( LED_BUILTIN, LOW );
}

void parseSerialData( byte pInputData )
{
  byte dataLabel = pInputData >> 4;
  
  // if the MSB is 1, then the input data is the pong mode
  if ( bitRead( dataLabel, 3 ) == 1 )
  {
    pongMode = dataLabel;
  }
  else if ( dataLabel < 4 )  // if it's a paddle coordinate
  {
    // the four LSBs hold the data value
    byte dataValue = pInputData & B00001111;
    
    byte inx = bitRead( dataLabel, 0 );
    byte jnx = bitRead( dataLabel, 1 );

    paddleCoords[jnx][inx] = dataValue;
  }
}

void loadHelixAnimation()
{
  byte mode = MODE_LOAD_HELIX;

  Wire.beginTransmission( SLAVE_ADDRESS );
  Wire.write( mode );
  Wire.endTransmission();
  
  // delay( 1000 );  // allow a bit of time for the slaves to process
}

void loadBlueSineWaves()
{
  byte mode = MODE_LOAD_BLUE_SINE_WAVE;
  
  Wire.beginTransmission( SLAVE_ADDRESS );
  Wire.write( mode );
  Wire.endTransmission();
  
  // delay( 1000 );  // allow a bit of time for the slaves to process
}

void loadSineWaveRibbons()
{
  byte mode = MODE_LOAD_RIBBONS;
  
  Wire.beginTransmission( SLAVE_ADDRESS );
  Wire.write( mode );
  Wire.endTransmission();
  
  // delay( 1000 );  // allow a bit of time for the slaves to process
}

void loadIntersectingPlanes()
{
  byte mode = MODE_LOAD_PLANES;
  
  Wire.beginTransmission( SLAVE_ADDRESS );
  Wire.write( mode );
  Wire.endTransmission();
  
  // delay( 1000 );  // allow a bit of time for the slaves to process
}

void run3Dpong( int pFrameDurationMillis )
{
  byte mode = MODE_3D_PONG;

  if ( pongBallXcoord >= 14.0 )
  {
    pongBallXvelocity = abs( pongBallXvelocity ) * (-1.0);
  }
  else if ( pongBallXcoord <= 1.0 )
  {
    pongBallXvelocity = abs( pongBallXvelocity );
  }    

  if ( pongBallYcoord >= 14.0 )
  {
    // The paddle at the "back" of the cube (y = 15) is only
    // active in multi-player mode.
    // if ( pongMode == B10010000 )  // If in multi-player mode
    if ( pongMode == B00001001 )
    {
      if ( pongBallHitPaddle( 'B' )) // B for "Back"
      {
        pongBallYvelocity = abs( pongBallYvelocity ) * (-1.0);
      }
      else
      {
        if ( pongPassIterations > 2 )
        {
          delay( 1000 );
          pongBallYcoord = 1.0;
          pongPassIterations = 0;
        }
        else
        {
          pongPassIterations++;
        }
      }
    }
    else
    {
      pongBallYvelocity = abs( pongBallYvelocity ) * (-1.0);
    }    
  }
  else if ( pongBallYcoord <= 1.0 )
  {
    // if ( pongMode == B10000000 || pongMode == B10010000 ) // if in single or multi-player mode
    if ( pongMode == B00001000 || pongMode == B00001001 )
    {
      if ( pongBallHitPaddle( 'F' )) // F for "Front"
      {    
        pongBallYvelocity = abs( pongBallYvelocity );
      }
      else
      {
        if ( pongPassIterations > 2 )
        {
          delay( 1000 );
          pongBallYcoord = 14.0;
          pongPassIterations = 0;
        }
        else
        {
          pongPassIterations++;
        }
      }
    }
    else
    {
      pongBallYvelocity = abs( pongBallYvelocity );
    }
  }
  
  if ( pongBallZcoord >= 14.0 )
  {
    pongBallZvelocity = abs( pongBallZvelocity ) * (-1.0);
  }
  else if ( pongBallZcoord <= 1.0 )
  {
    pongBallZvelocity = abs( pongBallZvelocity );
  }

  pongBallXcoord = pongBallXcoord + pongBallXvelocity;
  pongBallYcoord = pongBallYcoord + pongBallYvelocity;
  pongBallZcoord = pongBallZcoord + pongBallZvelocity;

  Wire.beginTransmission( SLAVE_ADDRESS );
  Wire.write( mode );
  Wire.write((byte) ( pongBallXcoord + 0.5 ));
  Wire.write((byte) ( pongBallYcoord + 0.5 ));
  Wire.write((byte) ( pongBallZcoord + 0.5 ));
  Wire.write( pongMode << 4 );  // pongMode in the slaves is bitshifted to the left (i.e. 11010000 iso 00001101 )
  Wire.write( paddleCoords[0][0] );
  Wire.write( paddleCoords[0][1] );
  Wire.write( paddleCoords[1][0] );
  Wire.write( paddleCoords[1][1] );
  Wire.endTransmission();

  delay( pFrameDurationMillis );
}

boolean pongBallHitPaddle( char pPosition )
{
  // byte pongMode = B10110000;    // Normal demo mode
  // byte paddleCoords[2][2] =
  // {
  //   { B00000000, B00000000 },   // Paddle 1 X and Z (front paddle)
  //   { B00000000, B00000000 }    // Paddle 2 X and Z
  // };
  boolean rCode = false;

  /*
  Serial.print( "pongBallXcoord = " );
  Serial.println( pongBallXcoord );
  Serial.print( "pongBallYcoord = " );
  Serial.println( pongBallYcoord );
  Serial.print( "pongBallZcoord = " );
  Serial.println( pongBallZcoord );
  */
  
  if ( pPosition == 'F' )
  {
    if ( abs( paddleCoords[0][0] - ((byte) ( pongBallXcoord + 0.5 ))) <= 2 )
    {
      if ( abs( paddleCoords[0][1] - ((byte) ( pongBallZcoord + 0.5 ))) <= 2 )
      {
        // Serial.println( "Pong ball hit front paddle!" );
        rCode = true;
      }
    }
  }
  else if ( pPosition == 'B' )
  {
    if ( abs( paddleCoords[1][0] - ((byte) ( pongBallXcoord + 0.5 ))) <= 2 )
    {
      if ( abs( paddleCoords[1][1] - ((byte) ( pongBallZcoord + 0.5 ))) <= 2 )
      {
        // Serial.println( "Pong ball hit back paddle!" );
        rCode = true;
      }
    }
  }  
  
  return rCode;
}

void runKineticBall( int pMovieDurationSeconds,
                     int pFrameDurationMillis
                   )
{
  byte mode = MODE_RUN_KINETIC_BALL;
  float xCoord = 2.0;
  float yCoord = 2.0;
  float zCoord = 2.0;
  float xVelocity = 1.0;
  float yVelocity = 0.5;
  float zVelocity = 0.75;
  int maxIterations = (int) ( pMovieDurationSeconds * 1000 / pFrameDurationMillis );

  for ( int inx = 0; inx < maxIterations; inx++ )
  {
    Wire.beginTransmission( SLAVE_ADDRESS );
    Wire.write( mode );
    Wire.write((byte) ( xCoord + 0.5 ));
    Wire.write((byte) ( yCoord + 0.5 ));
    Wire.write((byte) ( zCoord + 0.5 ));
    Wire.endTransmission();

    if ( xCoord >= 13.0 )
    {
      xVelocity = abs( xVelocity ) * (-1.0);
    }
    else if ( xCoord <= 2.0 )
    {
      xVelocity = abs( xVelocity );
    }    
    
    if ( yCoord >= 13.0 )
    {
      yVelocity = abs( yVelocity ) * (-1.0);      
    }
    else if ( yCoord <= 2.0 )
    {
      yVelocity = abs( yVelocity );      
    }
    
    if ( zCoord >= 13.0 )
    {
      zVelocity = abs( zVelocity ) * (-1.0);
    }
    else if ( zCoord <= 2.0 )
    {
      zVelocity = abs( zVelocity );
    }

    xCoord = xCoord + xVelocity;
    yCoord = yCoord + yVelocity;
    zCoord = zCoord + zVelocity;

    delay( pFrameDurationMillis );
  }  
}

void runRunningMan( int pMovieDurationSeconds,
                    int pFrameDurationMillis
                  )
{
  byte mode = 4;   // load running man

  Wire.beginTransmission( SLAVE_ADDRESS );
  Wire.write( mode );
  Wire.endTransmission();

  delay( 1000 );  // allow a second for array buffer on slaves to load data
  
  int maxIterations = (int) ( pMovieDurationSeconds * 1000 / pFrameDurationMillis );

  mode = 2;    // standard play mode

  byte frameNumber = 0;

  for ( int inx = 0; inx < maxIterations; inx++ )
  {
    if ( frameNumber == MAX_FRAMES )
    {
      frameNumber = 0;
    }

    Wire.beginTransmission( SLAVE_ADDRESS );
    Wire.write( mode );
    Wire.write( frameNumber );
    Wire.endTransmission();
      
    delay( pFrameDurationMillis );      

    frameNumber++;
  }    
}

void flashLightWav( int pFrameDurationMillis )
{
  byte mode = MODE_LOAD_LIGHTWAV;

  Wire.beginTransmission( SLAVE_ADDRESS );
  Wire.write( mode );
  Wire.endTransmission();

  delay( 1000 );  // allow a second for array buffer on slaves to load data
  
  // Start to scroll the "LIGHT.WAV" lettering across the front of
  // the cube from right to left. This will take 80 frames, 64 frames
  // spanning the lettering itself plus 16 blank "follow up" frames to scroll
  // the lettering completely off the cube.

  mode = MODE_SCROLL_RIGHT_LEFT;

  // Setting the panel pair ID to 0 vs. setting it to 1, 2 ... 8 will
  // cause the scrolling logic to happen in all the panel pairs, not just
  // isolated to one panel pair. Initially, I intended to have each panel
  // pair contain the same data to scroll. Then, by specifying the panel
  // pair ID 1, 2, 3 ... 8, you could scroll that lettering across
  // whichever panel pair you choose, but you were limited to scrolling a
  // single panel pair. I later added this 0 as an option to allow the
  // scrolling to occur across all panel pairs. What you want to scroll
  // in each panel pair in that case will depend on what data you store
  // in that panel pair.
  
  byte panelPair = 0;

  for ( byte rtolFrmNbr = 0; rtolFrmNbr < 80; rtolFrmNbr++ )
  {
    Wire.beginTransmission( SLAVE_ADDRESS );
    Wire.write( mode );
    Wire.write( panelPair );
    Wire.write( rtolFrmNbr );
    Wire.endTransmission();

    delay( pFrameDurationMillis );
  }
}

void loadHorse()
{
  byte mode = MODE_LOAD_HORSE;

  Wire.beginTransmission( SLAVE_ADDRESS );
  Wire.write( mode );
  Wire.endTransmission();  
}

void loadHouse()
{
  byte mode = MODE_LOAD_HOUSE;

  Wire.beginTransmission( SLAVE_ADDRESS );
  Wire.write( mode );
  Wire.endTransmission();  
}

void loadTelescopingBoxes()
{
  byte mode = MODE_LOAD_TEL_BOXES;
  
  Wire.beginTransmission( SLAVE_ADDRESS );
  Wire.write( mode );
  Wire.endTransmission();  
}

void loadDinerEnBlanc()
{
  byte mode = MODE_LOAD_DINER_EN_BLANC;  

  Wire.beginTransmission( SLAVE_ADDRESS );
  Wire.write( mode );
  Wire.endTransmission();  
}

void loadWhiteHelix()
{
  byte mode = MODE_LOAD_WHITE_HELIX;  

  Wire.beginTransmission( SLAVE_ADDRESS );
  Wire.write( mode );
  Wire.endTransmission();  
}

void loadWhiteWaves()
{
  byte mode = MODE_LOAD_WHITE_WAVES;  

  Wire.beginTransmission( SLAVE_ADDRESS );
  Wire.write( mode );
  Wire.endTransmission();  
}

void loadJewel()
{
  byte mode = MODE_LOAD_JEWEL;  

  Wire.beginTransmission( SLAVE_ADDRESS );
  Wire.write( mode );
  Wire.endTransmission();  
}

void runWhiteBall( int pMovieDurationSeconds,
                     int pFrameDurationMillis
                   )
{
  byte mode = MODE_RUN_WHITE_BALL;
  float xCoord = 2.0;
  float yCoord = 2.0;
  float zCoord = 2.0;
  float xVelocity = 1.0;
  float yVelocity = 0.5;
  float zVelocity = 0.75;
  int maxIterations = (int) ( pMovieDurationSeconds * 1000 / pFrameDurationMillis );

  for ( int inx = 0; inx < maxIterations; inx++ )
  {
    Wire.beginTransmission( SLAVE_ADDRESS );
    Wire.write( mode );
    Wire.write((byte) ( xCoord + 0.5 ));
    Wire.write((byte) ( yCoord + 0.5 ));
    Wire.write((byte) ( zCoord + 0.5 ));
    Wire.endTransmission();

    if ( xCoord >= 13.0 )
    {
      xVelocity = abs( xVelocity ) * (-1.0);
    }
    else if ( xCoord <= 2.0 )
    {
      xVelocity = abs( xVelocity );
    }    
    
    if ( yCoord >= 13.0 )
    {
      yVelocity = abs( yVelocity ) * (-1.0);      
    }
    else if ( yCoord <= 2.0 )
    {
      yVelocity = abs( yVelocity );      
    }
    
    if ( zCoord >= 13.0 )
    {
      zVelocity = abs( zVelocity ) * (-1.0);
    }
    else if ( zCoord <= 2.0 )
    {
      zVelocity = abs( zVelocity );
    }

    xCoord = xCoord + xVelocity;
    yCoord = yCoord + yVelocity;
    zCoord = zCoord + zVelocity;

    delay( pFrameDurationMillis );
  }  
}

void cleanOutCube()
{
  for ( byte panelPairId = 1; panelPairId <= 8; panelPairId++ )
  {
    for ( byte rowIndex = 0; rowIndex < NUM_ROWS; rowIndex++ )
    {
      sendToPanelPair( panelPairId, 0, 0, 0, rowIndex, B00000000, B00000000 );
      sendToPanelPair( panelPairId, 0, 0, 1, rowIndex, B00000000, B00000000 );
      sendToPanelPair( panelPairId, 0, 1, 0, rowIndex, B00000000, B00000000 );
      sendToPanelPair( panelPairId, 0, 1, 1, rowIndex, B00000000, B00000000 );
      sendToPanelPair( panelPairId, 0, 2, 0, rowIndex, B00000000, B00000000 );
      sendToPanelPair( panelPairId, 0, 2, 1, rowIndex, B00000000, B00000000 );
    }
  }  
}

void sendToPanelPair( byte pPanelPairId,
                      byte pFrameIndex,
                      byte pColorIndex,
                      byte pPanelIndex,
                      byte pRowIndex,
                      byte pData1,
                      byte pData2
                    )
{
  byte mode = 1;

  Wire.beginTransmission( SLAVE_ADDRESS );
  Wire.write( mode );
  Wire.write( pPanelPairId );
  Wire.write( pFrameIndex );
  Wire.write( pColorIndex );
  Wire.write( pPanelIndex );
  Wire.write( pRowIndex );
  Wire.write( pData1 );
  Wire.write( pData2 );
  Wire.endTransmission();
}

void cycleThroughAnimation( byte pStartFrameIndex,
                            byte pEndFrameIndex,
                            int  pAnimationDuration,   // how long you want the animation to run in seconds (e.g. 10) (ignored if pIterations != -1)
                            int  pFrameDuration,       // how long each frame should persist in milliseconds (e.g. 100)
                            int  pIterations           // number of times to cycle between pStartFrameIndex and pEndFrameIndex
                          )
{
  long startTime = millis();
  long endTime = startTime + pAnimationDuration * 1000;
  byte mode = MODE_STANDARD_PLAY;
  byte currentFrameIndex = pStartFrameIndex;

  if ( pIterations == -1 )
  {
    while ( startTime <= endTime )
    {
      Wire.beginTransmission( SLAVE_ADDRESS );
      Wire.write( mode );
      Wire.write( currentFrameIndex );
      Wire.endTransmission();
  
      if ( currentFrameIndex == pEndFrameIndex )
      {
        currentFrameIndex = pStartFrameIndex;
      }
      else
      {
        currentFrameIndex++;
      }
  
      delay( pFrameDuration );
      
      startTime = millis();
    }
  }
  else
  {
    for ( int inx = 0; inx < pIterations; inx++ )
    {    
      for ( byte frameIndex = pStartFrameIndex; frameIndex <= pEndFrameIndex; frameIndex++ )
      {    
        Wire.beginTransmission( SLAVE_ADDRESS );
        Wire.write( mode );
        Wire.write( frameIndex );
        Wire.endTransmission();
    
        delay( pFrameDuration );
      }  
    }    
  }
}
