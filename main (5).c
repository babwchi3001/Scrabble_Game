//------------------------------------------------------------------------------
// a3.c
//
// In this program, we simulate the Scrabble game, from which, the
// characteristics and field are obtained from a config file.
//
//
// Group: Lena Klambauer
//
// Author: 11937605
//------------------------------------------------------------------------------
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "framework.h"

#define ALLOWED_ARGUMENTS 2
#define SUCCESS 0
#define WRONG_ARGUMENTS_NR 1
#define CANNOT_OPEN_CONFIG_FILE 2
#define INVALID_CONFIG_FILE 3
#define OUT_MEMORY_ERROR 4

typedef struct _Word_ {
  char letter_;
  int letter_points_;
} Word;

// forward declarations
char** getConfigContent(FILE* config_text, int* return_value,
                        char** char_points_string,
                        int* player1_points, int* player2_points,
                        int* field_size, int* player_turn);
char* getLetterPoints(FILE* config_text);
int charToDecimal(FILE* config_text);
char** configToArray(FILE* config_text, int* return_value,
                     int* field_size, int* player_turn);
void gamePlayStart(char** file_elements_array, char* char_points_string,
                   int player1_points,
                   int player2_points, int field_size, int player_turn,
                   int* memory_error, char *config_name);
void printHelpCommand();
char* gamePlayInput();
Word** initializeGameField(char** file_elements_array,
                           char* char_points_string, int field_size);
void printLetterPlayerPoints(char* char_points_string, int player1_points,
                             int player2_points);
void gameProgressPrint(Word** game_play_field, char* char_points_string,
                       int field_size, int player1_points, int player2_points);
int gamePlaySaveCommand(char* config_name, Word** game_play_field,
                        char* char_points_string, int field_size,
                        int player1_points, int player2_points,
                        int player_turn);
int checkWordInput(Input* player_input, const char* char_points_string);
int checkEmptyField(Word** game_play_field, int field_size);
int wordPlacementCheck(Word** game_play_field, Input* player_input,
                       char* char_points_string, int field_size);
int pointLetterInput(char word_char, const char* char_points_string);
int gamePlayInsertCommand(Word** game_play_field, Input* player_input,
                          char* char_points_string, int field_size,
                          int* points_won);

//------------------------------------------------------------------------------
///
/// In the main function. We are implementing our Game program. By calling
/// the needed functions.
///
/// @return SUCCESS meaning the code ended without a problem.
/// @return OUT_MEMORY_ERROR if the memory could not be allocated.
/// @return CANNOT_OPEN_CONFIG_FILE if the file we called cannot be opened.
/// @return WRONG_ARGUMENTS_NR if the number of arguments exceedes 2.
/// @return INVALID_CONFIG_FILE if the file doesn't start with "Scrabble".
//
int main(int argc, char** argv)
{
  if(argc != ALLOWED_ARGUMENTS)
  {
    printf("Usage: ./a3 configfile\n");
    return WRONG_ARGUMENTS_NR;
  }
  int config_id = argc - 1;
  char* config_name = argv[config_id];

  FILE* config_text = fopen(config_name, "r");
  if(config_text == NULL)
  {
    printf("Error: Cannot open file: %s\n", config_name);
    return CANNOT_OPEN_CONFIG_FILE;
  }

  char* char_points_string = NULL;
  int player1_points = 0;
  int player2_points = 0;
  int field_size = 0;
  int player_turn = 0;
  int return_value = 0;
  char** file_elements_array = getConfigContent(config_text, &return_value,
                                                &char_points_string,
                                                &player1_points,
                                                &player2_points,
                                                &field_size,
                                                &player_turn);
  if(file_elements_array == NULL)
  {
    if(return_value == INVALID_CONFIG_FILE)
    {
      printf("Error: Invalid file: %s\n", config_name);
      return INVALID_CONFIG_FILE;
    }
    else if(return_value == OUT_MEMORY_ERROR)
    {
      printf("Error: Out of memory\n");
      return OUT_MEMORY_ERROR;
    }
  }

  int memory_error = 0;
  gamePlayStart(file_elements_array, char_points_string, player1_points,
                player2_points, field_size, player_turn, &memory_error,
                config_name);
  if(memory_error == OUT_MEMORY_ERROR)
  {
    printf("Error: Out of memory\n");
    return OUT_MEMORY_ERROR;
  }

  return SUCCESS;
}

