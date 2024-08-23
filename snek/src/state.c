#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_state_t *state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t *state, unsigned int snum);
static char next_square(game_state_t *state, unsigned int snum);
static void update_tail(game_state_t *state, unsigned int snum);
static void update_head(game_state_t *state, unsigned int snum);

/* Task 1 */
game_state_t *create_default_state() {
  // TODO: Implement this function.
  game_state_t *default_state = (game_state_t *)malloc(sizeof(game_state_t));
  
  default_state->num_rows = 18;
  default_state->num_snakes = 1;
  default_state->snakes = (snake_t *)malloc(sizeof(snake_t));
  default_state->board = (char **)malloc(18*sizeof(char*));
  if (!default_state->board){
      free(default_state->board);
      free(default_state->snakes);
      free(default_state);
  }

  for (unsigned int y = 0; y < default_state->num_rows; y++) {
      default_state->board[y] = (char *)malloc(21*sizeof(char));
      for (unsigned int x = 0; x < 20; x++) {
          
          if (default_state->board[y] == NULL) {
              free_state(default_state);
          }
          if (x == 0 || y == 0 || y == default_state->num_rows-1 || x == 19) {
              set_board_at(default_state, y, x, '#');
          }
          else {
              set_board_at(default_state, y, x, ' ');
          }
        
      }
      
      default_state->board[y][21] = '\0';
      default_state->board[y][20]= '\n';
  }
  
  
  set_board_at(default_state, 2, 9, '*');
  set_board_at(default_state, 2, 2, 'd');
  set_board_at(default_state, 2, 3, '>');
  set_board_at(default_state, 2, 4, 'D');

  snake_t* snake = default_state->snakes;
  snake->head_row = 2;
  snake->head_col = 4;
  snake->tail_row = 2;
  snake->tail_col = 2;
  snake->live = true;

  
  return default_state;
}
/* Task 2 */
void free_state(game_state_t *state) {
  // TODO: Implement this function.

  for (unsigned int y = 0; y < state->num_rows; y++) {
      free(state->board[y]);
  }
  free(state->board);
  free(state->snakes);
  free(state);
  return;
}

/* Task 3 */
void print_board(game_state_t *state, FILE *fp) {
  // TODO: Implement this function.
  for (unsigned int y = 0; y < state->num_rows; y++) {
      fprintf(fp, "%s", state->board[y]);
  }
  return;
}

/*
  Saves the current state into filename. Does not modify the state object.
  (already implemented for you).
*/
void save_board(game_state_t *state, char *filename) {
  FILE *f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_state_t *state, unsigned int row, unsigned int col) { return state->board[row][col]; }

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_state_t *state, unsigned int row, unsigned int col, char ch) {
  state->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
  // TODO: Implement this function.
  if (c == 'w' || c == 'a' || c == 's' || c == 'd') {
      return true;
  }
  return false;
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
  // TODO: Implement this function.
  if (c == 'W' || c == 'A' || c == 'S' || c == 'D' || c == 'x') {
      return true;
  }
  return false;
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  // TODO: Implement this function.
  if (is_tail(c) || is_head(c) || c == '^' || c == '<' || c == 'v' || c == '>'){
      return true;
  }
  return false;

}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c) {
  // TODO: Implement this function.
  if (c == '^') {
      return 'w';
  }
  if (c == '>') {
      return 'd';
  }
  if (c == '<') {
      return 'a';
  }
  if (c == 'v') {
      return 's';
  }
  

  return '?';
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c) {
  // TODO: Implement this function.
  if (c == 'W') {
      return '^';
  }
  if (c == 'A') {
      return '<';
  }
  if (c == 'S') {
      return 'v';
  }
  if (c == 'D') {
      return '>';
  }
  


  return '?';
}

/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  // TODO: Implement this function.
  if (c == 'v' || c == 's' || c == 'S') {
      return cur_row+1;
  }
  if (c == '^' || c == 'w' || c == 'W') {
      return cur_row-1;
  }

  return cur_row;
}

/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  // TODO: Implement this function.
  if (c == '>' || c == 'd' || c == 'D') {
      return cur_col+1;
  }
  if (c == '<' || c == 'a' || c == 'A') {
      return cur_col-1;
  }


  return cur_col;
}

/*
  Task 4.2

  Helper function for update_state. Return the character in the cell the snake is moving into.

  This function should not modify anything.
*/
static char next_square(game_state_t *state, unsigned int snum) {
  // TODO: Implement this function.
  
  /*
  char schar = get_board_at(state, state->snakes->head_row, state->snakes->head_col);
  unsigned int next_row = get_next_row(state->snakes->head_row, schar);
  unsigned int next_col = get_next_col(state->snakes->head_col, schar);

  return get_board_at(state, next_row, next_col);
  
  
  unsigned int curr_row = state->snakes[snum].head_row;
  unsigned int curr_col = state->snakes[snum].head_col;

  char schar = get_board_at(state, curr_row, curr_col);

  unsigned int next_row = get_next_row(curr_row, schar);
  unsigned int next_col = get_next_col(curr_col, schar);

  return get_board_at(state, next_row, next_col);
*/
    char plswork = get_board_at(state, state->snakes[snum].head_row, state->snakes[snum].head_col);
    return get_board_at(state, get_next_row(state->snakes[snum].head_row, plswork), get_next_col(state->snakes[snum].head_col, plswork)); 


}

