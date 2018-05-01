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
  // Some code triggered by the ISR will need to go here to change the
  // value of pongMode by looking at the four MSBs of the inbound bytes.
  
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
      
      /*
      byte dataLabel = inputData >> 4;
      
      // if the MSB is 1, then the input data is the pong mode
      if ( bitRead( dataLabel, 3 ) == 1 )
      {
        pongMode = dataLabel;
      }
      else if ( dataLabel < 4 )  // if it's a paddle coordinate
      {
        // the four LSBs hold the data value
        byte dataValue = inputData & B00001111;
        
        byte inx = bitRead( dataLabel, 0 );
        byte jnx = bitRead( dataLabel, 1 );

        paddleCoords[jnx][inx] = dataValue;
      }
      */
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
    if ( !gotNewData )
    {
      loadHelixAnimation();   // load the first half of the buffer with the helix animation
      
      cycleThroughAnimation( 0,   // pStartFrameIndex
                             15,  // pEndFrameIndex,
                             15,  // pAnimationDuration  // how long you want the animation to run in seconds (e.g. 10)
                             100, // pFrameDuration      // how long each frame should persist in milliseconds (e.g. 100)
                             -1   // pIterations         // number of iterations from pStartFrameIndex to pEndFrameIndex (-1 indicates NA)
                            );
    }      
      
    if ( !gotNewData )
    {    
      loadBlueSineWaves();    // load the second half with the sine wave pattern
      cycleThroughAnimation( 16, 31, 15, 100, -1 );
    }

    if ( !gotNewData )
    {
      loadIntersectingPlanes();
      cycleThroughAnimation( 0, 15, 15, 100, -1 );
    }

    if ( !gotNewData )
    {    
      loadSineWaveRibbons();
      cycleThroughAnimation( 16, 31, 15, 100, -1 );
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
