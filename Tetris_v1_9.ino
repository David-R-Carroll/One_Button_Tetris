#include <M5StickCPlus.h>
#include <M5Display.h>

/**************************************
TETRIS by Alexey Pajitnov

Ported tothe M5Stickc Plus by David Carroll.

***************************************/

TFT_eSprite fieldSprite = TFT_eSprite(&M5.Lcd);
TFT_eSprite scoreSprite = TFT_eSprite(&M5.Lcd);
TFT_eSprite splashSprite = TFT_eSprite(&M5.Lcd);

const bool tetrisPieces[][4][16] = { // 4x4 definition of the Tetris blocks.
  {
    // Blank (0) Block
    {
      0, 0, 0, 0,
      0, 0, 0, 0,
      0, 0, 0, 0,
      0, 0, 0, 0 },
    { 0, 0, 0, 0,
      0, 0, 0, 0,
      0, 0, 0, 0,
      0, 0, 0, 0 },
    { 0, 0, 0, 0,
      0, 0, 0, 0,
      0, 0, 0, 0,
      0, 0, 0, 0 },
    { 0, 0, 0, 0,
      0, 0, 0, 0,
      0, 0, 0, 0,
      0, 0, 0, 0 },
  },
  {
    // J Block
    {
      1, 0, 0, 0,
      1, 1, 1, 0,
      0, 0, 0, 0,
      0, 0, 0, 0 },
    { 1, 1, 0, 0,
      1, 0, 0, 0,
      1, 0, 0, 0,
      0, 0, 0, 0 },
    { 1, 1, 1, 0,
      0, 0, 1, 0,
      0, 0, 0, 0,
      0, 0, 0, 0 },
    { 0, 1, 0, 0,
      0, 1, 0, 0,
      1, 1, 0, 0,
      0, 0, 0, 0 },
  },
  {
    // L Block
    {
      0, 0, 1, 0,
      1, 1, 1, 0,
      0, 0, 0, 0,
      0, 0, 0, 0 },
    { 1, 0, 0, 0,
      1, 0, 0, 0,
      1, 1, 0, 0,
      0, 0, 0, 0 },
    { 1, 1, 1, 0,
      1, 0, 0, 0,
      0, 0, 0, 0,
      0, 0, 0, 0 },
    { 1, 1, 0, 0,
      0, 1, 0, 0,
      0, 1, 0, 0,
      0, 0, 0, 0 },
  },
  {
    // O Block
    {
      1, 1, 0, 0,
      1, 1, 0, 0,
      0, 0, 0, 0,
      0, 0, 0, 0 },
    { 1, 1, 0, 0,
      1, 1, 0, 0,
      0, 0, 0, 0,
      0, 0, 0, 0 },
    { 1, 1, 0, 0,
      1, 1, 0, 0,
      0, 0, 0, 0,
      0, 0, 0, 0 },
    { 1, 1, 0, 0,
      1, 1, 0, 0,
      0, 0, 0, 0,
      0, 0, 0, 0 },
  },
  {
    // S Block
    {
      0, 1, 1, 0,
      1, 1, 0, 0,
      0, 0, 0, 0,
      0, 0, 0, 0 },
    { 1, 0, 0, 0,
      1, 1, 0, 0,
      0, 1, 0, 0,
      0, 0, 0, 0 },
    { 0, 1, 1, 0,
      1, 1, 0, 0,
      0, 0, 0, 0,
      0, 0, 0, 0 },
    { 1, 0, 0, 0,
      1, 1, 0, 0,
      0, 1, 0, 0,
      0, 0, 0, 0 },
  },
  {
    // T Block
    {
      0, 1, 0, 0,
      1, 1, 1, 0,
      0, 0, 0, 0,
      0, 0, 0, 0 },
    { 1, 0, 0, 0,
      1, 1, 0, 0,
      1, 0, 0, 0,
      0, 0, 0, 0 },
    { 1, 1, 1, 0,
      0, 1, 0, 0,
      0, 0, 0, 0,
      0, 0, 0, 0 },
    { 0, 1, 0, 0,
      1, 1, 0, 0,
      0, 1, 0, 0,
      0, 0, 0, 0 },
  },
  {
    // Z Block
    {
      1, 1, 0, 0,
      0, 1, 1, 0,
      0, 0, 0, 0,
      0, 0, 0, 0 },
    { 0, 1, 0, 0,
      1, 1, 0, 0,
      1, 0, 0, 0,
      0, 0, 0, 0 },
    { 1, 1, 0, 0,
      0, 1, 1, 0,
      0, 0, 0, 0,
      0, 0, 0, 0 },
    { 0, 1, 0, 0,
      1, 1, 0, 0,
      1, 0, 0, 0,
      0, 0, 0, 0 },
  }
};

