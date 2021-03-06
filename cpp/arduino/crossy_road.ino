#include <gamma.h>
#include <RGBmatrixPanel.h>
#include <Adafruit_GFX.h>

// define the wiring of the LED screen
const uint8_t CLK  = 8;
const uint8_t LAT = A3;
const uint8_t OE = 9;
const uint8_t A = A0;
const uint8_t B = A1;
const uint8_t C = A2;

// define the wiring of the inputs
const int POTENTIOMETER_PIN_NUMBER = 5;
const int BUTTON_PIN_NUMBER = 10;

// global constant for the number of obstacles in the game
// max of 4 obstacles per rank... 9 ranks total on each level
const int MAX_OBSTACLES = 35;

// 3 speeds for obstacles... slow, medium, fast
const int speed_slow = 1500;
const int speed_med = 1000;
const int speed_fast = 750;

// x positions for each "rank" of level
// each rank has a height of 3 pixels
// 9 ranks total in each level
// different obstacles in each rank
const int rank_1 = 3;
const int rank_2 = 6;
const int rank_3 = 9;
const int rank_4 = 12;
const int rank_5 = 15;
const int rank_6 = 18;
const int rank_7 = 21;
const int rank_8 = 24;
const int rank_9 = 27;

// sets y position for different types of obstacles
const int car_right = 0;
const int car_left = 1;
const int van_right = 0;
const int van_left = -2;
const int bus_right = 0;
const int bus_left = 3;
const int water_right = -7;
const int water_left = 0;

// a global variable that represents the LED screen
RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, false);

// the following functions are for printing messages
//void print_opener();
void print_opener(int x_value);
void print_level(int level);
void print_lives(int lives);
void game_over();
void win_game();

class Color {
  public:
    int red;
    int green;
    int blue;
    Color() {
      red = 0;
      green = 0;
      blue = 0;
    }
    Color(int r, int g, int b) {
      red = r;
      green = g;
      blue = b;
    }
    uint16_t to_333() const {
      return matrix.Color333(red, green, blue);
    }
};

const Color BLACK(0, 0, 0);
const Color RED(4, 0, 0);
const Color ORANGE(6, 1, 0);
const Color YELLOW(4, 4, 0);
const Color GREEN(0, 25, 0);
const Color BLUE(0, 0, 4);
const Color PURPLE(1, 0, 2);
const Color WHITE(4, 4, 4);
const Color LIME(2, 4, 0);
const Color AQUA(0, 4, 4);


class Player {
  public:
    Player() {
      x = 0;
      y = 7;
      lives = 3;
    }
    
    // getters
    int get_x() const {
      return x;
    }
    int get_y() const {
      return y;
    }
    int get_lives() const {
      return lives;
    }

    void set_x(int x_arg) {
      x = x_arg;
    }
    // setter
    void set_y(int y_arg) {
      y = y_arg;
    }
    void initialize(int x_arg, int y_arg) {
      x = x_arg;
      y = y_arg;
    }
    // Modifies: lives
    void die() {
      lives -= 1;
    }
    
    // draws the Player
    // calls: draw_with_rgb
    void draw() {
      draw_with_rgb(BLUE, YELLOW);
    }

    void move(){
      erase();
      x++;
      draw();
    }
    
    // draws black where the Player used to be
    // calls: draw_with_rgb
    void erase() {
      draw_with_rgb(BLACK, BLACK);
    }

  private:
    int x;
    int y;
    int lives;

    // sets private data members x and y to initial values
    
    
    // draws the player 
    void draw_with_rgb(Color blue, Color maize) {
      for (int row = 0; row < 2; row++) {
        for (int col = 0; col < 2; col++) {
          if (row == 0 && col == 0) {
            matrix.drawPixel(x, y, maize.to_333());
          }
          if (row == 0 && col == 1) {
            matrix.drawPixel(x, y + 1, blue.to_333());
          }
          if (row == 1 && col == 0) {
            matrix.drawPixel(x + 1, y, blue.to_333());
          }
          if (row == 1 && col == 1) {
            matrix.drawPixel(x + 1, y + 1, maize.to_333());
          }
        }
      } 
    }
};

class Grass {
  public:
    // Constructors
    Grass() {
      x = 0;
      y = 0;
    }
    
    Grass(int x_arg, int y_arg) {
      x = x_arg;
      y = y_arg;
    }
    
    void initialize(int x_arg, int y_arg) {
      x = x_arg;
      y = y_arg;
    } 
    
    int get_x() const {
      return x;
    }
    
    int get_y() const {
      return y;
    }
    
    int set_x(int x_arg) {
      x = x_arg;
    }
    
    int set_y(int y_arg) {
      y = y_arg;
    }

    void draw_from(int bottom_row, int top_row, Color color) {
      set_x(bottom_row);
      set_y(0);
      draw_with_rgb(bottom_row, top_row, color);
    }
    
    void erase(int bottom_row, int top_row) {
      draw_with_rgb(bottom_row, top_row, BLACK);
    }  
    
  private:
    int x;
    int y;
    
    

    void draw_with_rgb(int bottom_row, int top_row, Color color) {
      for (int i = 0; i <= top_row - bottom_row; i++) {
       for (int j = 0; j < 16; j++) {
         matrix.drawPixel(x, y, color.to_333());
         y++;
       }
       x++;
       y = 0;
      }
    }
    
};

class Car {
  public:
    // Constructors
    Car() {
      x = 34;
      y = 0;
    }
    
    Car(int x_arg, int y_arg) {
      x = x_arg;
      y = y_arg;
    }
    
    void initialize(int x_arg, int y_arg) {
      x = x_arg;
      y = y_arg;
    }
    
    int get_x() const {
      return x;
    }
    
    int get_y() const {
      return y;
    }
    
    void move_right() {
      erase();
      y++;
      draw_right_bound_car();
    }
    
    void move_left() {
      erase();
      y--;
      draw_left_bound_car();
    }
    
    void draw_right_bound_car() {
      draw_with_rgb_right(BLUE, WHITE);
    }
    
    void draw_left_bound_car() {
      draw_with_rgb_left(RED, WHITE);
    }
    
    void erase() {
      draw_with_rgb_right(BLACK, BLACK);
    }  
    
  private:
    int x;
    int y;
    
    void draw_with_rgb_right(Color color, Color tires) {
          for (int row = 0; row < 3; row++) {
              for (int col = 0; col < 3; col++) {
                  if (row == 0 && col == 0) {
                      matrix.drawPixel(x, y, tires.to_333());
                  }
                  if (row == 0 && col == 1) {
                      matrix.drawPixel(x, y + 1, BLACK.to_333());
                  }
                  if (row == 0 && col == 2) {
                      matrix.drawPixel(x, y + 2, tires.to_333());
                  }
                  if (row == 1 && col == 0) {
                      matrix.drawPixel(x + 1, y, color.to_333());
                  }
                  if (row == 1 && col == 1) {
                      matrix.drawPixel(x + 1, y + 1, color.to_333());
                  }
                  if (row == 1 && col == 2) {
                      matrix.drawPixel(x + 1, y + 2, color.to_333());
                  }
                  if (row == 2 && col == 0) {
                      matrix.drawPixel(x + 2, y, color.to_333());
                  }
                  if (row == 2 && col == 1) {
                      matrix.drawPixel(x + 2, y + 1, color.to_333());
                  }
                  if (row == 2 && col == 2) {
                      matrix.drawPixel(x + 2, y + 2, BLACK.to_333());
                  }
              }
          }
      }