/*
  Task 4.3

  Helper function for update_state. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the head.
*/
static void update_head(game_state_t *state, unsigned int snum) {
  // TODO: Implement this function.
  
  char schar = get_board_at(state, state->snakes[snum].head_row, state->snakes[snum].head_col);
  unsigned int next_row = get_next_row(state->snakes[snum].head_row, schar);
  unsigned int next_col = get_next_col(state->snakes[snum].head_col, schar);
  
  set_board_at(state, next_row, next_col, schar);
  set_board_at(state, state->snakes[snum].head_row, state->snakes[snum].head_col, head_to_body(schar));

  state->snakes[snum].head_row = next_row;
  state->snakes[snum].head_col = next_col;


  return;
}

/*
  Task 4.4

  Helper function for update_state. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_state_t *state, unsigned int snum) {
  // TODO: Implement this function.
  snake_t* snake = &state->snakes[snum];
  char schar = get_board_at(state, snake->tail_row, snake->tail_col);
  
  unsigned int next_row = get_next_row(snake->tail_row, schar);
  unsigned int next_col = get_next_col(snake->tail_col, schar);
  
  char old_body = get_board_at(state, next_row, next_col);
  char new_tail = body_to_tail(old_body);

  set_board_at(state, next_row, next_col, new_tail);
  set_board_at(state, snake->tail_row, snake->tail_col, ' ');

  snake->tail_row = next_row;
  snake->tail_col = next_col;

  return;
}

/* Task 4.5 */
void update_state(game_state_t *state, int (*add_food)(game_state_t *state)) {
  // TODO: Implement this function.
  for (unsigned int i = 0; i < state->num_snakes; i++) {
      char nchar = next_square(state, i);

      if (is_snake(nchar) || nchar == '#') {
          set_board_at(state, state->snakes[i].head_row, state->snakes[i].head_col, 'x');
          state->snakes[i].live = false;
          return;
      }// snake hits snake body or wall
      else if (nchar == '*') {
          update_head(state, i);
          add_food(state);
      } // snake gets fruit
      else {
          update_head(state, i);
          update_tail(state, i);
      } // move as usual
  }
  return;
}

/* Task 5.1 */
char *read_line(FILE *fp) {
  // TODO: Implement this function.
  
  char str[999999];
  char* line = NULL;
  while (fgets(str, sizeof(str), fp) != NULL) {
      size_t len = strlen(str);
      line = (char*) realloc(line, (len+1)*sizeof(char));

      strcpy(line, str);
      if (str[len-1] != '\0'){
          break;}

      line[strlen(line)-1] = '\0';
  }

  return line;
}

/* Task 5.2 */
game_state_t *load_board(FILE *fp) {
  // TODO: Implement this function.
  game_state_t *state = (game_state_t*)malloc(sizeof(game_state_t));
  state->num_snakes = 0;
  state->snakes = NULL;
  state->board = NULL;
  

  unsigned int row = 0;
  char* line = NULL;
  
  while((line = read_line(fp)) != NULL){
      row++;
      state->board = (char**)realloc(state->board, row * sizeof(char*));
      state->board[row-1] = line;
  }
  state->num_rows = row;
 

    

  return state;
}

/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_state_t *state, unsigned int snum) {
  // TODO: Implement this function.
  //

  unsigned int row = state->snakes[snum].tail_row;
  unsigned int col = state->snakes[snum].tail_col;
  char snek = get_board_at(state, row, col);

  while (is_snake(snek) && !is_head(snek)){
      row = get_next_row(row, snek);
      col = get_next_col(col, snek);
      snek = get_board_at(state, row, col);
  }

  state->snakes[snum].head_row = row;
  state->snakes[snum].head_col = col;

  return;
}

/* Task 6.2 */
game_state_t *initialize_snakes(game_state_t *state) {
  // TODO: Implement this function.
  state->num_snakes = 0;
  
  

  for (unsigned int y = 0; y < state->num_rows; y++) {
      for (unsigned int x = 0; x < strlen(state->board[y]); x++) {
        
        char board = get_board_at(state, y, x);
        if (is_tail(board)) {
            state->snakes = realloc(state->snakes, (state->num_snakes+1)*sizeof(snake_t));

            state->snakes[state->num_snakes].tail_row = y;
            state->snakes[state->num_snakes].tail_col = x;

            find_head(state, state->num_snakes);
            state->num_snakes++;

        
        }
        
      }
  }
  

  return state;
}
