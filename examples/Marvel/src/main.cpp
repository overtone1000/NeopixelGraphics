#include <LEDGraphics.h>

#define START_DELAY 2000
#define LOOP_DELAY 50
#define SPARKLE_STEP 3

#define FADE_IN_MILLIS 10000
#define FINAL_BRIGHTNESS 100

#define FIRE_FADE_IN_MILLIS 2000

// Clock pin only needed for SPI based chipsets when not using hardware SPI
//#define CLOCK_PIN 8

// This is an array of leds.  One item for each led in your strip.
CRGB leds_left[NUM_LEDS];
CRGB leds_right[NUM_LEDS];
CRGB onboard_led[1];

LEDGraphics::LEDSet2D* left_forward;
LEDGraphics::LEDSet2D* left_backward;

LEDGraphics::LEDSet2D* right_forward;
LEDGraphics::LEDSet2D* right_backward;

LEDGraphics::LEDSet2D* onboard;

LEDGraphics::LEDSet2D* left_sparkles;
LEDGraphics::LEDSet2D* right_sparkles;

// This function sets up the ledsand tells the controller about them
void setup() {
	// sanity check delay - allows reprogramming if accidently blowing power w/leds
  delay(START_DELAY);
  Serial.begin(9600); //This is the default C++ what happens if you delete a member of an arrayhttps://docs.google.com/dCocument/d/1DzxBngHUOFZhpleMMP3qco2PPNh6JigrA3S00HgfX0Q/editefault boot output, so errors can be seen

  Serial.println("Starting setup.");
  
  //pinMode(DATA_PIN, OUTPUT);
  Serial.println("Adding LEDs.");
  FastLED.addLeds<WS2812, DATA_PIN_LEFT, GRB>(leds_left, NUM_LEDS);
  FastLED.addLeds<WS2812, DATA_PIN_RIGHT, GRB>(leds_right, NUM_LEDS);
  FastLED.addLeds<NEOPIXEL, ONBOARD_NEOPIXEL>(onboard_led, 1);
  FastLED.setBrightness(0);
  FastLED.show();

  Serial.println("Creating sparkle arrays.");
  CRGB* left_sparkle_arr[NUM_LEDS];
  CRGB* right_sparkle_arr[NUM_LEDS];
  int i = 0;
  int step = NUM_LEDS/SPARKLE_STEP;
  int wrap_count = 0;
  
  Serial.println("Populating sparkle arrays.");
  for(int n=0;n<NUM_LEDS;n++)
  {
    left_sparkle_arr[n] = leds_left+i;
    right_sparkle_arr[n] = leds_right+i;

    i+=step;
    if(i>=NUM_LEDS)
    {
      wrap_count++;
      i=wrap_count;
    }
  }

  Serial.println("Setting initial LED values.");
  onboard_led[0] = CRGB(2,2,2);
  for(int n=0;n<NUM_LEDS;n++)
  {
    leds_left[n] = CRGB(0,0,0);
    leds_right[n] = CRGB(0,0,0);
  }
  FastLED.show();

  Serial.println("Creating blue LEDSet2Ds.");
  left_forward=new LEDGraphics::LEDSet2D(leds_left,NUM_LEDS,0,NUM_LEDS,false);
  left_backward=new LEDGraphics::LEDSet2D(leds_left,NUM_LEDS,NUM_LEDS,0,true);
  right_forward=new LEDGraphics::LEDSet2D(leds_right,NUM_LEDS,0,NUM_LEDS,false);
  right_backward=new LEDGraphics::LEDSet2D(leds_right,NUM_LEDS,NUM_LEDS,0,true);
  onboard=new LEDGraphics::LEDSet2D(onboard_led,1,0,0,true);
  
  Serial.println("Creating sparkle LEDSet2Ds.");
  left_sparkles=new LEDGraphics::LEDSet2D(left_sparkle_arr,NUM_LEDS);
  right_sparkles=new LEDGraphics::LEDSet2D(right_sparkle_arr,NUM_LEDS);

  left_forward->SetDebugName("Left Forward");
  left_backward->SetDebugName("Right Forward");
  right_forward->SetDebugName("Left Backward");
  right_backward->SetDebugName("Left Backward");
  onboard->SetDebugName("On Board");
  left_sparkles->SetDebugName("Left Sparkles");
  right_sparkles->SetDebugName("Right Sparkles");

  Serial.println("Setting up digital inputs.");
  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);

  Serial.println("Finished setup.");
}

LEDGraphics::Wave slow_half(0.2,NUM_LEDS*2,0.1);
LEDGraphics::Wave med_half(0.49,NUM_LEDS,0.1);
LEDGraphics::Wave fast_third(0.99,NUM_LEDS/2,0.1);

LEDGraphics::Hill sparklewave(10000,0.5,SPARKLE_STEP,NUM_LEDS);

LEDGraphics::AddBrush blue(CRGB::Blue,1.0f);
LEDGraphics::BlendBrush orange(CRGB::DarkOrange,1.0f);
//CRGB background(0,0,5);
CRGB background(0,0,0);

void loop() {
  unsigned long current_time = millis();
  Serial.println("Starting loop at " + (String)current_time);

  if(current_time<FADE_IN_MILLIS)
  {
    uint8_t brightness = (float)FINAL_BRIGHTNESS*(float)current_time/(float)FADE_IN_MILLIS;
    Serial.println("Final brightness is " + (String)brightness);
    FastLED.setBrightness(brightness);
  }

  if(!digitalRead(BUTTON_LEFT))
  {
    Serial.println("Left is pushed.");
  }

  if(!digitalRead(BUTTON_RIGHT))
  {
    Serial.println("Right is pushed.");
  }

  for(int n=0;n<NUM_LEDS;n++)
  {
    leds_left[n] = background;
    leds_right[n] = background;
  }

  slow_half.Paint(current_time, left_forward, &blue);
  slow_half.Paint(current_time, right_forward, &blue);

  med_half.Paint(current_time, left_backward, &blue);
  med_half.Paint(current_time, right_backward, &blue);

  //fast_third.Paint(current_time, left_forward, &blue);
  //fast_third.Paint(current_time, right_forward, &blue);

  sparklewave.Paint(current_time, left_sparkles, &orange);
  sparklewave.Paint(current_time, right_sparkles, &orange);

  FastLED.show();
  delay(LOOP_DELAY);
}