//------------------------------------------------------------------------------
///
/// The charToDecimal is used to convert a string nt to an integer.
///
/// @param FILE* config_text the given file pointer.
///
/// @return sum meaning the code ended without a problem.
//
int charToDecimal(FILE* config_text)
{
  int char_to_int = 48;
  char new_line = '\n';
  int decimal = 10;
  int sum = 0;
  int file_char = 0;
  while((file_char = fgetc(config_text)) != new_line)
  {
    // calculating digit value
    int digit_value = file_char - char_to_int;
    sum *= decimal;
    sum += digit_value;
  }
  return sum;
}

//------------------------------------------------------------------------------
///
/// In main function getConfigContent, the content of the config file is saved
/// in different variables with the help of some other functions.
///
/// @param config_text the given file pointer.
/// @param return_value used to return a certain exit code in case of problems.
/// @param char_points_string used to get the amount of points per each input.
/// @param player1_points holds the value of the points for player 1.
/// @param player2_points holds the value of the points for player 2.
/// @param field_size holds the size of the field.
/// @param player_turn shows whos turn it is.
///
/// @return NULL in case of problems.
/// @return file_elements_array which holds the file in a string format.
//
char** getConfigContent(FILE* config_text, int* return_value,
                        char** char_points_string, int* player1_points,
                        int* player2_points, int* field_size,
                        int* player_turn)
{
  // check magic nr
  int magic_nr_size = 8;
  char magic_nr[] = "Scrabble";

  fseek(config_text, 0, SEEK_SET);

  char new_line = '\n';
  int wrong_config_content = 0;
  int counter = 0;
  int file_char = 0;
  while((file_char = fgetc(config_text)) != new_line)
  {
    if((counter >= magic_nr_size) || (file_char != magic_nr[counter]))
    {
      wrong_config_content = 1;
    }
    counter++;
  }
  if(wrong_config_content)
  {
    fclose(config_text);
    *return_value = INVALID_CONFIG_FILE;
    return NULL;
  }

  char** file_elements_array = configToArray(config_text, return_value,
                                             field_size, player_turn);
  if(file_elements_array == NULL)
  {
    fclose(config_text);
    return NULL;
  }

  *player1_points = charToDecimal(config_text);
  *player2_points = charToDecimal(config_text);

  *char_points_string = getLetterPoints(config_text);
  if(char_points_string == NULL)
  {
    fclose(config_text);
    int free_index = 0;
    for(free_index = 0; free_index < *field_size; free_index++)
      free(file_elements_array[free_index]);
    free(file_elements_array);
    *return_value = OUT_MEMORY_ERROR;
    return NULL;
  }

  fclose(config_text);

  return file_elements_array;
}

//------------------------------------------------------------------------------
///
/// In the function getLetterPoints, we return the last part of the config file
/// as a string. It contains all the letters and their respective points.
///
/// @param config_text the given file pointer.
///
/// @return NULL in case of problems.
/// @return file_elements holding the points for each letter.
//
char* getLetterPoints(FILE* config_text)
{
  char eos = '\0';
  char new_line = '\n';
  int malloc_init = 1;
  char* file_elements = (char*)malloc(malloc_init * sizeof(char));
  if(file_elements == NULL)
    return NULL;

  int index = 0;
  int file_char = 0;
  while((file_char = fgetc(config_text)) != EOF)
  {
    if(file_char == new_line)
    {
      file_elements[index] = eos;
      break;
    }
    file_elements[index] = (char)file_char;
    index++;
    char* temp_pointer = (char*)realloc(
        file_elements,(index + malloc_init) * sizeof(char));
    if(temp_pointer == NULL)
    {
      free(file_elements);
      return NULL;
    }
    file_elements = temp_pointer;

  }
  if(file_char == EOF)
    file_elements[index] = eos;

  return file_elements;
}