int blockGrid[20][7]; // Tracks the location of the current block.
int backgroundGrid[20][7]; // Tracks the location of all previous blocks.

const uint16_t blockColor[8] // Block colors (matches block number in tetrisPieces)
  // Black, Cyan,   Blue, Orange, Yellow,  Green, Magenta, Red
  { 0x0000, 0x07FF, 0x001F, 0xFD20, 0xFFE0, 0x07E0, 0xF81F, 0xF800 };


int blockXPos = 5; // Starting position of a block (far left).
int blockYPos = 0; // Starting position of a block (top) 
int blockRotation = 0;
int block; // Current block shape.
int score = 0; // Current score.
bool gameInProgress = false;

bool collision;  //True if a block runs into another block.


bool drawBlock(int block, int rotation, int x, int y) {
  int i;
  int j;
  bool collided = false;
  
    // If the block is on it's side and in the left column move it one over.
  if (rotation == 1 || rotation == 3) {
    if (y == 5) {
      blockXPos = 4;
      y = 4;
    }
  }

  // Check if this block is going to overlap an existing block.
  // Or if the block is at the bottom ((x + i) >= 10)).
  for (i = 0; i < 4; i++) {
    for (j = 0; j < 4; j++) {
      if (tetrisPieces[block][rotation][i + j * 4] == 1) {
        if (backgroundGrid[x + i][y + j] != 0 || (x + i) >= 14) {
          collided = true;
        }
      }
    }
  }

  // If there is no collision, then clear the block grid, and draw the block.
  if (!collided) {
    for (i = 0; i < 20; i++) {
      for (j = 0; j < 7; j++) {
        blockGrid[i][j] = 0;
      }
    }

    for (i = 0; i < 4; i++) {
      for (j = 0; j < 4; j++) {
        if (tetrisPieces[block][rotation][i + j * 4] == 1) {
          blockGrid[x + i][y + j] = block;
        }
      }
    }
  }
  return collided;
}


// Copy the block to the background after a collision.
void copyBlockToBackground() {
  int i;
  int j;
  for (i = 0; i < 20; i++) {
    for (j = 0; j < 7; j++) {
      if (blockGrid[i][j] != 0) {
        backgroundGrid[i][j] = blockGrid[i][j];
      }
    }
  }
}


// Scan backgroundGrid for full rows
// Remove the line and drop the blocks above.
// Increment the score.
void dropLine() {
  int i;
  int j;
  int k;
  int l;
  int blockCount;
  
  for (i = 14; i >=0; i--) {
    blockCount = 0;
    for (j = 0; j < 7; j++){
      if (backgroundGrid[i][j] > 0 || blockGrid[i][j] > 0) {
        blockCount++;
      }
    }

    if (blockCount > 6) { // full row
      score++; // Increment the score.  
      for (k = i; k >=0; k--) {  // Move blocks above the line down one row.
        for (l = 0; l < 7; l++) {
          backgroundGrid[k][l] = backgroundGrid[k-1][l];
          blockGrid[k][l] = blockGrid[k-1][l];
        }
      }
      for (l = 0; l < 7; l++) {
        backgroundGrid[0][l] = 0;
      }
    drawGrid();
    }
  }
}