      void draw_with_rgb_left(Color color, Color tires) {
          for (int row = 0; row < 3; row++) {
              for (int col = 0; col < 3; col++) {
                  if (row == 0 && col == 0) {
                      matrix.drawPixel(x, y, tires.to_333());
                  }
                  if (row == 0 && col == 1) {
                      matrix.drawPixel(x, y + 1, BLACK.to_333());
                  }
                  if (row == 0 && col == 2) {
                      matrix.drawPixel(x, y + 2, tires.to_333());
                  }
                  if (row == 1 && col == 0) {
                      matrix.drawPixel(x + 1, y, color.to_333());
                  }
                  if (row == 1 && col == 1) {
                      matrix.drawPixel(x + 1, y + 1, color.to_333());
                  }
                  if (row == 1 && col == 2) {
                      matrix.drawPixel(x + 1, y + 2, color.to_333());
                  }
                  if (row == 2 && col == 0) {
                      matrix.drawPixel(x + 2, y, BLACK.to_333());
                  }
                  if (row == 2 && col == 1) {
                      matrix.drawPixel(x + 2, y + 1, color.to_333());
                  }
                  if (row == 2 && col == 2) {
                      matrix.drawPixel(x + 2, y + 2, color.to_333());
                  }
              }
          }
      }
};

class Van {
  public:
    // Constructors
    Van() {
      x = 34;
      y = 0;
    }
    
    Van(int x_arg, int y_arg) {
      x = x_arg;
      y = y_arg;
    }
    
    void initialize(int x_arg, int y_arg) {
      x = x_arg;
      y = y_arg;
    }
    
    int get_x() const {
      return x;
    }
    
    int get_y() const {
      return y;
    }
    
    void move_right() {
      erase();
      y++;
      draw_right_bound_van();
    }
    
    void move_left() {
      erase();
      y--;
      draw_left_bound_van();
    }
    
    void draw_right_bound_van() {
      draw_with_rgb_right(PURPLE, WHITE);
    }
    
    void draw_left_bound_van() {
      draw_with_rgb_left(PURPLE, WHITE);
    }
    
    void erase() {
      draw_with_rgb_right(BLACK, BLACK);
    }  
    
  private:
    int x;
    int y;
    
    void draw_with_rgb_right(Color color, Color tires) {
      for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 4; col++) {
          if (row == 0 && col == 0) {
            matrix.drawPixel(x, y, tires.to_333());
          }
          if (row == 0 && col == 1) {
            matrix.drawPixel(x, y + 1, BLACK.to_333());
          }
          if (row == 0 && col == 2) {
            matrix.drawPixel(x, y + 2, BLACK.to_333());
          }
          if (row == 0 && col == 3) {
            matrix.drawPixel(x, y + 3, tires.to_333());
          }
          if (row == 1 && col == 0) {
            matrix.drawPixel(x + 1, y, color.to_333());
          }
          if (row == 1 && col == 1) {
            matrix.drawPixel(x + 1, y + 1, color.to_333());
          }
          if (row == 1 && col == 2) {
            matrix.drawPixel(x + 1, y + 2, color.to_333());
          }
          if (row == 1 && col == 3) {
            matrix.drawPixel(x + 1, y + 3, color.to_333());
          }
          if (row == 2 && col == 0) {
            matrix.drawPixel(x + 2, y, color.to_333());
          }
          if (row == 2 && col == 1) {
            matrix.drawPixel(x + 2, y + 1, color.to_333());
          }
          if (row == 2 && col == 2) {
            matrix.drawPixel(x + 2, y + 2, color.to_333());
          }
          if (row == 2 && col == 3) {
            matrix.drawPixel(x + 2, y + 3, BLACK.to_333());
          }
          
        }
      }
    }

      void draw_with_rgb_left(Color color, Color tires) {
      for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 4; col++) {
          if (row == 0 && col == 0) {
            matrix.drawPixel(x, y, tires.to_333());
          }
          if (row == 0 && col == 1) {
            matrix.drawPixel(x, y + 1, BLACK.to_333());
          }
          if (row == 0 && col == 2) {
            matrix.drawPixel(x, y + 2, BLACK.to_333());
          }
          if (row == 0 && col == 3) {
            matrix.drawPixel(x, y + 3, tires.to_333());
          }
          if (row == 1 && col == 0) {
            matrix.drawPixel(x + 1, y, color.to_333());
          }
          if (row == 1 && col == 1) {
            matrix.drawPixel(x + 1, y + 1, color.to_333());
          }
          if (row == 1 && col == 2) {
            matrix.drawPixel(x + 1, y + 2, color.to_333());
          }
          if (row == 1 && col == 3) {
            matrix.drawPixel(x + 1, y + 3, color.to_333());
          }
          if (row == 2 && col == 0) {
            matrix.drawPixel(x + 2, y, BLACK.to_333());
          }
          if (row == 2 && col == 1) {
            matrix.drawPixel(x + 2, y + 1, color.to_333());
          }
          if (row == 2 && col == 2) {
            matrix.drawPixel(x + 2, y + 2, color.to_333());
          }
          if (row == 2 && col == 3) {
            matrix.drawPixel(x + 2, y + 3, color.to_333());
          }
          
        }
      }
    }
};

class Bus {
  public:
    // Constructors
    Bus() {
      x = 34;
      y = 0;
    }
    
    Bus(int x_arg, int y_arg) {
      x = x_arg;
      y = y_arg;
    }
    
    void initialize(int x_arg, int y_arg) {
      x = x_arg;
      y = y_arg;
    }
    
    int get_x() const {
      return x;
    }
    
    int get_y() const {
      return y;
    }
    
    void move_right() {
      erase();
      y++;
      draw_right_bound_bus();
    }
    
    void move_left() {
      erase();
      y--;
      draw_left_bound_bus();
    }
    
    void draw_right_bound_bus() {
      draw_with_rgb_right(YELLOW, WHITE);
    }
    
    void draw_left_bound_bus() {
      draw_with_rgb_left(YELLOW, WHITE);
    }
    
    void erase() {
      draw_with_rgb_right(BLACK, BLACK);
    }  
  
  private:
    int x;
    int y;
    
    void draw_with_rgb_right(Color color, Color tires) {
        for (int row = 0; row < 3; row++) {
          for (int col = 0; col < 5; col++) {
            if (row == 0 && col == 0) {
              matrix.drawPixel(x, y, tires.to_333());
            }
            if (row == 0 && col == 1) {
              matrix.drawPixel(x, y + 1, BLACK.to_333());
            }
            if (row == 0 && col == 2) {
              matrix.drawPixel(x, y + 2, BLACK.to_333());
            }
            if (row == 0 && col == 3) {
              matrix.drawPixel(x, y + 3, BLACK.to_333());
            }
            if (row == 0 && col == 4) {
              matrix.drawPixel(x, y + 4, tires.to_333());
            }
            if (row == 1 && col == 0) {
              matrix.drawPixel(x + 1, y, color.to_333());
            }
            if (row == 1 && col == 1) {
              matrix.drawPixel(x + 1, y + 1, color.to_333()); 
            }
            if (row == 1 && col == 2) {
              matrix.drawPixel(x + 1, y + 2, color.to_333());
            }
            if (row == 1 && col == 3) {
              matrix.drawPixel(x + 1, y + 3, color.to_333()); 
            }
            if (row == 1 && col == 4) {
              matrix.drawPixel(x + 1, y + 4, color.to_333());
            }
            if (row == 2 && col == 0) {
              matrix.drawPixel(x + 2, y, color.to_333());
            }
            if (row == 2 && col == 1) {
              matrix.drawPixel(x + 2, y + 1, color.to_333());
            }
            if (row == 2 && col == 2) {
              matrix.drawPixel(x + 2, y + 2, color.to_333());
            }
            if (row == 2 && col == 3) {
              matrix.drawPixel(x + 2, y + 3, color.to_333());
            }
            if (row == 2 && col == 4) {
              matrix.drawPixel(x + 2, y + 4, BLACK.to_333());
            }
          }
        }
    }

