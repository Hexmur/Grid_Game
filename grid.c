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

        case 'c': // chest cell
          printf(" $ ");
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

void print_shop(char *array, int current_shop_items){
  printf("Current shop items %d\n", current_shop_items);
  for(int i=0; i < current_shop_items; i++){
    if(array[i]=='l') printf("%d | 10$ | max_health++: Increase your max health by 1\n", i+1);
    else if(array[i]=='m') printf("%d | 8$ | monster_spawn_rate--: Decrease the monster spawn rate by 2\n", i+1);
  }
  printf("\n");
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
  int player_coins = 0;
  char grid[16]; // 4x4 grid
  
  int shopMaxItems = 5;
  char shopArray[shopMaxItems];

  start_game:
    
    print_title_screen();
    
    int player_life = max_health;
    int player_pos = -1;
    int monster_rate = 6;

    for(int level=0; ; level++){
      monster_rate++; // increments by 1 every level

      // Shop every 2 levels        
      if(level%2 == 0 && level != 0){
        int current_shop_items = shopMaxItems;


        for(int i=0; i<shopMaxItems; i++){
          random_tmp = rand() % 2;
          if(random_tmp == 0){
            shopArray[i] = 'l';
          } else if(random_tmp == 1){
            shopArray[i] = 'm';
          }
        }

        buy_something:{

          printf("\x1b[H\x1b[2J\x1b[3J"); // clear screen
          printf("Shop\n\n"); 

          printf("Monsters Spawn Rate: %d\n", monster_rate);        
          printf("Level %d\n", level);        

          printf("%d/%d HP [ ", player_life, max_health);
          for(int i=0; i < player_life; i++){
            printf("♥ ");
          }
          for(int j=0; j < max_health - player_life; j++){
            printf("▢ ");
          }

          printf("]\n");

          printf("%d coins\n\n", player_coins);

          //
          
          print_shop(shopArray, current_shop_items);
          printf("Do you want to buy something? [Y/N] \n");
          
          if(fgets(buffer, sizeof(buffer), stdin) == NULL){
            break;
          }
          if(buffer[0] == 'Y' || buffer[0] == 'y'){
            printf("Select an item... ");
            
            if(fgets(buffer, sizeof(buffer), stdin) == NULL){
              break;
            }
              
            // Convert text to int
            int item_choose = atoi(buffer) - 1;

            printf("\n");
            if(item_choose < 0 || item_choose > current_shop_items){
              printf("Error: Select a row from 1 to %d... ", current_shop_items);
              goto buy_something;
            } else{
                if(shopArray[item_choose] == 'l'){
                  max_health++;
                  player_life = max_health;
                  player_coins -= 10;
                }else if(shopArray[item_choose] == 'm'){
                  if(monster_rate > 0){
                    monster_rate -= 2;
                    player_coins -= 8;
                  }
                }
                for(int k=item_choose; k<current_shop_items-1; k++){
                  shopArray[item_choose] = shopArray[item_choose+1];
                }
                current_shop_items--;
                if(current_shop_items <= 0){
                  printf("Shop has no items.\n");
                  fflush(stdout);
                  usleep(1000000);
                }
                else goto buy_something;
              }
       
          } else if(buffer[0] == 'N' || buffer[0] == 'n'){
            printf("Going to the next level...\n");
            fflush(stdout);
            usleep(1000000);
          } else{
            goto buy_something;
          }
      }
        fflush(stdout); 
        usleep(1000000);
      }   

      int monster_count = 0;
      int turn = 0;
      
      for(int i=0; i<16; i++){
        random_tmp = rand() % 16;
        if(random_tmp <= monster_rate){ // set cell to monster (monster_rate/15 chance)
          grid[i] = 'm';
          monster_count++;
        } else{
            random_tmp = rand() % 2;
            if(random_tmp == 0) grid[i] = 'c'; // set cell to chest cell
            else grid[i] = 'l'; // set cell to life_cell
          }
      }
      
      while(1){
        printf("\x1b[H\x1b[2J\x1b[3J"); // clear screen
        
        printf("Monster Rate: %d\n", monster_rate);        
        printf("Level %d\n", level);
        printf("Turn %d\n", turn);
        printf("Next level at turn %d\n", turn_goal);        

        printf("%d/%d HP [ ", player_life, max_health);
        for(int i=0; i < player_life; i++){
          printf("♥ ");
        }
        for(int j=0; j < max_health - player_life; j++){
          printf("▢ ");
        }

        printf("]\n");

        printf("%d coins\n", player_coins);
      
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
            
            switch(grid[choice]){
              case('m'):
                printf("~ ");
                break;
              
              case('l'):
                printf("♥ ");
                break;

              case('c'):
                printf("$ ");
                break;
            }

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

              case 'c':
                random_tmp = (rand() % 5) + 1;
                player_coins += random_tmp;
                
                printf("You earned %d coin", random_tmp);
                if(random_tmp > 1) printf("s");
                printf("\n");
                
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
