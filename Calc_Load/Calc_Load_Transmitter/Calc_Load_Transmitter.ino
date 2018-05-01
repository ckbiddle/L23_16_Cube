const byte interruptSendPin = 2;

void setup() {
  Serial.begin( 9600 );
  pinMode( interruptSendPin, OUTPUT );
  digitalWrite( interruptSendPin, LOW );
  
  delay( 5000 );
}

void loop() {

  digitalWrite( interruptSendPin, HIGH );
  delay( 1 );
  digitalWrite( interruptSendPin, LOW );
    
  // Serial.write( B10010000 );  // multi-player mode
  Serial.write( B10110000 );

  // for debugging
  // printlnByte ( B10010000 );
  
  for ( byte inx = 0; inx < 4; inx++ )
  {
    byte paddleIdentifier = inx << 4;
    byte paddleCoord = (byte) random( 16 );
    Serial.write( paddleIdentifier | paddleCoord );

    /*
    // for debugging    
    Serial.print( "paddleIdentifier = " );
    printlnByte( paddleIdentifier );
    Serial.print( "paddleCoord = " );
    printByte( paddleCoord );
    Serial.print( " (" );
    Serial.print( paddleCoord );
    Serial.println( ")" );
    Serial.print( "paddleIdentifier | paddleCoord = " );
    printlnByte( paddleIdentifier | paddleCoord );
    Serial.println();
    */
  }

  delay( 3000 );
}

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
