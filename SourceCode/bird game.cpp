#include <stdio.h>
#include <graphics.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#pragma comment(lib,"Winmm.lib")



//****************************С�����ݽṹ****************************************//

struct Bird         // ����С��ṹ��
{
	int x;		    // X����
	int y;	      	// Y����
	int a;			// ��һ��������
	int b;			// �ڶ���������
	int op;			// �������
	int result;		// �����

	bool isdelete;	// С���Ƿ�ɾ��

	Bird* next;		// ������һ��С���ָ��
};


Bird *headBird = NULL;           // �洢С������ͷ������β����ǰѡ�е�С��
Bird *endBird = NULL;
Bird *curSelBird = NULL;

//**********************************************************************************//

char option[4][10];    // ����ѡ��洢����
int currentScore = 0;   // ��ǰ����
char username[50];     // �û���



//*******************************������ƽ���Ĳ���************************************//
int optOffsetX = 0;
int optOffsetY = 50;
int optOffsetLX = -20;
int optOffsetLY = 50;
int optOffsetRX = 20;
int optOffsetRY = 50;

//********************************������ģ�麯��***************************************//


void putpicture(int dstx, int dsty, IMAGE *img, IMAGE *mask);       // ��������ͼƬ����
void setOption(int id, LPCTSTR str);           // ���ô�ѡ����ƺ���
void setScore(LPCTSTR str);    // ���÷��������ƺ���
void setUser(LPCTSTR str);      // �����û������ƺ���
void setTime(int timeLeft);    // ���õ���ʱʱ�亯��
void drawMenu();      // ���Ʋ˵�����
void addBird();    // ���С����
void updateOption(Bird* selBird, int* currentAnswer);    // ���´�ѡ���
void checkClick(int mx, int my, int* currentAnswer);   // ���������¼�����
void drawBird(IMAGE* bird, IMAGE* birdMask);       // ����С����
void StartBkMusic();      //�������ֺ���

//*************************************������*********************************************//
void main()
{



	clock_t startTime, currentTime;    //���ÿ�ʼ����ǰʱ��

	IMAGE bird;          // С��ͼ��

	IMAGE birdMask;        // С������ͼ��


	int currentAnswer;     // ��ǰ����ȷ��

	int timeLeft;        // ʣ��ʱ��



	printf("�������û�����");      //�����û���
	scanf("%s", username);

	StartBkMusic();     //��������

	initgraph(640, 480);                              // ��ʼ�����������뱳��ͼ��С��������ͼ
	loadimage(NULL, "./res/bkg.jpg");
	loadimage(&bird, "./res/Bird.jpg");
	loadimage(&birdMask, "./res/Bird_mask.jpg");

	MOUSEMSG m;            //�������

	BeginBatchDraw();           //��ʼ������ͼ

	startTime = clock();
	while (true)
	{
		loadimage(NULL, "./res/bkg.jpg");
		drawMenu();
		currentTime = clock();
		timeLeft = 60 - (currentTime - startTime) / CLOCKS_PER_SEC;      //��ǰʱ��
		setTime(timeLeft);                                               //��ʾ��ǰʱ��

		FlushBatchDraw();

		drawBird(&bird, &birdMask);


		if ((currentTime - startTime) / CLOCKS_PER_SEC > 60)   	// ����60��ʱ�˳���Ϸ����������
		{
			FILE *pFile = fopen("log.txt", "w");
			fwrite(username, 1, strlen(username), pFile);
			fwrite("\n", 1, strlen("\n"), pFile);
			char tmp[10];
			_itoa(currentScore, tmp, 10);
			fwrite(tmp, 1, strlen(tmp), pFile);
			fclose(pFile);
			exit(0);
		}

		if (MouseHit())           //��⵱ǰ�Ƿ��������Ϣ
		{
			m = GetMouseMsg();    //��ȡ�����Ϣ

			if (m.uMsg == WM_LBUTTONDOWN)
			{
				checkClick(m.x, m.y, &currentAnswer);
			}
		}
		FlushMouseMsgBuffer();
		Sleep(35);
	}

	EndBatchDraw();           //����������ͼ

	_getch();
	closegraph();
}

//********************************������ģ����ϸ����***************************************//