// Draw the current block, the existing blocks and the score to the screen.
void drawGrid() {
  int i;
  int j;
  String scoreText;


  fieldSprite.fillSprite(TFT_BLACK);  // Clear all the blocks and draw the borders. 
  fieldSprite.fillRect(0, 0, 240, 20, TFT_NAVY);
  fieldSprite.fillRect(0, 110, 240, 20, TFT_NAVY);

  scoreSprite.fillSprite(TFT_MAROON); // Draw the score.
  scoreSprite.setTextColor(0xE739); // Light Yellow 
  scoreSprite.setTextSize(2);
  scoreText = "SCORE:";
  scoreText.concat(String(score));
  scoreSprite.drawString(scoreText,20,0);


  
  for (i = 0; i < 20; i++) { // Draw the current block.
    for (j = 0; j < 7; j++) {
      fieldSprite.fillRect(i * (16), (j+1) * (14), 16, 14, blockColor[blockGrid[i][j]]);
      fieldSprite.drawRect(i * (16), (j+1) * (14), 16, 14, TFT_BLACK);
    }
  }

  for (i = 0; i < 20; i++) {  // Draw the existing blocks.
    for (j = 0; j < 7; j++) {
      if (backgroundGrid[i][j] != 0) {
        fieldSprite.fillRect(i * (16), (j+1) * (14), 16, 14, blockColor[backgroundGrid[i][j]]);
        fieldSprite.drawRect(i * (16), (j+1) * (14), 16, 14, TFT_BLACK);
      }
    }
  }
  M5.Lcd.setRotation(1); // Update the screen.
  fieldSprite.pushSprite(20, 0);

  M5.Lcd.setRotation(0);
  scoreSprite.pushSprite(0, 0);
}


// Choose a random block and prevent choosing the same block twice.
int randomBlock(int currentBlock){
  int newBlock;
  newBlock = random(1, 7);
  while (newBlock == currentBlock) { // Don't use the same block twice.
    newBlock = random(1, 7);
  }
  return newBlock;
}


// Check the button state.
int checkButton() {
  int i;
  int j;
  static int currentButton;
  static int clickTime;
  static int upClickTime;
  static int isUp;
  static int isDown;
  static bool wasUp;
  static bool wasDown;
  static bool shortClick = false; 
  static bool firstClick = false; 
  int clickType = 0;
 
  if (digitalRead(39) == 0) {  //Down. Clear the field for new game
    blockXPos = 5;
    blockYPos = 0;
    blockRotation = 0;
    score = 0;
    for (i = 0; i < 20; i++) {
      for (j = 0; j < 7; j++) {
        backgroundGrid[i][j] = 0;
      }
    }
    gameInProgress = false;
  }

 currentButton = digitalRead(37); // Top button.
  if (currentButton == 1) {  //Up
    wasUp = true;
    if (wasDown == true) { // Button transitioned from down to up.
      wasDown = false;
      clickTime = millis() - isUp; // Measure how long button has been down.
      Serial.print("Down time =");
      Serial.println(clickTime);
      Serial.println();
      Serial.print("Up time =");
      Serial.println(upClickTime);
      Serial.println();

      if (clickTime < 500) {   // Button has been down for less than 1/2 second.
        if (upClickTime < 300 ) { //Previous up was < 1/3 second so it's a double click.
          clickType = 2;
          firstClick = false;
        } else{                   // It's the first click in a while so wait to see if it's a double click.
          firstClick = true;
          if (firstClick) {
            Serial.println("firstClick = true");
          }
        }
      }
      
      if (clickTime > 600) { // button has been down for more than 0.6 seconds so its a hold.
        clickType = 3;
        shortClick = false;
      }
    } else {
      isUp = millis();
    }  
  }

  if (currentButton == 1 && firstClick ) {  // Double click did not hapen so return a single click.
    if ((millis() - isDown) > 300 ) {
      clickType = 1;
      firstClick = false;
    }
  }

  if (currentButton == 0) { // Down.
    wasDown = true;
    if (wasUp == true) { // Button transitioned from up to down.
      wasUp = false;
      upClickTime = millis() - isDown; // Measure how long it has been up.
    } else {
      isDown = millis();
    }
  }
  return clickType;
}