    void draw_with_rgb_left(Color color, Color tires) {
        for (int row = 0; row < 3; row++) {
          for (int col = 0; col < 5; col++) {
            if (row == 0 && col == 0) {
              matrix.drawPixel(x, y, tires.to_333());
            }
            if (row == 0 && col == 1) {
              matrix.drawPixel(x, y + 1, BLACK.to_333());
            }
            if (row == 0 && col == 2) {
              matrix.drawPixel(x, y + 2, BLACK.to_333());
            }
            if (row == 0 && col == 3) {
              matrix.drawPixel(x, y + 3, BLACK.to_333());
            }
            if (row == 0 && col == 4) {
              matrix.drawPixel(x, y + 4, tires.to_333());
            }
            if (row == 1 && col == 0) {
              matrix.drawPixel(x + 1, y, color.to_333());
            }
            if (row == 1 && col == 1) {
              matrix.drawPixel(x + 1, y + 1, color.to_333()); 
            }
            if (row == 1 && col == 2) {
              matrix.drawPixel(x + 1, y + 2, color.to_333());
            }
            if (row == 1 && col == 3) {
              matrix.drawPixel(x + 1, y + 3, color.to_333()); 
            }
            if (row == 1 && col == 4) {
              matrix.drawPixel(x + 1, y + 4, color.to_333());
            }
            if (row == 2 && col == 0) {
              matrix.drawPixel(x + 2, y, BLACK.to_333());
            }
            if (row == 2 && col == 1) {
              matrix.drawPixel(x + 2, y + 1, color.to_333());
            }
            if (row == 2 && col == 2) {
              matrix.drawPixel(x + 2, y + 2, color.to_333());
            }
            if (row == 2 && col == 3) {
              matrix.drawPixel(x + 2, y + 3, color.to_333());
            }
            if (row == 2 && col == 4) {
              matrix.drawPixel(x + 2, y + 4, color.to_333());
            }
          }
        }
    }
};


class Water {
  public:
    // Constructors
    Water() {
      x = 34;
      y = 0;
    }
    
    Water(int x_arg, int y_arg) {
      x = x_arg;
      y = y_arg;
    }
    
    void initialize(int x_arg, int y_arg) {
      x = x_arg;
      y = y_arg;
    } 
    
    int get_x() const {
      return x;
    }
    
    int get_y() const {
      return y;
    }
    
    void move_right() {
      erase();
      y++;
      draw();
    }
    
    void move_left() {
      erase();
      y--;
      draw();
    }
    
    void draw() {
      draw_with_rgb(AQUA);
    }
    
    void erase() {
      // draw orange to look like logs
      draw_with_rgb(BLACK);
    } 
     
  private:
    int x;
    int y;
    
    void draw_with_rgb(Color color) {
      for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 7; col++) {
          if (row == 0 && col == 0) {
            matrix.drawPixel(x, y, color.to_333());
          }
          if (row == 0 && col == 1) {
            matrix.drawPixel(x, y + 1, color.to_333());
          }
          if (row == 0 && col == 2) {
            matrix.drawPixel(x, y + 2, color.to_333());
          }
          if (row == 0 && col == 3) {
            matrix.drawPixel(x, y + 3, color.to_333());
          }
          if (row == 0 && col == 4) {
            matrix.drawPixel(x, y + 4, ORANGE.to_333());
          }
          if (row == 0 && col == 5) {
            matrix.drawPixel(x, y + 5, ORANGE.to_333());
          }
          if (row == 0 && col == 6) {
            matrix.drawPixel(x, y + 6, ORANGE.to_333());
          }
          if (row == 1 && col == 0) {
            matrix.drawPixel(x + 1, y, color.to_333());
          }
          if (row == 1 && col == 1) {
            matrix.drawPixel(x + 1, y + 1, color.to_333());
          }
          if (row == 1 && col == 2) {
            matrix.drawPixel(x + 1, y + 2, color.to_333());
          }
          if (row == 1 && col == 3) {
            matrix.drawPixel(x + 1, y + 3, color.to_333());
          }
          if (row == 1 && col == 4) {
            matrix.drawPixel(x + 1, y + 4, ORANGE.to_333());
          }
          if (row == 1 && col == 5) {
            matrix.drawPixel(x + 1, y + 5, ORANGE.to_333());
          }
          if (row == 1 && col == 6) {
            matrix.drawPixel(x + 1, y + 6, ORANGE.to_333());
          }
          if (row == 2 && col == 0) {
            matrix.drawPixel(x + 2, y, color.to_333());
          }
          if (row == 2 && col == 1) {
            matrix.drawPixel(x + 2, y + 1, color.to_333());
          }
          if (row == 2 && col == 2) {
            matrix.drawPixel(x + 2, y + 2, color.to_333());
          }
          if (row == 2 && col == 3) {
            matrix.drawPixel(x + 2, y + 3, color.to_333());
          }
          if (row == 2 && col == 4) {
            matrix.drawPixel(x + 2, y + 4, ORANGE.to_333());
          }
          if (row == 2 && col == 5) {
            matrix.drawPixel(x + 2, y + 5, ORANGE.to_333());
          }
          if (row == 2 && col == 6) {
            matrix.drawPixel(x + 2, y + 6, ORANGE.to_333());
          }
          
        }
      }
    }
};

class Game {
  public:
    Game() {
    
      level = 1;
      time = 0;
      time_rank1 = 0;
      time_rank2 = 0;
      time_rank3 = 0;
      time_rank4 = 0;
      time_rank5 = 0;
      time_rank6 = 0;
      time_rank7 = 0;
      time_rank8 = 0;
      time_rank9 = 0;
    }

      bool player_touched_car() {
        bool touched_car = false;
        for (int i = 0; i < MAX_OBSTACLES; i++) {
          if (player.get_x() == car[i].get_x() && player.get_y() == car[i].get_y() ||
              player.get_x() == car[i].get_x() && player.get_y() == car[i].get_y() + 2 ||
              player.get_x() == car[i].get_x() + 2 && player.get_y() == car[i].get_y() ||
              player.get_x() == car[i].get_x() + 2 && player.get_y() == car[i].get_y() + 2 ||

              player.get_x() == car[i].get_x() && player.get_y() + 1 == car[i].get_y() ||
              player.get_x() == car[i].get_x() && player.get_y() + 1 == car[i].get_y() + 2 ||
              player.get_x() == car[i].get_x() + 2 && player.get_y() + 1 == car[i].get_y() ||
              player.get_x() == car[i].get_x() + 2 && player.get_y() + 1 == car[i].get_y() + 2 ||

              player.get_x() + 1 == car[i].get_x() && player.get_y() == car[i].get_y() ||
              player.get_x() + 1 == car[i].get_x() && player.get_y() == car[i].get_y() + 2 ||
              player.get_x() + 1 == car[i].get_x() + 2 && player.get_y() == car[i].get_y() ||
              player.get_x() + 1 == car[i].get_x() + 2 && player.get_y() == car[i].get_y() + 2 ||

              player.get_x() + 1 == car[i].get_x() && player.get_y() + 1 == car[i].get_y() ||
              player.get_x() + 1 == car[i].get_x() && player.get_y() == car[i].get_y() + 2 ||
              player.get_x() + 1 == car[i].get_x() + 2 && player.get_y() == car[i].get_y() ||
              player.get_x() + 1 == car[i].get_x() + 2 && player.get_y() == car[i].get_y() + 2) {
                touched_car = true;
                break;
              }
        }
        return touched_car;
      }