//------------------------------------------------------------------------------
///
/// In the function configToArray, we save the game field from the config file
/// in a double char pointer.
///
/// @param config_text the given file pointer.
/// @param return_value used to return a certain exit code in case of problems.
/// @param field_size holds the size of the field.
/// @param player_turn shows whos turn it is.
///
/// @return NULL in case of problems.
/// @return file_elements_array which holds the file in a string format.
//
char** configToArray(FILE* config_text, int* return_value, int* field_size,
                     int* player_turn)
{
  int min_field_size = 4;
  int max_field_size = 26;
  int char_to_int = 48;
  int zero = 48;
  int nine = 57;
  char eos = '\0';
  char new_line = '\n';
  int file_char = 0;
  int malloc_init = 1;

  char** file_elements_array = (char**)malloc(
      malloc_init * sizeof(char*));
  if(file_elements_array == NULL)
  {
    *return_value = OUT_MEMORY_ERROR;
    return NULL;
  }
  char* file_elements = (char*)malloc(malloc_init * sizeof(char));
  if(file_elements == NULL)
  {
    free(file_elements_array);
    *return_value = OUT_MEMORY_ERROR;
    return NULL;
  }

  int index = 0;
  int outer_index = 0;
  while((file_char = fgetc(config_text)) != EOF)
  {
    if(file_char == new_line)
    {
      file_elements[index] = eos;
      file_elements_array[outer_index] = file_elements;
      outer_index++;

      int next_character = fgetc(config_text);
      if((next_character >= zero) && (next_character <= nine))
      {
        *player_turn = next_character - char_to_int;
        break;
      }
      fseek(config_text, -1, SEEK_CUR);

      char** temp_double_pointer = (char**)realloc(
          file_elements_array,(outer_index + malloc_init) * sizeof(char*));
      if(temp_double_pointer == NULL)
      {
        int free_index = 0;
        for(free_index = 0; free_index < 120; free_index++)
          free(file_elements_array[free_index]);
        free(file_elements_array);
        *return_value = OUT_MEMORY_ERROR;
        return NULL;
      }
      file_elements_array = temp_double_pointer;

      file_elements = (char*)malloc(malloc_init * sizeof(char));
      if(file_elements == NULL)
      {
        int free_index = 0;
        for(free_index = 0; free_index < outer_index; free_index++)
          free(file_elements_array[free_index]);
        free(file_elements_array);
        *return_value = OUT_MEMORY_ERROR;
        return NULL;
      }
      index = 0;
      continue;
    }

    file_elements[index] = (char)file_char;
    index++;
    char* temp_pointer = (char*)realloc(
        file_elements, (index + malloc_init) * sizeof(char));
    if(temp_pointer == NULL)
    {
      int free_index = 0;
      for(free_index = 0; free_index < outer_index; free_index++)
        free(file_elements_array[free_index]);
      free(file_elements_array);
      free(file_elements);
      *return_value = OUT_MEMORY_ERROR;
      return NULL;
    }
     file_elements = temp_pointer;
  }
  // consume '\n' after field
  fgetc(config_text);
  if((outer_index > max_field_size) ||
     outer_index < min_field_size)
  {
    int free_index = 0;
    for(free_index = 0; free_index < outer_index; free_index++)
      free(file_elements_array[free_index]);
    free(file_elements_array);
    *return_value = INVALID_CONFIG_FILE;
    return NULL;
  }

  *field_size = outer_index;
  return file_elements_array;
}

