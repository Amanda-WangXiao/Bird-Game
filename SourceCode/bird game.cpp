#include <stdio.h>
#include <graphics.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#pragma comment(lib,"Winmm.lib")



//****************************小鸟数据结构****************************************//

struct Bird         // 定义小鸟结构体
{
	int x;		    // X坐标
	int y;	      	// Y坐标
	int a;			// 第一个运算数
	int b;			// 第二个运算数
	int op;			// 运算符号
	int result;		// 运算答案

	bool isdelete;	// 小鸟是否被删除

	Bird* next;		// 链接下一个小鸟的指针
};


Bird *headBird = NULL;           // 存储小鸟链表头，链表尾，当前选中的小鸟
Bird *endBird = NULL;
Bird *curSelBird = NULL;

//**********************************************************************************//

char option[4][10];    // 四种选项存储数组
int currentScore = 0;   // 当前分数
char username[50];     // 用户名



//*******************************方便绘制界面的参数************************************//
int optOffsetX = 0;
int optOffsetY = 50;
int optOffsetLX = -20;
int optOffsetLY = 50;
int optOffsetRX = 20;
int optOffsetRY = 50;

//********************************各功能模块函数***************************************//


void putpicture(int dstx, int dsty, IMAGE *img, IMAGE *mask);       // 绘制掩码图片函数
void setOption(int id, LPCTSTR str);           // 设置答案选项并绘制函数
void setScore(LPCTSTR str);    // 设置分数并绘制函数
void setUser(LPCTSTR str);      // 设置用户并绘制函数
void setTime(int timeLeft);    // 设置倒计时时间函数
void drawMenu();      // 绘制菜单函数
void addBird();    // 添加小鸟函数
void updateOption(Bird* selBird, int* currentAnswer);    // 更新答案选项函数
void checkClick(int mx, int my, int* currentAnswer);   // 检测鼠标点击事件函数
void drawBird(IMAGE* bird, IMAGE* birdMask);       // 绘制小鸟函数
void StartBkMusic();      //播放音乐函数

//*************************************主函数*********************************************//
void main()
{



	clock_t startTime, currentTime;    //设置开始、当前时间

	IMAGE bird;          // 小鸟图像

	IMAGE birdMask;        // 小鸟掩码图像


	int currentAnswer;     // 当前的正确答案

	int timeLeft;        // 剩余时间



	printf("请输入用户名：");      //输入用户名
	scanf("%s", username);

	StartBkMusic();     //播放音乐

	initgraph(640, 480);                              // 初始化背景，载入背景图，小鸟及其掩码图
	loadimage(NULL, "./res/bkg.jpg");
	loadimage(&bird, "./res/Bird.jpg");
	loadimage(&birdMask, "./res/Bird_mask.jpg");

	MOUSEMSG m;            //设置鼠标

	BeginBatchDraw();           //开始批量绘图

	startTime = clock();
	while (true)
	{
		loadimage(NULL, "./res/bkg.jpg");
		drawMenu();
		currentTime = clock();
		timeLeft = 60 - (currentTime - startTime) / CLOCKS_PER_SEC;      //当前时间
		setTime(timeLeft);                                               //显示当前时间

		FlushBatchDraw();

		drawBird(&bird, &birdMask);


		if ((currentTime - startTime) / CLOCKS_PER_SEC > 60)   	// 超过60秒时退出游戏并保存数据
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

		if (MouseHit())           //检测当前是否有鼠标消息
		{
			m = GetMouseMsg();    //获取鼠标消息

			if (m.uMsg == WM_LBUTTONDOWN)
			{
				checkClick(m.x, m.y, &currentAnswer);
			}
		}
		FlushMouseMsgBuffer();
		Sleep(35);
	}

	EndBatchDraw();           //结束批量绘图

	_getch();
	closegraph();
}

//********************************各功能模块详细函数***************************************//

void drawBird(IMAGE* bird, IMAGE* birdMask)     // 绘制小鸟
{
	Bird *p;
	p = headBird;
	while (p != NULL)
	{

		if (p->isdelete)        // 跳过已被删除的小鸟
		{
			p = p->next;
			continue;
		}

		p->x += 3;              //飞行速度为3
		char str[50] = "";
		char tmp[10];

		strcat(str, _itoa(p->a, tmp, 10));  	// 将小鸟对应的问题组成字符串（通过strcat函数链接）

		switch (p->op)               // 将小鸟问题中的int运算符号转换成字符
		{

		case 0: // 加法
			strcat(str, "+");
			break;

		case 1:// 减法
			strcat(str, "-");
			break;

		case 2:// 乘法
			strcat(str, "x");
			break;

		case 3:// 除法
			strcat(str, "/");
			break;
		}
	
		strcat(str, _itoa(p->b, tmp, 10));


		outtextxy(p->x, p->y - 20, str);            // 绘制运算问题

		putpicture(p->x, p->y, bird, birdMask);  	// 绘制小鸟图像

		if (p->x > 520)								// 若小鸟已飞出边界
		{
			p->x = -20;
		}
		p = p->next;
	}
}

//**********************************************************************************//

void putpicture(int dstx, int dsty, IMAGE *img, IMAGE *mask)      // 绘制掩码图片
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
			// 遍历掩码图像，保留对应掩码图像黑色区域的小鸟原图像
			if (mas[j*w1 + i] <RGB(24, 24, 24))
				dst[(j + dsty)*w2 + i + dstx] = src[j*w1 + i];
		}
	}
	if (!GetWorkingImage())           //如果目标是窗口
		FlushBatchDraw();
}

//**********************************************************************************//

void setOption(int id, LPCTSTR str)          // 设置答案选项并绘制
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