      bool player_touched_van() {
        bool touched_van = false;
        for (int i = 0; i < MAX_OBSTACLES; i++) {
          if (player.get_x() == van[i].get_x() && player.get_y() == van[i].get_y() ||
              player.get_x() == van[i].get_x() && player.get_y() == van[i].get_y() + 1 ||
              player.get_x() == van[i].get_x() && player.get_y() == van[i].get_y() + 3 ||
              player.get_x() == van[i].get_x() + 2 && player.get_y() == van[i].get_y() ||
              player.get_x() == van[i].get_x() + 2 && player.get_y() == van[i].get_y() + 1 ||
              player.get_x() == van[i].get_x() + 2 && player.get_y() == van[i].get_y() + 3 ||

              player.get_x() == van[i].get_x() && player.get_y() + 1 == van[i].get_y() ||
              player.get_x() == van[i].get_x() && player.get_y() + 1 == van[i].get_y() + 1 ||
              player.get_x() == van[i].get_x() && player.get_y() + 1 == van[i].get_y() + 3 ||
              player.get_x() == van[i].get_x() + 2 && player.get_y() + 1 == van[i].get_y() ||
              player.get_x() == van[i].get_x() + 2 && player.get_y() + 1 == van[i].get_y() + 1 ||
              player.get_x() == van[i].get_x() + 2 && player.get_y() + 1 == van[i].get_y() + 3 ||

              player.get_x() + 1 == van[i].get_x() && player.get_y() == van[i].get_y() ||
              player.get_x() + 1 == van[i].get_x() && player.get_y() == van[i].get_y() + 1 ||
              player.get_x() + 1 == van[i].get_x() && player.get_y() == van[i].get_y() + 3 ||
              player.get_x() + 1 == van[i].get_x() + 2 && player.get_y() == van[i].get_y() ||
              player.get_x() + 1 == van[i].get_x() + 2 && player.get_y() == van[i].get_y() + 1 ||
              player.get_x() + 1 == van[i].get_x() + 2 && player.get_y() == van[i].get_y() + 3 ||

              player.get_x() + 1 == van[i].get_x() && player.get_y() + 1 == van[i].get_y() ||
              player.get_x() + 1 == van[i].get_x() && player.get_y() + 1 == van[i].get_y() + 1 ||
              player.get_x() + 1 == van[i].get_x() && player.get_y() + 1 == van[i].get_y() + 3 ||
              player.get_x() + 1 == van[i].get_x() + 2 && player.get_y() + 1 == van[i].get_y() ||
              player.get_x() + 1 == van[i].get_x() + 2 && player.get_y() + 1 == van[i].get_y() + 1 ||
              player.get_x() + 1 == van[i].get_x() + 2 && player.get_y() + 1 == van[i].get_y() + 3) {
                touched_van = true;
                break;
              }
        }
        return touched_van;
      }

      bool player_touched_bus() {
        bool touched_bus = false;
        for (int i = 0; i < MAX_OBSTACLES; i++) {
          if (player.get_x() == bus[i].get_x() && player.get_y() == bus[i].get_y() ||
              player.get_x() == bus[i].get_x() && player.get_y() == bus[i].get_y() + 2 ||
              player.get_x() == bus[i].get_x() && player.get_y() == bus[i].get_y() + 4 ||
              player.get_x() == bus[i].get_x() + 2 && player.get_y() == bus[i].get_y() ||
              player.get_x() == bus[i].get_x() + 2 && player.get_y() == bus[i].get_y() + 2 ||
              player.get_x() == bus[i].get_x() + 2 && player.get_y() == bus[i].get_y() + 4 ||

              player.get_x() == bus[i].get_x() && player.get_y() + 1 == bus[i].get_y() ||
              player.get_x() == bus[i].get_x() && player.get_y() + 1 == bus[i].get_y() + 2 ||
              player.get_x() == bus[i].get_x() && player.get_y() + 1 == bus[i].get_y() + 4 ||
              player.get_x() == bus[i].get_x() + 2 && player.get_y() + 1 == bus[i].get_y() ||
              player.get_x() == bus[i].get_x() + 2 && player.get_y() + 1 == bus[i].get_y() + 2 ||
              player.get_x() == bus[i].get_x() + 2 && player.get_y() + 1 == bus[i].get_y() + 4 ||

              player.get_x() + 1 == bus[i].get_x() && player.get_y() == bus[i].get_y() ||
              player.get_x() + 1 == bus[i].get_x() && player.get_y() == bus[i].get_y() + 2 ||
              player.get_x() + 1 == bus[i].get_x() && player.get_y() == bus[i].get_y() + 4 ||
              player.get_x() + 1 == bus[i].get_x() + 2 && player.get_y() == bus[i].get_y() ||
              player.get_x() + 1 == bus[i].get_x() + 2 && player.get_y() == bus[i].get_y() + 2 ||
              player.get_x() + 1 == bus[i].get_x() + 2 && player.get_y() == bus[i].get_y() + 4 ||

              player.get_x() + 1 == bus[i].get_x() && player.get_y() + 1 == bus[i].get_y() ||
              player.get_x() + 1 == bus[i].get_x() && player.get_y() + 1 == bus[i].get_y() + 2 ||
              player.get_x() + 1 == bus[i].get_x() && player.get_y() + 1 == bus[i].get_y() + 4 ||
              player.get_x() + 1 == bus[i].get_x() + 2 && player.get_y() + 1 == bus[i].get_y() ||
              player.get_x() + 1 == bus[i].get_x() + 2 && player.get_y() + 1 == bus[i].get_y() + 2 ||
              player.get_x() + 1 == bus[i].get_x() + 2 && player.get_y() + 1 == bus[i].get_y() + 4) {
                touched_bus = true;
                break;
              }
        }
        return touched_bus;
      }

      bool player_touched_water() {
        bool touched_water = false;
        for (int i = 0; i < MAX_OBSTACLES; i++) {
          if (player.get_x() == water[i].get_x() && player.get_y() == water[i].get_y() ||
              player.get_x() == water[i].get_x() && player.get_y() == water[i].get_y() + 1 ||
              player.get_x() == water[i].get_x() && player.get_y() == water[i].get_y() + 3 ||
              player.get_x() == water[i].get_x() + 2 && player.get_y() == water[i].get_y() ||
              player.get_x() == water[i].get_x() + 2 && player.get_y() == water[i].get_y() + 1 ||
              player.get_x() == water[i].get_x() + 2 && player.get_y() == water[i].get_y() + 3 ||

              player.get_x() == water[i].get_x() && player.get_y() + 1 == water[i].get_y() ||
              player.get_x() == water[i].get_x() && player.get_y() + 1 == water[i].get_y() + 1 ||
              player.get_x() == water[i].get_x() && player.get_y() + 1 == water[i].get_y() + 3 ||
              player.get_x() == water[i].get_x() + 2 && player.get_y() + 1 == water[i].get_y() ||
              player.get_x() == water[i].get_x() + 2 && player.get_y() + 1 == water[i].get_y() + 1 ||
              player.get_x() == water[i].get_x() + 2 && player.get_y() + 1 == water[i].get_y() + 3 ||

              player.get_x() + 1 == water[i].get_x() && player.get_y() == water[i].get_y() ||
              player.get_x() + 1 == water[i].get_x() && player.get_y() == water[i].get_y() + 1 ||
              player.get_x() + 1 == water[i].get_x() && player.get_y() == water[i].get_y() + 3 ||
              player.get_x() + 1 == water[i].get_x() + 2 && player.get_y() == water[i].get_y() ||
              player.get_x() + 1 == water[i].get_x() + 2 && player.get_y() == water[i].get_y() + 1 ||
              player.get_x() + 1 == water[i].get_x() + 2 && player.get_y() == water[i].get_y() + 3 ||

              player.get_x() + 1 == water[i].get_x() && player.get_y() + 1 == water[i].get_y() ||
              player.get_x() + 1 == water[i].get_x() && player.get_y() + 1 == water[i].get_y() + 1 ||
              player.get_x() + 1 == water[i].get_x() && player.get_y() + 1 == water[i].get_y() + 3 ||
              player.get_x() + 1 == water[i].get_x() + 2 && player.get_y() + 1 == water[i].get_y() ||
              player.get_x() + 1 == water[i].get_x() + 2 && player.get_y() + 1 == water[i].get_y() + 1 ||
              player.get_x() + 1 == water[i].get_x() + 2 && player.get_y() + 1 == water[i].get_y() + 3) {
                touched_water = true;
                break;
              }
        }
        return touched_water;
      }


