#include <curses.h>
#include <random>
#include <iostream>


//ぷよの色を表すの列挙型
//NONEが無し，RED,BLUE,..が色を表す
enum puyocolor{NONE, RED, BLUE, GREEN, YELLOW};

int landed = 0;

//ぷよ1つのデータを格納する構造体
struct puyodata
{
	enum puyocolor color;	//色
	bool handling;	//操作中か否か


	//構造体のコンストラクタ
	//この構造体のインスタンス定義時に一度だけ実行される関数
	//ここでは変数の値を初期化する
	puyodata()
	{
		color = NONE;
		handling = false;
	}
};

class Field {
private:
    struct puyodata *field_array;
    unsigned int field_line;
    unsigned int field_column;
    void Release();
public:
    void ChangeSize(unsigned int line, unsigned int column);
    struct puyodata GetValue(unsigned int y, unsigned int x);
    void SetValue(unsigned int y, unsigned int x, struct puyodata data);
    void GeneratePuyo();
    int LandingPuyo();
    void ControlDown();
    void ControlLeft();
    void ControlRight();
    Field() {ChangeSize(LINES/2, COLS/2);}
    ~Field(){Release();}
    unsigned int GetLine();
    unsigned int GetColumn();
};

unsigned int Field::GetLine(){
    return field_line;
}

unsigned int Field::GetColumn(){
    return field_column;
}

//フィールドを表す配列
struct puyodata *field_array = NULL;
//フィールドの行数，列数
unsigned int field_line = 0;
unsigned int field_column = 0;

//field_arrayのメモリ開放
void Field::Release()
{
	if (field_array == NULL)
	{
		return;
	}

	delete[] field_array;
	field_array = NULL;
}


//フィールドサイズ変更
void Field::ChangeSize(unsigned int line, unsigned int column)
{
	Field_Release();

	//新しいサイズでメモリ確保
	field_array = new struct puyodata[line*column];

	field_line = line;
	field_column = column;
}

//フィールドの指定された位置の値を返す
struct puyodata Field::GetValue(unsigned int y, unsigned int x)
{
	if (y >= field_line || x >= field_column)
	{
		//引数の値が正しくない
		struct puyodata temp;
		return temp;
	}

	return field_array[y*field_column + x];
}

//フィールドの指定された位置に値を書き込む
void Field::SetValue(unsigned int y, unsigned int x, struct puyodata data)
{
	if (y >= field_line || x >= field_column)
	{
		//引数の値が正しくない
		return;
	}

	field_array[y*field_column + x] = data;
}


//フィールドに新しいぷよ生成
void Field::GeneratePuyo()
{

    landed = 0;

struct puyodata newpuyo[4];

	newpuyo[0].color = RED;
	newpuyo[0].handling = true;

	newpuyo[1].color = BLUE;
	newpuyo[1].handling = true;

    newpuyo[2].color = YELLOW;
    newpuyo[2].handling = true;

    newpuyo[3].color = GREEN;
    newpuyo[3].handling = true;

    std::random_device rd;

	Field_SetValue(0, 5, newpuyo[rd()%4]);
	Field_SetValue(0, 6, newpuyo[rd()%4]);
}

//操作中ぷよの着地判定
//着地判定があるとtrueを返す
int Field::LandingPuyo()
{

	for (int y = 0; y < field_line; y++)
	{
		for (int x = 0; x < field_column; x++)
		{
			if (field_array[y*field_column + x].handling == true)
			{
				if (y == field_line - 1 || field_array[(y + 1)*field_column + x].color != NONE)
				{
					//操作中フラグをfalseにして着地状態にする
					field_array[y*field_column + x].handling = false;
					//左右のぷよも着地状態にする
                    landed += 1;

				}
			}
		}
	}

	return landed;
}

//下移動
void Field::ControlDown()
{
	//一時的格納場所メモリ確保
	struct puyodata *field_array_temp;
	field_array_temp = new struct puyodata[field_line*field_column];

	//1つ下の位置にfield_arrayからfield_array_tempへとコピー
	for (int y = 0; y < field_line; y++)
	{
		for (int x = 0; x < field_column; x++)
		{
			if (field_array[y*field_column + x].handling == true)
			{
				if (y < field_line-1)
				{
					field_array_temp[(y + 1)*field_column + x] = field_array[y*field_column + x];
				}
				else
				{
					field_array_temp[y*field_column + x] = field_array[y*field_column + x];
				}

				//コピー後にfield_arrayのデータは消す
				field_array[y*field_column + x].color = NONE;
				field_array[y*field_column + x].handling = false;
			}
		}
	}

	//field_array_tempからfield_arrayへコピー
	for (int y = 0; y < field_line; y++)
	{
		for (int x = 0; x < field_column; x++)
		{
			if (field_array_temp[y*field_column + x].handling == true)
			{
				field_array[y*field_column + x] = field_array_temp[y*field_column + x];
			}
		}
	}

	//一時的格納場所メモリ解放
	delete[] field_array_temp;
}