void setup() {
  Serial.begin(115200);
  while (!Serial) {
    Serial.print('.');
  }

  M5.begin();
  M5.Lcd.setRotation(1);
  
  fieldSprite.createSprite(240, 135); //Main playing field.
  scoreSprite.createSprite(240, 20);  // Score bar at the top.
  splashSprite.createSprite(135, 220); // Displays TETRIS and the rules.

  //Use analog pin 36 as a rendom number seed
  pinMode(36, INPUT);
  gpio_pulldown_dis(GPIO_NUM_25); // Disconnect puldowns so pin is free floating noise.
  gpio_pullup_dis(GPIO_NUM_25);

  randomSeed(analogRead(36));
  block = randomBlock(0);
}

void loop() {  // Run this loop every 50 milliseconds.
  int i;
  int j;
  int newBlock; // Used to prevent picking the same random block twice.
  static int click; // 0= No click, 1= single click, 2=double click, 3 = Hold.
  static unsigned long dropTime; // Amount of time since the last block drop.
  unsigned long dropDuration = 3000; // Current block drops one line every 3 seconds. Sets pace of the game.

  if (!gameInProgress) { // Show splash screen
    splashSprite.fillSprite(TFT_NAVY);
    splashSprite.setTextColor(0xE739); // Light Yellow
    splashSprite.setTextSize(3);
    splashSprite.drawString("TETRIS",13,20);  

    splashSprite.setTextSize(2);
    splashSprite.drawString("Clk:Left",5,60);  
    splashSprite.drawString("DblClk:Rot.",5,80);  
    splashSprite.drawString("Hold:Drop",5,100);
    splashSprite.drawString("CLICK",35,140);
    splashSprite.drawString("TO START",15,160);

    M5.Lcd.setRotation(0);
    splashSprite.pushSprite(0, 20);

    click = checkButton();
     if (click > 0) {           // Begin clip play
      gameInProgress = true; 
      for (i = 0; i <20; i++) {
        for (j = 0; j < 7; j++) {
          backgroundGrid[i][j] = 0;
          blockGrid[i][j] = 0;
        }
      }
      blockXPos = 5;
      blockYPos = 0;
      blockRotation = 0;
      score = 0;
      block = randomBlock(0);
    }
  } else { // Game is in progress.

    click = checkButton();
    if (click == 1) {  // Single click, move right
      blockXPos--;
      if (blockXPos < 0) { //Wrap around to the left side.
        blockXPos = 5;
      }
      // Draw the new position and check for a colission.
      collision = drawBlock(block, blockRotation, blockYPos, blockXPos);
      drawGrid();
    }
    if (click == 2) {  // Double click, rotate
      blockRotation++;
      if (blockRotation > 3) {
        blockRotation = 0;
      }
      // Draw the new position and check for a colission.
      collision = drawBlock(block, blockRotation, blockYPos, blockXPos);
      drawGrid();
    }
    if (click == 3) {  // Hold, drop
      // Keep dropping the block one line until it hits something, or the bottom.
      for (i = blockYPos; i < 15; i++) { 
        collision = drawBlock(block, blockRotation, i, blockXPos);
        drawGrid();
        delay(20);
        if (collision) {
          blockYPos = i;
          break;
        }
      }
    }

    dropLine(); // Check if any row is full (all seven blocks)

    if (millis() > (dropTime + dropDuration)) { // Three seconds have elampsed so drop the current block.
      dropTime = millis();
      
      collision = drawBlock(block, blockRotation, blockYPos, blockXPos);
      if (collision) {
        if (blockYPos == 1) { // Block could not move from start position, so game is over.
          gameInProgress = false;
          delay(1000);  // Wait before showing the splash screen.
        }
        copyBlockToBackground();

        // Start a new block
        blockXPos = 5;
        blockYPos = 0;
        blockRotation = 0;
        block = randomBlock(block);
        collision = drawBlock(block, blockRotation, blockYPos, blockXPos);
      }
      
      drawGrid();
      blockYPos = blockYPos + 1; // Move the block down one row.      
    }
  }
  delay(50);
}