//------------------------------------------------------------------------------
///
/// In the function gamePlayStart, we implement all the game play commands
/// and logic.
///
/// @param file_elements_array which holds the file in a string format
/// @param config_name name of config file.
/// @param memory_error used to return a certain exit code in case of problems.
/// @param char_points_string used to get the amount of points per each input.
/// @param player1_points holds the value of the points for player 1.
/// @param player2_points holds the value of the points for player 2.
/// @param field_size holds the size of the field.
/// @param player_turn shows whos turn it is.
///
/// @return
//
void gamePlayStart(char** file_elements_array, char* char_points_string,
                   int player1_points, int player2_points, int field_size,
                   int player_turn, int* memory_error, char* config_name)
{
  Word** game_play_field = initializeGameField(file_elements_array,
                                               char_points_string, field_size);
  if(game_play_field == NULL)
  {
    *memory_error = OUT_MEMORY_ERROR;
    return;
  }
  int winning_points = (field_size * field_size)/2;

  char eos = '\0';
  int printing_check = 1;
  int player_1 = 1;
  int player_2 = 2;
  int game_loop = 1;
  while(game_loop)
  {
    int change_player_flag = 1;

    if(printing_check)
      gameProgressPrint(game_play_field, char_points_string,
                        field_size, player1_points, player2_points);
    printing_check = 0;
    Input* player_input = (Input*)malloc(sizeof(Input));
    if(player_input == NULL)
    {
      *memory_error = OUT_MEMORY_ERROR;
      break;
    }
    printf("Player %d > ", player_turn);
    char* game_input = gamePlayInput();
    if(game_input == NULL)
    {
      free(player_input);
      *memory_error = OUT_MEMORY_ERROR;
      break;
    }
    if(game_input[0] == eos)
    {
      free(game_input);
      free(player_input);
      break;
    }

    parseCommand(game_input, player_input);

    if((player_input->is_error_) &&
       (player_input->command_ != UNKNOWN))
    {
      printf("Error: Insert parameters not valid!\n");
      free(game_input);
      free(player_input);
      continue;
    }

    if(player_input->command_ == INSERT)
    {
      printing_check = 1;
      int error_return_value = 1;
      int error_invalid_param = 2;
      int points_won = 0;
      int return_value = gamePlayInsertCommand(game_play_field, player_input,
                                               char_points_string, field_size,
                                               &points_won);
      if(return_value != SUCCESS)
      {
        if(return_value == error_return_value)
          printf("Error: Impossible move!\n");
        if(return_value == error_invalid_param)
          printf("Error: Insert parameters not valid!\n");
        printing_check = 0;
        change_player_flag = 0;
      }
      else
      {
        if(player_turn == player_1)
          player1_points += points_won;
        else
          player2_points += points_won;
      }
    }
    else if(player_input->command_ == SAVE)
    {
      int return_value = gamePlaySaveCommand(config_name, game_play_field,
                                             char_points_string, field_size,
                                             player1_points, player2_points,
                                             player_turn);
      if(return_value == CANNOT_OPEN_CONFIG_FILE)
        printf("Error: Could not save to file!\n");
      change_player_flag = 0;
    }
    else if(player_input->command_ == QUIT)
    {
      game_loop = 0;
    }
    else if(player_input->command_ == HELP)
    {
      printHelpCommand();
      change_player_flag = 0;
    }
    else
    {
      char* command_wrong = strtok(game_input, TOKEN_SEPARATORS);
      printf("Error: Unknown command: %s\n", command_wrong);
      change_player_flag = 0;
    }
    if(change_player_flag)
    {
      if(player_turn != player_1)
        player_turn = player_1;
      else
        player_turn = player_2;
    }

    free(game_input);
    free(player_input->word_);
    free(player_input);

    if(player1_points >= winning_points)
    {
      printf("Player %d has won the game with %d points!\n",
             player_1, player1_points);
      break;
    }
    if(player2_points >= winning_points)
    {
      printf("Player %d has won the game with %d points!\n",
             player_2, player2_points);
      break;
    }
  }
  free(char_points_string);
  int outer_iterator = 0;
  for(outer_iterator = 0; outer_iterator < field_size; outer_iterator++)
    free(game_play_field[outer_iterator]);
  free(game_play_field);
}

