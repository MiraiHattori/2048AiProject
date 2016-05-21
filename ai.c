#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

/*constant-------------------------------------------------------*/
#define COLUMN 4
#define ROW 4
#define DEBUG true


/*initial funtion definition-------------------------------------*/
void sendCommand();

void goUp();
void goDown();
void goLeft();
void goRight();

/*public variables-----------------------------------------------*/
int received_data[ROW * COLUMN + 1];

/*main program---------------------------------------------------*/

int main()
{
    while (true) {
        //scanfでは空白で区切られる
        for (int i = 0; i < ROW * COLUMN + 1; i++) {
            scanf("%d", &received_data[i]);
        }
        printf("down\n");
        usleep(1000);

        // 盤面状態の数字が返ってきてないときエラーを吐いて次のループへ
        if (received_data[ROW * COLUMN] != 0 && received_data[ROW * COLUMN] != 1 && received_data[ROW * COLUMN] != 2) {
            printf("An error has occured in 2048/2048.rb\n");
            continue;
        }
        //status 2を取得した時ゲームオーバー
        if (received_data[ROW * COLUMN] == 2) {
            printf("Game Over.\n");
            exit(1);
            continue;
        }

        /*AI program-----------------------------------------------------*/
        for (int i = 0; i < COLUMN; i++) {
            for (int j = 0; j < ROW; j++) {
                printf("%d", received_data[i * 4 + j]);
            }
            printf("\n");
        }
        printf("gamestatus %d\n", received_data[ROW * COLUMN]);

        if (received_data[ROW * COLUMN] == 1) {
            goDown();
        } else {
            static int debug_i = 0;
            switch (debug_i) {
            case 0:
                goUp();
                break;
            case 1:
                goRight();
                break;
            case 2:
                goUp();
                break;
            case 3:
                goLeft();
                break;
            default:
                printf("Invalid Command\n");
                break;
            }
            if (debug_i == 3) {
                debug_i = 0;
            } else {
                debug_i++;
            }
        }
    }
    return 0;
}

void goUp()
{
    printf("up\n");
}
void goDown()
{
    printf("down\n");
}
void goLeft()
{
    printf("left\n");
}
void goRight()
{
    printf("right\n");
}
