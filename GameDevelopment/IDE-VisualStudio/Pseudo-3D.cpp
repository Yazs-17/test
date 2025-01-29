#include <graphics.h> 
#include <cmath>      
#include <Conio.h>
#define mapWidth 24
#define mapHeight 24


int worldMap[mapWidth][mapHeight] ={
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,2,2,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,3,0,0,0,3,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,2,0,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,0,0,0,5,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

const int screenWidth = 640;  const int screenHeight = 480; 
COLORREF RGB_Red = RGB(255, 0, 0);
COLORREF RGB_Green = RGB(0, 255, 0);
COLORREF RGB_Blue = RGB(0, 0, 255);
COLORREF RGB_White = RGB(255, 255, 255);
COLORREF RGB_Yellow = RGB(255, 255, 0);

void verLine(int x, int y1, int y2, COLORREF color)
{
    for (int y = y1; y <= y2; y++)
    {
        putpixel(x, y, color);    }
}

int main()
{
    double posX = 22, posY = 12;
    double dirX = -1, dirY = 0;
    double planeX = 0, planeY = 0.66;
    double time = 0;
    double oldTime = 0;

    initgraph(screenWidth, screenHeight);     
    BeginBatchDraw();
    while (!_kbhit())     {
        for (int x = 0; x < screenWidth; x++)
        {
            double cameraX = 2 * x / (double)screenWidth - 1;
            double rayDirX = dirX + planeX * cameraX;
            double rayDirY = dirY + planeY * cameraX;

            int mapX = (int)posX;
            int mapY = (int)posY;

            double sideDistX, sideDistY;
            double deltaDistX = (rayDirX == 0) ? 1e30 : std::abs(1 / rayDirX);
            double deltaDistY = (rayDirY == 0) ? 1e30 : std::abs(1 / rayDirY);

            double perpWallDist;
            int stepX, stepY;

            int hit = 0;
            int side;

            if (rayDirX < 0)
            {
                stepX = -1;
                sideDistX = (posX - mapX) * deltaDistX;
            }
            else
            {
                stepX = 1;
                sideDistX = (mapX + 1.0 - posX) * deltaDistX;
            }

            if (rayDirY < 0)
            {
                stepY = -1;
                sideDistY = (posY - mapY) * deltaDistY;
            }
            else
            {
                stepY = 1;
                sideDistY = (mapY + 1.0 - posY) * deltaDistY;
            }

            while (hit == 0)
            {
                if (sideDistX < sideDistY)
                {
                    sideDistX += deltaDistX;
                    mapX += stepX;
                    side = 0;
                }
                else
                {
                    sideDistY += deltaDistY;
                    mapY += stepY;
                    side = 1;
                }

                                                //int worldMap[24][24] = worldMap;

                if (worldMap[mapX][mapY] > 0)
                    hit = 1;
            }

            if (side == 0)
                perpWallDist = (mapX - posX + (1 - stepX) / 2) / rayDirX;
            else
                perpWallDist = (mapY - posY + (1 - stepY) / 2) / rayDirY;

            int lineHeight = (int)(screenHeight / perpWallDist);
            int drawStart = -lineHeight / 2 + screenHeight / 2;
            if (drawStart < 0)
                drawStart = 0;
            int drawEnd = lineHeight / 2 + screenHeight / 2;
            if (drawEnd >= screenHeight)
                drawEnd = screenHeight - 1;

            COLORREF color;
            switch (worldMap[mapX][mapY])
            {
            case 1:
                color = RGB_Red;
                break;
            case 2:
                color = RGB_Green;
                break;
            case 3:
                color = RGB_Blue;
                break;
            case 4:
                color = RGB_White;
                break;
            default:
                color = RGB_Yellow;
                break;
            }

            if (side == 1)
            {
                color = GetRValue(color) / 2 + (GetGValue(color) / 2 << 8) + (GetBValue(color) / 2 << 16);
            }

            verLine(x, drawStart, drawEnd, color);
        }

        oldTime = time;
        time = GetTickCount();
        double frameTime = (time - oldTime) / 1000.0;
        
        FlushBatchDraw(); 
                double moveSpeed = frameTime * 5.0;
        double rotSpeed = frameTime * 3.0;

        if (GetAsyncKeyState(VK_UP) & 0x8000)         {
            posX += dirX * moveSpeed;
            posY += dirY * moveSpeed;
        }
        if (GetAsyncKeyState(VK_DOWN) & 0x8000)         {
            posX -= dirX * moveSpeed;
            posY -= dirY * moveSpeed;
        }
        if (GetAsyncKeyState(VK_RIGHT) & 0x8000)         {
            double oldDirX = dirX;
            dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
            dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
            double oldPlaneX = planeX;
            planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
            planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
        }
        if (GetAsyncKeyState(VK_LEFT) & 0x8000)         {
            double oldDirX = dirX;
            dirX = dirX * cos(rotSpeed) - dirY * sin(rotSpeed);
            dirY = oldDirX * sin(rotSpeed) + dirY * cos(rotSpeed);
            double oldPlaneX = planeX;
            planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
            planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
        }

        Sleep(15);         
        cleardevice();
        FlushBatchDraw();
    }
    EndBatchDraw();
    closegraph();     return 0;
}