      bool player_died() {
        if (player_touched_car() ||
            player_touched_van() ||
            player_touched_bus() ||
            player_touched_water()) {
              return true;
        }
        else {
          return false;
        }
      }
/***************************************************************/      
      void setup_game() {

        for (int x_value = 40; x_value > -250; x_value--) {
          
          print_opener(x_value);
          delay(40);
        }
        matrix.setTextWrap(true);
        setup_level();
      }

      void setup_level() {
        restart_level == false;
        matrix.fillScreen(BLACK.to_333());
        print_level(level);
        delay(2000);
        print_lives(player.get_lives());
        delay(2000);
        matrix.fillScreen(BLACK.to_333());

        grass.draw_from(0, 2, GREEN);
        grass.draw_from(30, 31, GREEN);

        if (level == 1) {
          int num_cars = 9;
          int num_vans = 3;
          int num_buses = 3;
          int van_rank2 = van_right;
          int car_rank4 = car_left;
          int car_rank5 = car_right;
          int car_rank7 = car_left;
          int bus_rank9 = bus_right;
          
          for (int i = 0; i < num_cars; i++) {
            if (i >= 0 && i <= 2) {
              car[i].initialize(rank_4, car_rank4);
              car[i].draw_left_bound_car();
              car_rank4 += 6;
            }
            else if (i >= 3 && i <= 5) {
              car[i].initialize(rank_5, car_rank5);
              car[i].draw_right_bound_car();
              car_rank5 += 6;
            }
            else if (i >= 6 && i <= 8) {
              car[i].initialize(rank_7, car_rank7);
              car[i].draw_left_bound_car();
              car_rank7 += 6;
            }
          }
          
          for (int i = 0; i < num_vans; i++) {
            van[i].initialize(rank_2, van_rank2);
            van[i].draw_right_bound_van();
            van_rank2 += 7;
          }

          for (int i = 0; i < num_buses; i++) {
            bus[i].initialize(rank_9, bus_rank9);
            bus[i].draw_right_bound_bus();
            bus_rank9 += 8;
          }  
        } // end level 1 setup

        if (level == 2) {
          int num_cars = 6;
          int num_vans = 3;
          int num_buses = 3;
          int num_water = 8;
          int car_rank1 = car_left;
          int water_rank3 = water_right;
          int van_rank5 = van_right;
          int bus_rank6 = bus_left;
          int car_rank7 = car_right;
          int water_rank9 = water_left;

          // draw "logs" at each rank with water
//          grass.draw_from(rank_3, rank_3 + 2, ORANGE);
//          grass.draw_from(rank_9, rank_9 + 2, ORANGE);


          for (int i = 0; i < num_cars; i++) {
            if (i >= 0 && i <= 2) {
              car[i].initialize(rank_1, car_rank1);
              car[i].draw_left_bound_car();
              car_rank1 += 6;
            }
            else if (i >= 3 && i <= 5) {
              car[i].initialize(rank_7, car_rank7);
              car[i].draw_right_bound_car();
              car_rank7 += 6;
            }
          }

         for (int i = 0; i < num_vans; i++) {
            van[i].initialize(rank_5, van_rank5);
            van[i].draw_right_bound_van();
            van_rank5 += 7;
         } 

         for (int i = 0; i < num_buses; i++) {
            bus[i].initialize(rank_6, bus_rank6);
            bus[i].draw_left_bound_bus();
            bus_rank6 += 8;
          }

         for (int i = 0; i < num_water; i++) {
            if (i >= 0 && i <= 3) {
              water[i].initialize(rank_3, water_rank3);
              water[i].draw();
              water_rank3 += 7;
            }
            else if (i >= 4 && i <= 7) {
              water[i].initialize(rank_9, water_rank9);
              water[i].draw();
              water_rank9 += 7;
            }
         }
        } // end level 2 setup

        if (level == 3) {
          int num_water = 32;
          int water_rank1 = water_right;
          int water_rank2 = water_left;
          int water_rank3 = water_right;
          int water_rank4 = water_left;
          int water_rank6 = water_right;
          int water_rank7 = water_left;
          int water_rank8 = water_right;
          int water_rank9 = water_right;

          for (int i = 0; i < num_water; i++) {
            if (i >= 0 && i <= 3) {
              water[i].initialize(rank_1, water_rank1);
              water[i].draw();
              water_rank1 += 7;
            }
            else if (i >= 4 && i <= 7) {
              water[i].initialize(rank_2, water_rank2);
              water[i].draw();
              water_rank2 += 7;
            }
            else if (i >= 8 && i <= 11) {
              water[i].initialize(rank_3, water_rank3);
              water[i].draw();
              water_rank3 += 7;
            }
            else if (i >= 12 && i <= 15) {
              water[i].initialize(rank_4, water_rank4);
              water[i].draw();
              water_rank4 += 7;
            }
            else if (i >= 16 && i <= 19) {
              water[i].initialize(rank_6, water_rank6);
              water[i].draw();
              water_rank6 += 7;
            }
            else if (i >= 20 && i <= 23) {
              water[i].initialize(rank_7, water_rank7);
              water[i].draw();
              water_rank7 += 7;
            }
            else if (i >= 24 && i <= 27) {
              water[i].initialize(rank_8, water_rank8);
              water[i].draw();
              water_rank8 += 7;
            }
            else if (i >= 28 && i <= 31) {
              water[i].initialize(rank_9, water_rank9);
              water[i].draw();
              water_rank9 += 7;
            }
          }
          
        } // end level 3 setup
          // Level 4 setup
      if (level == 4) {
      int num_cars = 6;
      int num_vans = 6;
      int num_buses = 6;
      int num_water = 8;
      int car_rank1 = car_right;
      int bus_rank2 = bus_left;        
      int van_rank3 = van_right;
      int water_rank4 = water_right;
      int water_rank6 = water_left;
      int van_rank7 = van_right;
      int bus_rank8 = bus_left;
      int car_rank9 = car_right;

         for (int i = 0; i < num_cars; i++) {
            if (i >= 0 && i <= 2) {
              car[i].initialize(rank_1, car_rank1);
              car[i].draw_right_bound_car();
              car_rank1 += 6;
            }
            else if (i >= 3 && i <= 5) {
              car[i].initialize(rank_9, car_rank9);
              car[i].draw_right_bound_car();
              car_rank9 += 6;
            }
          }

         for (int i = 0; i < num_vans; i++) {
          if (i >= 0 && i <= 2) {
            van[i].initialize(rank_3, van_rank3);
            van[i].draw_right_bound_van();
            van_rank3 += 7;
          }
          else if (i >=3 && i <=5) {
            van[i].initialize(rank_7, van_rank7);
            van[i].draw_right_bound_van();
            van_rank7 += 7;
          }
         } 

         for (int i = 0; i < num_buses; i++) {
          if (i >= 0 && i <= 2) {
            bus[i].initialize(rank_2, bus_rank2);
            bus[i].draw_left_bound_bus();
            bus_rank2 += 8;
          }
          else if (i >= 3 && i <= 5) {
            bus[i].initialize(rank_8, bus_rank8);
            bus[i].draw_left_bound_bus();
            bus_rank8 += 8;
          }
          }

         for (int i = 0; i < num_water; i++) {
            if (i >= 0 && i <= 3) {
              water[i].initialize(rank_4, water_rank4);
              water[i].draw();
              water_rank4 += 7;
            }
            else if (i >= 4 && i <= 7) {
              water[i].initialize(rank_6, water_rank6);
              water[i].draw();
              water_rank6 += 7;
            }
         }

   

         




        
      } // end level 4 setup

        // Level 5 setup

        if (level == 5) {
      int num_cars = 9;
      int num_vans = 3;
      int num_buses = 3;
      int num_water = 8;
      int van_rank1 = van_right;
      int bus_rank2 = bus_left;        
      int car_rank3 = car_left;
      int water_rank5 = water_right;
      int water_rank6 = water_right;
      int car_rank8 = car_left;
      int car_rank9 = car_right;

for (int i = 0; i < num_cars; i++) {
            if (i >= 0 && i <= 2) {
              car[i].initialize(rank_3, car_rank3);
              car[i].draw_left_bound_car();
              car_rank3 += 6;
            }
            else if (i >= 3 && i <= 5) {
              car[i].initialize(rank_8, car_rank8);
              car[i].draw_right_bound_car();
              car_rank8 += 6;
            }
            else if (i >= 6 && i <= 8) {
              car[i].initialize(rank_9, car_rank9);
              car[i].draw_left_bound_car();
              car_rank9 += 6;
            }
          }
          
           for (int i = 0; i < num_vans; i++) {
          if (i >= 0 && i <= 2) {
            van[i].initialize(rank_1, van_rank1);
            van[i].draw_right_bound_van();
            van_rank1 += 7;
          }
          
         } 
         
          for (int i = 0; i < num_buses; i++) {
          if (i >= 0 && i <= 2) {
            bus[i].initialize(rank_2, bus_rank2);
            bus[i].draw_left_bound_bus();
            bus_rank2 += 8;
          }
          }

          for (int i = 0; i < num_water; i++) {
            if (i >= 0 && i <= 3) {
              water[i].initialize(rank_5, water_rank5);
              water[i].draw();
              water_rank5 += 7;
            }
            else if (i >= 4 && i <= 7) {
              water[i].initialize(rank_6, water_rank6);
              water[i].draw();
              water_rank6 += 7;
            }
         }
        }   
      }

/***************************************************************/       
         
