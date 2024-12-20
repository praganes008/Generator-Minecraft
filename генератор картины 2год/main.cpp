#include "TXLib.h"
#include <fstream>
#include <windows.h>
#include <string.h>
#include <dirent.h>


using namespace std;                                                                                            //c�������� ����� ���������� � fstream

struct Button
  {
    int x;
    int y;
    int width;
    int height;
    const char* text;
    COLORREF color;
    string category;

  void draw()
  {
        txSetColor (TX_WHITE, 2);
        txSetFillColor (color);
        if(click())
        {
            txSetFillColor(TX_GREY);
        }

        txRectangle (x, y, x+width, y+height);
        txSetColor (TX_WHITE, 2);
        txSelectFont("Times New Roman", 30);
        txDrawText(x, y, x+width, y+height, text);
  }

    bool click()
    {
        return( txMouseButtons() == 1 &
        txMouseX()>x & txMouseX() < x+width &
        txMouseY()>y & txMouseY() < y+height );
    }

 };

struct Picture
{
    int x;
    int y;
    string adress;
    HDC image;
    int w_scr;
    int h_scr;
    int w;
    int h;
    string category;
    bool visible;

    void draw()
    {
        if(visible)
        {
        Win32::TransparentBlt (txDC(), x, y, w_scr, h_scr, image, 0, 0, w, h, TX_WHITE);
        }
    }

    bool click()
    {
                return( txMouseButtons() == 1 &
                txMouseX()>x & txMouseX() < x+w_scr &
                txMouseY()>y & txMouseY() < y+h_scr );

    }

};

void drawworkspase(int x, int y, int width, int height)
{

    txSetColor (TX_BLUE, 7);
    txSetFillColor (TX_WHITE);
    txRectangle (x, y, x+width, y+height);

}

string DialogFile(bool isSave)
{
  string filename = "";

OPENFILENAME ofn = {0};
TCHAR szFile[260]={0};
// Initialize remaining fields of OPENFILENAME structure
ZeroMemory(&ofn, sizeof(ofn));
ofn.lStructSize = sizeof(ofn);
ofn.hwndOwner = txWindow();
ofn.lpstrFile = szFile;
ofn.nMaxFile = sizeof(szFile);
ofn.lpstrFilter = ("Text\0*.TXT\0");
ofn.nFilterIndex = 1;
ofn.lpstrFileTitle = NULL;
ofn.nMaxFileTitle = 0;
ofn.lpstrInitialDir = NULL;
ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if(isSave)
    {
        if(GetSaveFileName(&ofn) == TRUE)
        {
                filename = ofn.lpstrFile;
                filename = filename + ".txt";

        }
    }

    else
    {
        if(GetOpenFileName(&ofn) == TRUE)
        {
                filename = ofn.lpstrFile;

        }
    }

 return filename;
}

int get_w(string adress)
{

 FILE *f1 = fopen(adress.c_str(), "rb");
 unsigned char header[54];
 fread(header, sizeof(unsigned char), 54, f1);
 int w = *(int *)&header[18];
 return w;

}

int get_h(string adress)
{
FILE *f1 = fopen(adress.c_str(), "rb");
 unsigned char header[54];
 fread(header, sizeof(unsigned char), 54, f1);
 int h = *(int *)&header[22];
 return h;
 }

int ReadFromDir(string adressDir, Picture menu_pic[], int countFiles)
{
    DIR *dir;
    struct dirent *ent;
    int x = 10;
    int y = 100;

    if((dir = opendir(adressDir.c_str())) != NULL)
    {
        while((ent = readdir (dir)) != NULL)
        {
            if((string)ent->d_name != "." && (string)ent->d_name != "..")
            {
                menu_pic[countFiles].x = x;
                menu_pic[countFiles].y = y;
                menu_pic[countFiles].adress = adressDir + (string)ent->d_name;
                countFiles++;
                y+=100;
                    if(y>600)
                    {
                     x = 110;
                     y = 100;
                    }
            }
        }
        closedir(dir);
    }

    return countFiles;
}

inline int GetFilePointer(HANDLE FileHandle)
{
    return SetFilePointer(FileHandle, 0, 0, FILE_CURRENT);
}

