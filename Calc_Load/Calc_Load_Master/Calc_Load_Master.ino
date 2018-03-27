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

void setup()
{
  Wire.begin(); // join i2c bus (address optional for master)
  delay( 5 * 1000 );  // Allow other parts of system to catch up
}

void loop()
{
  run3Dpong( 15, 25 );
  flashKings( 100 );

  // flashMakerHQ( 100 );   // frame duration
  // flashObra( 75 );

  loadHelixAnimation();   // load the first half of the buffer with the helix animation
  loadBlueSineWaves();    // load the second half with the sine wave pattern

  // Cycle through the animations. Tried using a time period (long startTime = millis(), etc.)
  // but didn't work for some reason. Could have been a memory issue when I was using the temporary array.
  // Now that I'm not using it, maybe the time thing will work.

  cycleThroughAnimation( 0,   // pStartFrameIndex
                         15,  // pEndFrameIndex,
                         15,  // pAnimationDuration  // how long you want the animation to run in seconds (e.g. 10)
                         100, // pFrameDuration      // how long each frame should persist in milliseconds (e.g. 100)
                         -1   // pIterations         // number of iterations from pStartFrameIndex to pEndFrameIndex (-1 indicates NA)
                        );
                          
  cycleThroughAnimation( 16, 31, 15, 100, -1 );

  flashKings( 100 );

  loadIntersectingPlanes();
  loadSineWaveRibbons();

  // loadSineWaveRibbonsPinkWhite();

  // Cycle through the animations.
  cycleThroughAnimation( 0, 15, 15, 100, -1 );
  cycleThroughAnimation( 16, 31, 15, 100, -1 );

  flashKings( 100 );

  // cleanOutCube();
  runKineticBall( 15, 50 );

  runRunningMan( 30, 50 );

  loadHorse();

  cycleThroughAnimation( 0, 14, 20, 75, -1 );

  flashKings( 100 );

  loadHouse();
  // loadHousePinkWhite();

  cycleThroughAnimation(  0,  0, 15, 100, -1 ); // display the house in one orientation
  cycleThroughAnimation(  1,  7, 0, 100,  1 );  // rotate 90 degrees
  cycleThroughAnimation(  8,  8, 5, 100, -1 );  // hold
  cycleThroughAnimation(  9, 15, 0, 100,  1 );  // rotate again 90 degrees
  cycleThroughAnimation( 16, 16, 5, 100, -1 );  // hold
  cycleThroughAnimation( 17, 23, 0, 100,  1 );  // rotate again 90 degrees
  cycleThroughAnimation( 24, 24, 5, 100, -1 );  // hold
  cycleThroughAnimation( 25, 31, 0, 100,  1 );  // rotate again 90 degrees
  cycleThroughAnimation(  0,  0, 5, 100, -1 );  // hold

  loadTelescopingBoxes();
  cycleThroughAnimation(  0, 27, 15, 75, -1 );  // hold for three seconds

  flashKings( 100 );

  loadDinerEnBlanc();
  cycleThroughAnimation( 0, 0, 10, 100, -1 );    // hold on the static image for 10 seconds
  cycleThroughAnimation( 0, 31, 20, 50, 10 );    // cycle through the frames rotating the Eiffel Tower
  
  loadWhiteHelix();
  cycleThroughAnimation( 0, 31, 30, 50, -1 );    // 30 seconds of the helix

  loadWhiteWaves();
  cycleThroughAnimation( 16, 31, 30, 100, -1 );   // 30 seconds of the waves

  flashKings( 100 );

  // cleanOutCube();
  runWhiteBall( 30, 25 );

  loadJewel();

  cycleThroughAnimation(  0,  0,  3, 100, -1 );    //  3 seconds of the jewel holding still
  cycleThroughAnimation(  0, 31, 15, 100, -1 );    // 15 seconds of the jewel rotating

  // flashTreBordenCo( 75 );
  // flashIdeasActivated( 75 );

  /*
  cleanOutCube();

  // load indicated color into frame 0
  // 1=RED, 2=GREEN, 3=BLUE, 4=YELLOW (1&2), 5=CYAN (2&3), 6=MAGENTA (1&3), 7=WHITE (1&2&3)
  loadSolidColor( 2 );

  cycleThroughAnimation( 0,  0, 15, 100, -1 );  // display frame 0 for 15 seconds
  */
  
  // lather, rinse, repeat ...
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

void loadSineWaveRibbonsPinkWhite()
{
  byte mode = MODE_LOAD_PNK_RIBBONS;
  
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

void loadHousePinkWhite()
{
  byte mode = MODE_LOAD_PNK_HOUSE;

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
  
void loadSolidColor( byte pColor )
{
  byte mode = MODE_LOAD_SOLID_COLOR;  
  
  Wire.beginTransmission( SLAVE_ADDRESS );
  Wire.write( mode );
  Wire.write( pColor );
  Wire.endTransmission();  
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

void run3Dpong( int pMovieDurationSeconds,
                int pFrameDurationMillis
              )
{
  byte mode = MODE_3D_PONG;
  float xCoord = 1.0;
  float yCoord = 1.0;
  float zCoord = 1.0;
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

    if ( xCoord >= 14.0 )
    {
      xVelocity = abs( xVelocity ) * (-1.0);
    }
    else if ( xCoord <= 1.0 )
    {
      xVelocity = abs( xVelocity );
    }    
    
    if ( yCoord >= 14.0 )
    {
      yVelocity = abs( yVelocity ) * (-1.0);      
    }
    else if ( yCoord <= 1.0 )
    {
      yVelocity = abs( yVelocity );      
    }
    
    if ( zCoord >= 14.0 )
    {
      zVelocity = abs( zVelocity ) * (-1.0);
    }
    else if ( zCoord <= 1.0 )
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

void flashMakerHQ( int pFrameDurationMillis )
{
  byte mode = MODE_LOAD_MAKERHQ;

  Wire.beginTransmission( SLAVE_ADDRESS );
  Wire.write( mode );
  Wire.endTransmission();

  delay( 1000 );  // allow a second for array buffer on slaves to load data
  
  mode = MODE_SCROLL_PANEL_BACK;

  for ( byte logoFrameNumber = 0; logoFrameNumber < MAX_FRAMES; logoFrameNumber++ )
  {
    Wire.beginTransmission( SLAVE_ADDRESS );
    Wire.write( mode );
    Wire.write( logoFrameNumber );
    Wire.endTransmission();

    if ( logoFrameNumber == 15 )
    {
      // At frame 15, the logo is at the back of the cube. Now
      // we start to scroll the "MakerHQ" lettering across the
      // front of the cube from right to left. This will take
      // 72 frames, 56 frames spanning the lettering itself
      // plus 16 blank "follow up" frames to scroll the
      // lettering completely off the cube.

      mode = MODE_SCROLL_RIGHT_LEFT;
      byte panelPair = 4;  // Scroll across panel pair four

      for ( byte rtolFrmNbr = 0; rtolFrmNbr < 72; rtolFrmNbr++ )
      {
        Wire.beginTransmission( SLAVE_ADDRESS );
        Wire.write( mode );
        Wire.write( panelPair );
        Wire.write( rtolFrmNbr );
        Wire.endTransmission();

        delay( pFrameDurationMillis );
      }
    }
    else
    {
      delay( pFrameDurationMillis );
    }    
  }
}

void flashKings( int pFrameDurationMillis )
{
  byte mode = MODE_LOAD_KINGS;

  Wire.beginTransmission( SLAVE_ADDRESS );
  Wire.write( mode );
  Wire.endTransmission();

  delay( 1000 );  // allow a second for array buffer on slaves to load data
  
  mode = MODE_SCROLL_PANEL_BACK;

  // for ( byte logoFrameNumber = 0; logoFrameNumber < MAX_FRAMES; logoFrameNumber++ )
  for ( byte logoFrameNumber = 0; logoFrameNumber < MAX_FRAMES - 4; logoFrameNumber++ )
  {
    Wire.beginTransmission( SLAVE_ADDRESS );
    Wire.write( mode );
    Wire.write( logoFrameNumber );
    Wire.endTransmission();

    // if ( logoFrameNumber == 15 )
    if ( logoFrameNumber == 13 )
    {
      // At frame 15, the logo is at the back of the cube. Now
      // we start to scroll the "KINGS" lettering across the
      // front of the cube from right to left. This will take
      // 64 frames, 48 frames spanning the lettering itself
      // plus 16 blank "follow up" frames to scroll the
      // lettering completely off the cube.

      mode = MODE_SCROLL_RIGHT_LEFT;
      
      byte panelPair = 4;   // Scroll across panel pair four

      for ( byte rtolFrmNbr = 0; rtolFrmNbr < 64; rtolFrmNbr++ )
      {
        Wire.beginTransmission( SLAVE_ADDRESS );
        Wire.write( mode );
        Wire.write( panelPair );
        Wire.write( rtolFrmNbr );
        Wire.endTransmission();

        delay( pFrameDurationMillis );
      }
    }
    else
    {
      delay( pFrameDurationMillis );
    }    
  }
}

void flashTreBordenCo( int pFrameDurationMillis )
{
  byte mode = MODE_LOAD_TREBORDENCO;

  Wire.beginTransmission( SLAVE_ADDRESS );
  Wire.write( mode );
  Wire.endTransmission();

  delay( 1000 );  // allow a second for array buffer on slaves to load data
  
  // Start to scroll the "TRE BORDEN / CO" lettering across the front of
  // the cube from right to left. This will take 174 frames, 154 frames
  // spanning the lettering itself, 4 frames for the white background
  // bordering the lettering, and 16 blank "follow up" frames to scroll the
  // image completely off the cube.

  mode = MODE_SCROLL_RIGHT_LEFT;
  
  byte panelPair = 4;  // Scroll across panel pair four

  for ( byte rtolFrmNbr = 0; rtolFrmNbr < 174; rtolFrmNbr++ )
  {
    Wire.beginTransmission( SLAVE_ADDRESS );
    Wire.write( mode );
    Wire.write( panelPair );
    Wire.write( rtolFrmNbr );
    Wire.endTransmission();

    delay( pFrameDurationMillis );
  }
}

void flashIdeasActivated( int pFrameDurationMillis )
{
  byte mode = MODE_LOAD_IDEAS_ACT;

  Wire.beginTransmission( SLAVE_ADDRESS );
  Wire.write( mode );
  Wire.endTransmission();

  delay( 1000 );  // allow a second for array buffer on slaves to load data
  
  // Start to scroll the "IDEAS ACTIVATED" lettering across the front of
  // the cube from right to left. This will take 169 frames, 153 frames
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

  for ( byte rtolFrmNbr = 0; rtolFrmNbr < 169; rtolFrmNbr++ )
  {
    Wire.beginTransmission( SLAVE_ADDRESS );
    Wire.write( mode );
    Wire.write( panelPair );
    Wire.write( rtolFrmNbr );
    Wire.endTransmission();

    delay( pFrameDurationMillis );
  }
}

void flashObra( int pFrameDurationMillis )
{
  byte mode = MODE_LOAD_OBRA;

  Wire.beginTransmission( SLAVE_ADDRESS );
  Wire.write( mode );
  Wire.endTransmission();

  delay( 1000 );  // allow a second for array buffer on slaves to load data
  
  // Start to scroll the "OBRA" lettering across the front of the cube from
  // right to left. This will take 64 frames, 48 frames spanning the
  // lettering itself and 16 blank "follow up" frames to scroll the image
  // completely off the cube.

  mode = MODE_SCROLL_RIGHT_LEFT;
  
  byte panelPair = 4;  // Scroll across panel pair four

  for ( byte rtolFrmNbr = 0; rtolFrmNbr < 64; rtolFrmNbr++ )
  {
    Wire.beginTransmission( SLAVE_ADDRESS );
    Wire.write( mode );
    Wire.write( panelPair );
    Wire.write( rtolFrmNbr );
    Wire.endTransmission();

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

  // run the cleaned out frame 0 for a couple of seconds to clean out any lights in the cube.  
  cycleThroughAnimation( 0,   // pStartFrameIndex
                         0,   // pEndFrameIndex,
                         2,   // pAnimationDuration   // how long you want the animation to run in seconds (e.g. 10)
                         100, // pFrameDuration       // how long each frame should persist in milliseconds (e.g. 100)
                         -1   // pIterations          // number of iterations from pStartFrameIndex to pEndFrameIndex (-1 indicates NA)
                        );
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