    // advances the game simulation one step and renders the graphics
    // see spec for details of game
    // Modifies: global variable matrix
    void update(int potentiometer_value, bool button_pressed){
     
      if (player.get_lives() > 0) {
        // move the player forward
      player.draw();
      if (button_pressed && (millis() - time) > 150) {
        player.move(); 
        time = millis();
      }

      // move the player left and right
      player.erase();
      // improves movement... potentiometer only sets player value if more than 10 away from numbers divisible by 64
      if ((potentiometer_value / 64 == (potentiometer_value + 10) / 64) && 
          (potentiometer_value / 64 == (potentiometer_value - 10) / 64)) {
       if (potentiometer_value > 960) {
         player.set_y(14);
       }
        else {
          player.set_y(potentiometer_value / 64);
        }
      }
      player.draw();
      
      
        // draw initial grass
        player.draw();
        grass.draw_from(0, 2, GREEN);
        grass.draw_from(30, 31, GREEN);
        player.draw();
      }
      
      if (player_died()) {
        player.erase();
        player.die();
        for (int i = 0; i < MAX_OBSTACLES; i++) {
          car[i].erase();
          car[i].initialize(34, 0);
          van[i].erase();
          van[i].initialize(34, 0);
          bus[i].erase();
          bus[i].initialize(34, 0);
          water[i].erase();
          water[i].initialize(34, 0);
        }
        
        
        
        player.initialize(0, 7);
        restart_level = true;
      }

      if (restart_level) {
        player.erase();
        for (int i = 0; i < MAX_OBSTACLES; i++) {
          car[i].erase();
          van[i].erase();
          bus[i].erase();
          water[i].erase();
        }
        matrix.fillScreen(BLACK.to_333());
        if (player.get_lives() == 0) {
          matrix.fillScreen(BLACK.to_333());
          game_over();
          return;
        }
        
      
        setup_level();
        
        restart_level = false;
      }

      if (player.get_x() > 31) {
 
        level++;

        player.initialize(0, player.get_y());
        
        for (int i = 0; i < MAX_OBSTACLES; i++) {
          car[i].erase();
          car[i].initialize(34, 0);
          van[i].erase();
          van[i].initialize(34, 0);
          bus[i].erase();
          bus[i].initialize(34, 0);
          water[i].erase();
          water[i].initialize(34, 0);
        }

        if (level > 5) {
        win_game();
      }
        
        setup_level();
        
      }
      

      
      if (level == 1) {
        int num_cars = 9;
        int num_vans = 3;
        int num_buses = 3;
        
        player.draw();
        // draw all grass on level
        grass.draw_from(rank_1, rank_1 + 2, GREEN);
        grass.draw_from(rank_3, rank_3 + 2, GREEN);
        grass.draw_from(rank_6, rank_6 + 2, GREEN);
        grass.draw_from(rank_8, rank_8 + 2, GREEN);
        player.draw();

        // move cars
        for (int i = 0; i < num_cars; i++) {
          if (i >= 0 && i <= 2) {
            if ((millis() - time_rank4) > speed_slow) {
              for (int i = 0; i <= 2; i++) {
                car[i].move_left();
                if (car[i].get_y() < -2) {
                  car[i].initialize(rank_4, 15);
                  car[i].draw_left_bound_car();
                }
              }
            time_rank4 = millis();
            }
          }
          else if (i >= 3 && i <= 5) {
            if ((millis() - time_rank5) > speed_slow) {
              for (int i = 3; i <= 5; i++) {
                car[i].move_right();
                if (car[i].get_y() > 15) {
                  car[i].initialize(rank_5, -2);
                  car[i].draw_right_bound_car();
                }
              }
            time_rank5 = millis();
            }
          }
          else if (i >= 6 && i <= 8) {
            if ((millis() - time_rank7) > speed_med) {
              for (int i = 6; i <= 8; i++) {
                car[i].move_left();
                if (car[i].get_y() < -2) {
                  car[i].initialize(rank_7, 15);
                  car[i].draw_left_bound_car();
                }
              }
            time_rank7 = millis();
            }
          }
        }

        // move vans
        for (int i = 0; i < num_vans; i++) {
          if (i >= 0 && i <= 2) {
            if ((millis() - time_rank2) > speed_slow) {
              for (int i = 0; i <= 2; i++) {
                van[i].move_right();
                if (van[i].get_y() > 17) {
                  van[i].initialize(rank_2, -3);
                  van[i].draw_right_bound_van();
                }
              }
            time_rank2 = millis();
            }
          }
        }

        // move buses
        for (int i = 0; i < num_buses; i++) {
          if (i >= 0 && i <= 2) {
            if ((millis() - time_rank9) > speed_slow) {
              for (int i = 0; i <= 2; i++) {
                bus[i].move_right();
                if (bus[i].get_y() > 19) {
                  bus[i].initialize(rank_9, -4);
                  bus[i].draw_right_bound_bus();
                }
              }
            time_rank9 = millis();
            }
          }
        }
        
      } // end level 1

      if (level == 2) {
        int num_cars = 6;
        int num_vans = 3;
        int num_buses = 3;
        int num_water = 8;

        player.draw();
        // draw all grass on level
        grass.draw_from(rank_2, rank_2 + 2, GREEN);
        grass.draw_from(rank_4, rank_4 + 2, GREEN);
        grass.draw_from(rank_8, rank_8 + 2, GREEN);
        player.draw(); 

        // move cars
        for (int i = 0; i < num_cars; i++) {
          if (i >= 0 && i <= 2) {
            if ((millis() - time_rank1) > speed_slow) {
              for (int i = 0; i <= 2; i++) {
                car[i].move_left();
                if (car[i].get_y() < -2) {
                  car[i].initialize(rank_1, 15);
                  car[i].draw_left_bound_car();
                }
              }
            time_rank1 = millis();
            }
          }
          else if (i >= 3 && i <= 5) {
            if ((millis() - time_rank7) > speed_med) {
              for (int i = 3; i <= 5; i++) {
                car[i].move_right();
                if (car[i].get_y() > 15) {
                  car[i].initialize(rank_7, -2);
                  car[i].draw_right_bound_car();
                }
              }
            time_rank7 = millis();
            }
          }
          
        }

        // move vans
        for (int i = 0; i < num_vans; i++) {
          if (i >= 0 && i <= 2) {
            if ((millis() - time_rank5) > speed_slow) {
              for (int i = 0; i <= 2; i++) {
                van[i].move_right();
                if (van[i].get_y() > 17) {
                  van[i].initialize(rank_5, -3);
                  van[i].draw_right_bound_van();
                }
              }
            time_rank5 = millis();
            }
          }
        }

        // move buses
        for (int i = 0; i < num_buses; i++) {
          if (i >= 0 && i <= 2) {
            if ((millis() - time_rank6) > speed_slow) {
              for (int i = 0; i <= 2; i++) {
                bus[i].move_left();
                if (bus[i].get_y() < -4) {
                  bus[i].initialize(rank_6, 19);
                  bus[i].draw_left_bound_bus();
                }
              }
            time_rank6 = millis();
            }
          }
        }

        player.draw();
        // draw water so player doesn't erase it while on log
        for (int i = 0; i < num_water; i++) {
          water[i].draw();
          player.draw();
        }
        player.draw();
        
        // move water
        for (int i = 0; i < num_water; i++) {
          if (i >= 0 && i <= 3) {
            if ((millis() - time_rank3) > speed_slow) {
              for (int i = 0; i <= 3; i++) {
                water[i].move_right();
                if (water[i].get_y() > 20) {
                  water[i].initialize(rank_3, -7);
                  water[i].draw();
                  player.draw();
                }
              }
            time_rank3 = millis();
            }
          }
          else if (i >= 4 && i <= 7) {
            if ((millis() - time_rank9) > speed_slow) {
              for (int i = 4; i <= 7; i++) {
                water[i].move_left();
                if (water[i].get_y() < -6) {
                  water[i].initialize(rank_9, 21);
                  water[i].draw();
                  player.draw();
                }
              }
            time_rank9 = millis();
            }
          }
        } 
      } // end level 2

      if (level == 3) {
        int num_water = 32;

        player.draw();
        
        // draw water so player doesn't erase it while on log
        for (int i = 0; i < num_water; i++) {
          water[i].draw();
          player.draw();
        }
        player.draw();

        // draw all grass on level
        grass.draw_from(rank_5, rank_5 + 2, GREEN);

        // move water
        for (int i = 0; i < num_water; i++) {
          if (i >= 0 && i <= 3) {
            if ((millis() - time_rank1) > speed_slow) {
              for (int i = 0; i <= 3; i++) {
                water[i].move_right();
                if (water[i].get_y() > 20) {
                  water[i].initialize(rank_1, -7);
                  water[i].draw();
                  player.draw();
                }
              }
            time_rank1 = millis();
            }
          }
          else if (i >= 4 && i <= 7) {
            if ((millis() - time_rank2) > speed_slow) {
              for (int i = 4; i <= 7; i++) {
                water[i].move_left();
                if (water[i].get_y() < -6) {
                  water[i].initialize(rank_2, 21);
                  water[i].draw();
                  player.draw();
                }
              }
            time_rank2 = millis();
            }
          }
          else if (i >= 8 && i <= 11) {
            if ((millis() - time_rank3) > speed_med) {
              for (int i = 8; i <= 11; i++) {
                water[i].move_right();
                if (water[i].get_y() > 20) {
                  water[i].initialize(rank_3, -7);
                  water[i].draw();
                  player.draw();
                }
              }
            time_rank3 = millis();
            }
          }
          else if (i >= 12 && i <= 15) {
            if ((millis() - time_rank4) > speed_slow) {
              for (int i = 12; i <= 15; i++) {
                water[i].move_left();
                if (water[i].get_y() < -6) {
                  water[i].initialize(rank_4, 21);
                  water[i].draw();
                  player.draw();
                }
              }
            time_rank4 = millis();
            }
          }
          else if (i >= 16 && i <= 19) {
            if ((millis() - time_rank6) > speed_slow) {
              for (int i = 16; i <= 19; i++) {
                water[i].move_right();
                if (water[i].get_y() > 20) {
                  water[i].initialize(rank_6, -7);
                  water[i].draw();
                  player.draw();
                }
              }
            time_rank6 = millis();
            }
          }
          else if (i >= 20 && i <= 23) {
            if ((millis() - time_rank7) > speed_slow) {
              for (int i = 20; i <= 23; i++) {
                water[i].move_left();
                if (water[i].get_y() < -6) {
                  water[i].initialize(rank_7, 21);
                  water[i].draw();
                  player.draw();
                }
              }
            time_rank7 = millis();
            }
          }
          else if (i >= 24 && i <= 27) {
            if ((millis() - time_rank8) > speed_fast) {
              for (int i = 24; i <= 27; i++) {
                water[i].move_right();
                if (water[i].get_y() > 20) {
                  water[i].initialize(rank_8, -7);
                  water[i].draw();
                  player.draw();
                }
              }
            time_rank8 = millis();
            }
          }
          else if (i >= 28 && i <= 31) {
            if ((millis() - time_rank9) > speed_slow) {
              for (int i = 28; i <= 31; i++) {
                water[i].move_right();
                if (water[i].get_y() > 20) {
                  water[i].initialize(rank_9, -7);
                  water[i].draw();
                  player.draw();
                }
              }
            time_rank9 = millis();
            }
          }

          
        }
      } // end level 3

      if (level == 4) {

      int num_cars = 6;
      int num_vans = 6;
      int num_buses = 6;
      int num_water = 8;

        player.draw();
        // draw water so player doesn't erase it while on log
        for (int i = 0; i < num_water; i++) {
          water[i].draw();
          player.draw();
        }
        

        player.draw();
        grass.draw_from(rank_5, rank_5 + 2, GREEN);
        player.draw();

        // move cars
        for (int i = 0; i < num_cars; i++) {
          if (i >= 0 && i <= 2) {
            if ((millis() - time_rank1) > speed_fast) {
              for (int i = 0; i <= 2; i++) {
                car[i].move_right();
                if (car[i].get_y() > 15) {
                  car[i].initialize(rank_1, -2);
                  car[i].draw_right_bound_car();
                }
              }
            time_rank1 = millis();
            }
          }
          else if (i >= 3 && i <= 5) {
            if ((millis() - time_rank9) > speed_slow) {
              for (int i = 3; i <= 5; i++) {
                car[i].move_right();
                if (car[i].get_y() > 15) {
                  car[i].initialize(rank_9, -2);
                  car[i].draw_right_bound_car();
                }
              }
            time_rank9 = millis();
            }
          } 
        }

        // move vans
        for (int i = 0; i < num_vans; i++) {
          if (i >= 0 && i <= 2) {
            if ((millis() - time_rank3) > speed_med) {
              for (int i = 0; i <= 2; i++) {
                van[i].move_right();
                if (van[i].get_y() > 17) {
                  van[i].initialize(rank_3, -3);
                  van[i].draw_right_bound_van();
                }
              }
            time_rank3 = millis();
            }
        
          }
          else if (i >= 3 && i <= 5) {
            if ((millis() - time_rank7) > speed_med) {
              for (int i = 3; i <= 5; i++) {
                van[i].move_right();
                if (van[i].get_y() > 17) {
                  van[i].initialize(rank_7, -3);
                  van[i].draw_right_bound_van();
                }
              }
            time_rank7 = millis();
            }
          }
        }

        // move buses
        for (int i = 0; i < num_buses; i++) {
          if (i >= 0 && i <= 2) {
            if ((millis() - time_rank2) > speed_slow) {
              for (int i = 0; i <= 2; i++) {
                bus[i].move_left();
                if (bus[i].get_y() < -4) {
                  bus[i].initialize(rank_2, 19);
                  bus[i].draw_left_bound_bus();
                }
              }
            time_rank2 = millis();
            }
          }
          if (i >= 3 && i <= 5) {
            if ((millis() - time_rank8) > speed_slow) {
              for (int i = 3; i <= 5; i++) {
                bus[i].move_left();
                if (bus[i].get_y() < -4) {
                  bus[i].initialize(rank_8, 19);
                  bus[i].draw_left_bound_bus();
                }
              }
            time_rank8 = millis();
            }
          }
        }

//        Logs
            for (int i = 0; i < num_water; i++) {
            if (i >= 0 && i <= 3) {
            if ((millis() - time_rank4) > speed_fast) {
              for (int i = 0; i <= 3; i++) {
                water[i].move_right();
                if (water[i].get_y() > 20) {
                  water[i].initialize(rank_4, -7);
                  water[i].draw();
                  player.draw();
                }
              }
            time_rank4 = millis();
            }
            }
            else if (i >= 4 && i <= 7) {
            if ((millis() - time_rank6) > speed_med) {
              for (int i = 4; i <= 7; i++) {
                water[i].move_left();
                if (water[i].get_y() < -6) {
                  water[i].initialize(rank_6, 21);
                  water[i].draw();
                  player.draw();
                }
              }
            time_rank6 = millis();
            }
          }
      }
      }
    // end level 4

    // start level 5

    if (level == 5) {
     int num_cars = 9;
     int num_vans = 3;
     int num_buses = 3;
     int num_water = 8;

        
        

        player.draw();
        // draw water so player doesn't erase it while on log
        for (int i = 0; i < num_water; i++) {
          water[i].draw();
          player.draw();
        }
        player.draw();
 
        player.draw();  
        grass.draw_from(rank_4, rank_4 + 2, GREEN);
        grass.draw_from(rank_7, rank_7 + 2, GREEN);
        player.draw();

// move cars
        // move cars
        for (int i = 0; i < num_cars; i++) {
          if (i >= 0 && i <= 2) {
            if ((millis() - time_rank3) > speed_fast) {
              for (int i = 0; i <= 2; i++) {
                car[i].move_left();
                if (car[i].get_y() < -2) {
                  car[i].initialize(rank_3, 15);
                  car[i].draw_left_bound_car();
                }
              }
            time_rank3 = millis();
            }
          }
          else if (i >= 6 && i <= 8) {
            if ((millis() - time_rank9) > speed_slow) {
              for (int i = 6; i <= 8; i++) {
                car[i].move_right();
                if (car[i].get_y() > 15) {
                  car[i].initialize(rank_9, -2);
                  car[i].draw_right_bound_car();
                }
              }
            time_rank9 = millis();
            }
          } 
          else if (i >= 3 && i <= 5) {
            if ((millis() - time_rank8) > speed_med) {
              for (int i = 3; i <= 5; i++) {
                car[i].move_left();
                if (car[i].get_y() < -2) {
                  car[i].initialize(rank_8, 15);
                  car[i].draw_left_bound_car();
                }
              }
            time_rank8 = millis();
            }
          
          }
        }

        for (int i = 0; i < num_water; i++) {
          if (i >= 0 && i <= 3) {
            if ((millis() - time_rank5) > speed_slow) {
              for (int i = 0; i <= 3; i++) {
                water[i].move_right();
                if (water[i].get_y() > 20) {
                  water[i].initialize(rank_5, -7);
                  water[i].draw();
                  player.draw();
                }
              }
            time_rank5 = millis();
            }
          }
            else if (i >= 4 && i <= 7) {
            if ((millis() - time_rank6) > speed_fast) {
              for (int i = 4; i <= 7; i++) {
                water[i].move_right();
                if (water[i].get_y() > 20) {
                  water[i].initialize(rank_6, -7);
                  water[i].draw();
                  player.draw();
                }
              }
            time_rank6 = millis();
            }
            }
        }
      for (int i = 0; i < num_buses; i++) {
          if (i >= 0 && i <= 2) {
            if ((millis() - time_rank2) > speed_slow) {
              for (int i = 0; i <= 2; i++) {
                bus[i].move_left();
                if (bus[i].get_y() < -4) {
                  bus[i].initialize(rank_2, 19);
                  bus[i].draw_left_bound_bus();
                }
              }
            time_rank2 = millis();
            }
          }
      }

      for (int i = 0; i < num_vans; i++) {
          if (i >= 0 && i <= 2) {
            if ((millis() - time_rank1) > speed_slow) {
              for (int i = 0; i <= 2; i++) {
                van[i].move_right();
                if (van[i].get_y() > 17) {
                  van[i].initialize(rank_1, -3);
                  van[i].draw_right_bound_van();
                }
              }
            time_rank1 = millis();
            }
          }
      }

    }

    player.erase();
    player.draw();   
} // end update

    
  private:
    int level;
    bool restart_level;
    unsigned long time;
    unsigned long time_rank1;
    unsigned long time_rank2;
    unsigned long time_rank3;
    unsigned long time_rank4;
    unsigned long time_rank5;
    unsigned long time_rank6;
    unsigned long time_rank7;
    unsigned long time_rank8;
    unsigned long time_rank9;
    Player player;
    Grass grass;
    Car car[MAX_OBSTACLES];
    Van van[MAX_OBSTACLES];
    Bus bus[MAX_OBSTACLES];
    Water water[MAX_OBSTACLES];
    
  
};