bool SaveBMPFile(char *filename, HBITMAP bitmap, HDC bitmapDC, int width, int height)
{
    bool Success=0;
    HBITMAP OffscrBmp=NULL;
    HDC OffscrDC=NULL;
    LPBITMAPINFO lpbi=NULL;
    LPVOID lpvBits=NULL;
    HANDLE BmpFile=INVALID_HANDLE_VALUE;
    BITMAPFILEHEADER bmfh;
    if ((OffscrBmp = CreateCompatibleBitmap(bitmapDC, width, height)) == NULL)
        return 0;
    if ((OffscrDC = CreateCompatibleDC(bitmapDC)) == NULL)
        return 0;
    HBITMAP OldBmp = (HBITMAP)SelectObject(OffscrDC, OffscrBmp);
    BitBlt(OffscrDC, 0, 0, width, height, bitmapDC, 0, 0, SRCCOPY);
    if ((lpbi = (LPBITMAPINFO)(new char[sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD)])) == NULL)
        return 0;
    ZeroMemory(&lpbi->bmiHeader, sizeof(BITMAPINFOHEADER));
    lpbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    SelectObject(OffscrDC, OldBmp);
    if (!GetDIBits(OffscrDC, OffscrBmp, 0, height, NULL, lpbi, DIB_RGB_COLORS))
        return 0;
    if ((lpvBits = new char[lpbi->bmiHeader.biSizeImage]) == NULL)
        return 0;
    if (!GetDIBits(OffscrDC, OffscrBmp, 0, height, lpvBits, lpbi, DIB_RGB_COLORS))
        return 0;
    if ((BmpFile = CreateFile(filename,
                        GENERIC_WRITE,
                        0, NULL,
                        CREATE_ALWAYS,
                        FILE_ATTRIBUTE_NORMAL,
                        NULL)) == INVALID_HANDLE_VALUE)
        return 0;
    DWORD Written;
    bmfh.bfType = 19778;
    bmfh.bfReserved1 = bmfh.bfReserved2 = 0;
    if (!WriteFile(BmpFile, &bmfh, sizeof(bmfh), &Written, NULL))
        return 0;
    if (Written < sizeof(bmfh))
        return 0;
    if (!WriteFile(BmpFile, &lpbi->bmiHeader, sizeof(BITMAPINFOHEADER), &Written, NULL))
        return 0;
    if (Written < sizeof(BITMAPINFOHEADER))
        return 0;
    int PalEntries;
    if (lpbi->bmiHeader.biCompression == BI_BITFIELDS)
        PalEntries = 3;
    else PalEntries = (lpbi->bmiHeader.biBitCount <= 8) ?
                      (int)(1 << lpbi->bmiHeader.biBitCount) : 0;
    if(lpbi->bmiHeader.biClrUsed)
    PalEntries = lpbi->bmiHeader.biClrUsed;
    if(PalEntries){
    if (!WriteFile(BmpFile, &lpbi->bmiColors, PalEntries * sizeof(RGBQUAD), &Written, NULL))
        return 0;
        if (Written < PalEntries * sizeof(RGBQUAD))
            return 0;
    }
    bmfh.bfOffBits = GetFilePointer(BmpFile);
    if (!WriteFile(BmpFile, lpvBits, lpbi->bmiHeader.biSizeImage, &Written, NULL))
        return 0;
    if (Written < lpbi->bmiHeader.biSizeImage)
        return 0;
    bmfh.bfSize = GetFilePointer(BmpFile);
    SetFilePointer(BmpFile, 0, 0, FILE_BEGIN);
    if (!WriteFile(BmpFile, &bmfh, sizeof(bmfh), &Written, NULL))
        return 0;
    if (Written < sizeof(bmfh))
        return 0;

    CloseHandle (BmpFile);

    delete [] (char*)lpvBits;
    delete [] lpbi;

    DeleteDC (OffscrDC);
    DeleteObject (OffscrBmp);


    return 1;
}

bool ScreenCapture(int x, int y, int width, int height, char *filename, HWND hwnd)
{
    HDC hDC = GetDC(hwnd);
    HDC hDc = CreateCompatibleDC(hDC);

    HBITMAP hBmp = CreateCompatibleBitmap(hDC, width, height);

    HGDIOBJ old= SelectObject(hDc, hBmp);
    BitBlt(hDc, 0, 0, width, height, hDC, x, y, SRCCOPY);

    bool ret = SaveBMPFile(filename, hBmp, hDc, width, height);

    SelectObject(hDc, old);

    DeleteObject(hBmp);

    DeleteDC (hDc);
    ReleaseDC (hwnd, hDC);

    return ret;
}