void setScore(LPCTSTR str)             // 设置分数并绘制
{

	settextcolor(WHITE);
	outtextxy(340 + optOffsetX, 370 + optOffsetY, str);
}

//**********************************************************************************//

void setUser(LPCTSTR str)              // 设置用户并绘制
{


	settextcolor(WHITE);
	outtextxy(340 + optOffsetX, 340 + optOffsetY, str);
}

//**********************************************************************************//

void setTime(int timeLeft)             // 设置倒计时时间
{


	settextcolor(WHITE);

	char tmp[10];                   // 将倒计时转换为字符串
	outtextxy(360 + optOffsetX, 400 + optOffsetY, _itoa(timeLeft, tmp, 10));
}

//**********************************************************************************//

void drawMenu()            // 绘制菜单
{
	int ans_x[4], ans_y[4];

	int menu_width, menu_height;
	int ans_width, ans_height;

	int add_x, add_y, delete_x, delete_y, exit_x, exit_y, save_x, save_y, load_x, load_y;


	setlinecolor(BLACK);                        	       // 绘制分隔线条
	line(0, 320 + optOffsetY, 640, 320 + optOffsetY);
	line(290, 320 + optOffsetY, 290, 480);
	line(390, 320 + optOffsetY, 390, 480);


	setfillcolor(BLUE);                         	// 绘制“添加，删除，退出，保存，导入”按钮

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


	setfillcolor(YELLOW);                                	// 绘制四个选项
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


	settextcolor(WHITE);                                           	// 填充按钮文字
	outtextxy(70 + optOffsetLX, 352 + optOffsetLY, "增加");
	outtextxy(70 + optOffsetLX, 392 + optOffsetLY, "删除");
	outtextxy(150 + optOffsetLX, 392 + optOffsetLY, "退出");
	outtextxy(220 + optOffsetLX, 352 + optOffsetLY, "存成绩");
	outtextxy(230 + optOffsetLX, 392 + optOffsetLY, "导入");

	outtextxy(300 + optOffsetX, 340 + optOffsetY, "姓名:");
	outtextxy(300 + optOffsetX, 370 + optOffsetY, "得分:");
	outtextxy(300 + optOffsetX, 400 + optOffsetY, "倒计时:");


	setOption(1, option[0]);                  	// 为四个选项设置值
	setOption(2, option[1]);
	setOption(3, option[2]);
	setOption(4, option[3]);


	setUser(username);                 // 设置用户名


	char tmp[10];                           	// 设置当前分数
	setScore(_itoa(currentScore, tmp, 10));
}

//**********************************************************************************//

void addBird()           // 添加小鸟
{

	Bird* newBird = (Bird*)malloc(sizeof(Bird));        //申请存储空间
	newBird->x = 0;
	if (endBird == NULL)
	{
		newBird->y = 30;
	}
	else {
		switch (endBird->y)                  //防止小鸟交叉出现
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


	srand((unsigned)time(NULL));         	// 随机设置问题
	newBird->a = 2 + rand() % 10;
	newBird->b = 1 + rand() % (newBird->a - 1);
	newBird->op = rand() % 4;


	switch (newBird->op)                                   	// 根据对应运算符，计算正确答案
	{

	case 0:	                                              // 加法运算
		newBird->result = newBird->a + newBird->b;
		break;

	case 1:		                                          // 减法运算
		newBird->result = newBird->a - newBird->b;
		break;

	case 2:		                                          // 乘法运算
		newBird->result = newBird->a * newBird->b;
		break;

	case 3:		                                          // 除法运算
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


void updateOption(Bird* selBird, int* currentAnswer)            // 更新答案选项
{
	srand((unsigned)time(NULL));
	int ran = rand() % 4;
	char tmp[10];
	for (int i = 0; i < 4; i++)
	{
		if (i == ran)
		{

			strcpy(option[i], _itoa(selBird->result, tmp, 10));     // 设置正确选项
			*currentAnswer = selBird->result;
		}
		else {

			int ranAns = rand() % 30;
			while (ranAns == selBird->result)
			{
				ranAns = rand() % 30;
			}
			strcpy(option[i], _itoa(ranAns, tmp, 10));          // 设置随机错误选项
		}
	}
}



//**********************************************************************************//

void checkClick(int mx, int my, int* currentAnswer)             // 检测鼠标点击事件
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


	if (mx >= add_x && my >= add_y                                  	// 点击增加
		&& mx <= add_x + menu_width && my <= add_y + menu_height)
	{
		addBird();
		return;
	}

	Bird* p = headBird;
	while (p != NULL)
	{
		if (p->isdelete)                            //如果小鸟被删除
		{
			p = p->next;
			continue;
		}
		if (mx >= p->x && my >= p->y                 	// 点击小鸟
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
		if (mx >= ans_x[i] && my >= ans_y[i]                                	// 点击答案
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

	if (mx >= delete_x && my >= delete_y                                    	// 点击删除
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

	if (mx >= exit_x && my >= exit_y                                     	// 点击退出
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

	if (mx >= save_x && my >= save_y                                      	// 点击存状态
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

	if (mx >= load_x && my >= load_y                                      	// 点击导入
		&& mx <= load_x + menu_width && my <= load_y + menu_height)
	{
		FILE *pFile = fopen("log.txt", "r");
		if (pFile == NULL)
			return;
		char tmp[50];
		fgets(tmp, 1024, pFile);
		strcpy(username, tmp);

		if (username[strlen(username) - 1] == '\n')                          	// 删除文件中的换行符
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

void StartBkMusic()                         //播放音乐函数
{
	PlaySound(TEXT("res\\m_menubackground.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);

}