void drawBird(IMAGE* bird, IMAGE* birdMask)     // ����С��
{
	Bird *p;
	p = headBird;
	while (p != NULL)
	{

		if (p->isdelete)        // �����ѱ�ɾ����С��
		{
			p = p->next;
			continue;
		}

		p->x += 3;              //�����ٶ�Ϊ3
		char str[50] = "";
		char tmp[10];

		strcat(str, _itoa(p->a, tmp, 10));  	// ��С���Ӧ����������ַ�����ͨ��strcat�������ӣ�

		switch (p->op)               // ��С�������е�int�������ת�����ַ�
		{

		case 0: // �ӷ�
			strcat(str, "+");
			break;

		case 1:// ����
			strcat(str, "-");
			break;

		case 2:// �˷�
			strcat(str, "x");
			break;

		case 3:// ����
			strcat(str, "/");
			break;
		}
	
		strcat(str, _itoa(p->b, tmp, 10));


		outtextxy(p->x, p->y - 20, str);            // ������������

		putpicture(p->x, p->y, bird, birdMask);  	// ����С��ͼ��

		if (p->x > 520)								// ��С���ѷɳ��߽�
		{
			p->x = -20;
		}
		p = p->next;
	}
}

//**********************************************************************************//

void putpicture(int dstx, int dsty, IMAGE *img, IMAGE *mask)      // ��������ͼƬ
{
	DWORD *src = GetImageBuffer(img);
	DWORD *mas = GetImageBuffer(mask);
	DWORD *dst = GetImageBuffer(GetWorkingImage());
	int w1 = img->getwidth(), h1 = img->getheight(), w2;
	int i, j;
	if (GetWorkingImage() == NULL)
		w2 = getwidth();
	else
		w2 = GetWorkingImage()->getwidth();
	for (i = 0; i < w1; i++)
	{
		for (j = 0; j < h1; j++)
		{
			// ��������ͼ�񣬱�����Ӧ����ͼ���ɫ�����С��ԭͼ��
			if (mas[j*w1 + i] <RGB(24, 24, 24))
				dst[(j + dsty)*w2 + i + dstx] = src[j*w1 + i];
		}
	}
	if (!GetWorkingImage())           //���Ŀ���Ǵ���
		FlushBatchDraw();
}

//**********************************************************************************//

void setOption(int id, LPCTSTR str)          // ���ô�ѡ�����
{


	settextcolor(WHITE);
	switch (id)
	{
	case 1:
		outtextxy(395 + optOffsetRX, 375 + optOffsetRY, str);
		break;
	case 2:
		outtextxy(455 + optOffsetRX, 375 + optOffsetRY, str);
		break;
	case 3:
		outtextxy(515 + optOffsetRX, 375 + optOffsetRY, str);
		break;
	case 4:
		outtextxy(575 + optOffsetRX, 375 + optOffsetRY, str);
		break;
	}
}

//**********************************************************************************//

void setScore(LPCTSTR str)             // ���÷���������
{

	settextcolor(WHITE);
	outtextxy(340 + optOffsetX, 370 + optOffsetY, str);
}

//**********************************************************************************//

void setUser(LPCTSTR str)              // �����û�������
{


	settextcolor(WHITE);
	outtextxy(340 + optOffsetX, 340 + optOffsetY, str);
}

//**********************************************************************************//

void setTime(int timeLeft)             // ���õ���ʱʱ��
{


	settextcolor(WHITE);

	char tmp[10];                   // ������ʱת��Ϊ�ַ���
	outtextxy(360 + optOffsetX, 400 + optOffsetY, _itoa(timeLeft, tmp, 10));
}

//**********************************************************************************//