int main()
        {
    txCreateWindow (1500, 800);
    txTextCursor(false);
    //���-�� ������
    int count_btn = 13;
    //������ ����������
    int btn_save = count_btn-5;
    //������ ��������
    int btn_load = count_btn-4;
    //������ ������
    int btn_exit = count_btn-2;
    //������ ������
    int btn_help = count_btn-3;
    //���-�� ��������
    int count_pic = 0;
    //���-�� ����������� �������� � ������� �������
    int nCentralpic = 0;
    //���������� ��� �� ������� ��������
    int vybor = -1;
    //���������� ������� �� �������� �� ������� �������
    bool mouse_down = false;

    string PAGE = "work";

    //������
    Button btn[count_btn];
    btn[0] = {10, 10, 170, 35, "���", TX_BLUE, "���"};
    btn[1] = {200, 10, 170, 35, "��������", TX_BLUE, "��������"};
    btn[2] = {390, 10, 170, 35, "���������", TX_BLUE, "���������"};
    btn[3] = {580, 10, 170, 35, "������", TX_BLUE, "������"};
    btn[4] = {770, 10, 170, 35, "��������", TX_BLUE, "��������"};
    btn[5] = {960, 10, 170, 35, "�������", TX_BLUE, "�������"};
    btn[6] = {1150, 10, 170, 35, "��������", TX_BLUE, "��������"};
    btn[7] = {1335, 10, 170, 35, "���������", TX_BLUE, "���������"};
    btn[8] = {250, 55, 170, 35, "���������", TX_LIGHTBLUE, ""};
    btn[9] = {440, 55, 170, 35, "���������", TX_LIGHTBLUE, ""};
    btn[10] = {630, 55, 170, 35, "������", TX_LIGHTBLUE, ""};
    btn[11] = {820, 55, 170, 35, "�����", TX_LIGHTBLUE, ""};
    btn[12] = {1010, 55, 170, 35, "��������", TX_LIGHTBLUE, ""};



    //�������� � ����
    Picture menu_pic[100];

    count_pic = ReadFromDir("pictures/���/",menu_pic, count_pic);
    count_pic = ReadFromDir("pictures/��������/",menu_pic, count_pic);
    count_pic = ReadFromDir("pictures/���������/",menu_pic, count_pic);
    count_pic = ReadFromDir("pictures/������/",menu_pic, count_pic);
    count_pic = ReadFromDir("pictures/��������/",menu_pic, count_pic);
    count_pic = ReadFromDir("pictures/�������/",menu_pic, count_pic);
    count_pic = ReadFromDir("pictures/��������/",menu_pic, count_pic);
    count_pic = ReadFromDir("pictures/���������/",menu_pic, count_pic);

   for(int i=0; i<count_pic; i++)
   {

    menu_pic[i].visible = false;

    int pos1 = menu_pic[i].adress.find("/");
    int pos2 = menu_pic[i].adress.find("/", pos1+1);
    menu_pic[i].category = menu_pic[i].adress.substr(pos1+1, pos2 - (pos1+1));

    menu_pic[i].w = get_w(menu_pic[i].adress);
    menu_pic[i].h = get_h(menu_pic[i].adress);

    menu_pic[i].w_scr = menu_pic[i].w/5;
    menu_pic[i].h_scr = menu_pic[i].h/5;

    if(menu_pic[i].category == "���")
    {
    menu_pic[i].w_scr = menu_pic[i].w/2;
    menu_pic[i].h_scr = menu_pic[i].h/2;
    }
    else
    {
    menu_pic[i].w_scr = menu_pic[i].w/5;
    menu_pic[i].h_scr = menu_pic[i].h/5;
    }

    if(menu_pic[i].category == "���������")
    {
    menu_pic[i].w_scr = menu_pic[i].w/12;
    menu_pic[i].h_scr = menu_pic[i].h/5;
    }

    menu_pic[i].image =  txLoadImage(menu_pic[i].adress.c_str());

   }

    //co������ ������� �������� �� ������� �������, ��� ������ ��������
    Picture centr_pic[100];


    while(!btn[btn_exit].click())//!GetAsyncKeyState (VK_ESCAPE)
    {

        txSetFillColor(TX_LIGHTGREEN);
        txClear();
        txBegin();

        // ������� �������
        drawworkspase(250, 100, 1240, 670);

        if(PAGE == "work")
        {

        //��������� ������
        for(int i=0; i<count_btn; i++)
        {
            btn[i].draw();
        }

        //��������� �������� � ����
        for(int i=0; i<count_pic; i++)
        {
            menu_pic[i].draw();
        }

        //��������� �������� �� ���������
        for(int i=0; i<nCentralpic; i++)
        {
            centr_pic[i].draw();
        }

        //������� �������� �������� ���� � ����������� �� ������� ������
        for(int nbutton=0; nbutton<count_btn; nbutton++)
        {
            if(btn[nbutton].click())
            {
                 for(int npic=0; npic<count_pic; npic++)
                 {
                      menu_pic[npic].visible = false;
                      if(menu_pic[npic].category == btn[nbutton].category)
                      {
                        menu_pic[npic].visible = true;
                      }
                 }
            }

        }

        //��� ������ ����������� �������� �� ������� �������
        for(int npic=0; npic<count_pic; npic++)
        {
            if(menu_pic[npic].click() && menu_pic[npic].visible)
            {
                 while(txMouseButtons() == 1)
                 {
                 txSleep(10);
                 }

                 centr_pic[nCentralpic] =   { 350,
                                              150,
                                              menu_pic[npic].adress,
                                              menu_pic[npic].image,
                                              menu_pic[npic].w,
                                              menu_pic[npic].h,
                                              menu_pic[npic].w,
                                              menu_pic[npic].h,
                                              menu_pic[npic].category,
                                              true


                                            };
                        nCentralpic++;
            }

        }

        for(int npic=0; npic<nCentralpic; npic++)                                                                           //�� ���� �� ����� �������, �������� ���������� �����(1,2,3...)
        {
          if(centr_pic[npic].click() && centr_pic[npic].visible)
          {
           vybor = npic;
           mouse_down = false;
          }
        }

    //������������ ������� ������
    if(vybor>=0)
    {
        if(txMouseButtons() == 1 && !mouse_down)
        {
            centr_pic[vybor].x = txMouseX() - centr_pic[vybor].w_scr/2;
            centr_pic[vybor].y = txMouseY() - centr_pic[vybor].h_scr/2;
        }
            else
            {
                if(txMouseButtons() != 1)
                {
                mouse_down = true;
                }
            }
    }

    //����������� ������� �����������
    if(vybor>=0)
    {
     //
    if(GetAsyncKeyState (VK_UP))
        {
        centr_pic[vybor].y -= 5;
        }

    if(GetAsyncKeyState (VK_DOWN))
        {
        centr_pic[vybor].y += 5;
        }

    if(GetAsyncKeyState (VK_LEFT))
        {
        centr_pic[vybor].x -= 5;
        }

    if(GetAsyncKeyState (VK_RIGHT))
        {
        centr_pic[vybor].x += 5;
        }
     //
    if(GetAsyncKeyState (VK_OEM_PLUS))                                                                                          //���� ����� ����, �� �������� �� 1.05 ������ � ������
        {
        centr_pic[vybor].w_scr = centr_pic[vybor].w_scr*1.05;
        centr_pic[vybor].h_scr = centr_pic[vybor].h_scr*1.05;
        if(centr_pic[vybor].w_scr>700)
        {
        centr_pic[vybor].w_scr=700;
        centr_pic[vybor].h_scr=700;
        }
        }

    if(GetAsyncKeyState (VK_OEM_MINUS))                                                                                          //���� ����� ����, �� �������� �� 1.05 ������ � ������
        {
        centr_pic[vybor].w_scr = centr_pic[vybor].w_scr*0.95;
        centr_pic[vybor].h_scr = centr_pic[vybor].h_scr*0.95;
        if(centr_pic[vybor].w_scr<50)
        {
        centr_pic[vybor].w_scr=50;
        centr_pic[vybor].h_scr=50;
        }
        }
    }

    //�������� �������
    if(vybor>=0 && GetAsyncKeyState (VK_DELETE))                                                                                            // ������� ��������� ������, �������� � ���� �� 1 � ����� = -1(������ �� ���� ���������� ������� �������), ������ mouse_down ����� ���(������ ���� �� ��� ������) � �������� ���������
    {
        centr_pic[vybor] = centr_pic[nCentralpic - 1];
        nCentralpic--;
        vybor = -1;
        mouse_down = true;
    }

    //���������� ����� � ����, �� ������� �� �������
    if(btn[btn_save].click())
    {
            string filename = DialogFile(true);
            ofstream fileout;               // ����� ��� ������
            fileout.open(filename);      // ��������� ���� ��� ������(���������)
            if (fileout.is_open())
            {
                for(int i=0; i<nCentralpic; i++)
                {
                fileout << centr_pic[i].x << endl;                                                                                          //��� ���������� ������� ���� ��������, ����� �, ����� ��� �����, � ����� ��� ������ � ������
                fileout << centr_pic[i].y << endl;
                fileout << centr_pic[i].adress << endl;
                fileout << centr_pic[i].h_scr << endl;
                fileout << centr_pic[i].w_scr << endl;
                }
            }
            fileout.close();                    //��������� ����

    }

    //�������� ���������� �� �����
    if(btn[btn_load].click())
    {
          string filename = DialogFile(false);

          while(txMouseButtons() == 1)
         {
         txSleep(10);
         }

          for(int i=0; i>0; i--)
          {
            centr_pic[nCentralpic] = centr_pic[nCentralpic - 1];
            nCentralpic--;
            vybor = -1;
            mouse_down = true;
          }

        char buff[50];
        ifstream filein("���������.txt");      // ��������� ���� ��� ������(���������)
        while(filein.good())
        {
            filein.getline(buff, 50);
            int x = atoi(buff);
            filein.getline(buff, 50);
            int y = atoi(buff);
            filein.getline(buff, 50);
            string adress = buff;
            filein.getline(buff, 50);
            int h_scr = atoi(buff);
            filein.getline(buff, 50);
            int w_scr = atoi(buff);

         for(int i=0; i<count_pic; i++)
         {

         if(menu_pic[i].adress == adress)
         {
         centr_pic[nCentralpic] =   { x,
                                      y,
                                      adress,
                                      menu_pic[i].image,
                                      w_scr,
                                      h_scr,
                                      menu_pic[i].w,
                                      menu_pic[i].h,
                                      menu_pic[i].category,
                                      true


                                    };
        nCentralpic ++;
         }
         }
                                                                                                                        //atoi - ������� �� ����, � ����� ����� ������
        }
        filein.close();
    }

    if(btn[12].click())
    {
        ScreenCapture(300, 105, 1185, 660, "1.bmp", txWindow());
        txMessageBox("��������� � 1.bmp");
    }

    }

        if(btn[btn_help].click())
        {

         PAGE = "help";

        }

        if(PAGE == "help")
        {
        txClear();
        txSetColor (TX_RED, 7);
        txSetFillColor (TX_BLUE);
        txRectangle(0, 0, 1500, 800);
        drawworkspase(100, 50, 1300, 700);
        txSelectFont("Comic Sans MS", 50);
        //������ �������
        txSetColor (TX_RED, 7);
        txDrawText(200, 20, 1250, 200, "������ �� ���������");
        //������� �������
        txSelectFont("Comic Sans MS", 32);
        txSetColor (TX_BLACK, 7);
        txDrawText(190, 95, 1255, 190, "����������� ���������� �������");
        //���� �����
        txSelectFont("Comic Sans MS", 30);
        txSetColor (TX_BLACK, 7);
        txDrawText(165, 160, 1250, 800, "� ��� ��������� ������������ ������ ��������, ���������, ��������, ���, � ���� ������\n"
                                        "�� ���������� ���� ���������. � ���� ��������� ������������ 8 ������\n"
                                        "� ������� ����������� ��������, � 5 �������������� ������.\n"
                                        "��� �������������� �������� �� ������� �������?\n"
                                        "����������� � ���������: + � -\n"
                                        "������� ������� � ������� �������: Delete(Del)\n"
                                        "����������� �������������� ����� �������� ���� ��� ���������\n"
                                        "����������� ��������� �������������� �������������� �� ����������� �� ����������\n"
                                        "�������� ������� ������� �������������� �� ������ � ��������������� ���������, � ���������� ����������� ��� �������� ������� �������\n"
                                        "������ ��������� ��������� ��������� ���� � �������� ���� ���������\n"
                                        "��� ������� ������ ���������, �� ��������� ��������� ���� ���������� ����� � ��� ������� �� ���� ����������� � ��� �� ������� �������, ��� �� ������ ������ ���������� � ���� ��������\n"
                                        "����� �� ��������� �������������� �� ��������������� ������, � ����� �� ������� �� ������� Escape(Esc)\n"
                                        "����� ��������� � ��������� ���� ��������, ����� ��� �� � �� ��� ����� ���(� �������� fffff  ��� �� �� �����), ��� �� ������ ��� 400 �� 400(��� ��� 100 �� 100, � ��� �������� 800 �� 250), � ��� �� ������ ����� BMP(bmp) ����������� ����� paint, �.�. ����� ������ ��������� �� ��������\n"
                                        "� �����, ��� �� ��� ����� ���� ����������, ������� ����!");


        if(GetAsyncKeyState (VK_ESCAPE))
        {
         PAGE = "work";
        }


        }

    txEnd();
    txSleep(10);
    }

    for(int i=0; i<count_pic; i++)
    {
        txDeleteDC(menu_pic[i].image);
    }

    for(int i=0; i<nCentralpic; i++)
    {
        txDeleteDC(centr_pic[i].image);
    }

        txDisableAutoPause();
        txTextCursor (false);
        return 0;
        }