//左移動
void Field::ControlLeft()
{
	//一時的格納場所メモリ確保
	struct puyodata *field_array_temp;
	field_array_temp = new struct puyodata[field_line*field_column];

	//1つ左の位置にfield_arrayからfield_array_tempへとコピー
	for (int y = 0; y < field_line; y++)
	{
		for (int x = 0; x < field_column; x++)
		{
			if (field_array[y*field_column + x].handling == true)
			{
				if (0 < x)
				{
					field_array_temp[y*field_column + (x - 1)] = field_array[y*field_column + x];
				}
				else
				{
					field_array_temp[y*field_column + x] = field_array[y*field_column + x];
				}

				//コピー後にfield_arrayのデータは消す
				field_array[y*field_column + x].color = NONE;
				field_array[y*field_column + x].handling = false;
			}
		}
	}

	//field_array_tempからfield_arrayへコピー
	for (int y = 0; y < field_line; y++)
	{
		for (int x = 0; x < field_column; x++)
		{
			if (field_array_temp[y*field_column + x].handling == true)
			{
				field_array[y*field_column + x] = field_array_temp[y*field_column + x];
			}
		}
	}

	//一時的格納場所メモリ解放
	delete[] field_array_temp;
}

//右移動
void Field::ControlRight()
{
	//一時的格納場所メモリ確保
	struct puyodata *field_array_temp;
	field_array_temp = new struct puyodata[field_line*field_column];

	//1つ右の位置にfield_arrayからfield_array_tempへとコピー
	for (int y = 0; y < field_line; y++)
	{
		for (int x = 0; x < field_column; x++)
		{
			if (field_array[y*field_column + x].handling == true)
			{
				if (x <= field_column)
				{
					field_array_temp[y*field_column + (x + 1)] = field_array[y*field_column + x];
				}
				else
				{
					field_array_temp[y*field_column + x] = field_array[y*field_column + x];
				}

				//コピー後にfield_arrayのデータは消す
				field_array[y*field_column + x].color = NONE;
				field_array[y*field_column + x].handling = false;
			}
		}
	}

	//field_array_tempからfield_arrayへコピー
	for (int y = 0; y < field_line; y++)
	{
		for (int x = 0; x < field_column; x++)
		{
			if (field_array_temp[y*field_column + x].handling == true)
			{
				field_array[y*field_column + x] = field_array_temp[y*field_column + x];
			}
		}
	}

	//一時的格納場所メモリ解放
	delete[] field_array_temp;
}


//表示
void Display(Field& instans)
{
	//フィールド表示
	for (int y = 0; y < instans.GetLine(); y++)
	{
		for (int x = 0; x < instans.GetColumn(); x++)
		{
			switch (isntans.GetValue(y, x).color)
			{
			case NONE:
				mvaddch(y, x, ' ');
				break;
			case RED:
                init_pair(1, COLOR_RED, COLOR_BLACK);
                attrset(COLOR_PAIR(1));
				mvaddch(y, x, 'R');
				break;
			case BLUE:
                init_pair(2, COLOR_BLUE, COLOR_BLACK);
                attrset(COLOR_PAIR(2));
				mvaddch(y, x, 'B');
				break;
			case GREEN:
                init_pair(3, COLOR_GREEN, COLOR_BLACK);
                attrset(COLOR_PAIR(3));
				mvaddch(y, x, 'G');
				break;
			case YELLOW:
                init_pair(4, COLOR_YELLOW, COLOR_BLACK);
                attrset(COLOR_PAIR(4));
				mvaddch(y, x, 'Y');
				break;
			default:
				mvaddch(y, x, '?');
				break;
			}

		}
	}


	//情報表示
	int count = 0;
	for (int y = 0; y < instans.GetLine(); y++)
	{
		for (int x = 0; x < instans.GetColumn(); x++)
		{
			if (instans.GetValue(y, x).color != NONE)
			{
				count++;
			}
		}

	}

	char msg[256];
	sprintf(msg, "Field: %d x %d, Puyo number: %03d", field_line, field_column, count);
	mvaddstr(2, COLS - 35, msg);
}


//ここから実行される
int main(int argc, char **argv){

    Field field;
	//画面の初期化
	initscr();
	//カラー属性を扱うための初期化
	start_color();

	//キーを押しても画面に表示しない
	noecho();
	//キー入力を即座に受け付ける
	cbreak();

	curs_set(0);
	//キー入力受付方法指定
	keypad(stdscr, TRUE);

	//キー入力非ブロッキングモード
	timeout(0);


	//初期化処理
	Field_ChangeSize(LINES/2, COLS/2);	//フィールドは画面サイズの縦横1/2にする
	Field_GeneratePuyo();	//最初のぷよ生成

	int delay = 0;
	int waitCount = 2000;


	//メイン処理ループ
	while (1)
	{
		//キー入力受付
		int ch;
		ch = getch();

		//Qの入力で終了
		if (ch == 'Q')
		{
			break;
		}

		//入力キーごとの処理
		switch (ch)
		{
		case KEY_LEFT:
           if (field.LandingPuyo() == 2)
			    field.ControlLeft();
			break;
		case KEY_RIGHT:
            if (field.LandingPuyo() == 2)
			    field.ControlRight();
			break;
		case 'Z':
			//ぷよ回転処理など
			break;
		default:
			break;
		}


		//処理速度調整のためのif文
		if (delay%waitCount == 0){
			//ぷよ下に移動
			field.ControlDown();

			//ぷよ着地判定
			if (field.LandingPuyo() == 2){
				//着地したら新しいぷよ生成
				field.GeneratePuyo();
			}
		}
		delay++;


		//表示
		Display(field);
	}


	//画面をリセット
	endwin();

	return 0;
}