//------------------------------------------------------------------------------
///
/// In the function initializeGameField, we set the gameplay field, were each
/// place of the field is a struct containing a letter and its points.
///
/// @param file_elements_array which holds the file in a string format
/// @param char_points_string used to get the amount of points per each input.
/// @param field_size holds the size of the field.
///
/// @return NULL in case of problems.
/// @return game_play_field gives the state of the playing field.
//
Word** initializeGameField(char** file_elements_array, char* char_points_string,
                           int field_size)
{
  int char_to_decimal = 48;
  char space = ' ';
  char eos = '\0';

  Word** game_play_field = (Word**)malloc(field_size * sizeof(Word*));
  if(game_play_field == NULL)
  {
    int outer_iterator = 0;
    for(outer_iterator = 0; outer_iterator < field_size; outer_iterator++)
      free(file_elements_array[outer_iterator]);
    free(file_elements_array);
    free(char_points_string);
    return NULL;
  }

  int outer_iterator = 0;
  for(outer_iterator = 0; outer_iterator < field_size; outer_iterator++)
  {
    Word* game_play_row = (Word*)malloc(field_size * sizeof(Word));
    if(game_play_row == NULL)
    {
      int free_iterator = 0;
      for(free_iterator = 0; free_iterator < field_size; free_iterator++)
        free(file_elements_array[free_iterator]);
      free(file_elements_array);
      free(char_points_string);

      for(free_iterator = 0; free_iterator < outer_iterator; free_iterator++)
        free(game_play_field[free_iterator]);
      free(game_play_field);
      return NULL;
    }

    int inner_iterator = 0;
    for(inner_iterator = 0; inner_iterator < field_size; inner_iterator++)
    {
      game_play_row[inner_iterator].letter_ = space;
      game_play_row[inner_iterator].letter_points_ = 0;
    }
    game_play_field[outer_iterator] = game_play_row;
  }

  for(outer_iterator = 0; outer_iterator < field_size; outer_iterator++)
  {
    int inner_iterator = 0;
    for(inner_iterator = 0;
        file_elements_array[outer_iterator][inner_iterator] != eos;
        inner_iterator++)
    {
      char actual_character =
          file_elements_array[outer_iterator][inner_iterator];

      if(actual_character != space)
      {
        game_play_field[outer_iterator][inner_iterator].letter_ =
            actual_character;

        int point_iterate = 0;
        for(point_iterate = 0; char_points_string[point_iterate] != eos;
            point_iterate++)
        {
          if(char_points_string[point_iterate] == tolower(actual_character))
          {
            int points_char =
                (int)char_points_string[point_iterate + 1] - char_to_decimal;
            game_play_field[outer_iterator][inner_iterator].letter_points_ =
                points_char;
            break;
          }
        }
      }
    }
  }
  // free config string
  for(outer_iterator = 0; outer_iterator < field_size; outer_iterator++)
    free(file_elements_array[outer_iterator]);
  free(file_elements_array);

  return game_play_field;
}

//------------------------------------------------------------------------------
///
/// In the function gamePlayInput, we get the game input from the console and
/// return it as a string.
///
/// @return NULL in case of problems.
/// @return game_input which is our input.
//
char* gamePlayInput()
{
  char eos = '\0';
  char new_line = '\n';
  int malloc_init = 1;
  char* game_input = (char*)malloc(malloc_init * sizeof(char));
  if(game_input == NULL)
    return NULL;

  int input_counter = 0;
  char input_char = 0;
  while((input_char = (char)getchar()) != new_line)
  {
    if(input_char == EOF)
      break;

    game_input[input_counter] = (char)tolower(input_char);

    input_counter++;
    char* temp_pointer = (char*)realloc(
        game_input,(input_counter + malloc_init) * sizeof(char));
    if(temp_pointer == NULL)
    {
      free(game_input);
      return NULL;
    }
    game_input = temp_pointer;
  }
  game_input[input_counter] = eos;
  return game_input;
}