void drawMenu()            // ���Ʋ˵�
{
	int ans_x[4], ans_y[4];

	int menu_width, menu_height;
	int ans_width, ans_height;

	int add_x, add_y, delete_x, delete_y, exit_x, exit_y, save_x, save_y, load_x, load_y;


	setlinecolor(BLACK);                        	       // ���Ʒָ�����
	line(0, 320 + optOffsetY, 640, 320 + optOffsetY);
	line(290, 320 + optOffsetY, 290, 480);
	line(390, 320 + optOffsetY, 390, 480);


	setfillcolor(BLUE);                         	// ���ơ���ӣ�ɾ�����˳������棬���롱��ť

	menu_width = 70;
	menu_height = 20;

	add_x = 50 + optOffsetLX;
	add_y = 350 + optOffsetLY;
	fillrectangle(add_x, add_y, add_x + menu_width, add_y + menu_height);
	delete_x = 50 + optOffsetLX;
	delete_y = 390 + optOffsetLY;
	fillrectangle(delete_x, delete_y, delete_x + menu_width, delete_y + menu_height);
	exit_x = 130 + optOffsetLX;
	exit_y = 390 + optOffsetLY;
	fillrectangle(exit_x, exit_y, exit_x + menu_width, exit_y + menu_height);
	save_x = 210 + optOffsetLX;
	save_y = 350 + optOffsetLY;
	fillrectangle(save_x, save_y, save_x + menu_width, save_y + menu_height);
	load_x = 210 + optOffsetLX;
	load_y = 390 + optOffsetLY;
	fillrectangle(load_x, load_y, load_x + menu_width, load_y + menu_height);


	setfillcolor(YELLOW);                                	// �����ĸ�ѡ��
	ans_width = 40;
	ans_height = 30;

	ans_x[0] = 390 + optOffsetRX;
	ans_y[0] = 370 + optOffsetRY;
	fillrectangle(ans_x[0], ans_y[0], ans_x[0] + ans_width, ans_y[0] + ans_height);
	ans_x[1] = 450 + optOffsetRX;
	ans_y[1] = 370 + optOffsetRY;
	fillrectangle(ans_x[1], ans_y[1], ans_x[1] + ans_width, ans_y[1] + ans_height);
	ans_x[2] = 510 + optOffsetRX;
	ans_y[2] = 370 + optOffsetRY;
	fillrectangle(ans_x[2], ans_y[2], ans_x[2] + ans_width, ans_y[2] + ans_height);
	ans_x[3] = 570 + optOffsetRX;
	ans_y[3] = 370 + optOffsetRY;
	fillrectangle(ans_x[3], ans_y[3], ans_x[3] + ans_width, ans_y[3] + ans_height);


	settextcolor(WHITE);                                           	// ��䰴ť����
	outtextxy(70 + optOffsetLX, 352 + optOffsetLY, "����");
	outtextxy(70 + optOffsetLX, 392 + optOffsetLY, "ɾ��");
	outtextxy(150 + optOffsetLX, 392 + optOffsetLY, "�˳�");
	outtextxy(220 + optOffsetLX, 352 + optOffsetLY, "��ɼ�");
	outtextxy(230 + optOffsetLX, 392 + optOffsetLY, "����");

	outtextxy(300 + optOffsetX, 340 + optOffsetY, "����:");
	outtextxy(300 + optOffsetX, 370 + optOffsetY, "�÷�:");
	outtextxy(300 + optOffsetX, 400 + optOffsetY, "����ʱ:");


	setOption(1, option[0]);                  	// Ϊ�ĸ�ѡ������ֵ
	setOption(2, option[1]);
	setOption(3, option[2]);
	setOption(4, option[3]);


	setUser(username);                 // �����û���


	char tmp[10];                           	// ���õ�ǰ����
	setScore(_itoa(currentScore, tmp, 10));
}

//**********************************************************************************//

void addBird()           // ���С��
{

	Bird* newBird = (Bird*)malloc(sizeof(Bird));        //����洢�ռ�
	newBird->x = 0;
	if (endBird == NULL)
	{
		newBird->y = 30;
	}
	else {
		switch (endBird->y)                  //��ֹС�񽻲����
		{
		case 30:
			newBird->y = 135;
			break;
		case 135:
			newBird->y = 240;
			break;
		case 240:
			newBird->y = 30;
			break;
		}
	}


	srand((unsigned)time(NULL));         	// �����������
	newBird->a = 2 + rand() % 10;
	newBird->b = 1 + rand() % (newBird->a - 1);
	newBird->op = rand() % 4;


	switch (newBird->op)                                   	// ���ݶ�Ӧ�������������ȷ��
	{

	case 0:	                                              // �ӷ�����
		newBird->result = newBird->a + newBird->b;
		break;

	case 1:		                                          // ��������
		newBird->result = newBird->a - newBird->b;
		break;

	case 2:		                                          // �˷�����
		newBird->result = newBird->a * newBird->b;
		break;

	case 3:		                                          // ��������
		newBird->result = newBird->a / newBird->b;
		break;
	}

	newBird->isdelete = FALSE;
	newBird->next = NULL;

	if (endBird == NULL)
	{
		headBird = newBird;
		endBird = headBird;
	}
	else {
		endBird->next = newBird;
		endBird = endBird->next;
	}
}


void updateOption(Bird* selBird, int* currentAnswer)            // ���´�ѡ��
{
	srand((unsigned)time(NULL));
	int ran = rand() % 4;
	char tmp[10];
	for (int i = 0; i < 4; i++)
	{
		if (i == ran)
		{

			strcpy(option[i], _itoa(selBird->result, tmp, 10));     // ������ȷѡ��
			*currentAnswer = selBird->result;
		}
		else {

			int ranAns = rand() % 30;
			while (ranAns == selBird->result)
			{
				ranAns = rand() % 30;
			}
			strcpy(option[i], _itoa(ranAns, tmp, 10));          // �����������ѡ��
		}
	}
}



//**********************************************************************************//

