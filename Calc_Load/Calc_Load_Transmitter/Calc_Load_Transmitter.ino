#define PADDLE_1_XCOORD_PIN A0
#define PADDLE_1_YCOORD_PIN A1
#define PADDLE_1_SWITCH_PIN A2
#define INTERRUPT_SEND_PIN  2
#define PADDLE_SPEED 0.5

float switchVal = 1.0;
float previousSwitchVal = 1.0;

float paddle1RawX = 0.0;
float paddle1RawY = 0.0;
float paddle1CalcX = 7.5;
float paddle1CalcY = 7.5;
byte  paddle1X = 0;
byte  paddle1Y = 0;

byte iteration = 0;

byte playMode = B10110000;  // Initialized to normal demo mode

void setup()
{
  Serial.begin( 9600 );
  
  pinMode( INTERRUPT_SEND_PIN, OUTPUT );
  pinMode( PADDLE_1_XCOORD_PIN, INPUT );
  pinMode( PADDLE_1_YCOORD_PIN, INPUT );
  pinMode( PADDLE_1_SWITCH_PIN, INPUT );
  
  digitalWrite( INTERRUPT_SEND_PIN, LOW );
  
  delay( 3000 );
}

void loop()
{
  switchVal = analogRead( PADDLE_1_SWITCH_PIN );
  paddle1RawX = analogRead( PADDLE_1_XCOORD_PIN );
  paddle1RawY = analogRead( PADDLE_1_YCOORD_PIN );
  
  if ( paddle1RawX > 600.00 )
  {
    if ( paddle1CalcX < 14.9 )
    {
      paddle1CalcX += PADDLE_SPEED;
      paddle1X = round( paddle1CalcX );
    }
  }
  else if ( paddle1RawX < 400.00 )
  {
    if ( paddle1CalcX > 0.1 )
    {
      paddle1CalcX -= PADDLE_SPEED;
      paddle1X = round( paddle1CalcX );
    }
  }    

  if ( paddle1RawY < 400.00 )
  {
    if ( paddle1CalcY < 14.9 )
    {
      paddle1CalcY += PADDLE_SPEED;
      paddle1Y = round( paddle1CalcY );
    }
  }
  else if ( paddle1RawY > 600.00 )
  {
    if ( paddle1CalcY > 0.1 )
    {
      paddle1CalcY -= PADDLE_SPEED;
      paddle1Y = round( paddle1CalcY );
    }
  }    

  if ( switchVal < 0.1 && switchVal > -0.1 )  // if switchVal is "zero"
  {
    if ( previousSwitchVal >= 1.0 )
    {
      if ( playMode == B10110000 ) // if in normal demo mode
      {
        playMode = B10000000; // switch to single-player mode
      }
      else
      {
        playMode = B10110000;  // switch back to normal demo mode
      }
    }
  } 

  previousSwitchVal = switchVal;

  digitalWrite( INTERRUPT_SEND_PIN, HIGH );
  delay( 1 );
  digitalWrite( INTERRUPT_SEND_PIN, LOW );

  Serial.write( playMode );
  Serial.write( B00000000 | paddle1X );  // Paddle 1 X coordinate
  Serial.write( B00010000 | paddle1Y );  // Paddle 1 Y coordinate
  Serial.write( B00100000 );             // Paddle 2 X coordinate (zero for now)
  Serial.write( B00110000 );             // Paddle 2 Y coordinate (zero for now)

//  printByte( playMode ); Serial.print( " | " );
//  printByte( B00000000 | paddle1X ); Serial.print( " | " );
//  printByte( B00010000 | paddle1Y ); Serial.print( " | " );
//  printByte( B00100000 ); Serial.print( " | " );
//  printlnByte( B00110000 );

  delay( 200 );  
}

/*
void loop()
{
  switchVal = analogRead( PADDLE_1_SWITCH_PIN );
  paddle1RawX = analogRead( PADDLE_1_XCOORD_PIN );
  paddle1RawY = analogRead( PADDLE_1_YCOORD_PIN );
  
  if ( paddle1RawX > 600.00 )
  {
    if ( paddle1CalcX < 14.9 )
    {
      paddle1CalcX += PADDLE_SPEED;
      paddle1X = round( paddle1CalcX );
    }
  }
  else if ( paddle1RawX < 400.00 )
  {
    if ( paddle1CalcX > 0.1 )
    {
      paddle1CalcX -= PADDLE_SPEED;
      paddle1X = round( paddle1CalcX );
    }
  }    

  if ( paddle1RawY < 400.00 )
  {
    if ( paddle1CalcY < 14.9 )
    {
      paddle1CalcY += PADDLE_SPEED;
      paddle1Y = round( paddle1CalcY );
    }
  }
  else if ( paddle1RawY > 600.00 )
  {
    if ( paddle1CalcY > 0.1 )
    {
      paddle1CalcY -= PADDLE_SPEED;
      paddle1Y = round( paddle1CalcY );
    }
  }    

  if ( switchVal < 0.1 && switchVal > -0.1 )  // if switchVal is "zero"
  {
    if ( previousSwitchVal >= 1.0 )
    {
      if ( playMode == B10110000 ) // if in normal demo mode
      {
        playMode = B10010000; // switch to single-player mode
      }
      else
      {
        playMode = B10110000;  // switch back to normal demo mode
      }
    }
  } 

  previousSwitchVal = switchVal;

//  Serial.print( "playMode = " );
//  printByte( playMode );
//  Serial.print( ", paddle1X = " );
//  Serial.print( paddle1X );
//  Serial.print( ", paddle1Y = " );
//  Serial.println( paddle1Y );
  
  delay( 250 );  
}
*/

/*
void loop() {

  digitalWrite( INTERRUPT_SEND_PIN, HIGH );
  delay( 1 );
  digitalWrite( INTERRUPT_SEND_PIN, LOW );
    
  // Serial.write( B10010000 );  // multi-player mode
  // Serial.write( B10110000 );  // normal-demo mode
  Serial.write( B10010000 );  // single-player mode

  // for debugging
  // printlnByte ( B10010000 );
  
  for ( byte inx = 0; inx < 4; inx++ )
  {
    byte paddleIdentifier = inx << 4;
    byte paddleCoord = (byte) random( 16 );
    Serial.write( paddleIdentifier | paddleCoord );

    // for debugging    
    // Serial.print( "paddleIdentifier = " );
    // printlnByte( paddleIdentifier );
    // Serial.print( "paddleCoord = " );
    // printByte( paddleCoord );
    // Serial.print( " (" );
    // Serial.print( paddleCoord );
    // Serial.println( ")" );
    // Serial.print( "paddleIdentifier | paddleCoord = " );
    // printlnByte( paddleIdentifier | paddleCoord );
    // Serial.println();
  }

  delay( 3000 );
}
*/

void printByte( byte pData )
{
  for ( byte inx = 0; inx < 8; inx++ )
  {
    if ( bitRead( pData, 7 - inx ) == 0 )
    {
      Serial.print( "0" );
    }
    else
    {
      Serial.print( "1" );
    }  
  }
}

void printlnByte( byte pData )
{
  for ( byte inx = 0; inx < 8; inx++ )
  {
    if ( bitRead( pData, 7 - inx ) == 0 )
    {
      Serial.print( "0" );
    }
    else
    {
      Serial.print( "1" );
    }  
  }

  Serial.println();  
}