//------------------------------------------------------------------------------
///
/// In the function printHelpCommand, we print the possible game commands.
///
/// @return
//
void printHelpCommand()
{
  printf("Commands:\n"
         " - insert <ROW> <COLUMN> <H/V> <WORD>\n"
         "    <H/V> stands for H: horizontal, V: vertical.\n"
         "\n"
         " - help\n"
         "    Prints this help text.\n"
         "\n"
         " - quit\n"
         "    Terminates the game.\n"
         "\n"
         " - save\n"
         "    Saves the game to the current config file.\n"
         "\n"
         " - load <CONFIGFILE>\n"
         "    load config file and start game.\n");
}

//------------------------------------------------------------------------------
///
/// In the function printLetterPlayerPoints, we print the first part of the
/// game state.
///
/// @param char_points_string used to get the amount of points per each input.
/// @param player1_points holds the value of the points for player 1.
/// @param player2_points holds the value of the points for player 2.
///
/// @return
//
void printLetterPlayerPoints(char* char_points_string, int player1_points,
                             int player2_points)
{
  printf("\n");
  // print char - points max per line 9
  char eos = '\0';
  char space = ' ';
  int space_max = 9;
  int space_counter = 0;
  int print_iterator = 0;
  for(print_iterator = 0; char_points_string[print_iterator] != eos;
      print_iterator++)
  {
    if(char_points_string[print_iterator] == space)
    {
      printf(",");
      space_counter++;
    }

    printf("%c", toupper((int)char_points_string[print_iterator]));

    if(space_counter == space_max)
    {
      printf("\n");
      space_counter = 0;
    }
  }
  if(space_counter != space_max)
    printf("\n");

  printf("  P1:%5d Points\n", player1_points);
  printf("  P2:%5d Points\n", player2_points);
}

//------------------------------------------------------------------------------
///
/// In the function gameProgressPrint, we print the current game state.
///
/// @param field_size holds the size of the field
/// @param char_points_string used to get the amount of points per each input.
/// @param player1_points holds the value of the points for player 1.
/// @param player2_points holds the value of the points for player 2.
/// @param game_play_field holds the actual state of the game field.
///
/// @return
//
void gameProgressPrint(Word** game_play_field, char* char_points_string,
                       int field_size, int player1_points, int player2_points)
{
  printLetterPlayerPoints(char_points_string, player1_points, player2_points);

  // print upper part of field
  int print_iterator = 0;
  int start_letter_a = 65;
  for(print_iterator = 0; print_iterator < field_size; print_iterator++)
  {
    if(print_iterator == 0)
      printf(" |");
    printf("%c", start_letter_a + print_iterator);
  }
  printf("\n");
  int new_field_size = field_size + 2;
  for(print_iterator = 0; print_iterator < new_field_size; print_iterator++)
    printf("-");
  printf("\n");

  // print field content
  int outer_iterator = 0;
  for(outer_iterator = 0; outer_iterator < field_size; outer_iterator++)
  {
    printf("%c|", start_letter_a + outer_iterator);
    int inner_iterator = 0;
    for(inner_iterator = 0; inner_iterator < field_size; inner_iterator++)
    {
      printf("%c",
             game_play_field[outer_iterator][inner_iterator].letter_);
    }
    printf("\n");
  }
  printf("\n");
}