// a global variable that represents the game Space Invaders
Game game;

// see https://www.arduino.cc/reference/en/language/structure/sketch/setup/
void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN_NUMBER, INPUT);
  matrix.begin();
  // Initializes level 1
  game.setup_game();
  matrix.fillScreen(BLACK.to_333());
}

// see https://www.arduino.cc/reference/en/language/structure/sketch/loop/
void loop() {
  int potentiometer_value = analogRead(POTENTIOMETER_PIN_NUMBER);
  bool button_pressed = (digitalRead(BUTTON_PIN_NUMBER) == HIGH);
  game.update(potentiometer_value, button_pressed);
  }

// displays opener
void print_opener(int x_value) {
  matrix.setTextWrap(false);
  
  matrix.setTextSize(2);
  matrix.setTextColor(BLUE.to_333());
  matrix.setCursor(x_value, 1);
  matrix.print("Crossy "); 
  matrix.setTextColor(YELLOW.to_333());
  matrix.setCursor(x_value + 82, 1);
  matrix.print("State ");
  matrix.setTextColor(YELLOW.to_333());
  matrix.setCursor(x_value + 152, 1);
  matrix.print("Street");
  
  matrix.fillScreen(BLACK.to_333());
  
  matrix.setTextSize(2);
  matrix.setTextColor(BLUE.to_333());
  matrix.setCursor(x_value, 1);
  matrix.print("Crossy "); 
  matrix.setTextColor(YELLOW.to_333());
  matrix.setCursor(x_value + 82, 1);
  matrix.print("State ");
  matrix.setTextColor(YELLOW.to_333());
  matrix.setCursor(x_value + 152, 1);
  matrix.print("Street");
  
}