void checkClick(int mx, int my, int* currentAnswer)             // ���������¼�
{
	int ans_x[4], ans_y[4];

	int menu_width, menu_height;
	int ans_width, ans_height;
	int add_x, add_y, delete_x, delete_y, exit_x, exit_y, save_x, save_y, load_x, load_y;

	ans_width = 40;
	ans_height = 30;
	menu_width = 70;
	menu_height = 20;
	ans_x[0] = 390 + optOffsetRX;
	ans_y[0] = 370 + optOffsetRY;
	ans_x[1] = 450 + optOffsetRX;
	ans_y[1] = 370 + optOffsetRY;
	ans_x[2] = 510 + optOffsetRX;
	ans_y[2] = 370 + optOffsetRY;
	ans_x[3] = 570 + optOffsetRX;
	ans_y[3] = 370 + optOffsetRY;

	add_x = 50 + optOffsetLX;
	add_y = 350 + optOffsetLY;
	delete_x = 50 + optOffsetLX;
	delete_y = 390 + optOffsetLY;
	exit_x = 130 + optOffsetLX;
	exit_y = 390 + optOffsetLY;
	save_x = 210 + optOffsetLX;
	save_y = 350 + optOffsetLY;
	load_x = 210 + optOffsetLX;
	load_y = 390 + optOffsetLY;


	if (mx >= add_x && my >= add_y                                  	// �������
		&& mx <= add_x + menu_width && my <= add_y + menu_height)
	{
		addBird();
		return;
	}

	Bird* p = headBird;
	while (p != NULL)
	{
		if (p->isdelete)                            //���С��ɾ��
		{
			p = p->next;
			continue;
		}
		if (mx >= p->x && my >= p->y                 	// ���С��
			&& mx <= p->x + 130 && my <= p->y + 127)
		{
			if (p == curSelBird)
				return;
			curSelBird = p;
			updateOption(p, currentAnswer);
			return;
		}
		p = p->next;
	}

	for (int i = 0; i < 4; i++)
	{
		if (mx >= ans_x[i] && my >= ans_y[i]                                	// �����
			&& mx <= ans_x[i] + ans_width && my <= ans_y[i] + ans_height)
		{
			if (atoi(option[i]) == *currentAnswer)
			{
				currentScore += 1;
				curSelBird->isdelete = true;
				*currentAnswer = -1;
				for (int j = 0; j < 4; j++)
				{
					strcpy(option[j], "");
				}

			}
			else currentScore -= 1;
			return;
		}
	}

	if (mx >= delete_x && my >= delete_y                                    	// ���ɾ��
		&& mx <= delete_x + menu_width && my <= delete_y + menu_height)
	{
		if (curSelBird == NULL)
		{
			Bird* p;
			p = headBird;
			while (p != NULL)
			{
				if (p->isdelete)
				{
					p = p->next;
					continue;
				}
				p->isdelete = true;
				*currentAnswer = -1;
				for (int j = 0; j < 4; j++)
				{
					strcpy(option[j], "");
				}
				return;
			}
		}
		else
		{
			curSelBird->isdelete = TRUE;
			curSelBird = NULL;
			for (int j = 0; j < 4; j++)
			{
				strcpy(option[j], "");
			}
		}
	}

	if (mx >= exit_x && my >= exit_y                                     	// ����˳�
		&& mx <= exit_x + menu_width && my <= exit_y + menu_height)
	{
		FILE *pFile = fopen("log.txt", "w");
		fwrite(username, 1, strlen(username), pFile);
		fwrite("\n", 1, strlen("\n"), pFile);
		char tmp[10];
		_itoa(currentScore, tmp, 10);
		fwrite(tmp, 1, strlen(tmp), pFile);
		fclose(pFile);
		exit(0);
	}

	if (mx >= save_x && my >= save_y                                      	// �����״̬
		&& mx <= save_x + menu_width && my <= save_y + menu_height)
	{
		FILE *pFile = fopen("log.txt", "w");
		fwrite(username, 1, strlen(username), pFile);
		fwrite("\n", 1, strlen("\n"), pFile);
		char tmp[10];
		_itoa(currentScore, tmp, 10);
		fwrite(tmp, 1, strlen(tmp), pFile);
		fclose(pFile);
	}

	if (mx >= load_x && my >= load_y                                      	// �������
		&& mx <= load_x + menu_width && my <= load_y + menu_height)
	{
		FILE *pFile = fopen("log.txt", "r");
		if (pFile == NULL)
			return;
		char tmp[50];
		fgets(tmp, 1024, pFile);
		strcpy(username, tmp);

		if (username[strlen(username) - 1] == '\n')                          	// ɾ���ļ��еĻ��з�
		{
			username[strlen(username) - 1] = '\0';
		}
		fgets(tmp, 1024, pFile);
		fgets(tmp, 1024, pFile);
		currentScore = atoi(tmp);
		fclose(pFile);
	}
}

//**********************************************************************************//

void StartBkMusic()                         //�������ֺ���
{
	PlaySound(TEXT("res\\m_menubackground.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);

}