//------------------------------------------------------------------------------
///
/// In the function gamePlaySaveCommand, saves the game field, player turn,
/// players points and char-points string to the old config file.
/// It overwrites it with the new content.
///
/// @param field_size holds the size of the field
/// @param char_points_string used to get the amount of points per each input.
/// @param player1_points hold the value of the points for player 1.
/// @param player2_points hold the value of the points for player 2.
/// @param config_name name of config file.
/// @param game_play_field holds the actual state of the game field.
/// @param player_turn shows whos turn it is.
///
/// @return CANNOT_OPEN_CONFIG_FILE if the file cannot be opened.
/// @return SUCCESS if no problems were detected.
//
int gamePlaySaveCommand(char* config_name, Word** game_play_field,
                        char* char_points_string, int field_size,
                        int player1_points, int player2_points,
                        int player_turn)
{
  char magic_nr[] = "Scrabble";

  FILE* config_text = fopen(config_name, "w");
  if(config_text == NULL)
    return CANNOT_OPEN_CONFIG_FILE;

  fputs(magic_nr, config_text);
  fputs("\n", config_text);

  int outer_iterator = 0;
  for(outer_iterator = 0; outer_iterator < field_size; outer_iterator++)
  {
    int inner_iterator = 0;
    for(inner_iterator = 0; inner_iterator < field_size; inner_iterator++)
    {
      fputc(game_play_field[outer_iterator][inner_iterator].letter_,
            config_text);
    }
    fputs("\n", config_text);
  }
  fprintf(config_text,"%d\n", player_turn);
  fprintf(config_text,"%d\n", player1_points);
  fprintf(config_text,"%d\n", player2_points);
  fputs(char_points_string, config_text);

  fclose(config_text);
  return SUCCESS;
}

//------------------------------------------------------------------------------
///
/// In the function checkWordInput, we check if the word input can be placed
/// on the game field or not.
///
/// @param char_points_string used to get the amount of points per each input.
/// @param player_input the given input.
///
/// @return SUCCESS if no problems were detected.
//
int checkWordInput(Input* player_input, const char* char_points_string)
{
  char eos = '\0';
  int error_return_value = 1;
  int letter_exists_count = 0;
  int word_iterator = 0;
  for(word_iterator = 0; player_input->word_[word_iterator] != eos;
      word_iterator++)
  {
    int word_point_iterator = 0;
    for(word_point_iterator = 0;
        char_points_string[word_point_iterator] != eos; word_point_iterator++)
    {
      if(player_input->word_[word_iterator] ==
         char_points_string[word_point_iterator])
        letter_exists_count++;
    }
  }
  int word_input_size = (int)strlen(player_input->word_);
  if(letter_exists_count != word_input_size)
    return error_return_value;
  return SUCCESS;
}

//------------------------------------------------------------------------------
///
/// In the function checkEmptyField, we check if the game field is empty
/// or not.
///
/// @param game_play_field holds the current state of the game field.
/// @param field_size holds the size of the field.
///
/// @return SUCCESS if no problems were detected.
/// @return empty_field if no letters are found.
//
int checkEmptyField(Word** game_play_field, int field_size)
{
  char space = ' ';
  int empty_field = 1;
  int outer_iterator = 0;
  for(outer_iterator = 0; outer_iterator < field_size; outer_iterator++)
  {
    int inner_iterator = 0;
    for(inner_iterator = 0; inner_iterator < field_size; inner_iterator++)
    {
      char field_char = game_play_field[outer_iterator][inner_iterator].letter_;
      if(field_char != space)
        return SUCCESS;
    }
  }
  return empty_field;
}