// displays Level
void print_level(int level) {
  matrix.fillScreen(BLACK.to_333());
  matrix.setCursor(1,0);
  matrix.setTextSize(1);
  matrix.setTextColor(BLUE.to_333());
  matrix.print("Level "); 
  matrix.print(level);
}

// displays number of lives
void print_lives(int lives) {
  matrix.fillScreen(BLACK.to_333());
  matrix.setCursor(1, 0);
  matrix.setTextSize(1);
  matrix.setTextColor(BLUE.to_333());
  matrix.print("Lives ");
  matrix.print(lives);
    
}

// displays "game over"
void game_over() {
    matrix.fillScreen(BLACK.to_333());   
    matrix.setCursor(4,0);
    matrix.setTextSize(1);
    matrix.setTextColor(BLUE.to_333());
    matrix.print("GAME ");
    matrix.setCursor(4,8);
    matrix.setTextColor(YELLOW.to_333());
    matrix.print("OVER");
    delay(1000000);
}

void win_game() {
    matrix.fillScreen(BLACK.to_333());
    matrix.setCursor(7,0);
    matrix.setTextSize(1);
    matrix.setTextColor(BLUE.to_333());
    matrix.print("YOU ");
    matrix.setCursor(5,8);
    matrix.setTextColor(YELLOW.to_333());
    matrix.print("WIN!");
    delay(1000000);
}
