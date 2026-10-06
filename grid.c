#include <stdio.h>
#include <unistd.h> // for using usleep()
#include <stdlib.h>
#include <time.h>

/* Print the grid */
void print_grid(char *grid, int player_pos){
  printf("player_pos: %d\n\n", player_pos);
  for(int i=0; i<4; i++){
    printf("+---+---+---+---+\n");
    for(int j=0; j<4; j++){
      putchar('|');

      switch (grid[i*4+j]){

        case 'l': // life cell
          printf(" ♥ ");
          break;

        case 'm': // monster cell
          printf(" ~ ");
          break;

        default:
          if(i*4+j == player_pos){
            printf(" O ");
          }
          else printf("   ");
          break;

      }
    }
    printf("|\n");
  }
  printf("+---+---+---+---+\n");
}

void print_title_screen(){
  printf("\x1b[H\x1b[2J\x1b[3J"); // clear screen
  printf("Welcome to the grid_game!\nPress enter to start ");
  int temp;
  while((temp = getchar()) != '\n' && temp != EOF);
}


int main(void){
  srand(time(NULL));   
  char buffer[64];
  int choice = 0;
  int random_tmp;
  int random_col;
  int row = 0;
  int turn_goal = 6;
  int max_health = 4;
  char grid[16]; // 4x4 grid

  start_game:
    
    print_title_screen();
    
    int player_life = 4;
    int player_pos = -1;
    int monster_rate = 6;

    for(int level=0; ; level++){    
      int monster_count = 0;
      int turn = 0;
      monster_rate++; // increments by 1 every level
      
      for(int i=0; i<16; i++){
        random_tmp = rand() % 16;
        if(random_tmp <= monster_rate){ // set cell to monster (monster_rate/15 chance)
          grid[i] = 'm';
          monster_count++;
        } else grid[i] = 'l'; // set cell to life_cell
      }
      
      while(1){
        printf("\x1b[H\x1b[2J\x1b[3J"); // clear screen
        
        printf("Monster Rate: %d\n", monster_rate);        
        printf("Level %d\n", level);
        printf("Turn %d\n", turn);
        printf("Next level at turn %d\n", turn_goal);        

        printf("%d HP [ ", player_life);
        for(int i=0; i < player_life; i++){
          printf("♥ ");
        }
        printf("]\n");
      
        print_grid(grid, player_pos);
        
        if(turn >= turn_goal){
          next_level_choose:
            printf("Do you want to go to the next level? [Y/N] ");
            if(fgets(buffer, sizeof(buffer), stdin) == NULL){
              break;
            }
            if(buffer[0] == 'Y' || buffer[0] == 'y'){
              break; // exit from while(1)
            } else if(buffer[0] == 'N' || buffer[0] == 'n'){
              printf("You choose to continue in this level...\n");
              usleep(1000000);
            } else{
              goto next_level_choose;
            }
        }
             
        printf("Select a row from 1 to 4... ");
        
        choose_row:{

          if(fgets(buffer, sizeof(buffer), stdin) == NULL){
            break;
          }
            
          // Convert text to int
          row = atoi(buffer) - 1;

          printf("\n");
          if(row < 0 || row > 3){
            printf("Error: Select a row from 1 to 4... ");
            goto choose_row;
          } else{ // Check if the row is empty 
            int count = 0;
            for(int i=0; i<4; i++){
              if(grid[((row)*4)+i] == 'x'){
                count++;
                if(count == 4){
                  printf("The selected row is empty. Please select another row ");
                  goto choose_row;
                }
              } else break;
            }
            
            // Check if the cell is empty. If it's empty, redo the rand
            do{
              random_col = rand() % 4;
              choice = ((row)*4)+random_col; 
            } while(grid[choice] == 'x');
             
            if(grid[choice]=='m'){ // kill monster
              monster_count--;
            }
            printf("O ");
            fflush(stdout); usleep(500000);
            
            printf("vs ");
            fflush(stdout); usleep(500000);
            
            printf("%c\n", grid[choice]);
            fflush(stdout); usleep(500000);
            
            switch(grid[choice]){

              case 'l':
                if(player_life < max_health){
                  printf("You earned a life!\n");
                  player_life++;
                } else{
                  printf("You have already %d/%d life\n", max_health,  max_health);
                }
                break;

              case 'm':
                printf("You lose a life!\n");
                player_life--;

                if(player_life <= 0){
                  fflush(stdout);
                  usleep(500000);
                  
                  printf("You Lose!\n");
                  
                  fflush(stdout);
                  usleep(1000000);

                  goto start_game;
                }
                break;
            }
            
            fflush(stdout);
            usleep(100000);

            grid[choice] = 'x';
            player_pos = choice;
            }

          turn++;
          usleep(1000000);
        }
      }
    
    }

  return 0;
}