//------------------------------------------------------------------------------
///
/// In the function wordPlacementCheck, we check if the word input
/// can be placed on the game field or not. It is done by colling
/// different help functions and if else checks.
///
/// @param game_play_field holds the current state of the game field.
/// @param field_size holds the size of the field.
/// @param player_input the given input.
/// @param char_points_string used to get the amount of points per each input.
///
/// @return SUCCESS if no problems were detected.
/// @return error_return_value
//
int wordPlacementCheck(Word** game_play_field, Input* player_input,
                       char* char_points_string, int field_size)
{
  char eos = '\0';
  char space = ' ';
  int error_return_value = 1;
  int error_invalid_param = 2;
  int char_to_coordinate = 97;
  int small_a = 97;
  int small_z = 122;

  // check word placement bigger than field
  int word_start_position = 0;
  int word_size = (int)strlen(player_input->word_);
  if(player_input->orientation_)
    word_start_position = player_input->row_ - char_to_coordinate;
  else
    word_start_position = player_input->column_ - char_to_coordinate;

  int word_position_length = word_start_position + word_size;
  if(word_position_length > field_size)
    return error_return_value;

  // check word content
  int word_iterator = 0;
  for(word_iterator = 0; player_input->word_[word_iterator] != eos;
      word_iterator++)
  {
    char word_char =  player_input->word_[word_iterator];
    if((word_char < small_a) || (word_char > small_z))
      return error_return_value;
  }
  int check_word_input = checkWordInput(player_input, char_points_string);
  if(check_word_input)
    return error_return_value;

  // check coordinates content
  if((player_input->row_ < small_a) || (player_input->row_ > small_z))
    return error_invalid_param;
  if((player_input->column_ < small_a) || (player_input->column_ > small_z))
    return error_invalid_param;
  if(((player_input->row_ - char_to_coordinate) >= field_size) ||
     ((player_input->column_ - char_to_coordinate) >= field_size))
    return error_invalid_param;

  // if field is empty dont do the extra field check
  int field_empty = checkEmptyField(game_play_field, field_size);
  if(field_empty)
    return SUCCESS;

  // check word placement on field
  int row_coordinate = player_input->row_ - char_to_coordinate;
  int column_coordinate = player_input->column_ - char_to_coordinate;

  int wrong_placement = 1;
  for(word_iterator = 0; player_input->word_[word_iterator] != eos;
      word_iterator++)
  {
    char word_char =  player_input->word_[word_iterator];

    char field_char =
        game_play_field[row_coordinate][column_coordinate].letter_;
    field_char = (char)tolower(field_char);

    if((field_char != word_char) && (field_char != space))
      return error_return_value;

    if(field_char == word_char)
      wrong_placement = 0;

    if(player_input->orientation_)
      row_coordinate++;
    else
      column_coordinate++;
  }
  if(wrong_placement)
    return error_return_value;

  return SUCCESS;
}

//------------------------------------------------------------------------------
///
/// In the function pointLetterInput, we get the points representing a
/// specific letter. It is done by parsing the char_points_string variable.
///
/// @param word_char
/// @param char_points_string used to get the amount of points per each input.
///
/// @return word_points
//
int pointLetterInput(char word_char, const char* char_points_string)
{
  char eos = '\0';
  int char_to_int = 48;
  int word_points = 0;

  int word_point_iterator = 0;
  for(word_point_iterator = 0; char_points_string[word_point_iterator] != eos;
      word_point_iterator++)
  {
    if(word_char== char_points_string[word_point_iterator])
    {
      word_points = char_points_string[word_point_iterator + 1] - char_to_int;
    }
  }
  return word_points;
}

//------------------------------------------------------------------------------
///
/// In the function gamePlayInsertCommand, we implement the insert command
/// logic and its needed checks.
///
/// @param game_play_field holds the current state of the game field.
/// @param field_size holds the size of the field.
/// @param player_input the given input.
/// @param char_points_string used to get the amount of points per each input.
/// @param points_won
///
/// @return SUCCESS if no problems were detected.
/// @return return_value
//
int gamePlayInsertCommand(Word** game_play_field, Input* player_input,
                          char* char_points_string, int field_size,
                          int* points_won)
{
  char eos = '\0';
  char space = ' ';
  int char_to_coordinate = 97;

  int return_value = wordPlacementCheck(game_play_field, player_input,
                                        char_points_string, field_size);
  if(return_value != SUCCESS)
    return return_value;

  int row_coordinate = player_input->row_ - char_to_coordinate;
  int column_coordinate = player_input->column_ - char_to_coordinate;

  int word_iterator = 0;
  for(word_iterator = 0; player_input->word_[word_iterator] != eos;
      word_iterator++)
  {
    char word_char = (char)toupper(player_input->word_[word_iterator]);
    int letter_points = pointLetterInput(player_input->word_[word_iterator],
                                         char_points_string);
    if(game_play_field[row_coordinate][column_coordinate].letter_ == space)
      *points_won += letter_points;
    game_play_field[row_coordinate][column_coordinate].letter_ = word_char;
    game_play_field[row_coordinate][column_coordinate].letter_points_ =
        letter_points;

    if(player_input->orientation_)
      row_coordinate++;
    else
      column_coordinate++;
  }
  return SUCCESS;
}
